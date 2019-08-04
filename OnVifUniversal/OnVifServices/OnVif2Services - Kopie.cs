using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OnVifServices.OnvifMedia20;
using OnVifServices.OnvifDevice;
using System.Net;
using System.ServiceModel;
using System.ServiceModel.Channels;

namespace OnVifServices
{
   public sealed class Media20Services
        {
            String m_Hostname;
            String m_User;
            String m_Password;
            GetProfilesResponse m_profiles;
            String m_ErrorMessage;


            Media2Client m_mediaClient;

            BasicAuthBehaviour m_BasicBasicAuthBehaviour;
            PasswordDigestBehavior m_PasswordDigestBehavior;
            GetServicesResponse m_services;
            GetDeviceInformationResponse m_deviceInformations;
            HostnameInformation m_HostnameInformation;

            public BasicAuthBehaviour GetBasicBasicAuthBehaviour
            {
                get { return m_BasicBasicAuthBehaviour; }
            }

            public PasswordDigestBehavior GetPasswordDigestBehavior
            {
                get { return m_PasswordDigestBehavior; }
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

            public GetProfilesResponse ProfilesResponse
            {
                get { return this.m_profiles; }
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
            }



            public Media20Services(string Hostname, string User, string Password)
            {

                m_services = null;
                m_mediaClient = null;
                m_profiles = null;
                m_Hostname = Hostname;
                m_User = User;
                m_Password = Password;
                m_BasicBasicAuthBehaviour = new BasicAuthBehaviour(m_User, m_Password);
                m_PasswordDigestBehavior = new PasswordDigestBehavior(m_User, m_Password, 0);
                m_ErrorMessage = "";
                m_deviceInformations = null;
                m_HostnameInformation = null;


            }

            public async Task<OnvifMedia20.GetVideoEncoderConfigurationsResponse> getVideoEncodingsAsync(string targetProfileToken)
            {
                try
                {
                    if (m_mediaClient == null)
                    {
                        bool b = await this.InitalizeMediaAsync();
                        if (m_mediaClient != null) return null;
                    }

                    var configuration = await m_mediaClient.GetVideoEncoderConfigurationsAsync(targetProfileToken, m_profiles.Profiles[0].token);


                    return configuration;
                }
                catch (Exception ex)
                {
                    m_ErrorMessage = ex.Message;
                    return null;
                }
            }

            public async Task<OnvifMedia20.GetVideoEncoderConfigurationOptionsResponse> getVideoEncodingConfigurationOptionsAsync(string targetProfileToken)
            {
                try
                {
                    if (m_mediaClient == null)
                    {
                        bool b = await this.InitalizeMediaAsync();
                        if (m_mediaClient != null) return null;
                    }

                    var configuration = await m_mediaClient.GetVideoEncoderConfigurationOptionsAsync(targetProfileToken, m_profiles.Profiles[0].token);


                    return configuration;
                }
                catch (Exception ex)
                {
                    m_ErrorMessage = ex.Message;
                    return null;
                }
            }


            public async Task<Boolean> SetVideoEncodingsAsync(OnvifMedia20.VideoEncoder2Configuration encoding)
            {
                try
                {
                    if (m_mediaClient == null)
                    {
                        bool b = await this.InitalizeMediaAsync();
                        if (m_mediaClient != null) return false;
                    }

                    await m_mediaClient.SetVideoEncoderConfigurationAsync(encoding);


                    return true;
                }
                catch (Exception ex)
                {
                    m_ErrorMessage = ex.Message;
                    return false;
                }
            }



            public async Task<GetStreamUriResponse> getStreamURIAsync(string targetProfileToken)
            {
                GetStreamUriResponse mediaUris = null;
                try
                {
                    if (m_mediaClient == null)
                    {
                        bool b = await this.InitalizeMediaAsync();
                        if (m_mediaClient != null) return mediaUris;
                    }
                    /*
                    //   string targetProfileToken;
                    var streamSetup = new StreamSetup();
                    streamSetup.Stream = StreamType.RTPUnicast;
                    streamSetup.Transport = new Transport();
                    streamSetup.Transport.Protocol = TransportProtocol.RTSP;
                    streamSetup.Transport.Tunnel = null;
                    MediaUri mediaUri;
                    */
                    string Protocol = "RTSP";
                    mediaUris = await m_mediaClient.GetStreamUriAsync(Protocol, targetProfileToken);
 

                }
                catch (Exception ex)
                {
                    m_ErrorMessage = ex.Message;

                }
                return mediaUris;
            }

            public async Task<bool> InitalizeMediaAsync()
            {
                try
                {
                    if (m_profiles != null) return true;

                    if (m_services == null)
                    {
                        bool b = await this.GetServicesAsync();
                        if (m_services != null) return false;
                    }



                    foreach (var service in m_services.Service)
                    {


                        if (service.Namespace == "http://www.onvif.org/ver10/media/wsdl")
                        {
                            var messageElement = new TextMessageEncodingBindingElement();
                            messageElement.MessageVersion = MessageVersion.CreateVersion(EnvelopeVersion.Soap12, AddressingVersion.None);
                            HttpTransportBindingElement httpBinding = new HttpTransportBindingElement();
                            httpBinding.AuthenticationScheme = AuthenticationSchemes.Digest;
                            CustomBinding bind = new CustomBinding(messageElement, httpBinding);
                            m_mediaClient = new Media2Client(bind, new EndpointAddress($"http://{m_Hostname}/onvif/Media"));
                            m_mediaClient.Endpoint.EndpointBehaviors.Add(m_PasswordDigestBehavior);
                            m_mediaClient.Endpoint.EndpointBehaviors.Add(m_BasicBasicAuthBehaviour);

                            m_profiles = await m_mediaClient.GetProfilesAsync(null,null);
                    





                    }
                    }

                }
                catch (Exception ex)
                {
                    m_ErrorMessage = ex.Message;
                    return false;
                }
                return (m_profiles != null);
            }

            public async Task<bool> GetServicesAsync()
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

                    DeviceClient device = new DeviceClient(binding, address);

                    bool useAuth = !string.IsNullOrEmpty(m_BasicBasicAuthBehaviour.Username) && !string.IsNullOrEmpty(m_BasicBasicAuthBehaviour.Password);
                    double diff = 0;
                    if (useAuth)
                    {
                        device.Endpoint.EndpointBehaviors.Add(m_BasicBasicAuthBehaviour);
                    }
                    try
                    {
                        device.Endpoint.EndpointBehaviors.Add(m_PasswordDigestBehavior);
                        //ensure date and time are in sync
                        //add basic auth for compat with some cameras
                        var sdt = await device.GetSystemDateAndTimeAsync();

                        var d = sdt.UTCDateTime.Date;
                        var t = sdt.UTCDateTime.Time;

                        var dt = new System.DateTime(d.Year, d.Month, d.Day, t.Hour, t.Minute, t.Second);

                        diff = (System.DateTime.UtcNow - dt).TotalSeconds;
                        m_PasswordDigestBehavior.TimeOffset = diff;

                        GetDeviceInformationRequest request = new GetDeviceInformationRequest();


                        m_deviceInformations = await device.GetDeviceInformationAsync(request);


                        m_HostnameInformation = await device.GetHostnameAsync();


                    }
                    catch (Exception ex)
                    {
                        m_ErrorMessage = ex.Message;
                        return false;
                    }



                    //   device.InnerChannel.OperationTimeout = TimeSpan.FromMinutes(1);
                    binding.SendTimeout = binding.CloseTimeout = binding.ReceiveTimeout = binding.OpenTimeout = TimeSpan.FromSeconds(5);

                    m_services = await device.GetServicesAsync(true);



                    foreach (var service in m_services.Service)
                    {
                        if (service.Namespace == "http://www.onvif.org/ver10/media/wsdl")
                        {

                        }
                    }

                }

                catch (Exception ex)
                {
                    m_ErrorMessage = ex.Message;
                    return false;
                }
                return true;
            }


        }

}
