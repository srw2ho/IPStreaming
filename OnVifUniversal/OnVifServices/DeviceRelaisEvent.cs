using System;

namespace OnVifServices
{
    public sealed class DeviceRelaisEvent
    {

        public DateTime Timestamp { get; } = DateTime.UtcNow;
        public DeviceRelaisEvent()
        {

        }
    }
}