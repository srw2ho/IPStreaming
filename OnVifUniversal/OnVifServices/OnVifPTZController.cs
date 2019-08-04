using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OnVifServices.OnvifMedia10;

using OnVifServices.OnvifPTZService;
using System.Net;
using System.ServiceModel;
using System.ServiceModel.Channels;
using Windows.System.Threading;


namespace OnVifServices
{
    public class OnVifPTZController: OnVifServiceBase
    {
        private enum Direction { None, Up, Down, Left, Right, LeftUp, LeftDown, RightUp, RightDown, ZoomIn, ZoomOut };


   //     OnVifDevice m_onVifDevice;

        OnvifPTZService.PTZSpeed m_velocity;
        PTZVector m_vector;
        PTZConfigurationOptions m_options;
        bool m_relative = false;
   //     bool m_initialised = false;
        Direction m_direction;
        float m_panDistance;
        float m_tiltDistance;

        PTZClient m_ptzClient;


        GetConfigurationsResponse m_configs;

   //     String m_ErrorMessage;
  //      OnVifServices.OnvifMedia10.MediaClient m_mediaClient;
     //   OnVifServices.OnvifMedia20.Media2Client m_media2Client;
        //     Media10Services m_media10Services;

        GetPresetsResponse m_Presets;
        OnvifPTZService.Capabilities m_capabilities;

        ThreadPoolTimer m_Timer;
        bool m_bMoveActiv;

        double m_timeinmsec;
        bool m_doContineousMode;
        string m_ProfileRefToken;
 //       private OnVifDevice onVifDevice;
        //       private IOnVifMedia onVifMedia;

      
        public string ProfileRefToken { set { m_ProfileRefToken = value; } }


        public  OnVifPTZController(OnVifDevice OnVifDevice):base (OnVifDevice)
        {
       //     m_onVifDevice = OnVifDevice;

     //       m_mediaClient = null;
            //m_profile = null;

            m_Timer = null;
            m_velocity = null;
            m_vector = null;
            m_options = null;
            m_relative = false;
            m_initialised = false;
            m_direction = m_direction = Direction.None;
            m_Presets = null;
            m_capabilities = null;
            m_ptzClient = null;
            m_ErrorMessage = "";
            m_configs = null;
            m_initialised = false;
            m_panDistance = 0;
            m_tiltDistance = 0;
            m_bMoveActiv = false;
            m_timeinmsec = 200;
            m_doContineousMode = false;
            m_ProfileRefToken = "";

        }


        public int PanIncrements { get; set; } = 5;

        public int TiltIncrements { get; set; } = 5;
        public int ZoomIncrements { get; set; } = 10;

        public GetPresetsResponse PresetsResponse { get { return m_Presets; } }


   //     public bool IsInitialized { get { return m_initialised; } }



