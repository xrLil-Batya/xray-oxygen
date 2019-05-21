#pragma once

namespace XRay
{
	public ref class MapManager abstract
	{
		static void HideIndicators(); // Property set false
		static void ShowIndicators(); // Property set true
		static void HideIndicatorsSafe();
	public:
		enum class ETypeIndicator: ::System::Int16
		{
			HideSafe = -1,
			Hide,
			Show
		};
	public:
		/// <summary>Check: Map has is current spot by object ID?</summary>
		static bool MapHasObjectSpot(u16 id, LPCSTR spot_type);
		/// <summary>Set: Set spot to level map by object ID</summary>
		static void MapAddObjectSpot(u16 id, LPCSTR spot_type, LPCSTR text);
		/// <summary>Set: Del spot from level map by object ID</summary>
		static void MapRemoveObjectSpot(u16 id, LPCSTR spot_type);
		/// <summary>Set: Set spot to level map by object ID</summary>
		static void MapAddObjectSpotSer(u16 id, LPCSTR spot_type, LPCSTR text);
		/// <summary>Set: Change spot hint from level map by object ID</summary>
		static void MapChangeSpotHint(u16 id, LPCSTR spot_type, LPCSTR text);

		static property ETypeIndicator Indicators
		{
			void set(ETypeIndicator);
		}

		static property bool ShowMinimap
		{
			void set(bool bShow);
		}

	};

}


