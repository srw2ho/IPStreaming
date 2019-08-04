#include "pch.h"
#include "SystemInformationHelpers.h"
#include "DeviceFamilyTrigger.h"


using namespace Platform;
using namespace Windows::Foundation;

namespace IPStreamingCPP {
	namespace Triggers {
		void DeviceFamiliyTrigger::DeviceFamily::set(Platform::String^ value)
		{
			m_queriedDeviceFamily = value;
			m_currentDeviceFamily = SystemInformationHelpers::getDeviceFamily();
			SetActive(m_queriedDeviceFamily== m_currentDeviceFamily);
		}

		Platform::String^ DeviceFamiliyTrigger::DeviceFamily::get()
		{
			return m_queriedDeviceFamily;
		}


	}
}
