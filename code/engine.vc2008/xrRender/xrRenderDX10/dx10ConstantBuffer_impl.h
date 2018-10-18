#pragma once

IC Fvector4* dx10ConstantBuffer::Access(u16 offset)
{
	//	TODO: DX10: Implement code which will check if set actually changes code.
	m_bChanged = true;

	//	Check buffer size in client code: don't know if actual data will cross
	//	buffer boundaries.
	VERIFY(offset<(int)m_uiBufferSize);
	BYTE*	res = ((BYTE*)m_pBufferData) + offset;	
	return (Fvector4*) res;
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, const Fmatrix& A)
{
	VERIFY		(RC_float == C->type);

	Fvector4*	it	= Access(L.index);
	switch		(L.cls)
	{
	case RC_2x4:
		VERIFY( u32((u32)L.index+2*lineSize) <= m_uiBufferSize );
		it[0].set			(A._11, A._21, A._31, A._41);
		it[1].set			(A._12, A._22, A._32, A._42);
		break;
	case RC_3x4:
		VERIFY( u32((u32)L.index+3*lineSize) <= m_uiBufferSize );
		it[0].set			(A._11, A._21, A._31, A._41);
		it[1].set			(A._12, A._22, A._32, A._42);
		it[2].set			(A._13, A._23, A._33, A._43);
		break;
	case RC_4x4:
		VERIFY( u32((u32)L.index+4*lineSize) <= m_uiBufferSize );
		it[0].set			(A._11, A._21, A._31, A._41);
		it[1].set			(A._12, A._22, A._32, A._42);
		it[2].set			(A._13, A._23, A._33, A._43);
		it[3].set			(A._14, A._24, A._34, A._44);
		break;
	default:
#ifdef DEBUG
		Debug.fatal		(DEBUG_INFO,"Invalid constant run-time-type for '%s'",*C->name);
#else
		NODEFAULT;
#endif
	}
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, const Fvector4& A)
{
	VERIFY		(RC_float	== C->type);
	VERIFY		(RC_1x4		== L.cls || RC_1x3 == L.cls || RC_1x2 == L.cls);

	VERIFY( u32((u32)L.index+lineSize) <= m_uiBufferSize );
	float*		it		= (float*)Access(L.index);

	size_t		count	= 4;
	switch(L.cls)
	{
		case RC_1x2:
			count = 2;
			break;
		case RC_1x3:
			count = 3;
			break;
		case RC_1x4:
			count = 4;
			break;
		default:
			break;
	}

    std::memcpy(it, &A[0], count*sizeof(float));
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, float A)
{
	VERIFY		(RC_float	== C->type);
	VERIFY		(RC_1x1		== L.cls);
	float*		it	= (float*)Access(L.index);
	VERIFY( u32((u32)L.index+sizeof(float)) <= m_uiBufferSize );
	*it			= A;
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, int A)
{
	VERIFY		(RC_int		== C->type);
	VERIFY		(RC_1x1		== L.cls);
	int*		it	= (int*)Access(L.index);
	VERIFY( u32((u32)L.index+sizeof(int)) <= m_uiBufferSize );
	*it			= A;
}

IC void dx10ConstantBuffer::seta(R_constant* C, R_constant_load& L, u32 e, const Fmatrix& A)
{
	VERIFY		(RC_float == C->type);
	u32			base;
	Fvector4*	it;
	switch		(L.cls)
	{
	case RC_2x4:
		base				= (u32)L.index + 2*lineSize*e;
		it		= Access((u16)base);
		VERIFY( (base+2*lineSize) <= m_uiBufferSize );
		it[0].set			(A._11, A._21, A._31, A._41);
		it[1].set			(A._12, A._22, A._32, A._42);
		break;
	case RC_3x4:
		base				= (u32)L.index + 3*lineSize*e;
		it		= Access((u16)base);
		VERIFY( (base+3*lineSize) <= m_uiBufferSize );
		it[0].set			(A._11, A._21, A._31, A._41);
		it[1].set			(A._12, A._22, A._32, A._42);
		it[2].set			(A._13, A._23, A._33, A._43);
		break;
	case RC_4x4:
		base				= (u32)L.index + 4*lineSize*e;
		it		= Access((u16)base);
		VERIFY( (base+4*lineSize) <= m_uiBufferSize );
		it[0].set			(A._11, A._21, A._31, A._41);
		it[1].set			(A._12, A._22, A._32, A._42);
		it[2].set			(A._13, A._23, A._33, A._43);
		it[3].set			(A._14, A._24, A._34, A._44);
		break;
	default:
#ifdef DEBUG
		Debug.fatal		(DEBUG_INFO,"Invalid constant run-time-type for '%s'",*C->name);
#else
		NODEFAULT;
#endif
	}
}

IC void dx10ConstantBuffer::seta(R_constant* C, R_constant_load& L, u32 e, const Fvector4& A)
{
	VERIFY		(RC_float	== C->type);
	VERIFY		(RC_1x4		== L.cls || RC_1x3 == L.cls || RC_1x2 == L.cls);

	u32			base			= (u32)L.index + lineSize*e;
	Fvector4*	it				= Access((u16)base);
	VERIFY( (base+lineSize) <= m_uiBufferSize );
	it->set	(A);
}

IC void* dx10ConstantBuffer::AccessDirect(R_constant_load& L, u32 DataSize)
{
	//	Check buffer size in client code: don't know if actual data will cross
	//	buffer boundaries.
	VERIFY(L.index<(int)m_uiBufferSize);
	BYTE* res = ((BYTE*)m_pBufferData) + L.index;

	if ((u32)L.index+DataSize <= m_uiBufferSize)
	{
		m_bChanged = true;
		return res;
	}
	else return 0;
}