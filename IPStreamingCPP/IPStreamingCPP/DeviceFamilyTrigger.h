#pragma once
using namespace Platform;

namespace IPStreamingCPP {
	namespace Triggers {

		public ref class DeviceFamiliyTrigger sealed :public Windows::UI::Xaml::StateTriggerBase
		{
			Platform::String^ m_queriedDeviceFamily;
			Platform::String^ m_currentDeviceFamily;
			


		public:
			DeviceFamiliyTrigger()
			{
				m_queriedDeviceFamily = ref new Platform::String(L"");

			}
			property  Platform::String^ DeviceFamily
			{
				Platform::String^ get();
				void set(Platform::String^ value);
			}
		private:

		};
	}
}