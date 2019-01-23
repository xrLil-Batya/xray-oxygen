#pragma once
#include <string>

// refs
struct	xr_token;

class   CInifile;
struct  xr_token;


class XRCORE_API CInifile 
{
public:
	typedef std::pair<shared_str, shared_str> Item;
	struct XRCORE_API Sect 
	{
		shared_str Name;
		xr_hash_map<shared_str, shared_str> Data;
		xr_vector<Item> Unordered;
		bool line_exist(const char*, const char** = 0);
	};
	typedef xr_hash_map<shared_str, Sect*> Root;

	// factorisation
	static CInifile* Create(const char*, bool = TRUE);
	static void      Destroy(CInifile*);

	static IC bool   IsBOOL(const char* B)
	{
		return (xr_strcmp(B, "on") == 0 || xr_strcmp(B, "yes") == 0 || xr_strcmp(B, "true") == 0 || xr_strcmp(B, "1") == 0);
	}

private:
	char* fName;
	Root  DATA;
	void  Load(IReader*, const char*);

public:
	bool bReadOnly;
	bool bSaveAtEnd;

public:
	CInifile(IReader*, const char* = 0);
	CInifile(const char*, bool = true, bool = true, bool = true);
	virtual ~CInifile();

	bool save_as(const char* = 0);
	void save_as(IWriter& writer, bool bcheck = false) const;
	xr_string get_as_string();

	const char* fname() { return fName; };

	Sect& r_section(const char*) const;
	Sect& r_section(const shared_str&) const;
	bool  line_exist(const char*, const char*);
	bool  line_exist(const shared_str&, const shared_str&);
	u32   line_count(const char*);
	u32   line_count(const shared_str&);
	bool  section_exist(const char*);
	bool  section_exist(const shared_str&);
	Root& sections() { return DATA; }
	const Root& sections() const { return DATA; }

	CLASS_ID r_clsid(const char*, const char*);
	CLASS_ID r_clsid(const shared_str& S, const char* L) {
		return r_clsid(S.c_str(), L);
	}

	const char*		r_string(const char*, const char*) const;
	const char*		r_string(const shared_str& S, const char* L) const { return r_string(S.c_str(), L); }

	shared_str	r_string_wb(const char*, const char*) const;
	shared_str	r_string_wb(const shared_str& S, const char* L) const { return r_string_wb(S.c_str(), L); }

	u8			r_u8(const char*, const char*)const;
	u8			r_u8(const shared_str& S, const char* L) const { return r_u8(S.c_str(), L); }
	u16			r_u16(const char*, const char*)const;
	u16			r_u16(const shared_str& S, const char* L) const { return r_u16(S.c_str(), L); }
	u32			r_u32(const char*, const char*)const;
	u32			r_u32(const shared_str& S, const char* L) const { return r_u32(S.c_str(), L); }
	u64			r_u64(const char*, const char*)const;
	u64			r_u64(const shared_str& S, const char* L) const { return r_u64(S.c_str(), L); }

	s8			r_s8(const char*, const char*) const;
	s8			r_s8(const shared_str& S, const char* L) const { return r_s8(S.c_str(), L); }
	s16			r_s16(const char*, const char*) const;
	s16			r_s16(const shared_str& S, const char* L) const { return r_s16(S.c_str(), L); }
	s32			r_s32(const char*, const char*) const;
	s32			r_s32(const shared_str& S, const char* L) const { return r_s32(S.c_str(), L); }

	float		r_float(const char*, const char*) const;
	float		r_float(const shared_str& S, const char* L) const { return r_float(S.c_str(), L); }

	Fcolor		r_fcolor(const char*, const char*)const;
	Fcolor		r_fcolor(const shared_str& S, const char* L) const { return r_fcolor(S.c_str(), L); }

	u32			r_color(const char*, const char*)const;
	u32			r_color(const shared_str& S, const char* L) const { return r_color(S.c_str(), L); }

	Ivector2	r_ivector2(const char*, const char*)const;
	Ivector2	r_ivector2(const shared_str& S, const char* L) const { return r_ivector2(S.c_str(), L); }

	Ivector3	r_ivector3(const char*, const char*)const;
	Ivector3	r_ivector3(const shared_str& S, const char* L) const { return r_ivector3(S.c_str(), L); }

	Ivector4	r_ivector4(const char*, const char*)const;
	Ivector4	r_ivector4(const shared_str& S, const char* L)const { return r_ivector4(S.c_str(), L); }

	Fvector2	r_fvector2(const char*, const char*)const;
	Fvector2	r_fvector2(const shared_str& S, const char* L) const { return r_fvector2(S.c_str(), L); }

	Fvector3	r_fvector3(const char*, const char*)const;
	Fvector3	r_fvector3(const shared_str& S, const char* L) const { return r_fvector3(S.c_str(), L); }

	Fvector4	r_fvector4(const char*, const char*)const;
	Fvector4	r_fvector4(const shared_str& S, const char* L) const { return r_fvector4(S.c_str(), L); }

	bool		r_bool(const char*, const char*)const;
	bool		r_bool(const shared_str& S, const char* L) const { return r_bool(S.c_str(), L); }

	int			r_token(const char*, const char*, const xr_token *token_list);
	bool		r_line(const char*, int, const char**, const char**);
	bool		r_line(const shared_str&, int, const char**, const char**);

	void		w_string(const char*, const char*, const char*);
	void		w_u8(const char*, const char*, u8);
	void		w_u16(const char*, const char*, u16);
	void		w_u32(const char*, const char*, u32);
	void		w_u64(const char*, const char*, u64);
	void		w_s8(const char*, const char*, s8);
	void		w_s16(const char*, const char*, s16);
	void		w_s32(const char*, const char*, s32);
	void		w_float(const char*, const char*, float);
	void		w_fcolor(const char*, const char*, const Fcolor&);
	void		w_color(const char*, const char*, u32);
	void		w_ivector2(const char*, const char*, const Ivector2&);
	void		w_ivector3(const char*, const char*, const Ivector3&);
	void		w_ivector4(const char*, const char*, const Ivector4&);
	void		w_fvector2(const char*, const char*, const Fvector2&);
	void		w_fvector3(const char*, const char*, const Fvector3&);
	void		w_fvector4(const char*, const char*, const Fvector4&);
	void		w_bool(const char*, const char*, bool);

	void		remove_line(const char*, const char*);
};

// Main configuration file
extern XRCORE_API CInifile *pSettings;