        public override async Task<bool> InitalizeAsync()
        {
            if (this.m_onVifDevice.ServicesResponse == null)
            {
                bool b = await this.m_onVifDevice.InitalizeDeviceAsync();
                if (this.m_onVifDevice.ServicesResponse == null) return false;
            }
      
            try
            {
                foreach (var service in this.m_onVifDevice.ServicesResponse.Service)
                {


                    if (service.Namespace == "http://www.onvif.org/ver20/ptz/wsdl")
                    {
                        string serviceAdress = service.XAddr;
                        m_ErrorMessage = "";

                  //      m_profile = this.m_media10Services.ProfilesResponse.Profiles[0];

                        HttpTransportBindingElement httpBinding = new HttpTransportBindingElement();
                        httpBinding.AuthenticationScheme = AuthenticationSchemes.Digest;
                        var messageElement = new TextMessageEncodingBindingElement();
                        messageElement.MessageVersion = MessageVersion.CreateVersion(EnvelopeVersion.Soap12, AddressingVersion.None);
                        CustomBinding binding = new CustomBinding(messageElement, httpBinding);


                        this.m_ptzClient = new PTZClient(binding, new EndpointAddress(serviceAdress));


                        /*
                     while (m_ptzClient.Endpoint.EndpointBehaviors.Count > 0)
                     {
                         m_ptzClient.Endpoint.EndpointBehaviors.RemoveAt(0);
                     }
                     */



            m_ptzClient.Endpoint.EndpointBehaviors.Add(m_onVifDevice.GetBasicBasicAuthBehaviour);
                        m_ptzClient.Endpoint.EndpointBehaviors.Add(m_onVifDevice.GetPasswordDigestBehavior);


                        m_configs = await m_ptzClient.GetConfigurationsAsync();

                        m_options = await m_ptzClient.GetConfigurationOptionsAsync(m_configs.PTZConfiguration[0].token);



                        m_capabilities = await m_ptzClient.GetServiceCapabilitiesAsync();

                        m_velocity = new OnvifPTZService.PTZSpeed()
                        {
                            PanTilt = new OnvifPTZService.Vector2D()
                            {
                                x = 0,
                                y = 0,
                                space = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].URI,
                            }
                            ,
                            Zoom = new OnvifPTZService.Vector1D()
                            {
                                x = 0,
                                space = m_options.Spaces.ContinuousZoomVelocitySpace[0].URI,
                            }

                        };


                        m_vector = new PTZVector()
                        {
                            PanTilt = new OnvifPTZService.Vector2D()
                            {
                                x = 0,
                                y = 0,
                                space = m_options.Spaces.RelativePanTiltTranslationSpace[0].URI
                            }
                        };

                        m_initialised = true;

                        m_bMoveActiv = false;



                        if (m_relative)
                        {
                            m_velocity.PanTilt.space = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].URI;
                            m_velocity.PanTilt.space = m_configs.PTZConfiguration[0].DefaultPTZSpeed.PanTilt.space;
                            m_panDistance = (m_options.Spaces.RelativePanTiltTranslationSpace[0].XRange.Max -
                            m_options.Spaces.RelativePanTiltTranslationSpace[0].XRange.Min) / PanIncrements;
                            m_tiltDistance = (m_options.Spaces.RelativePanTiltTranslationSpace[0].YRange.Max -
                            m_options.Spaces.RelativePanTiltTranslationSpace[0].YRange.Min) / TiltIncrements;

                        }

                        break;

                    }
                }
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifPTZController.InitalizeAsync", ex);
         //       return false;
            }
            return m_initialised;

        }

        public async Task<GetPresetsResponse> readPTZPresetsAsync()
        {
            try
            {
                if (m_configs == null) return null;
                m_Presets = await m_ptzClient.GetPresetsAsync(m_ProfileRefToken);

                return m_Presets;
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifPTZController.readPTZPresetsAsync", ex);
  //              return null;
            }

        }




        protected string getfreePresetName()
        {
            if (m_Presets == null) return "";

            string presetName = "";
            int j = 1;
            int count = 100;
            while (count >= 0)
            {
                presetName = String.Format("Preset{0}", j);
                bool bTokenFound = false;

                for (int i = 0; i < m_Presets.Preset.Length; i++)
                {
                    var preset = m_Presets.Preset[i];
                    if (preset.Name == presetName)
                    {
                        bTokenFound = true;
                        break;
                    }

                }
                if (!bTokenFound)
                {
                    return presetName;
                }

                j++;
            }


            return "";



        }

        protected string getfreePresetToken()
        {
            if (m_Presets == null) return "";

            string preseToken = "";
            int j = 1;
            int count = 100;
            while (count >= 0)
            {
                preseToken = String.Format("{0}", j);
                bool bTokenFound = false;

                for (int i = 0; i < m_Presets.Preset.Length; i++)
                {
                    var preset = m_Presets.Preset[i];
                    
                    if (preset.token == preseToken)
                    {
                        bTokenFound = true;
                        break;
                 
                    }

                }
                if (!bTokenFound)
                {
                    return preseToken;
                }

                j++;
            }

            return "";

        }

        public async Task<Boolean> SetHomePositionAsync()
        {
            try
            {
                if (m_configs == null) return false;
                await m_ptzClient.SetHomePositionAsync(m_ProfileRefToken);
                return true;

            }

            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifPTZController.SetHomePositionAsync", ex);
   //             return false;
            }

        }
        public async Task<Boolean> GotoHomePositionAsync()
        {
            try
            {
                if (m_configs == null) return false;
                await m_ptzClient.GotoHomePositionAsync(m_ProfileRefToken, null);
                return true;

            }

            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifPTZController.GotoHomePositionAsync", ex);
    //            return false;
            }

        }


        public PTZPreset getPTZByToken(string preseToken)
        {
            if (m_Presets == null) return null;
            for (int i = 0; i < m_Presets.Preset.Length; i++)
            {
                var preset = m_Presets.Preset[i];

                if (preset.token == preseToken)
                {
                    return preset;
                }
            }

            return null;

        }

        public async Task<Boolean> GotoPresetAsync(string presettoken)
        {
            try
            {
                if (m_Presets == null)
                {
                    m_Presets = await readPTZPresetsAsync();
                    if (m_Presets == null) return false;
                }
                PTZPreset preset = getPTZByToken(presettoken);
                if (preset != null)
                {
                    await m_ptzClient.GotoPresetAsync(m_ProfileRefToken, preset.token, null);
                    return true;
                }
               
                return false;

            }

            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifPTZController.GotoPresetAsync", ex);
//                return false;
            }



        }

        public async Task<Boolean> RemovePresetAsync(string preseToken)
        {
            try
            {

   
                if (m_Presets == null)
                {
                    m_Presets = await readPTZPresetsAsync();
                    if (m_Presets == null) return false;
                }
                PTZPreset preset = getPTZByToken(preseToken);

                if (preset != null)
                {
 
                    await m_ptzClient.RemovePresetAsync(m_ProfileRefToken, preset.token);
                    return true;
                }
                return false;
            }

            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifPTZController.RemovePresetAsync", ex);
 //               return false;
            }

        }


        public async Task<Boolean> setPTZPresetAsync(string preseToken)
        {
            try
            {

                if (m_Presets == null)
                {
                    m_Presets = await readPTZPresetsAsync();
                    if (m_Presets == null) return false;
                }
                PTZPreset preset = getPTZByToken(preseToken);

                OnVifServices.OnvifPTZService.SetPresetRequest request = null;

                if (preset != null)
                {
                    request = new SetPresetRequest(m_ProfileRefToken, preset.Name, preset.token);
                }
                else
                {
                    string presettoken = getfreePresetToken();
                    string presetname = getfreePresetName();
                    request = new SetPresetRequest(m_ProfileRefToken, presetname, presettoken);
                }

                if (request != null)
                {
                    SetPresetResponse presetResponse = await m_ptzClient.SetPresetAsync(request);
                    return true;
                }
                return false;
      
            }

      
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifPTZController.setPTZPresetAsync", ex);
   //             return false;
            }


        }

        protected void startTimer(double timeinmsec)
        {
            if (m_initialised)
             {
                if (m_Timer == null)
                {
                    m_Timer = Windows.System.Threading.ThreadPoolTimer.CreatePeriodicTimer(new TimerElapsedHandler(DoMotion), TimeSpan.FromMilliseconds(timeinmsec));
                }

            }
        }

        public async Task StopAsync()
        {
            try
            {
                if (m_initialised)
                {
                    stoppTimer();
                    m_direction = Direction.None;
                    await m_ptzClient.StopAsync(m_ProfileRefToken, true, true);
                }
                m_bMoveActiv = false;
            }
            catch (Exception ex)
            {
                m_bMoveActiv = false;
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifPTZController.StopAsync", ex);
          //      return;
            }

        }



        public void stoppTimer()
        {
            if (m_Timer != null)
            {
                m_Timer.Cancel();
                m_Timer = null;
            }

        }


        public void PanLeftTiltUp()
        {
            if (m_initialised)
            {
           //     if (m_relative)
                {
                    m_direction = Direction.LeftUp;
                    DoMove();
                    if (m_doContineousMode) startTimer(m_timeinmsec);
                }

            }
        }
        public void PanLeftTiltDown()
        {
            if (m_initialised)
            {
  //              if (m_relative)
                {
                    m_direction = Direction.LeftDown;
                    DoMove();
                    if (m_doContineousMode) startTimer(m_timeinmsec);
                }

            }
        }
        public void PanRightTiltDown()
        {
            if (m_initialised)
            {
 //               if (m_relative)
                {
                    m_direction = Direction.RightDown;
                    DoMove();
                    if (m_doContineousMode) startTimer(m_timeinmsec);
                }

            }
        }
        public void PanRightTiltUp()
        {
            if (m_initialised)
            {
  //              if (m_relative)
                {
                    m_direction = Direction.RightUp;
                    DoMove();
                    if (m_doContineousMode) startTimer(m_timeinmsec);
                }

            }
        }

        public void PanLeft()
        {
            if (m_initialised)
            {
    //            if (m_relative)
                {
                    m_direction = Direction.Left;

                    DoMove();
                    if (m_doContineousMode) startTimer(m_timeinmsec);
                }
             
            }
        }
        public void PanRight()
        {
            if (m_initialised)
            {
  //              if (m_relative)
                {
                    m_direction = Direction.Right;
                    DoMove();
                    if (m_doContineousMode) startTimer(m_timeinmsec);
                }

            }
        }

        public void TiltUp()
        {
            if (m_initialised)
            {
     //           if (m_relative)
                {
                    m_direction = Direction.Up;
                    DoMove();
                    if (m_doContineousMode) startTimer(m_timeinmsec);
                }

            }
        }

        public void TiltDown()
        {
            if (m_initialised)
            {
  //              if (m_relative)
                {
                    m_direction = Direction.Down;
                    DoMove();
                    if (m_doContineousMode) startTimer(m_timeinmsec);
                }

            }
        }
        public void ZoomIn()
        {
            if (m_initialised)
            {
                //              if (m_relative)
                {
                    m_direction = Direction.ZoomIn;
                    DoMove();
                    if (m_doContineousMode) startTimer(m_timeinmsec);
                }

            }
        }
        public void ZoomOut()
        {
            if (m_initialised)
            {
                //              if (m_relative)
                {
                    m_direction = Direction.ZoomOut;
                    DoMove();
                    if (m_doContineousMode) startTimer(m_timeinmsec);
                }

            }
        }



        async private void DoMove()
        {
            if (m_bMoveActiv) return;
            if (m_ptzClient == null) return;
            try
            {


            m_bMoveActiv = true;
            bool move = true;

            switch (m_direction)
            {
                    //   LeftUp, LeftDown, RightUp, RightDown
                    case Direction.LeftUp:
                        m_velocity.PanTilt.x = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].XRange.Min / PanIncrements;
                        m_velocity.PanTilt.y = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].YRange.Max / TiltIncrements;
                        m_vector.PanTilt.x = -m_panDistance;
                        m_vector.PanTilt.y = m_tiltDistance;
                        m_velocity.Zoom.x = 0;
                        break;
                    case Direction.LeftDown:
                        m_velocity.PanTilt.x = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].XRange.Min / PanIncrements; 
                        m_velocity.PanTilt.y = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].YRange.Min / TiltIncrements; 
                        m_vector.PanTilt.x = -m_panDistance;
                        m_vector.PanTilt.y = -m_tiltDistance;
                        m_velocity.Zoom.x = 0;
                        break;

                    case Direction.RightUp:
                        m_velocity.PanTilt.x = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].XRange.Max / PanIncrements;
                        m_velocity.PanTilt.y = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].YRange.Max / TiltIncrements;
                        m_vector.PanTilt.x = m_panDistance;
                        m_vector.PanTilt.y = m_tiltDistance;
                        m_velocity.Zoom.x = 0;
                        break;
                    case Direction.RightDown:
                        m_velocity.PanTilt.x = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].XRange.Max / PanIncrements;
                        m_velocity.PanTilt.y = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].YRange.Min / TiltIncrements;
                        m_vector.PanTilt.x = m_panDistance;
                        m_vector.PanTilt.y = -m_tiltDistance;
                        m_velocity.Zoom.x = 0;
                        break;
                    case Direction.Up:
                        m_velocity.PanTilt.x = 0;
                        m_velocity.PanTilt.y = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].YRange.Max / TiltIncrements;
                        m_vector.PanTilt.x = 0;
                        m_vector.PanTilt.y = m_tiltDistance;
                        m_velocity.Zoom.x = 0;
                        break;

                    case Direction.Down:
            
                        m_velocity.PanTilt.x = 0;
                        m_velocity.PanTilt.y = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].YRange.Min / TiltIncrements;
                        m_vector.PanTilt.x = 0;
                        m_vector.PanTilt.y = -m_tiltDistance;
                        m_velocity.Zoom.x = 0;
                        break;

                case Direction.Left:
                    m_velocity.PanTilt.x = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].XRange.Min / PanIncrements;
                    m_velocity.PanTilt.y = 0;
                    m_vector.PanTilt.x = -m_panDistance;
                    m_vector.PanTilt.y = 0;
                    m_velocity.Zoom.x = 0;
                    break;

                case Direction.Right:
                    m_velocity.PanTilt.x = m_options.Spaces.ContinuousPanTiltVelocitySpace[0].XRange.Max / PanIncrements;
                    m_velocity.PanTilt.y = 0;
                    m_vector.PanTilt.x = m_panDistance;
                    m_vector.PanTilt.y = 0;
                    m_velocity.Zoom.x = 0;


                    break;

               case Direction.ZoomIn:
                   m_velocity.PanTilt.x = 0;
                   m_velocity.PanTilt.y = 0;
                   m_vector.PanTilt.x = 0;
                   m_vector.PanTilt.y = 0;
                        
                   m_velocity.Zoom.x = m_options.Spaces.ContinuousZoomVelocitySpace[0].XRange.Max / ZoomIncrements; ;
                  //      m_velocity.Zoom.x = 0.5f;
                   break;

               case Direction.ZoomOut:
                   m_velocity.PanTilt.x = 0;
                   m_velocity.PanTilt.y = 0;
                   m_vector.PanTilt.x = 0;
                   m_vector.PanTilt.y = 0;
                   m_velocity.Zoom.x = m_options.Spaces.ContinuousZoomVelocitySpace[0].XRange.Min / ZoomIncrements; ;
       //                 m_velocity.Zoom.x = -0.5f;
                   break;

                case Direction.None:
                default:
                    move = false;
                    break;
            }
            if (move)
            {
                    //   await m_ptzClient.RelativeMoveAsync(m_profile.token, m_vector, m_velocity);
                    if (!m_relative)
                    {

       //                 await m_ptzClient.ContinuousMoveAsync(m_profile.token, m_velocity, m_options.PTZTimeout.Min);
                        await m_ptzClient.ContinuousMoveAsync(m_ProfileRefToken, m_velocity, m_options.PTZTimeout.Min);
             //           await this.StopAsync();
                    }
                    else
                    {
                        await m_ptzClient.RelativeMoveAsync(m_ProfileRefToken, m_vector, null);
                    }

            }

            m_bMoveActiv = false;
            return;
            }
            catch (Exception ex)
            {
                m_bMoveActiv = false;
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifPTZController.DoMove", ex);
           //     return ;
            }
            
        }

        private  void DoMotion(ThreadPoolTimer timer)
        {
            DoMove();
            return;
        }

    }
}
