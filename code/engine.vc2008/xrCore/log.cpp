#include "stdafx.h"

#include <time.h>
#include <fstream>
#include "resource.h"
#include "log.h"

static xrLogger theLogger;

void Log(const char* s)
{
	theLogger.SimpleMessage(s);
}

void Msg(const char *format, ...)
{
	va_list		mark;
	va_start	(mark, format );
	theLogger.Msg(format, mark);
    va_end		(mark);
}

void Log				(const char *msg, const Fvector &dop) {
	u32			buffer_size = (xr_strlen(msg) + 2 + 3*(64 + 1) + 1) * sizeof(char);
	PSTR buf	= (PSTR)_alloca( buffer_size );

	xr_sprintf	(buf, buffer_size,"%s (%f,%f,%f)",msg, VPUSH(dop) );
	Log			(buf);
}

void Log				(const char *msg, const Fmatrix &dop)	{
	u32			buffer_size = (xr_strlen(msg) + 2 + 4*( 4*(64 + 1) + 1 ) + 1) * sizeof(char);
	PSTR buf	= (PSTR)_alloca( buffer_size );

	xr_sprintf	(buf, buffer_size,"%s:\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n",
		msg,
		dop.i.x, dop.i.y, dop.i.z, dop._14_,
		dop.j.x, dop.j.y, dop.j.z, dop._24_,
		dop.k.x, dop.k.y, dop.k.z, dop._34_,
		dop.c.x, dop.c.y, dop.c.z, dop._44_
	);
	Log			(buf);
}

void xrLogger::Msg(LPCSTR Msg, va_list argList)
{
	string4096	formattedMessage;
	int MsgSize = _vsnprintf(formattedMessage, sizeof(formattedMessage) - 1, Msg, argList);
	formattedMessage[MsgSize] = 0;

	if (IsDebuggerPresent() && bFastDebugLog)
	{
		OutputDebugStringA(formattedMessage);
		OutputDebugStringA("\n");
	}

	SimpleMessage(formattedMessage, MsgSize);
}

void xrLogger::SimpleMessage(LPCSTR Message, u32 MessageSize /*= 0*/)
{
	switch (MessageSize)
	{
	case (u32(-1)): return;
	case 0:			MessageSize = xr_strlen(Message); break;
	default:		break;
	}
	xrCriticalSectionGuard guard(logDataGuard);
	logData.emplace(LogRecord(Message, MessageSize));
}

void xrLogger::OpenLogFile()
{
	theLogger.InternalOpenLogFile();
}

const string_path& xrLogger::GetLogPath()
{
	return theLogger.logFileName;
}

void xrLogger::EnableFastDebugLog()
{
	theLogger.bFastDebugLog = true;
}

void LogThreadEntryStartup(void* nullParam)
{
	theLogger.LogThreadEntry();
}

void xrLogger::InitLog()
{
	thread_spawn(LogThreadEntryStartup, "X-Ray Log Thread", 0, nullptr);
}

void xrLogger::FlushLog()
{
	theLogger.bFlushRequested = true;
}

void xrLogger::CloseLog()
{
	FlushLog();
	theLogger.InternalCloseLog();
}

void xrLogger::AddLogCallback(LogCallback logCb)
{
	if (logCb == nullptr) return;
	theLogger.logCallbackList.push_back(logCb);
}

void xrLogger::RemoveLogCallback(LogCallback logCb)
{
	theLogger.logCallbackList.remove(logCb);
}

void xrLogger::InternalCloseLog()
{
	if (hLogThread != nullptr)
	{
		bIsAlive = false;
		WaitForSingleObject(hLogThread, INFINITE);
		hLogThread = nullptr;
	}

	if(TryLogFile) TryLogFile->close();
}

xrLogger::xrLogger()
	: TryLogFile(nullptr), bFastDebugLog(false), 
	bIsAlive(true), hLogThread(nullptr), bFlushRequested(false)
{}

xrLogger::~xrLogger()
{
	InternalCloseLog();
}

void xrLogger::InternalOpenLogFile()
{
	string256 CurrentDate;
	string256 CurrentTime;
	
	Time time;
	xr_strconcat(CurrentDate, time.GetDayString().c_str(), "." , time.GetMonthString().c_str(), "." , time.GetDayString().c_str());
	xr_strconcat(CurrentTime, time.GetHoursString().c_str(), "h", time.GetMinutesString().c_str(), "m", time.GetSecondsString().c_str(), "s");

	xr_strconcat(logFileName, "[", CurrentDate, " " , CurrentTime, "]", ".log");
	if (FS.path_exist("$logs$"))
	{
		FS.update_path(logFileName, "$logs$", logFileName);
	}

	// Check and close
	TryLogFile = new std::ofstream(logFileName, std::ios::app);
	CHECK_OR_EXIT(TryLogFile, "Can't create log file");
}

void xrLogger::LogThreadEntry()
{
	while (bIsAlive)
	{
		bool bHaveMore = true;
		LogRecord theRecord;

		do
		{
			{
				xrCriticalSectionGuard guard(logDataGuard);
				if (!logData.empty())
				{
					theRecord = logData.front();
					logData.pop();
					bHaveMore = !logData.empty();
				}
				else break; // we don't have any messages
			}

			xr_vector<xr_string> LogLines = theRecord.Message.Split('\n');

			string256 TimeOfDay;
			int TimeOfDaySize = xr_sprintf(TimeOfDay, "[%s.%s.%s] ",
				theRecord.time.GetHoursString().c_str(), theRecord.time.GetMinutesString().c_str(), theRecord.time.GetSecondsString().c_str());
			
			for (const xr_string& line : LogLines)
			{
				string4096 finalLine;
				xr_strconcat(finalLine, TimeOfDay, line.c_str());

				// Output to MSVC debug output
				if (IsDebuggerPresent() && !bFastDebugLog)
				{
					OutputDebugStringA(finalLine);
					OutputDebugStringA("\n");
				}

				if (TryLogFile != nullptr)
				{
					(*TryLogFile) << (finalLine) << std::endl;

					// Write buffer to file
					if(bFlushRequested)
						TryLogFile->flush();
				}

				for (const LogCallback& FnCallback : logCallbackList)
				{
					FnCallback(finalLine);
				}
			}

		} while (bHaveMore);

		// Write buffer to file
		if(TryLogFile)
			TryLogFile->flush();

		Sleep(13); // work at 60 FPS roughly
	}
}

xrLogger::LogRecord::LogRecord(LPCSTR Msg, u32 sizeMsg)
	: Message(Msg, sizeMsg)
{}
