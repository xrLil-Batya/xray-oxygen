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
		static ref struct SGameTime
		{
			u32 Days;
			u32 Hours;
			u32 Minutes;
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