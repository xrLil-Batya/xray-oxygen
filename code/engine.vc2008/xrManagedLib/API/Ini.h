#pragma once
#include "Filesystem.h"
#include <msclr\marshal.h>
using namespace System;

namespace XRay
{
	public ref class Ini sealed
	{
		msclr::interop::marshal_context Marshal;
	private:
		CInifile* pIni;

	public:
		/// <summary> Returns file name </summary>
		property String^ FileName
		{
			String^ get() 
			{ 
				return gcnew System::String(pIni->fname());
			}
		};

	public:
		Ini(String^ fileName);
		Ini(String^ fileName, bool readOnly);
		Ini(String^ fileName, bool readOnly, bool load);
		Ini(String^ fileName, bool readOnly, bool load, bool saveAtEnd);

		virtual ~Ini();

		/// <summary> Check if there is such a section </summary>
		bool IsSectionExist(String^ section);

		/// <summary> Check if there is such a line </summary>
		bool IsLineExist(String^ section, String^ line);

		/// <summary> Returns lines count on section</summary>
		u32 GetLineCount(String^ section);

		/// <summary> Returns boolean value </summary>
		bool r_bool(String^ section, String^ line);

		/// <summary> Returns u8 value </summary>
		u8 r_u8(String^ section, String^ line);
		/// <summary> Returns u16 value </summary>
		u16 r_u16(String^ section, String^ line);
		/// <summary> Returns u32 value </summary>
		u32	r_u32(String^ section, String^ line);
		/// <summary> Returns u64 value </summary>
		u64 r_u64(String^ section, String^ line);

		/// <summary> Returns s8 value </summary>
		s8 r_s8(String^ section, String^ line);
		/// <summary> Returns s16 value </summary>
		s16 r_s16(String^ section, String^ line);
		/// <summary> Returns s32 value </summary>
		s32 r_s32(String^ section, String^ line);

		/// <summary> Returns float value </summary>
		float r_float(String^ section, String^ line);

		/// <summary> Returns string value </summary>
		String^ r_string(String^ section, String^ line);

		void SaveAs(String^ fileName);

		virtual String^ ToString() override;
	};
}