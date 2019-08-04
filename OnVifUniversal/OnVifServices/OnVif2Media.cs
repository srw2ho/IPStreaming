using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


using System.Net;
using System.ServiceModel;
using System.ServiceModel.Channels;

using OnVifServices.OnvifMedia20;

namespace OnVifServices
{
    public class OnVif2Media: OnVifServiceBase
    {

        GetProfilesResponse m_profiles;


        OnVifServices.OnvifMedia20.Media2Client m_mediaClient = null;

        public OnVif2Media(OnVifDevice onVifDevice): base (onVifDevice)
        {

        }


        public string ProfileRefToken
        {
            get
            {
                if (m_profiles == null) return "";
                return m_profiles.Profiles[0].token;

            }
        }
   

        public GetProfilesResponse ProfilesResponse
        {
            get { return this.m_profiles; }
        }

        public async Task<OnvifMedia20.GetAudioEncoderConfigurationOptionsResponse> getAudioEncodingConfigurationOptionsAsync(string configtoken, string targetProfileToken)
        {
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return null;
                }

    //            var configuration = await m_mediaClient.GetAudioEncoderConfigurationOptionsAsync( targetProfileToken, m_profiles.Profiles[0].token);

                var configuration = await m_mediaClient.GetAudioEncoderConfigurationOptionsAsync(configtoken, targetProfileToken);

                return configuration;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif2Media.getAudioEncodingConfigurationOptionsAsync", ex);
   //             return null;
            }
        }


        public async Task<OnvifMedia20.GetVideoEncoderConfigurationOptionsResponse> getVideoEncodingConfigurationOptionsAsync(string configtoken, string targetProfileToken)
        {
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return null;
                }

    //            var configuration = await m_mediaClient.GetVideoEncoderConfigurationOptionsAsync( targetProfileToken, m_profiles.Profiles[0].token);
                var configuration = await m_mediaClient.GetVideoEncoderConfigurationOptionsAsync(configtoken, targetProfileToken);


                return configuration;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif2Media.getVideoEncodingConfigurationOptionsAsync", ex);
 //               return null;
            }
        }



        public async Task<OnvifMedia20.GetAudioEncoderConfigurationsResponse> getAudioEncodingsAsync(string configToken, string targetProfileToken)
        {
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return null;
                }

     
                //var configuration = await m_mediaClient.GetAudioEncoderConfigurationsAsync(targetProfileToken, m_profiles.Profiles[0].token);
                var configuration = await m_mediaClient.GetAudioEncoderConfigurationsAsync(configToken, targetProfileToken);


                return configuration;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif2Media.getAudioEncodingsAsync", ex);
     //           return null;
            }
        }

        public async Task<OnvifMedia20.GetVideoEncoderConfigurationsResponse> getVideoEncodingsAsync(string configToken, string targetProfileToken)
        {
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return null;
                }
           

         //       var configuration = await m_mediaClient.GetVideoEncoderConfigurationsAsync(targetProfileToken, m_profiles.Profiles[0].token);
                var configuration = await m_mediaClient.GetVideoEncoderConfigurationsAsync(configToken, targetProfileToken);


                return configuration;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif2Media.getVideoEncodingsAsync", ex);
   //             return null;
            }
        }

        public async Task<Boolean> SetAudioEncodingsAsync(OnvifMedia20.AudioEncoder2Configuration encoding)
        {
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return false;
                }
                await m_mediaClient.SetAudioEncoderConfigurationAsync(encoding);


                return true;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif2Media.SetAudioEncodingsAsync", ex);
     //           return false;
            }
        }

        public async Task<Boolean> SetVideoEncodingsAsync(OnvifMedia20.VideoEncoder2Configuration encoding)
        {
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return false;
                }

                await m_mediaClient.SetVideoEncoderConfigurationAsync(encoding);


                return true;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif2Media.SetVideoEncodingsAsync", ex);
  //              return false;
            }
        }

        public async Task<IList<string>> getStreamURIAsync(string targetProfileToken)
        {
            List<string> mediaUris = new List<string>();
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return mediaUris;
                }

                string protocol = "RTSP";
                var mediaUri = await m_mediaClient.GetStreamUriAsync(protocol, targetProfileToken);
                if ((mediaUri != null))
                {
                    mediaUris.Add(mediaUri.Uri);
                }

            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif2Media.getStreamURIAsync", ex);

            }
            return mediaUris;
        }

        public override async Task<bool> InitalizeAsync()
        {

            m_initialised = false;

            if (this.m_onVifDevice.ServicesResponse == null)
            {
                bool b = await this.m_onVifDevice.InitalizeDeviceAsync();
                if (this.m_onVifDevice.ServicesResponse == null) return false;
            }


            try
            {
                foreach (var service in this.m_onVifDevice.ServicesResponse.Service)
                {


                    if (service.Namespace == "http://www.onvif.org/ver20/media/wsdl")
                    {

                        string serviceAdress = service.XAddr;
                        var messageElement = new TextMessageEncodingBindingElement();
                        messageElement.MessageVersion = MessageVersion.CreateVersion(EnvelopeVersion.Soap12, AddressingVersion.None);
                        HttpTransportBindingElement httpBinding = new HttpTransportBindingElement();
                        httpBinding.AuthenticationScheme = AuthenticationSchemes.Digest;
                        CustomBinding bind = new CustomBinding(messageElement, httpBinding);
                        m_mediaClient = new Media2Client(bind, new EndpointAddress(serviceAdress));

                        m_mediaClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetPasswordDigestBehavior);
                        m_mediaClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetBasicBasicAuthBehaviour);

                        var type = new List<string>();
                        type.Add("All");

                        m_profiles = await m_mediaClient.GetProfilesAsync(null, type.ToArray());

                        m_initialised = true;
                        break;


                    }

                }
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif2Media.InitalizeAsync", ex);
       //         return false;
            }

            return m_initialised;
        }

    }


}
