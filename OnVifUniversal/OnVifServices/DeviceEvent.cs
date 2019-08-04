using System;

namespace OnVifServices
{
 
    public sealed class DeviceEvent
    {

        public DateTime Timestamp { get; } = DateTime.UtcNow;
        public string Message { get; }

 
        public DeviceEvent(string message)
        {
            this.Message = message;
        }
    }
}