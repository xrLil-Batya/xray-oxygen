#pragma once
#include <msclr\marshal.h>
using namespace System;

namespace XRay
{
	/// <summary> X-Ray configs parser (.ini/.ltx)</summary>
	public ref class INIParser sealed
	{
		msclr::interop::marshal_context Marshal;
		using string = System::String^;
	public:

		/// <summary> Save your config to file. </summary>
		bool SaveAs(string);
		/// <summary> Returns file in form a string </summary>
		string GetAsString();

		/// <summary> Check if there is such a line </summary>
		bool  LineExist(string sect, string line);
		/// <summary> Returns lines count on section</summary>
		u32   LineCount(string SectName);
		/// <summary> Check if there is such a section </summary>
		bool  SectionExist(string SectName);

		/// <summary> Returns u8 value </summary>
		u8			r_u8 (string Sect, string Val);
		/// <summary> Returns u16 value </summary>
		u16			r_u16(string Sect, string Val);
		/// <summary> Returns u32 value </summary>
		u32			r_u32(string Sect, string Val);
		/// <summary> Returns u64 value </summary>
		u64			r_u64(string Sect, string Val);

		/// <summary> Returns s8 value </summary>
		s8			r_s8 (string Sect, string Val);
		/// <summary> Returns s16 value </summary>
		s16			r_s16(string Sect, string Val);
		/// <summary> Returns s32 value </summary>
		s32			r_s32(string Sect, string Val);

		/// <summary> Returns string value </summary>
		string		r_string(string Sect, string Val);
		/// <summary> Returns boolean value </summary>
		bool		r_bool(string Sect, string Val);
		/// <summary> Returns float value </summary>
		float		r_float(string Sect, string Val);
	public: 
		/// <summary> Returns file name </summary>
		property string FileName
		{
			string get() { return gcnew System::String(pSettings->fname()); }
		};
	};
}