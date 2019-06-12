#pragma once

typedef struct tagSPattern 
{
	unsigned int  dwCardinality;
	unsigned int* dwaVariableIndexes;
} SPattern;

typedef struct tagSEFHeader 
{
	unsigned int dwBuilderVersion;
	unsigned int dwDataFormat;
} SEFHeader;

class CReadEvaluation
{
	unsigned int* m_dwaAtomicFeatureRange;
	unsigned int* m_dwaPatternIndexes;
	unsigned int* m_dwaVariableTypes;

	unsigned int	m_dwVariableCount;
	float*			m_faParameters;
	unsigned int	m_dwPatternCount;
	unsigned int	m_dwParameterCount;
	unsigned int	m_dwFunctionType;


	float			m_fMinResultValue;
	float			m_fMaxResultValue;

	SEFHeader		m_tEFHeader;
	SPattern*		m_tpPatterns;

public:
	CReadEvaluation();
	~CReadEvaluation();

	void Load(const char* FileName);
	void MakeOutFile(const char* FileName, const char* OutFileName);
};

