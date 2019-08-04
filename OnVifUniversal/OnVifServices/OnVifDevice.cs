using System;
 using System.Reflection;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

//using OnVifServices.OnvifMedia20;
using OnVifServices.OnvifPTZService;
using OnVifServices.OnvifDevice;
using System.Net;
using System.ServiceModel;
using System.ServiceModel.Channels;


namespace OnVifServices
{

    public sealed class OnVifDevice
    {

        String m_ErrorMessage;

        String m_Hostname;
        String m_User;
        String m_Password;

        BasicAuthBehaviour m_BasicBasicAuthBehaviour;
        PasswordDigestBehavior m_PasswordDigestBehavior;
        GetServicesResponse m_services;
        GetDeviceInformationResponse m_deviceInformations;
        HostnameInformation m_HostnameInformation;
        OnVifServices.OnvifDevice.Capabilities m_Capabilities;

        DeviceClient m_device;
        GetCapabilitiesResponse m_capabilitiesResponse;
        SystemDateTime m_SystemDateTime;

        public OnVifDevice(string Hostname, string User, string Password)
        {

            m_services = null;

            m_Hostname = Hostname;
            m_User = User;
            m_Password = Password;
            m_BasicBasicAuthBehaviour = new BasicAuthBehaviour(m_User, m_Password);
            m_PasswordDigestBehavior = new PasswordDigestBehavior(m_User, m_Password, 0);
            m_ErrorMessage = "";
            m_deviceInformations = null;
            m_HostnameInformation = null;
            m_Capabilities = null;
            m_device = null;
            m_capabilitiesResponse = null;
            m_SystemDateTime = null;


        }

        public void setConnectionData(string Hostname, string User, string Password)
        {

            m_Hostname = Hostname;
            m_User = User;
            m_Password = Password;
            m_BasicBasicAuthBehaviour.Password = Password;
            m_BasicBasicAuthBehaviour.Username = User;
            m_PasswordDigestBehavior.Password = Password;
            m_PasswordDigestBehavior.Username = User;

        }

        public BasicAuthBehaviour GetBasicBasicAuthBehaviour
        {
            get { return m_BasicBasicAuthBehaviour; }
        }

        public PasswordDigestBehavior GetPasswordDigestBehavior
        {
            get { return m_PasswordDigestBehavior; }
        }

        public OnVifServices.OnvifDevice.Capabilities Capabilities
        {
            get { return this.m_Capabilities; }
        }


        public GetDeviceInformationResponse DeviceInformationResponse
        {
            get { return this.m_deviceInformations; }
        }

  

        public HostnameInformation HostInformation
        {
            get { return this.m_HostnameInformation; }
        }


        public string ErrorMessage
        {
            get { return this.m_ErrorMessage; }
     //       set { this.m_ErrorMessage = value; }
        }


        public GetServicesResponse ServicesResponse
        {
            get { return this.m_services; }
        }

        public String Hostname
        {
            get { return this.m_Hostname; }
            set { this.m_Hostname = value; }
        }
        public String User
        {
            get { return this.m_User; }
            set { this.m_User = value; }
        }

        public DeviceClient Device
        {
            get { return this.m_device; }
        }

        public async Task<OnvifDevice.Capabilities> GetDeviceCapabilitiesAsync()
        {
            if (m_device == null)
            {
                await InitalizeDeviceAsync();
                if (m_device == null) return null;
            }

            if (m_capabilitiesResponse == null)
            {
                OnVifServices.OnvifDevice.CapabilityCategory[] capability = new OnVifServices.OnvifDevice.CapabilityCategory[1];
                capability[0] = CapabilityCategory.All;
                m_capabilitiesResponse = await m_device.GetCapabilitiesAsync(capability);

            }

   
            if (m_capabilitiesResponse == null) return null;
   
            return m_capabilitiesResponse.Capabilities;
        }

