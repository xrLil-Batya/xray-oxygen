#include "UICustomSpin.h"

class UI_API CUISpinText : public CUICustomSpin
{
public:
					CUISpinText			();
	// CUIOptionsItem
	virtual void	SetCurrentOptValue	();	// opt->current
	virtual void	SaveBackUpOptValue	();	// current->backup
	virtual void	SaveOptValue		();	// current->opt
	virtual void	UndoOptValue		();	// backup->current
	virtual bool	IsChangedOptValue	() const;	// backup!=current

	// own
	virtual void	OnBtnUpClick();
	virtual void	OnBtnDownClick();

			void	AddItem_(const char* item, int id);
			LPCSTR	GetTokenText();
protected:
	virtual bool	CanPressUp		();
	virtual bool	CanPressDown	();
	virtual void	IncVal			(){};
	virtual void	DecVal			(){};
			void	SetItem			(int v);
			
	struct SInfo
	{
		shared_str	_orig;
		shared_str	_transl;
		int			_id;
	};
			
protected:
	using Items = xr_vector<SInfo>;
	using Items_it = Items::iterator;

    Items	m_list;
	int		m_curItem;
	int		m_opt_backup_value;
};