#ifndef xr_iniH
#define xr_iniH

#include "fastdelegate.h"
#include "string"

// refs
class	CInifile;
struct	xr_token;



class XRCORE_API CInifile
{
public:
	struct XRCORE_API	Item
	{
		shared_str	first;
		shared_str	second;
		Item() : first(0), second(0)
		{};
	};
	typedef xr_vector<Item>				Items;
	typedef Items::const_iterator		SectCIt;
	typedef Items::iterator				SectIt_;
    struct XRCORE_API	Sect {
		shared_str		Name;
		Items			Data;

		BOOL			line_exist	(const char* L, const char** val=0);
	};
	typedef	xr_vector<Sect*>		Root;
	typedef Root::iterator			RootIt;
	typedef Root::const_iterator	RootCIt;
	
#ifndef _EDITOR
	typedef fastdelegate::FastDelegate1<const char*, bool>	allow_include_func_t;
#endif
	static CInifile*	Create		( const char* szFileName, BOOL ReadOnly=TRUE);
	static void			Destroy		( CInifile*);
    static IC BOOL		IsBOOL		( const char* B)	{ return (xr_strcmp(B,"on")==0 || xr_strcmp(B,"yes")==0 || xr_strcmp(B,"true")==0 || xr_strcmp(B,"1")==0);}
private:
	enum{eSaveAtEnd = (1<<0), eReadOnly= (1<<1), eOverrideNames=(1<<2),};
	Flags8			m_flags;
	string_path		m_file_name;
	Root			DATA;
	
	void			Load		(IReader* F, const char* path
                                #ifndef _EDITOR
                                    , allow_include_func_t	allow_include_func = NULL
                                #endif
                                );
public:
				CInifile		( IReader* F,
								   const char* path=0
                                #ifndef _EDITOR
								   ,allow_include_func_t allow_include_func = NULL
                                #endif
                                    );

				CInifile		( const char* szFileName,
								  BOOL ReadOnly=TRUE,
								  BOOL bLoadAtStart=TRUE,
								  BOOL SaveAtEnd=TRUE,
								  u32 sect_count=0
                                #ifndef _EDITOR
								   ,allow_include_func_t allow_include_func = NULL
                                #endif
                                    );

	virtual 	~CInifile		( );
    bool		save_as         ( const char* new_fname=0 );
	void		save_as			(IWriter& writer, bool bcheck=false)const;
	void		set_override_names(BOOL b){m_flags.set(eOverrideNames,b);}
	void		save_at_end		(BOOL b){m_flags.set(eSaveAtEnd,b);}
	const char*		fname			( ) const { return m_file_name; };

	Sect&		r_section		( const char* S			)const;
	Sect&		r_section		( const shared_str& S	)const;
	Sect&		r_sectionStd	(const std::string& S)const;
	BOOL		line_exist		( const char* S, const char* L )const;
	BOOL		line_exist      (const shared_str& S, const shared_str& L)const;
	BOOL		line_existStd	(const std::string& S, const std::string& L)const;
	u32			line_count		( const char* S			)const;
	u32			line_count		( const shared_str& S	)const;
	u32			line_countStd	(const std::string& S)const;
	u32			section_count	( )const;
	BOOL		section_exist	( const char* S			)const;
	BOOL		section_exist	( const shared_str& S	)const;
	BOOL		section_existStd(const std::string& S)const;
	Root&		sections		( ){return DATA;}
	Root const&	sections		( ) const {return DATA;}

