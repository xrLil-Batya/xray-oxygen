#pragma once

namespace XRay
{
	public ref class WeatherFX
	{
	public:
		WeatherFX();
		void Create(::System::String^ Name);
		void Stop();

		property bool Enable
		{
			bool get();
		}
	};
}