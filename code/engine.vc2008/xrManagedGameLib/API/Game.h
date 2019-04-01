#pragma once
namespace XRay
{
	public enum class ESingleGameDifficulty : ::System::UInt32
	{
		egdNovice = 0,
		egdStalker = 1,
		egdVeteran = 2,
		egdMaster = 3,
		egdCount,
		egd_force_u32 = u32(-1)
	};

	public ref class Game abstract sealed
	{
	public:
		ref struct SGameTime
		{
		private:

			static ::System::UInt32 _years;
			static ::System::UInt32 _months;
			static ::System::UInt32 _weeks;
			static ::System::UInt32 _days;
			static ::System::UInt32 _hours;
			static ::System::UInt32 _minutes;
			static ::System::UInt32 _seconds;
			static ::System::UInt32 _miliseconds;

		public:

			static property ::System::UInt32 Years
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}

			static property ::System::UInt32 Months
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}
			//////Weeks not used in Original //////
			static property ::System::UInt32 Weeks
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}
			
			static property ::System::UInt32 Days
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}

			static property ::System::UInt32 Hours
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}

			static property ::System::UInt32 Minutes
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}

			static property ::System::UInt32 Seconds
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}

			static property ::System::UInt32 Miliseconds
			{
				::System::UInt32 get();
				void set(::System::UInt32 value);
			}

			static void ChangeGameTime(u32 days, u32 hours, u32 mins);
			//static void ChangeGameTime(float fvalue);
		};

		/// <summary>Returns current game time</summary>
		static property SGameTime^ Time
		{
			SGameTime^ get(); 
			void set(SGameTime^ NewTime);
		}

		static property bool IsDeveloperMode
		{
			bool get()
			{
				return GamePersistent().IsDeveloperMode();
			}
		}
	};
}