	CLASS_ID	r_clsid			( const char* S, const char* L )const;
	CLASS_ID	r_clsid			( const shared_str& S, const char* L )const				{ return r_clsid(*S,L);			}
	CLASS_ID	r_clsidStd		(const std::string& S, const char* L)const				{ return r_clsid(S.c_str(), L); }
	const char* 		r_string		( const char* S, const char* L)const;															// оставляет кавычки
	const char* 		r_string		( const shared_str& S, const char* L)const				{ return r_string(*S,L);		}	// оставляет кавычки
	const char* 		r_stringStd		(const std::string& S, const char* L)const { return r_string(S.c_str(), L); }	// оставляет кавычки
	shared_str	r_string_wb		(const char* S, const char* L)const;
	std::string	r_string_wbStd  (const char* S, const char* L)const;// убирает кавычки
	shared_str	r_string_wb		( const shared_str& S, const char* L)const				{ return r_string_wb(*S,L);		}	// убирает кавычки
	std::string	r_string_wbStd	(const std::string& S, const char* L)const { return r_string_wbStd(S.c_str(), L); }	// убирает кавычки
	u8	 		r_u8			( const char* S, const char* L ) const;
	u8	 		r_u8			( const shared_str& S, const char* L )const				{ return r_u8(*S,L);			}
	u8	 		r_u8Std			(const std::string& S, const char* L)const { return r_u8(S.c_str(), L); }
	u16	 		r_u16			( const char* S, const char* L )const;
	u16	 		r_u16			( const shared_str& S, const char* L )const				{ return r_u16(*S,L);			}
	u16	 		r_u16Std		(const std::string& S, const char* L)const { return r_u16(S.c_str(), L); }
	u32	 		r_u32			( const char* S, const char* L )const;
	u32	 		r_u32			(const shared_str& S, const char* L)const { return r_u32(*S, L);						}
	u32	 		r_u32Std		(const std::string S, const char* L)const { return r_u32(S.c_str(), L); }
	u64	 		r_u64			( const char* S, const char* L )const;
	s8	 		r_s8			( const char* S, const char* L )const;
	s8	 		r_s8			( const shared_str& S, const char* L )const				{ return r_s8(*S,L);			}
	s8	 		r_s8Std			(const std::string& S, const char* L)const { return r_s8(S.c_str(), L); }   
	s16	 		r_s16			( const char* S, const char* L )const;
	s16	 		r_s16			( const shared_str& S, const char* L )const				{ return r_s16(*S,L);			}
	s16	 		r_s16Std		(const std::string& S, const char* L)const { return r_s16(S.c_str(), L);}
	s32	 		r_s32			( const char* S, const char* L )const;
	s32	 		r_s32			( const shared_str& S, const char* L )const				{ return r_s32(*S,L);			}
	s32	 		r_s32Std		(const std::string& S, const char* L)const { return r_s32(S.c_str(), L); }
	s64	 		r_s64			( const char* S, const char* L )const;
	float		r_float			( const char* S, const char* L )const;
	float		r_float			( const shared_str& S, const char* L )const				{ return r_float(*S,L);			}
	float		r_floatStd		(const std::string& S, const char* L)const { return r_float(S.c_str(), L); }
	Fcolor		r_fcolor		( const char* S, const char* L )const;
	Fcolor		r_fcolor		( const shared_str& S, const char* L )const				{ return r_fcolor(*S,L);		}	
	Fcolor		r_fcolorStd		(const std::string& S, const char* L)const { return r_fcolor(S.c_str(), L); }
	u32			r_color			( const char* S, const char* L )const;
	u32			r_color			( const shared_str& S, const char* L )const				{ return r_color(*S,L);			}	
	u32			r_color			(const std::string& S, const char* L)const { return r_color(S.c_str(), L); }
	Ivector2	r_ivector2		( const char* S, const char* L )const;
	Ivector2	r_ivector2		( const shared_str& S, const char* L )const				{ return r_ivector2(*S,L);		}	
	Ivector2	r_ivector2Std	(const std::string& S, const char* L)const { return r_ivector2(S.c_str(), L); }
	Ivector3	r_ivector3		( const char* S, const char* L )const;
	Ivector3	r_ivector3		( const shared_str& S, const char* L )const				{ return r_ivector3(*S,L);		}   //////////-t
	Ivector3	r_ivector3Std	(const std::string& S, const char* L)const { return r_ivector3(S.c_str(), L); }
	Ivector4	r_ivector4		( const char* S, const char* L )const;
	Ivector4	r_ivector4		( const shared_str& S, const char* L )const				{ return r_ivector4(*S,L);		}
	Ivector4	r_ivector4Std	(const std::string& S, const char* L)const { return r_ivector4(S.c_str(), L); }
	Fvector2	r_fvector2		( const char* S, const char* L )const;
	Fvector2	r_fvector2		( const shared_str& S, const char* L )const				{ return r_fvector2(*S,L);		}
	Fvector2	r_fvector2Std	(const std::string& S, const char* L)const { return r_fvector2(S.c_str(), L); }
	Fvector3	r_fvector3		( const char* S, const char* L )const;
	Fvector3	r_fvector3		( const shared_str& S, const char* L )const				{ return r_fvector3(*S,L);		}
	Fvector3	r_fvector3Std	(const std::string& S, const char* L)const { return r_fvector3(S.c_str(), L); }
	Fvector4	r_fvector4		( const char* S, const char* L )const;
	Fvector4	r_fvector4		( const shared_str& S, const char* L )const				{ return r_fvector4(*S,L);		}
	Fvector4	r_fvector4		(const std::string& S, const char* L)const { return r_fvector4(S.c_str(), L); }
	BOOL		r_bool			( const char* S, const char* L )const;
	BOOL		r_bool			( const shared_str& S, const char* L )const				{ return r_bool(*S,L);			}
	BOOL		r_bool			(const std::string& S, const char* L)const { return r_bool(S.c_str(), L); }
	int			r_token			( const char* S, const char* L,	const xr_token *token_list)const;
	BOOL		r_line			( const char* S, int L,	const char** N, const char** V )const;
	BOOL		r_line			( const shared_str& S, int L,	const char** N, const char** V )const;
	BOOL		r_lineStd		(const std::string& S, int L, const char** N, const char** V)const;

