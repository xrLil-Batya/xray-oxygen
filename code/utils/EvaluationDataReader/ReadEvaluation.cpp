#include "ReadEvaluation.h"
#include "../../engine.vc2008/xrCore/xrCore.h"
#include <sstream>
#include <fstream>

CReadEvaluation::CReadEvaluation()
{
}

CReadEvaluation::~CReadEvaluation()
{
}

void CReadEvaluation::Load(const char* FileName)
{
	string_path caPath;
	if (!FS.exist(caPath, "$game_ai$", FileName))
	{
		Msg("! Evaluation function : File not found \"%s\"", caPath);
		R_ASSERT(false);
		return;
	}

	IReader* F = FS.r_open(caPath);
	F->r(&m_tEFHeader, sizeof(SEFHeader));

	F->r(&m_dwVariableCount, sizeof(m_dwVariableCount));
	m_dwaAtomicFeatureRange = xr_alloc<u32>(m_dwVariableCount);
	std::memset(m_dwaAtomicFeatureRange, 0, m_dwVariableCount * sizeof(u32));

	for (u32 i = 0; i < m_dwVariableCount; ++i)
		F->r(m_dwaAtomicFeatureRange + i, sizeof(u32));

	m_dwaVariableTypes = xr_alloc<u32>(m_dwVariableCount);
	F->r(m_dwaVariableTypes, m_dwVariableCount * sizeof(u32));
	F->r(&m_dwFunctionType, sizeof(u32));
	m_fMinResultValue = F->r_float();
	m_fMaxResultValue = F->r_float();

	F->r(&m_dwPatternCount, sizeof(m_dwPatternCount));

	m_tpPatterns = xr_alloc<SPattern>(m_dwPatternCount);
	for (u32 i = 0; i < m_dwPatternCount; ++i) 
	{
		F->r(&(m_tpPatterns[i].dwCardinality), sizeof(m_tpPatterns[i].dwCardinality));
		m_tpPatterns[i].dwaVariableIndexes = xr_alloc<u32>(m_tpPatterns[i].dwCardinality);
		F->r(m_tpPatterns[i].dwaVariableIndexes, m_tpPatterns[i].dwCardinality * sizeof(u32));

		u32			m_dwComplexity = 1;
		for (int j = 0; j < (int)m_tpPatterns[i].dwCardinality; ++j)
			m_dwComplexity *= m_dwaAtomicFeatureRange[m_tpPatterns[i].dwaVariableIndexes[j]];
		m_dwParameterCount += m_dwComplexity;
	}

	m_faParameters = xr_alloc<float>(m_dwParameterCount);
	F->r(m_faParameters, m_dwParameterCount * sizeof(float));
	FS.r_close(F);
}

void CReadEvaluation::MakeOutFile(const char* FileName, const char* OutFileName)
{
	Load(FileName);

	std::stringstream HeaderString;
	HeaderString << "[header]" << std::endl << "version = " << m_tEFHeader.dwBuilderVersion << std::endl 
							   << "format = " << m_tEFHeader.dwDataFormat << std::endl << std::endl;

	HeaderString << "[pattern]" << std::endl << "count = " << m_dwPatternCount << std::endl;
	for (u32 it = 0; it < m_dwPatternCount; it++)
	{
		HeaderString << xr_string("cardinality_") + std::to_string(it).c_str() + " = " << m_tpPatterns[it].dwCardinality << std::endl;
		for(u32 iter = 0; iter < m_tpPatterns[it].dwCardinality; iter++)
			HeaderString << xr_string("indexes_") + std::to_string(iter).c_str() + " = " << m_tpPatterns[it].dwaVariableIndexes[iter] << std::endl;
	}

	for (u32 it = 0; it < m_dwParameterCount; it++)
	{
		HeaderString << xr_string("param_") + std::to_string(it).c_str() + " = " << m_faParameters[it] << std::endl;
	}

	HeaderString << std::endl << "[evaluation]" << std::endl << "atomic_count = " << m_dwVariableCount << std::endl;
	for(u32 Iter = 0; Iter < m_dwVariableCount; Iter++)
	{ 
		HeaderString << xr_string("atomic_") + std::to_string(Iter).c_str() + " = " << m_dwaAtomicFeatureRange[Iter] << std::endl;
		HeaderString << xr_string("atomic_type_") + std::to_string(Iter).c_str() + " = " << m_dwaVariableTypes[Iter] << std::endl;
	}

	HeaderString << "atomic_func_type = " << m_dwFunctionType << std::endl;
	HeaderString << "max_result = " << m_fMaxResultValue << std::endl;
	HeaderString << "min_result = " << m_fMinResultValue << std::endl;

	std::ofstream Writter(OutFileName);
	Writter.write(HeaderString.str().c_str(), HeaderString.str().size());
	Writter.close();
}
