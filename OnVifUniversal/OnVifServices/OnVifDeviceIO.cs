using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using OnVifServices.OnvifMedia10;
using OnVifServices.OnvifDevice;
using OnVifServices.OnvifEvent10;

using System.Net;
using System.ServiceModel;
using System.ServiceModel.Channels;
using Windows.System.Threading;
using OnVifServices.OnvifDeviceIO10;

namespace OnVifServices
{
    public sealed  class OnVifDeviceIO: OnVifServiceBase
    {


  //      OnVifDevice m_onVifDevice;

//        String m_ErrorMessage;

  //      bool m_initialised;
        private readonly TimeSpan m_subscriptionTerminationTime;
        DeviceIOPortClient m_DeviceIOPortClient;

        public event EventHandler<DeviceRelaisEvent> EventReceived;

        public OnVifDeviceIO(OnVifDevice onVifDevice):base(onVifDevice)

        {
         //   m_ErrorMessage = "";

        //    m_onVifDevice = onVifDevice;

        //    m_initialised = false;
            m_subscriptionTerminationTime = TimeSpan.FromSeconds(20);
            m_DeviceIOPortClient = null;
        }

        private void OnEventReceived(DeviceRelaisEvent @event)
        {
            if (EventReceived!=null)
            {
                EventReceived.Invoke(this, @event);
            }

        }

        public async Task PullInputsAsync(System.Threading.CancellationToken cancellationToken)
        {


            if (m_DeviceIOPortClient == null)
            {

                return;
            }
            OnvifDevice.Capabilities capabilities = await this.m_onVifDevice.GetDeviceCapabilitiesAsync();

            if (capabilities.Device.IO.InputConnectors == 0) return;


            try
            {




                while (!cancellationToken.IsCancellationRequested)
                {

                    OnVifServices.OnvifDeviceIO10.GetDigitalInputsResponse inputresponse = await m_DeviceIOPortClient.GetDigitalInputsAsync();

                    foreach (var digital in inputresponse.DigitalInputs)
                    {
                        bool idlestate = digital.IdleStateSpecified;
                        if (digital.IdleState == OnvifDeviceIO10.DigitalIdleState.closed)
                        {

                
                        }
                        if (digital.IdleState == OnvifDeviceIO10.DigitalIdleState.open)
                        {

                  
                        }

                        {

                            var @event = new DeviceRelaisEvent();
                            OnEventReceived(@event);

                        }


                    }


     



                }

            }

            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;

            }

        }

        public async Task PullRelaisOutputsAsync(System.Threading.CancellationToken cancellationToken)
        {


            if (m_DeviceIOPortClient == null)
            {

                return;
            }
            OnvifDevice.Capabilities capabilities = await this.m_onVifDevice.GetDeviceCapabilitiesAsync();

            try
            {




                while (!cancellationToken.IsCancellationRequested)
                {
                    
                    OnVifServices.OnvifDeviceIO10.GetDigitalInputsResponse inputresponse = await m_DeviceIOPortClient.GetDigitalInputsAsync();




             
                    OnVifServices.OnvifDeviceIO10.GetRelayOutputsResponse1 relayresponse = await m_DeviceIOPortClient.GetRelayOutputsAsync();

                    foreach (var digital in inputresponse.DigitalInputs)
                    {
                        if (digital.IdleState == OnvifDeviceIO10.DigitalIdleState.closed)
                        {

                        }
                        if (digital.IdleState == OnvifDeviceIO10.DigitalIdleState.open)
                        {

        
                        }

                        {

                            var @event = new DeviceRelaisEvent();
                            OnEventReceived(@event);

                        }


                    }


                    foreach (var relay in relayresponse.RelayOutputs)
                    {
                        //    RelayLogicalState logigalstate = RelayLogicalState.active;

                        //          if (relay.. == OnvifMedia10.R.closed)

                        OnVifServices.OnvifDeviceIO10.GetRelayOutputOptionsResponse relayoptions = await m_DeviceIOPortClient.GetRelayOutputOptionsAsync(relay.token);

                        if (relay.Properties.IdleState == OnvifDeviceIO10.RelayIdleState.open)
                        {
                            OnVifServices.OnvifDeviceIO10.SetRelayOutputStateResponse resp = await m_DeviceIOPortClient.SetRelayOutputStateAsync(relay.token, OnvifDeviceIO10.RelayLogicalState.inactive);
           

                            var @event = new DeviceRelaisEvent();
                            OnEventReceived(@event);
                           
                        }
                        if (relay.Properties.IdleState == OnvifDeviceIO10.RelayIdleState.closed)
                        {
                            OnVifServices.OnvifDeviceIO10.SetRelayOutputStateResponse resp1 = await m_DeviceIOPortClient.SetRelayOutputStateAsync(relay.token, OnvifDeviceIO10.RelayLogicalState.active);

                            var @event = new DeviceRelaisEvent();
                            OnEventReceived(@event);
           



                        }


                    }
              


                }

            }

            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;

            }

        }

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


                    if (service.Namespace == "http://www.onvif.org/ver10/deviceIO/wsdl")
                    {

                        string serviceAdress = service.XAddr;

                        HttpTransportBindingElement httpBinding = new HttpTransportBindingElement();
                        httpBinding.AuthenticationScheme = AuthenticationSchemes.Digest;
                        var messageElement = new TextMessageEncodingBindingElement();
                        messageElement.MessageVersion = MessageVersion.CreateVersion(EnvelopeVersion.Soap12, AddressingVersion.WSAddressing10);
                        CustomBinding binding = new CustomBinding(messageElement, httpBinding);
                        m_DeviceIOPortClient = new DeviceIOPortClient(binding, new EndpointAddress(serviceAdress));
                        m_DeviceIOPortClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetPasswordDigestBehavior);
                        m_DeviceIOPortClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetBasicBasicAuthBehaviour);
                        m_initialised = true;
                    }

                }
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                throw new OnVifException("OnVifDeviceIO.InitalizeAsync", ex);
           //     return false;
            }

            return m_initialised;
        }
    }
}
