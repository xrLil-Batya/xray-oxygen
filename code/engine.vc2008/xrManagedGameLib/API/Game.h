#pragma once

using namespace System;

namespace XRay
{
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