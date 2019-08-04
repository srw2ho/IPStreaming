using OnVifServices;
using OnVifServices.OnvifMedia10;
using OnVifServices.OnvifMedia20;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OnVifServicesRunTime
{
    // please take attention:
    // Code not tested, no device available which support OnVif1Media
    //

    internal class OnVifMedia : IOnVifMedia
    {
        private bool m_binitalized;
        private OnVifServices.OnVif1Media m_OnVif1Media;
        private OnVifServices.OnVif2Media m_OnVif2Media;
        OnVifDevice m_onVifDevice;

        String m_ErrorMessage;
        private IList<OnVifCameraProfile> m_ProfileEncodings;

       // public event PropertyChangedEventHandler PropertyChanged = null;

        internal OnVifMedia(OnVifDevice onVifDevice)
        {
            m_ProfileEncodings = new ObservableCollection<OnVifCameraProfile>();
            m_onVifDevice = onVifDevice;
            m_OnVif1Media = null;
            m_OnVif2Media = null;
            m_binitalized = false;
            m_ErrorMessage = "";

        }

        public bool Isinitalized { get { return m_binitalized; } }

        public IList<OnVifCameraProfile> ProfileEncodings
        {
            get { return m_ProfileEncodings; }
            set
            {
                if (value != this.m_ProfileEncodings)
                {
                    this.m_ProfileEncodings = value;

                }
     //           RaisePropertyChanged("ProfileEncodings");

            }

        }

        public string ProfileRefToken
        {

            get
            {
                if (this.m_OnVif1Media != null)
                {
                    return this.m_OnVif1Media.ProfilesResponse.Profiles[0].token;
                }
                else if (this.m_OnVif2Media != null)
                {
                    return this.m_OnVif2Media.ProfilesResponse.Profiles[0].token;
                }
                return ""; // empty

            }

        }
        public OnVifCameraProfile getOnVifProfile(int nProfileIdx)
        {
            OnVifCameraProfile profile = null;
            if (nProfileIdx < this.m_ProfileEncodings.Count) profile = this.m_ProfileEncodings[nProfileIdx];
            return profile;

        }

        internal OnVifServices.OnvifMedia10.Profile getCamera1Profile(int nProfileIdx)
        {
            if (nProfileIdx < this.m_OnVif1Media.ProfilesResponse.Profiles.Length)
            {
                return m_OnVif1Media.ProfilesResponse.Profiles[nProfileIdx];
            }

            return null;

        }

        internal OnVifServices.OnvifMedia20.MediaProfile getCamera2Profile(int nProfileIdx)
        {
            if (nProfileIdx < this.m_OnVif2Media.ProfilesResponse.Profiles.Length)
            {
                return m_OnVif2Media.ProfilesResponse.Profiles[nProfileIdx];
            }

            return null;

        }


        public IList<OnVifCameraProfile> createProfiles()
        {
         

            this.m_ProfileEncodings.Clear();

            for (int i = 0; i < this.getNummberOfProfileTokens(); i++)
            { 
                OnVifCameraProfile profile = new OnVifCameraProfile(i);
                this.m_ProfileEncodings.Add(profile);

            }

            return this.m_ProfileEncodings;

        }
    
        public int getNummberOfProfileTokens()
        {
            if (this.m_OnVif1Media != null) {
                return this.m_OnVif1Media.ProfilesResponse.Profiles.Length;
            }
            else if (this.m_OnVif2Media != null)
            {
               return this.m_OnVif2Media.ProfilesResponse.Profiles.Length;
            }
            return 0;

        }

        public string getProfileToken(int nProfileIdx)
        {
            if (this.m_OnVif1Media != null)
            {
                return this.m_OnVif1Media.ProfilesResponse.Profiles[0].token;
            }
            else if (this.m_OnVif2Media != null)
            {
                return this.m_OnVif2Media.ProfilesResponse.Profiles[0].token;
            }
            return ""; // empty

        }
        public string getProfileName(int nProfileIdx)
        {
            if (this.m_OnVif1Media != null)
            {
                if (nProfileIdx < this.m_OnVif1Media.ProfilesResponse.Profiles.Length)
                {
                    Profile ProfileResponse = this.m_OnVif1Media.ProfilesResponse.Profiles[nProfileIdx];
                    return ProfileResponse.Name;
                }
            }
            else if (this.m_OnVif2Media != null)
            {
                if (nProfileIdx < this.m_OnVif2Media.ProfilesResponse.Profiles.Length)
                {
                    MediaProfile ProfileResponse = this.m_OnVif2Media.ProfilesResponse.Profiles[nProfileIdx];
                    return ProfileResponse.Name;
                }
            }
            return ""; // empty

        }


        public async Task<IDictionary<string, IList<string>>> ReadMediaUrisAsync(int nProfileIdx)
        {

            Dictionary<string, IList<string>> profileUrls = new Dictionary<string, IList<string>>();
            OnVifCameraProfile profile = getOnVifProfile(nProfileIdx);

            if (profile == null) return profileUrls;

    
            if (this.m_OnVif1Media != null)
            {
                Profile cameaProfile = getCamera1Profile(nProfileIdx);
                if (cameaProfile == null) return profileUrls;

                IList<string> urlList = await this.m_OnVif1Media.getStreamURIAsync(cameaProfile.token);
                if (urlList != null)
                {

                    profileUrls.Add(cameaProfile.Name, urlList);
                }
            }
            else if (this.m_OnVif2Media != null)
            {

                MediaProfile cameaProfile = getCamera2Profile(nProfileIdx);
                if (cameaProfile == null) return profileUrls;

                IList<string> urlList = await this.m_OnVif2Media.getStreamURIAsync(cameaProfile.token);
                if (urlList != null)
                {

                    profileUrls.Add(cameaProfile.Name, urlList);
                }
            }


            return profileUrls;



        }

        public async Task<Boolean> writeVideoEncodingInternalAsync(int nProfileIdx)
        {
            OnVifCameraProfile profile = getOnVifProfile(nProfileIdx);

            if (profile == null) return false;

            Boolean written = false;
            if (this.m_OnVif1Media != null)
            {
                Profile cameaProfile = getCamera1Profile(nProfileIdx);
                if (cameaProfile == null) return false;

                var videoEncoderConfiguration = await this.m_OnVif1Media.getVideoEncodingsAsync(cameaProfile.VideoEncoderConfiguration.token);
                if (videoEncoderConfiguration != null)
                {

                    videoEncoderConfiguration.Resolution.Height = profile.VideoProfileEncoding.Resolution.Height;
                    videoEncoderConfiguration.Resolution.Width = profile.VideoProfileEncoding.Resolution.Width;

                    videoEncoderConfiguration.RateControl.BitrateLimit = profile.VideoProfileEncoding.BitrateLimit;
                    videoEncoderConfiguration.RateControl.FrameRateLimit = (int) profile.VideoProfileEncoding.FrameRateLimit;
                    videoEncoderConfiguration.RateControl.EncodingInterval = profile.VideoProfileEncoding.EncodingInterval;

                    written = await this.m_OnVif1Media.SetVideoEncodingsAsync(videoEncoderConfiguration);
                    profile.IsVideoProfileChanged = !written;

                }
            }
            else if (this.m_OnVif2Media != null)
            {

                MediaProfile cameaProfile = getCamera2Profile(nProfileIdx);
                if (cameaProfile == null) return false;
                if (cameaProfile.Configurations == null) return false;
                if (cameaProfile.Configurations.VideoEncoder == null) return false;

                var videoEncoderConfiguration = await this.m_OnVif2Media.getVideoEncodingsAsync(cameaProfile.Configurations.VideoEncoder.token, cameaProfile.token);
                if (videoEncoderConfiguration != null)
                {
                    OnVifServices.OnvifMedia20.VideoEncoder2Configuration findencoding = null;
                    if (videoEncoderConfiguration.Configurations.Length > 0){
                        findencoding = videoEncoderConfiguration.Configurations[0];
                    }
 
                    if (findencoding != null)
                    {

                        findencoding.GovLength = profile.VideoProfileEncoding.GovLength;
                        findencoding.GovLengthSpecified = profile.VideoProfileEncoding.GovLengthSpecified;
                        findencoding.Resolution.Height = profile.VideoProfileEncoding.Resolution.Height;
                        findencoding.Resolution.Width = profile.VideoProfileEncoding.Resolution.Width;

                        findencoding.RateControl.ConstantBitRateSpecified = profile.VideoProfileEncoding.ConstantBitRateSpecified;
                        findencoding.RateControl.ConstantBitRate = profile.VideoProfileEncoding.ConstantBitRate;
                        findencoding.RateControl.BitrateLimit = profile.VideoProfileEncoding.BitrateLimit;
                        findencoding.RateControl.FrameRateLimit = profile.VideoProfileEncoding.FrameRateLimit;

                        findencoding.Resolution.Height = profile.VideoProfileEncoding.Resolution.Height;
                        findencoding.Resolution.Width = profile.VideoProfileEncoding.Resolution.Width;
                     
                        written = await this.m_OnVif2Media.SetVideoEncodingsAsync(findencoding);
                        profile.IsVideoProfileChanged = !written;
                    }



                }
            }



            return written;

        }

        public async Task<Boolean> writeAudioEncodingInternalAsync(int nProfileIdx)
        {

            OnVifCameraProfile profile = getOnVifProfile(nProfileIdx);

            if (profile == null) return false;

            Boolean written = false;
            if (this.m_OnVif1Media != null)
            {
                Profile cameaProfile = getCamera1Profile(nProfileIdx);
                if (cameaProfile == null) return false;
                if (cameaProfile.VideoEncoderConfiguration == null) return false;

                var AudioEncoderConfiguration = await this.m_OnVif1Media.getAudioEncodingsAsync(cameaProfile.VideoEncoderConfiguration.token);
                if (AudioEncoderConfiguration != null)
                {

                    AudioEncoderConfiguration.Bitrate = profile.AudioProfileEncoding.Bitrate;
                    AudioEncoderConfiguration.SampleRate = profile.AudioProfileEncoding.SampleRate;

                    written = await this.m_OnVif1Media.SetAudioEncodingsAsync(AudioEncoderConfiguration);
                    profile.IsVideoProfileChanged = written;

                }
            }
            else if (this.m_OnVif2Media != null)
            {

                MediaProfile cameaProfile = getCamera2Profile(nProfileIdx);
                if (cameaProfile == null) return false;
                if (cameaProfile.Configurations == null) return false;
                if (cameaProfile.Configurations.AudioEncoder == null) return false;

                var AudioEncoderConfiguration = await this.m_OnVif2Media.getAudioEncodingsAsync(cameaProfile.Configurations.AudioEncoder.token, cameaProfile.token);
                if (AudioEncoderConfiguration != null)
                {
                    OnVifServices.OnvifMedia20.AudioEncoder2Configuration findencoding = null;
                    if (AudioEncoderConfiguration.Configurations.Length > 0)
                    {
                        findencoding = AudioEncoderConfiguration.Configurations[0];
                    }

                    if (findencoding != null)
                    {
                        findencoding.Bitrate = profile.AudioProfileEncoding.Bitrate;
                        findencoding.SampleRate = profile.AudioProfileEncoding.SampleRate;
                        written = await this.m_OnVif2Media.SetAudioEncodingsAsync(findencoding);
                        profile.IsAudioProfileChanged = written;
                    }

                }
            }



            return written;

        }

        public async Task<OnVifCameraProfile> getAudioEncodingInternalAsync(int nProfileIdx)
        {
            OnVifCameraProfile profile = getOnVifProfile(nProfileIdx);
            if (profile == null) return profile;


            if (this.m_OnVif1Media != null)
            {
                Profile cameaProfile = getCamera1Profile(nProfileIdx);
                if (cameaProfile == null) return profile;
                if (cameaProfile.AudioEncoderConfiguration == null) return profile;
                var AudioEncoderConfiguration = await m_OnVif1Media.getAudioEncodingsAsync(cameaProfile.AudioEncoderConfiguration.token);
                if (AudioEncoderConfiguration != null)
                {

                    profile.AudioProfileEncoding.AudioEncoderConfiguration = AudioEncoderConfiguration;
                    profile.AudioProfileEncoding.AudioEncoder2Configuration = null;

                }

             
            }
            else if (this.m_OnVif2Media != null)
            {

                MediaProfile cameaProfile = getCamera2Profile(nProfileIdx);
                if (cameaProfile == null) return profile;

                if (cameaProfile.Configurations == null) return profile;
                if (cameaProfile.Configurations.AudioEncoder == null) return profile;

                //                var videoEncoderConfiguration = await this.m_OnVif2Media.getVideoEncodingsAsync(cameaProfile.Configurations.VideoEncoder.token);
            //    var videoEncoderConfiguration = await this.m_OnVif2Media.getVideoEncodingsAsync(cameaProfile.Configurations.VideoEncoder.token, cameaProfile.token);



                //   var AudioEncoderConfiguration = await m_OnVif2Media.getAudioEncodingsAsync(cameaProfile.Configurations.AudioEncoder.token);
                var AudioEncoderConfiguration = await m_OnVif2Media.getAudioEncodingsAsync(cameaProfile.Configurations.AudioEncoder.token, cameaProfile.token);

                
                if (AudioEncoderConfiguration != null)
                {

                    profile.AudioProfileEncoding.AudioEncoderConfiguration = null;
                    profile.AudioProfileEncoding.AudioEncoder2Configuration = AudioEncoderConfiguration.Configurations;

                }

            
            }



            return profile;


        }

        public async Task<OnVifCameraProfile> getVideoEncodingInternalAsync(int nProfileIdx)
        {
            OnVifCameraProfile profile = getOnVifProfile(nProfileIdx) ;

            if (profile == null) return profile;

  
            if (this.m_OnVif1Media != null)
            {
                Profile cameaProfile = getCamera1Profile(nProfileIdx);
                if (cameaProfile == null) return profile;
                if (cameaProfile.VideoEncoderConfiguration == null) return profile;

                var videoEncoderConfiguration = await this.m_OnVif1Media.getVideoEncodingsAsync(cameaProfile.VideoEncoderConfiguration.token);
                if (videoEncoderConfiguration != null)
                {

                    profile.VideoProfileEncoding.VideoEncoderConfiguration = videoEncoderConfiguration;
                    profile.VideoProfileEncoding.VideoEncoder2Configuration = null;


                }
            }
            else if (this.m_OnVif2Media != null)
            {

                MediaProfile cameaProfile = getCamera2Profile(nProfileIdx);
                if (cameaProfile == null) return profile;
                if (cameaProfile.Configurations == null) return profile;
                if (cameaProfile.Configurations.VideoEncoder == null) return profile;

                //                var videoEncoderConfiguration = await this.m_OnVif2Media.getVideoEncodingsAsync(cameaProfile.Configurations.VideoEncoder.token);
                var videoEncoderConfiguration = await this.m_OnVif2Media.getVideoEncodingsAsync(cameaProfile.Configurations.VideoEncoder.token, cameaProfile.token);

                if (videoEncoderConfiguration != null)
                {
                    profile.VideoProfileEncoding.VideoEncoderConfiguration = null;
                    profile.VideoProfileEncoding.VideoEncoder2Configuration = videoEncoderConfiguration.Configurations;


                }
            }
        


            return profile;


        }

        public async Task<OnVifCameraProfile> getAudioEncodingConfigurationOptionsInternalAsync(int nProfileIdx)
        {
            OnVifCameraProfile profile = getOnVifProfile(nProfileIdx);
            if (profile == null) return profile;


            if (this.m_OnVif1Media != null)
            {
                Profile cameaProfile = getCamera1Profile(nProfileIdx);
                if (cameaProfile == null) return profile;
                if (cameaProfile.AudioEncoderConfiguration == null) return profile;

                var AudioEncoderConfiguration = await m_OnVif1Media.getAudioEncodingConfigurationOptionsAsync(cameaProfile.AudioEncoderConfiguration.token);
                if (AudioEncoderConfiguration != null)
                {
                    profile.AudioProfileEncodingConfiguration.Options = AudioEncoderConfiguration.Options;
                    profile.AudioProfileEncodingConfiguration.Options2 = null; ;

                }


            }
            else if (this.m_OnVif2Media != null)
            {

                MediaProfile cameaProfile = getCamera2Profile(nProfileIdx);
                if (cameaProfile == null) return profile;
                if (cameaProfile.Configurations == null) return profile;
                if (cameaProfile.Configurations.AudioEncoder == null) return profile;
                var AudioEncoderConfiguration = await m_OnVif2Media.getAudioEncodingConfigurationOptionsAsync(cameaProfile.Configurations.AudioEncoder.token, cameaProfile.token);
                if (AudioEncoderConfiguration != null)
                {
                    profile.AudioProfileEncodingConfiguration.Options = null;
                    profile.AudioProfileEncodingConfiguration.Options2 = AudioEncoderConfiguration.Options;

                }



            }



            return profile;
        }

        public async Task<OnVifCameraProfile> getVideoEncodingConfigurationOptionsInternalAsync(int nProfileIdx)
        {
            OnVifCameraProfile profile = getOnVifProfile(nProfileIdx);

            if (profile == null) return profile;


            if (this.m_OnVif1Media != null)
            {
                Profile cameaProfile = getCamera1Profile(nProfileIdx);
                if (cameaProfile == null) return profile;
                if (cameaProfile.VideoEncoderConfiguration==null) return profile;

                var videoEncoderConfiguration = await this.m_OnVif1Media.getVideoEncodingConfigurationOptionsAsync(cameaProfile.VideoEncoderConfiguration.token);
                if (videoEncoderConfiguration != null)
                {
                    profile.VideoProfileEncodingConfiguration.VideoEncoder2ConfigurationOptions = null;
                    profile.VideoProfileEncodingConfiguration.VideoEncoderConfigurationOptions = videoEncoderConfiguration;
                    profile.VideoProfileEncodingConfiguration = profile.VideoProfileEncodingConfiguration; // for updating RaisePropertyChanged
              

                }
            }
            else if (this.m_OnVif2Media != null)
            {

                MediaProfile cameaProfile = getCamera2Profile(nProfileIdx);
                if (cameaProfile == null) return profile;
                if (cameaProfile.Configurations == null) return profile;
                if (cameaProfile.Configurations.VideoEncoder == null) return profile;
                var videoEncoderConfiguration = await this.m_OnVif2Media.getVideoEncodingConfigurationOptionsAsync(cameaProfile.Configurations.VideoEncoder.token, cameaProfile.token);
                if (videoEncoderConfiguration != null)
                {
                    profile.VideoProfileEncodingConfiguration.VideoEncoder2ConfigurationOptions = videoEncoderConfiguration.Options;
                    profile.VideoProfileEncodingConfiguration.VideoEncoderConfigurationOptions = null;
                    profile.VideoProfileEncodingConfiguration = profile.VideoProfileEncodingConfiguration; // for updating RaisePropertyChanged

                }
            }



            return profile;

        }

        public async Task<bool> InitalizeAsync()
        {

            m_binitalized = false;

            if (this.m_onVifDevice.Device == null)
            {
                bool b = await this.m_onVifDevice.InitalizeDeviceAsync();
                if (this.m_onVifDevice.Device == null) return false;
            }
            if (this.m_onVifDevice.ServicesResponse==null)
            {
                bool b = await this.m_onVifDevice.InitalizeDeviceAsync();
                if (this.m_onVifDevice.ServicesResponse == null) return false;
            }

            try
            {
                bool bMedia1Present = false;
                bool bMedia2Present = false;
                foreach (var service in this.m_onVifDevice.ServicesResponse.Service)
                {


                    if (service.Namespace == "http://www.onvif.org/ver10/media/wsdl")
                    {

                        bMedia1Present = true;
                    

                    }

                    if (service.Namespace == "http://www.onvif.org/ver20/media/wsdl")
                    {
                        bMedia2Present = true;
 


                    }

                }

                if (bMedia2Present)
                {
                    this.m_ProfileEncodings.Clear();
                    m_OnVif2Media = new OnVifServices.OnVif2Media(this.m_onVifDevice);
                    m_binitalized = await m_OnVif2Media.InitalizeAsync();
                }
                else
                if (bMedia1Present)
                {
                    this.m_ProfileEncodings.Clear();
                    m_OnVif1Media = new OnVifServices.OnVif1Media(this.m_onVifDevice);
                    m_binitalized = await m_OnVif1Media.InitalizeAsync();

                }
       
                createProfiles();
            }
            catch (Exception ex)
            {
               m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifMedia.InitalizeAsync", ex);
          //      return false;
            }

            return m_binitalized;
        }

           
    }
  
}
