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

		static property bool IsDeveloperMode
		{
			bool get()
			{
				return GamePersistent().IsDeveloperMode();
			}
		}
	};
}