    void		w_string		( const char* S, const char* L, const char*			V, const char* comment=0 );
	void		w_u8			( const char* S, const char* L, u8				V, const char* comment=0 );
	void		w_u16			( const char* S, const char* L, u16				V, const char* comment=0 );
	void		w_u32			( const char* S, const char* L, u32				V, const char* comment=0 );
	void		w_u64			( const char* S, const char* L, u64				V, const char* comment=0 );
	void		w_s64			( const char* S, const char* L, s64				V, const char* comment=0 );
    void		w_s8			( const char* S, const char* L, s8				V, const char* comment=0 );
	void		w_s16			( const char* S, const char* L, s16				V, const char* comment=0 );
	void		w_s32			( const char* S, const char* L, s32				V, const char* comment=0 );
	void		w_float			( const char* S, const char* L, float				V, const char* comment=0 );
    void		w_fcolor		( const char* S, const char* L, const Fcolor&		V, const char* comment=0 );
    void		w_color			( const char* S, const char* L, u32				V, const char* comment=0 );
    void		w_ivector2		( const char* S, const char* L, const Ivector2&	V, const char* comment=0 );
	void		w_ivector3		( const char* S, const char* L, const Ivector3&	V, const char* comment=0 );
	void		w_ivector4		( const char* S, const char* L, const Ivector4&	V, const char* comment=0 );
	void		w_fvector2		( const char* S, const char* L, const Fvector2&	V, const char* comment=0 );
	void		w_fvector3		( const char* S, const char* L, const Fvector3&	V, const char* comment=0 );
	void		w_fvector4		( const char* S, const char* L, const Fvector4&	V, const char* comment=0 );
	void		w_bool			( const char* S, const char* L, BOOL				V, const char* comment=0 );

    void		remove_line		( const char* S, const char* L );
};

// Main configuration file
extern XRCORE_API CInifile const * pSettings;
//extern XRCORE_API CInifile const * pSettingsAuth;

#endif //__XR_INI_H__
