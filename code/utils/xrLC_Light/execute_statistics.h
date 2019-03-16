////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EXECUTE_STATISTICS_H_INCLUDED
#define EXECUTE_STATISTICS_H_INCLUDED
#ifdef	COLLECT_EXECUTION_STATS
class IReader;
class IWriter;
class execute_time_statistics 
{
public:
	execute_time_statistics		(): m_time(-1.f){}
	float	m_time;
	
	void	read				( IReader	&r );
	void	write				( IWriter	&w ) const ;
	void	log					()const;

private:

}; // class execute_statistics
class execute_statistics
{
public:
	execute_time_statistics		time_stats;
	string_path					dir;
	void	read				( IReader	&r );
	void	write				( IWriter	&w ) const ;
	void	log					()const;
};

#endif
#endif // #ifndef EXECUTE_STATISTICS_H_INCLUDED
