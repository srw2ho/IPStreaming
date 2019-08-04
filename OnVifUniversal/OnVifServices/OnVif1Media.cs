using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OnVifServices.OnvifMedia10;
using System.Net;
using System.ServiceModel;
using System.ServiceModel.Channels;

namespace OnVifServices
{
    public class OnVif1Media: OnVifServiceBase
    {
     //   String m_ErrorMessage;

    //    OnVifDevice m_onVifDevice;
        GetProfilesResponse m_profiles;
    //    bool m_initialised = false;

        OnVifServices.OnvifMedia10.MediaClient m_mediaClient;
        public OnVif1Media(OnVifDevice onVifDevice):base (onVifDevice)
        {

        }


        public string ProfileRefToken
        {
            get {
                if (m_profiles == null) return "";
                return m_profiles.Profiles[0].token;

            }
        }


        public GetProfilesResponse ProfilesResponse
        {
            get { return this.m_profiles; }
        }

        public async Task<OnvifMedia10.AudioEncoderConfiguration> getAudioEncodingsAsync(string targetProfileToken)
        {
            AudioEncoderConfiguration configuration = null;
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return null;

                }
      

                configuration = await m_mediaClient.GetAudioEncoderConfigurationAsync(targetProfileToken);



            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif1Media.getAudioEncodingsAsync", ex);
  //              return null;
            }


            return configuration;


        }


        public async Task<OnvifMedia10.VideoEncoderConfiguration> getVideoEncodingsAsync(string targetProfileToken)
        {
            VideoEncoderConfiguration configuration = null;
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return null;
                }

                 configuration = await m_mediaClient.GetVideoEncoderConfigurationAsync(targetProfileToken);


   
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif1Media.getVideoEncodingsAsync", ex);
 //               return null;
            }

            return configuration;
        }

        public async Task<OnvifMedia10.AudioEncoderConfigurationOptions> getAudioEncodingConfigurationOptionsAsync(string targetProfileToken)
        {
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return null;
                }

                var configuration = await m_mediaClient.GetAudioEncoderConfigurationOptionsAsync(targetProfileToken, m_profiles.Profiles[0].token);


                return configuration;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif1Media.getAudioEncodingConfigurationOptionsAsync", ex);
    //            return null;
            }

        }

        public async Task<OnvifMedia10.VideoEncoderConfigurationOptions> getVideoEncodingConfigurationOptionsAsync(string targetProfileToken)
        {
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return null;
                }

                var configuration = await m_mediaClient.GetVideoEncoderConfigurationOptionsAsync(targetProfileToken, m_profiles.Profiles[0].token);


                return configuration;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif1Media.getVideoEncodingConfigurationOptionsAsync", ex);
     //           return null;
            }
        }

        public async Task<Boolean> SetAudioEncodingsAsync(OnvifMedia10.AudioEncoderConfiguration encoding)
        {
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return false;
                }

                await m_mediaClient.SetAudioEncoderConfigurationAsync(encoding, true);


                return true;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif1Media.SetAudioEncodingsAsync", ex);
        //        return false;
            }
        }

        public async Task<Boolean> SetVideoEncodingsAsync(OnvifMedia10.VideoEncoderConfiguration encoding)
        {
            try
            {
                if (m_mediaClient == null)
                {
                    bool b = await this.InitalizeAsync();
                    if (m_mediaClient != null) return false;
                }

                await m_mediaClient.SetVideoEncoderConfigurationAsync(encoding, true);


                return true;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif1Media.SetVideoEncodingsAsync", ex);
       //         return false;
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

                var streamSetup = new StreamSetup();
                streamSetup.Stream = StreamType.RTPUnicast;
                streamSetup.Transport = new Transport();
                streamSetup.Transport.Protocol = TransportProtocol.RTSP;
                streamSetup.Transport.Tunnel = null;
                MediaUri mediaUri;

                mediaUri = await m_mediaClient.GetStreamUriAsync(streamSetup, targetProfileToken);
                if ((mediaUri != null))
                {
                    mediaUris.Add(mediaUri.Uri);
                }

            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif1Media.getStreamURIAsync", ex);

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


                    if (service.Namespace == "http://www.onvif.org/ver10/media/wsdl")
                    {

                        string serviceAdress = service.XAddr;
                        var messageElement = new TextMessageEncodingBindingElement();
                        messageElement.MessageVersion = MessageVersion.CreateVersion(EnvelopeVersion.Soap12, AddressingVersion.None);
                        HttpTransportBindingElement httpBinding = new HttpTransportBindingElement();
                        httpBinding.AuthenticationScheme = AuthenticationSchemes.Digest;
                        CustomBinding bind = new CustomBinding(messageElement, httpBinding);
                        m_mediaClient = new MediaClient(bind, new EndpointAddress(serviceAdress));

                        m_mediaClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetPasswordDigestBehavior);
                        m_mediaClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetBasicBasicAuthBehaviour);


                        m_profiles = await m_mediaClient.GetProfilesAsync();

                        m_initialised = true;
                        break;

       
                    }

                }
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVif1Media.InitalizeAsync", ex);

    //            return false;
            }

            return m_initialised;
        }

      

    }

}
