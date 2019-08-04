using System;

namespace OnVifServices
{
    internal class DeviceEventReceiverException : Exception
    {
        public DeviceEventReceiverException()
        {
        }

        public DeviceEventReceiverException(string message) : base(message)
        {
        }

        public DeviceEventReceiverException(string message, Exception innerException) : base(message, innerException)
        {
        }
    }
}