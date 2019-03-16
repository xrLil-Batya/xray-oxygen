// tntQAVI.h - декодер AVI видеопотока
// Oles (C), 2002-2007
#include "stdafx.h"
#pragma hdrstop
#include "tntQAVI.h"

CAviPlayerCustom::CAviPlayerCustom()
{
    ZeroMemory(this, sizeof(*this));
	m_dwFrameCurrent = 0xfffffffd;	// страхуемся от 0xffffffff + 1 == 0
	m_dwFirstFrameOffset = 0;
}

CAviPlayerCustom::~CAviPlayerCustom()
{
	if(m_aviIC)
	{
		ICDecompressEnd(m_aviIC);
		ICClose(m_aviIC);
	}

	if (m_pDecompressedBuf)
		xr_free(m_pDecompressedBuf);

	if (m_pMovieData)
		xr_free(m_pMovieData);
	if (m_pMovieIndex)
		xr_free(m_pMovieIndex);

	xr_delete(alpha);
}

//---------------------------------
BOOL CAviPlayerCustom::Load(char* fname)
{
	// Check for alpha
	string_path		aname;
	xr_strconcat(aname, fname, "_alpha");
	if (FS.exist(aname))
	{
		alpha = xr_new<CAviPlayerCustom>();
		alpha->Load(aname);
	}

	// РћС‚РєСЂС‹С‚СЊ С‡РµСЂРµР· mmioOpen( ) AVI С„Р°Р№Р»
	HMMIO hmmioFile = mmioOpen(fname, NULL, MMIO_READ /*MMIO_EXCLUSIVE*/);
	if (hmmioFile == NULL) {

		return FALSE;
	}

	// РќР°Р№С‚Рё С‡Р°РЅРє FOURCC('movi')

	MMCKINFO mmckinfoParent;
	memset(&mmckinfoParent, 0, sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('A', 'V', 'I', ' ');
	MMRESULT res;
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, NULL, MMIO_FINDRIFF))) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	memset(&mmckinfoParent, 0, sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('h', 'd', 'r', 'l');
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, NULL, MMIO_FINDLIST))) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}
	//-------------------------------------------------------------------
	//++strl
	memset(&mmckinfoParent, 0, sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('s', 't', 'r', 'l');
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, NULL, MMIO_FINDLIST))) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	//++strh
	memset(&mmckinfoParent, 0, sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('s', 't', 'r', 'h');
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, NULL, MMIO_FINDCHUNK))) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	AVIStreamHeaderCustom	strh;
	memset(&strh, 0, sizeof(strh));
	if (mmckinfoParent.cksize != (DWORD)mmioRead(hmmioFile, (HPSTR)&strh, mmckinfoParent.cksize)) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}



	AVIFileInit();
	PAVIFILE aviFile = 0;
	if (AVIERR_OK != AVIFileOpen(&aviFile, fname, OF_READ, 0))	return FALSE;

	AVIFILEINFO		aviInfo;
	memset(&aviInfo, 0, sizeof(aviInfo));
	if (AVIERR_OK != AVIFileInfo(aviFile, &aviInfo, sizeof(aviInfo))) {
		AVIFileRelease(aviFile);
		return FALSE;
	}

	m_dwFrameTotal = aviInfo.dwLength;
	m_fCurrentRate = (float)aviInfo.dwRate / (float)aviInfo.dwScale;

	m_dwWidth = aviInfo.dwWidth;
	m_dwHeight = aviInfo.dwHeight;

	AVIFileRelease(aviFile);

	R_ASSERT(m_dwWidth && m_dwHeight);

	m_pDecompressedBuf = (BYTE *)xr_malloc(m_dwWidth * m_dwHeight * 4 + 4);

	//++strf
	memset(&mmckinfoParent, 0, sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('s', 't', 'r', 'f');
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, NULL, MMIO_FINDCHUNK))) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// РїРѕР»СѓС‡Р°РµРј РІС…РѕРґРЅРѕР№ С„РѕСЂРјР°С‚ РґРµРєРѕРјРїСЂРµСЃСЃРѕСЂР° РІ BITMAPINFOHEADER
	if (mmckinfoParent.cksize != (DWORD)mmioRead(hmmioFile, (HPSTR)&m_biInFormat, mmckinfoParent.cksize)) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// СЃРѕР·РґР°РµРј РІС‹С…РѕРґРЅРѕР№ С„РѕСЂРјР°С‚ РґРµРєРѕРјРїСЂРµСЃСЃРѕСЂР°	(xRGB)
	m_biOutFormat.biSize = sizeof(m_biOutFormat);
	m_biOutFormat.biBitCount = 32;
	m_biOutFormat.biCompression = BI_RGB;
	m_biOutFormat.biPlanes = 1;
	m_biOutFormat.biWidth = m_dwWidth;
	m_biOutFormat.biHeight = m_dwHeight;
	m_biOutFormat.biSizeImage = m_dwWidth * m_dwHeight * 4;

	// РќР°Р№С‚Рё РїРѕРґС…РѕРґСЏС‰РёР№ РґРµРєРѕРјРїСЂРµСЃСЃРѕСЂ
	m_aviIC = ICLocate(ICTYPE_VIDEO, NULL, &m_biInFormat, &m_biOutFormat, \
		// ICMODE_DECOMPRESS
		ICMODE_FASTDECOMPRESS
	);
	if (m_aviIC == 0) {

		return FALSE;
	}

	// РџСЂРѕРёРЅРёС‚РёС‚СЊ РґРµРєРѕРјРїСЂРµСЃСЃРѕСЂ
	if (ICERR_OK != ICDecompressBegin(m_aviIC, &m_biInFormat, &m_biOutFormat)) {

		return FALSE;
	}

	//--strf
	if (MMSYSERR_NOERROR != mmioAscend(hmmioFile, &mmckinfoParent, 0)) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	//--strh
	if (MMSYSERR_NOERROR != mmioAscend(hmmioFile, &mmckinfoParent, 0)) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	//--strl
	if (MMSYSERR_NOERROR != mmioAscend(hmmioFile, &mmckinfoParent, 0)) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	//-------------------------------------------------------------------
	MMCKINFO mmckinfoSubchunk;
	std::memset(&mmckinfoSubchunk, 0, sizeof(mmckinfoSubchunk));
	mmckinfoSubchunk.fccType = mmioFOURCC('m', 'o', 'v', 'i');
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoSubchunk, NULL, MMIO_FINDLIST)) \
		|| mmckinfoSubchunk.cksize <= 4)
	{

		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	mmioSeek(hmmioFile, mmckinfoSubchunk.dwDataOffset, SEEK_SET);

	// Р’С‹РґРµР»РёС‚СЊ РїР°РјСЏС‚СЊ РїРѕРґ СЃР¶Р°С‚С‹Рµ  РґР°РЅРЅС‹Рµ РІСЃРµРіРѕ РєР»РёРїР°
	m_pMovieData = (BYTE *)xr_malloc(mmckinfoSubchunk.cksize);
	if (m_pMovieData == NULL) {

		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// РџР«РЎ РѕС‡РµРЅСЊ Р»СЋР±РёР»Рё DWORD'С‹... РќР°СЃС‚РѕР»СЊРєРѕ РІРѕС‚ СЃРёР»СЊРЅРѕ.
	if (mmckinfoSubchunk.cksize != (DWORD)mmioRead(hmmioFile, (HPSTR)m_pMovieData, mmckinfoSubchunk.cksize)) {

		xr_free(m_pMovieData);	m_pMovieData = NULL;
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	if (MMSYSERR_NOERROR != mmioAscend(hmmioFile, &mmckinfoSubchunk, 0)) {

		xr_free(m_pMovieData);	m_pMovieData = NULL;
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// РќР°Р№С‚Рё С‡Р°РЅРє FOURCC('idx1')
	memset(&mmckinfoSubchunk, 0, sizeof(mmckinfoSubchunk));
	mmckinfoSubchunk.fccType = mmioFOURCC('i', 'd', 'x', '1');

	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoSubchunk, NULL, MMIO_FINDCHUNK)) \
		|| mmckinfoSubchunk.cksize <= 4)
	{
		xr_free(m_pMovieData);	m_pMovieData = NULL;
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// Р’С‹РґРµР»РёС‚СЊ РїР°РјСЏС‚СЊ РїРѕРґ РёРЅРґРµРєСЃ
	m_pMovieIndex = (AVIINDEXENTRY *)xr_malloc(mmckinfoSubchunk.cksize);
	if (m_pMovieIndex == NULL) {

		xr_free(m_pMovieData);	m_pMovieData = NULL;
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	if (mmckinfoSubchunk.cksize != (DWORD)mmioRead(hmmioFile, (HPSTR)m_pMovieIndex, mmckinfoSubchunk.cksize)) {

		xr_free(m_pMovieIndex);	m_pMovieIndex = NULL;
		xr_free(m_pMovieData);	m_pMovieData = NULL;
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// Р—Р°РєСЂС‹С‚СЊ AVI С„Р°Р№Р» С‡РµСЂРµР· mmioClose( )
	mmioClose(hmmioFile, 0);

	if (alpha) {
		R_ASSERT(m_dwWidth == alpha->m_dwWidth);
		R_ASSERT(m_dwHeight == alpha->m_dwHeight);
	}

	//-----------------------------------------------------------------
	return TRUE;
}

BOOL CAviPlayerCustom::DecompressFrame( DWORD dwFrameNum )
{
	// РїРѕР»СѓС‡Р°РµРј СЌР»РµРјРµРЅС‚ РёРЅРґРµРєСЃР°
	AVIINDEXENTRY	*pCurrFrameIndex = &m_pMovieIndex[ dwFrameNum ];

	m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
	R_ASSERT( m_biInFormat.biSizeImage != 0 );

	DWORD	dwFlags;
	dwFlags = (pCurrFrameIndex->dwFlags & AVIIF_KEYFRAME) ? 0 : ICDECOMPRESS_NOTKEYFRAME;
	m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
	dwFlags |= (m_biInFormat.biSizeImage) ? 0 : ICDECOMPRESS_NULLFRAME;

	if( ICERR_OK != ICDecompress(m_aviIC, dwFlags, &m_biInFormat, (m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8), &m_biOutFormat, m_pDecompressedBuf) ) {
		return	FALSE;
	}

	if (alpha)	{
		// update
		BYTE*	alpha_buf;
		alpha->GetFrame(&alpha_buf);
		u32*	dst		= (u32*)m_pDecompressedBuf;
		u32*	src		= (u32*)alpha_buf;
		u32*	end		= dst+u32(m_dwWidth*m_dwHeight);
		for (; dst!=end; src++,dst++)
		{
			u32&	d	= *dst;
			u32		s	= *src;
			u32		a	= (color_get_R(s)+color_get_G(s)+color_get_B(s))/3;
			d			= subst_alpha	(d,a);
		}
	}

	return	TRUE;
}

/*
GetFrame

РІРѕР·РІСЂР°С‰Р°РµС‚ TRUE РµСЃР»Рё РєР°РґСЂ РёР·РјРµРЅРёР»СЃСЏ, РёРЅР°С‡Рµ FALSE
*/
BOOL CAviPlayerCustom::GetFrame( BYTE **pDest )
{
	R_ASSERT( pDest );

	DWORD	dwCurrFrame;
	dwCurrFrame	= CalcFrame();

//** debug	dwCurrFrame = 112;

	// Р•СЃР»Рё Р·Р°РґР°РЅРЅС‹Р№ РєР°РґСЂ СЂР°РІРµРЅ РїСЂРµРґРёРґСѓС‰РµРјСѓ
	if( dwCurrFrame == m_dwFrameCurrent ) {

		*pDest				= m_pDecompressedBuf;

		return	FALSE;
	} else
	// Р•СЃР»Рё Р·Р°РґР°РЅРЅС‹Р№ РєР°РґСЂ СЌС‚Рѕ РџСЂРµРґРёРґСѓС‰РёР№ РєР°РґСЂ + 1
	if( dwCurrFrame == m_dwFrameCurrent + 1 ) {
	
		++m_dwFrameCurrent;	//	dwCurrFrame == m_dwFrameCurrent + 1

		*pDest	= m_pDecompressedBuf;

		DecompressFrame( m_dwFrameCurrent );
		return	TRUE;
	} else {
		
		// Р­С‚Рѕ РїСЂРѕРёР·РІРѕР»СЊРЅС‹Р№ РєР°РґСЂ

		if( ! (m_pMovieIndex[ dwCurrFrame ].dwFlags & AVIIF_KEYFRAME) ) {

			// Р­С‚Рѕ РќР• РєР»СЋС‡РµРІРѕР№ РєР°РґСЂ -
			// РґРµР»Р°РµРј PreRoll РѕС‚ Р±Р»РёР¶Р°Р№С€РµРіРѕ РїСЂРµРґРёРґСѓС‰РµРіРѕ РєР»СЋС‡РµРІРѕРіРѕ РєР°РґСЂР° РґРѕ Р—Р°РґР°РЅРЅРѕРіРѕ-1
			PreRoll( dwCurrFrame );
		}			

		m_dwFrameCurrent	= dwCurrFrame;
		
		*pDest	= m_pDecompressedBuf;

		// Р”РµРєРѕРјРїСЂРµСЃСЃРёРј Р·Р°РґР°РЅРЅС‹Р№ РєР°РґСЂ
		DecompressFrame( m_dwFrameCurrent );
		return	TRUE;
	}
}

// РјРёРЅРёРјСѓРј РїСЂРѕРІРµСЂРѕРє РЅР° РІР°Р»РёРґРЅРѕСЃС‚СЊ РїРµСЂРµРґР°РЅРЅРѕРіРѕ РґР»СЏ РїСЂРµСЂРѕР»Р»Р° РєР°РґСЂР° - РЅСѓР¶РЅР° СЃРєРѕСЂРѕСЃС‚СЊ
VOID CAviPlayerCustom::PreRoll( DWORD dwFrameNum )
{
	int i;

	AVIINDEXENTRY	*pCurrFrameIndex;
	DWORD		res;

	// РЅР°С…РѕРґРёРј РІ РјР°СЃСЃРёРІРµ РёРЅРґРµРєСЃРѕРІ РїРµСЂРІС‹Р№ РїСЂРµРґС€РµСЃС‚РІСѓСЋС‰РёР№ РµРјСѓ РєР»СЋС‡РµРІРѕР№ РєР°РґСЂ
	// РёР»Рё Р±РµСЂРµРј РєР°РґСЂ, РєРѕСЂСЂРµРєС‚РЅРѕ СЂР°СЃР¶Р°С‚С‹Р№ РґРѕ СЌС‚РѕРіРѕ
	for( i=(int)dwFrameNum-1 ; i>0 ; i-- ) {

		if( m_pMovieIndex[ i ].dwFlags & AVIIF_KEYFRAME )	break;

		if( (int)m_dwFrameCurrent == i ) {

			// РЅР°Рј СЂР°РЅСЊС€Рµ РІСЃС‚СЂРµС‚РёР»СЃСЏ СЂР°СЃР¶Р°С‚С‹Р№ РїРµСЂРµРґ СЌС‚РёРј РєР°РґСЂ:
			// РґРµРєРѕРјРїСЂРµСЃСЃРёРј РІСЃРµ РїРѕСЃР»РµРґСѓСЋС‰РёРµ РќР•РєР»СЋС‡РµРІС‹Рµ РєР°РґСЂС‹ СЃ С„Р»Р°РіР°РјРё PREROLL & NOTKEYFRAME
			for( i++ ; i<(int)dwFrameNum ; i++ ) {

				pCurrFrameIndex = &m_pMovieIndex[ i ];

				DWORD	dwFlags;
				dwFlags = ICDECOMPRESS_PREROLL | ICDECOMPRESS_NOTKEYFRAME | ICDECOMPRESS_HURRYUP;
				m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
				dwFlags |= (m_biInFormat.biSizeImage) ? 0 : ICDECOMPRESS_NULLFRAME;

				res = ICDecompress(m_aviIC, dwFlags, &m_biInFormat, (m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8) /*m_pCompressedBuf*/, &m_biOutFormat, m_pDecompressedBuf);
				if( ICERR_OK != res && ICERR_DONTDRAW != res ) {	// РїСЂРѕРІРµСЂРєР° РЅР° ICERR_DONTDRAW РІРІРµРґРµРЅР° РёР·-Р·Р° indeo 5.11

					R_ASSERT( 0 );
				}

			} // for(...
		
			return;
		}	// if( (int)m_dwFrameCurrent == i )...
	}	// for( i=(int)dwFrameNum-1 ; i>0 ; i-- )...


	// РїРѕР»СѓС‡Р°РµРј СЌР»РµРјРµРЅС‚ РёРЅРґРµРєСЃР°
	pCurrFrameIndex = &m_pMovieIndex[ i ];
	m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
	R_ASSERT( m_biInFormat.biSizeImage );

	// РґРµРєРѕРјРїСЂРµСЃСЃРёРј РєР»СЋС‡РµРІРѕР№ РєР°РґСЂ СЃ С„Р»Р°РіРѕРј ICDECOMPRESS_PREROLL 
	res = ICDecompress(m_aviIC, ICDECOMPRESS_PREROLL | ICDECOMPRESS_HURRYUP, &m_biInFormat, m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8/*m_pCompressedBuf*/, &m_biOutFormat, m_pDecompressedBuf);
	if( ICERR_OK != res && ICERR_DONTDRAW != res ) {

		R_ASSERT( 0 );
	}

	// РґРµРєРѕРјРїСЂРµСЃСЃРёРј РІСЃРµ РїРѕСЃР»РµРґСѓСЋС‰РёРµ РќР•РєР»СЋС‡РµРІС‹Рµ РєР°РґСЂС‹ СЃ С„Р»Р°РіР°РјРё PREROLL & NOTKEYFRAME
	for( i++ ; i<(int)dwFrameNum ; i++ ) {

		pCurrFrameIndex = &m_pMovieIndex[ i ];

		DWORD	dwFlags;
		dwFlags = ICDECOMPRESS_PREROLL | ICDECOMPRESS_NOTKEYFRAME | ICDECOMPRESS_HURRYUP;
		m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
		dwFlags |= (m_biInFormat.biSizeImage) ? 0 : ICDECOMPRESS_NULLFRAME;

		res = ICDecompress(m_aviIC, dwFlags, &m_biInFormat, (m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8) /*m_pCompressedBuf*/, &m_biOutFormat, m_pDecompressedBuf);
		if( ICERR_OK != res && ICERR_DONTDRAW != res ) {	// РїСЂРѕРІРµСЂРєР° РЅР° ICERR_DONTDRAW РІРІРµРґРµРЅР° РёР·-Р·Р° indeo 5.11

			R_ASSERT( 0 );
		}
	} // for(...

}

void CAviPlayerCustom::GetSize(DWORD *dwWidth, DWORD *dwHeight)
{
	if( dwWidth )	*dwWidth = m_dwWidth;
	if( dwHeight )	*dwHeight = m_dwHeight;
}

int CAviPlayerCustom::SetSpeed( INT nPercent )
{
	m_fCurrentRate	= m_fRate * FLOAT( nPercent / 100.0f );
	return int(m_fCurrentRate / m_fRate * 100);
}

DWORD CAviPlayerCustom::CalcFrame()
	{	
		if(!m_dwFirstFrameOffset)
			m_dwFirstFrameOffset = RDEVICE.dwTimeContinual-1;

	return DWORD( floor( (RDEVICE.dwTimeContinual-m_dwFirstFrameOffset) * m_fCurrentRate / 1000.0f) ) % m_dwFrameTotal;
}
