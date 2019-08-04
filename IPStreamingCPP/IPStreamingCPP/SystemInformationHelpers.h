#pragma once

namespace IPStreamingCPP {
    class SystemInformationHelpers sealed
    {

    public:
        // For now, the 10-foot experience is enabled only on Xbox.
        static bool IsTenFootExperience() { return IsXbox(); }

		static Platform::String^ getDeviceFamily()
		{
			// Calculate this once and cache the result.
			static Platform::String^ m_DeviceFamiliy = (Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily);
			return m_DeviceFamiliy;
		}
    private:
        static bool IsXbox()
        {
            // Calculate this once and cache the result.
        //    m_isXbox = (Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily == "Windows.Xbox");
			static bool m_isXbox = (getDeviceFamily() == "Windows.Xbox");
            return m_isXbox;
        }
	
    };
}


