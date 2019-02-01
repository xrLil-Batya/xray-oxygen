#pragma once

typedef	void		crashhandler		(void);
typedef	void		on_dialog			(bool before);

extern XRCORE_API DWORD gMainThreadId;
extern XRCORE_API DWORD gSecondaryThreadId;
XRCORE_API bool IsMainThread();
XRCORE_API bool IsSecondaryThread();

class XRCORE_API	xrDebug
{
private:
	crashhandler*	handler	;
	on_dialog*		m_on_dialog;
    DWORD           m_mainThreadId = 0;
	xrCriticalSection Lock;

public:
	void			_initialize();
    void            _initializeAfterFS();
	void			_destroy();
	
public:
	crashhandler*	get_crashhandler	()							{ return handler;	};
	void			set_crashhandler	(crashhandler* _handler)	{ handler=_handler;	};

	on_dialog*		get_on_dialog		()							{ return m_on_dialog;	}
	void			set_on_dialog		(on_dialog* on_dialog)		{ m_on_dialog = on_dialog;	}

	const char*		error2string		(long  code	);

	void			gather_info			(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, char* assertion_info, unsigned int assertion_info_size);
	template <size_t count>
	inline void		gather_info			(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, char (&assertion_info)[count])
	{
		gather_info	( expression, description, argument0, argument1, file, line, function, assertion_info, count);
	}

	void			fail				(const char *e1, const char *file, int line, const char *function, bool &ignore_always);
	void			fail				(const char *e1, const char *e2, const char *file, int line, const char *function, bool &ignore_always);
	void			fail				(const char *e1, const char *e2, const char *e3, const char *file, int line, const char *function, bool &ignore_always);
	void			fail				(const char *e1, const char *e2, const char *e3, const char *e4, const char *file, int line, const char *function, bool &ignore_always);
	void			error				(long  code, const char* e1, const char *file, int line, const char *function, bool &ignore_always);
	void			error				(long  code, const char* e1, const char* e2, const char *file, int line, const char *function, bool &ignore_always);
	void _cdecl		fatal				(const char *file, int line, const char *function, const char* F,...);
	void			do_exit				(HWND hWnd, LPCSTR message);
    void			do_exit2			(HWND hwnd, const string4096& message, bool& ignore_always);
	void			backend				(const char* reason, const char* expression, const char *argument0, const char *argument1, const char* file, int line, const char *function, bool &ignore_always);
};

LONG WINAPI UnhandledFilter(struct _EXCEPTION_POINTERS* pExceptionInfo);

extern XRCORE_API	xrDebug		Debug;
extern XRCORE_API   HWND		gGameWindow;

#include "xrDebug_macros.h"