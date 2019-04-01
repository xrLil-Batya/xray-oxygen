#pragma once
#include "Actor.h"
#include "xrTime.h"

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

	public enum class eTutorialState
	{
		eStart = 0,
		eStop,
		eCheckOnActiveTutorial // no used yet
	};


	public ref class Game abstract sealed
	{

	public: 
		using CallBack = void(::System::String^);
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

		/// <summary>Get or set game difficulty</summary>
		static property ESingleGameDifficulty GameDifficulty
		{
			ESingleGameDifficulty get();
			void set(ESingleGameDifficulty dif);
		}

		static property bool IsDeveloperMode
		{
			bool get()
			{
				return GamePersistent().IsDeveloperMode();
			}
		}

		static property u64 StartTime
		{
			u64 get();
		};

		static property float SndVolume
		{
			float get();
			void set(float Value);
		}

		static ::System::String^ TranslateString(LPCSTR str);
		static void GlobalSend(NET_Packet& P);
		static void UserEventGen(NET_Packet& P, u32 _event, u32 _dest);


		static void setTutorialState(LPCSTR name, eTutorialState tutorialState);
		static bool getTutorialState();

		static int GlobalCommunityGoodwill(LPCSTR _community, int _entity_id);
		static void GlobalSetCommunityGoodwill(LPCSTR _community, int _entity_id, int val);
		static void GlobalChangeCommunityGoodwill(LPCSTR _community, int _entity_id, int val);
		static int GlobalGetCommunityRelation(LPCSTR comm_from, LPCSTR comm_to);
		static void GlobalSetCommunityRelation(LPCSTR comm_from, LPCSTR comm_to, int value);
		static int GlobalGetGeneralGoodwillBetween(u16 from, u16 to);

		static Fbox GetBoundingVolume();
		static void PrefetchSnd(LPCSTR name);
		static void IterateSounds(LPCSTR prefix, u32 max_count, CallBack callback);

		static void SpawnSection(LPCSTR sSection, Fvector3 vPosition, u32 LevelVertexID, u16 ParentID) { SpawnSection(sSection, vPosition, LevelVertexID, ParentID, false); };
		static void SpawnSection(LPCSTR sSection, Fvector3 vPosition, u32 LevelVertexID, u16 ParentID, bool bReturnItem);
		static void ShowMinimap(bool bShow);
		static GameObject^ GlobalGetTargetObject();
		static float GlobalGetTargetDist();
		static ::System::UInt32 GlobalGetTargetElement();


	};
}