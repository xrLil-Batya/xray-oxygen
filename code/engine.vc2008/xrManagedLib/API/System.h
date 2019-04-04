#pragma once

#include "../xrCore/oxy_version.h"
using namespace System;

extern XRCORE_API u32 build_id;
extern XRCORE_API LPCSTR build_date;
using String = System::String;

namespace XRay
{
	/// <summary>
	/// A various information from system, such as CPU info, available memory
	/// </summary>
	public ref class System abstract sealed
	{
	public:
		/// <summary>
		/// Major version of Engine and Game Module
		/// </summary>
		static property String^ Version
		{
			String^ get()
			{
				return ENGINE_VERSION;
			}
		}

		/// <summary>
		/// Get version string, like: X-Ray Oxygen version "1.7f" Build 7298 (Jan 29 2019) Commit[ox_dev; cd5027c1157f6a5d749b0ba4a7ec0b5ba61e3e27]
		/// </summary>
		static property String^ GetVersionString
		{
			String^ get()
			{
				return "X-Ray Oxygen version \"" + ENGINE_VERSION
					+ "\" Build " + BuildId
					+ " (" + BuildDate + ") Commit["
					+ OxygenBranch + "; " + OxygenCommitHash + "]";
			}
		}

		/// <summary>
		/// Build id, which unique per day
		/// </summary>
		static property System::UInt32 BuildId
		{
			System::UInt32 get()
			{
				return build_id;
			}
		}

		/// <summary>
		/// Build id, which unique per day
		/// </summary>
		static property String^ BuildDate
		{
			String^ get()
			{
				return gcnew String(build_date);
			}
		}

		/// <summary>
		/// Oxygen commit hash, that was used to build this engine
		/// </summary>
		static property String^ OxygenCommitHash
		{
			String^ get()
			{
				return gcnew String(_HASH);
			}
		}

		/// <summary>
		/// Oxygen repository branch that was used to build this engine
		/// </summary>
		static property String^ OxygenBranch
		{
			String^ get()
			{
				return gcnew String(_BRANCH);
			}
		}

		/// <summary>
		/// CPU information (frequency, available features)
		/// </summary>
		ref class CPU abstract sealed
		{
		public:
			/// <summary>
			/// CPU frequency in Herts
			/// </summary>
			static property ::System::UInt32 Frequency
			{
				::System::UInt32 get()
				{
					return ::CPU::Info.m_dwTickCount[0];
				}
			}

			/// <summary>
			/// Numbers of physical CPU cores
			/// </summary>
			static property ::System::UInt32 Cores
			{
				::System::UInt32 get()
				{
					return ::CPU::Info.n_cores;
				}
			}

			/// <summary>
			/// Numbers of CPU logical cores. If you have Hyper-Threading you might have twice as you physical cores
			/// </summary>
			static property ::System::UInt32 Threads
			{
				::System::UInt32 get()
				{
					return ::CPU::Info.n_threads;
				}
			}

			/// <summary>
			/// Percentage of system load, where 0.0 - 0%, 100.0 - 100%
			/// </summary>
			static property ::System::Double Usage
			{
				::System::Double get()
				{
					double LoadCount = 0.0;
					::CPU::Info.getCPULoad(LoadCount);
					return LoadCount;
				}
			}
		};
	};
}