        public Boolean GetCameraData(IDictionary<string, string> openWith)
        {

            openWith.Clear();
            //     IDictionary<string, string> openWith = new Dictionary<string, string>();
            if (m_device != null)
            {
                var hostnameInfo = this.HostInformation;
                if (hostnameInfo != null)
                {
                    openWith.Add("Name", hostnameInfo.Name);

                }
                if (m_deviceInformations != null)
                {
                    openWith.Add("FirmwareVersion", m_deviceInformations.FirmwareVersion);
                    openWith.Add("HardwareId", m_deviceInformations.HardwareId);
                    openWith.Add("Manufacturer", m_deviceInformations.Manufacturer);
                    openWith.Add("Model", m_deviceInformations.Model);
                    openWith.Add("SerialNumber", m_deviceInformations.SerialNumber);

                }
            }
            return (openWith.Count > 0);
        }


        public async Task<bool> setActDateandTimeAsync()
        {
            if (m_SystemDateTime == null) return false;

            try
            {
                TimeZone timeZone = null;
                if (m_SystemDateTime.TimeZone!=null)
                {
                    timeZone = m_SystemDateTime.TimeZone;
                }
                System.DateTime utc = System.DateTime.UtcNow;

                OnvifDevice.DateTime utcsetDate = new OnvifDevice.DateTime();
                utcsetDate.Date = new OnvifDevice.Date();
                utcsetDate.Date.Day = utc.Day;
                utcsetDate.Date.Month = utc.Month;
                utcsetDate.Date.Year = utc.Year;

                utcsetDate.Time = new OnvifDevice.Time();
                utcsetDate.Time.Hour = utc.Hour;
                utcsetDate.Time.Minute = utc.Minute;
                utcsetDate.Time.Second = utc.Second;


                await m_device.SetSystemDateAndTimeAsync(SetDateTimeType.Manual, m_SystemDateTime.DaylightSavings, timeZone, utcsetDate);
            }

            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifDevice.setActDateandTimeAsync", ex);
                //            return false;
            }

            return true;

        }
     


        public async Task<bool> InitalizeDeviceAsync()
        {


            try
            {
                if (m_services != null) return true;


                HttpTransportBindingElement httpBinding = new HttpTransportBindingElement();
                httpBinding.AuthenticationScheme = AuthenticationSchemes.Digest;
                var messageElement = new TextMessageEncodingBindingElement();
                messageElement.MessageVersion = MessageVersion.CreateVersion(EnvelopeVersion.Soap12, AddressingVersion.None);
                CustomBinding binding = new CustomBinding(messageElement, httpBinding);

                EndpointAddress address = new EndpointAddress($@"http://{m_Hostname}/onvif/device_service");

                binding.SendTimeout = binding.CloseTimeout = binding.ReceiveTimeout = binding.OpenTimeout = TimeSpan.FromSeconds(15);

                m_device = new DeviceClient(binding, address);
            //    var discoverymode = m_device.GetRemoteDiscoveryModeAsync();

                bool useAuth = !string.IsNullOrEmpty(m_BasicBasicAuthBehaviour.Username) && !string.IsNullOrEmpty(m_BasicBasicAuthBehaviour.Password);
                double diff = 0;
               

                if (useAuth)
                {
                  m_device.Endpoint.EndpointBehaviors.Add(m_BasicBasicAuthBehaviour);
                  m_device.Endpoint.EndpointBehaviors.Add(m_PasswordDigestBehavior);
                }

                //ensure date and time are in sync
                //add basic auth for compat with some cameras
                m_SystemDateTime = await m_device.GetSystemDateAndTimeAsync();

        
                var d = m_SystemDateTime.UTCDateTime.Date;
                var t = m_SystemDateTime.UTCDateTime.Time;

                var dt = new System.DateTime(d.Year, d.Month, d.Day, t.Hour, t.Minute, t.Second);

                diff = (System.DateTime.UtcNow - dt).TotalSeconds;
                m_PasswordDigestBehavior.TimeOffset = diff;

                GetDeviceInformationRequest request = new GetDeviceInformationRequest();
                m_deviceInformations = await m_device.GetDeviceInformationAsync(request);

                this.m_HostnameInformation = await m_device.GetHostnameAsync();

                m_services = await m_device.GetServicesAsync(true);

            }
            
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifDevice.InitalizeDeviceAsync", ex);
    //            return false;
            }

            return (m_services!=null);
        }

    }
}
