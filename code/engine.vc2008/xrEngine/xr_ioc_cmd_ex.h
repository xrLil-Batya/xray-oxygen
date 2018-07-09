class ENGINE_API CCC_U32 : public IConsole_Command
{
protected:
	unsigned*	value;
	unsigned	min, max;
public:
	const unsigned	GetValue() const { return *value; };
	void			GetBounds(u32& imin, u32& imax) const { imin = min; imax = max; }

					CCC_U32(const char* N, unsigned* V, unsigned _min = 0, unsigned _max = 999) :
					IConsole_Command(N), value(V), min(_min), max(_max) {};
	
	virtual void	Execute(const char* args)
	{
		int v = atoi(args);
		if (v<min || v>max) InvalidSyntax();
		else *value = v;
	}

	virtual void	Status(TStatus& S)
	{	
		itoa(*value,S,10);
	}

	virtual void Info(TInfo& I)
	{	
		xr_sprintf(I,sizeof(I),"integer value in range [%d,%d]",min,max);
	}

	virtual void fill_tips(vecTips& tips, u32 mode)
	{
		TStatus  str;
		xr_sprintf( str, sizeof(str), "%d  (current)  [%d,%d]", *value, min, max );
		tips.push_back( str );
 		IConsole_Command::fill_tips( tips, mode );
	}
};