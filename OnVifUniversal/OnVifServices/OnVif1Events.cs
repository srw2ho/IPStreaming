using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using OnVifServices.OnvifMedia10;
using OnVifServices.OnvifDevice;
using OnVifServices.OnvifEvent10;

using System.Net;
using System.ServiceModel;
using System.ServiceModel.Channels;
using Windows.System.Threading;
using System.Threading.Tasks;

namespace OnVifServices
{

    public class EventArgs<T> : EventArgs
    {
        public EventArgs(T data)
        {
            Data = data;
        }
        public T Data { get; set; }
    }

    // [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single, ConcurrencyMode = ConcurrencyMode.Single, AddressFilterMode = AddressFilterMode.Any)]
    public class NotificationConsumerService : OnvifEvent10.NotificationConsumer
    {
        public event EventHandler<EventArgs<OnvifEvent10.Notify1>> NewNotification;

        public void Notify(OnvifEvent10.Notify1 request)
        {
            var threadSafeEventHandler = NewNotification;
            if (threadSafeEventHandler != null)
                threadSafeEventHandler.Invoke(this, new EventArgs<OnvifEvent10.Notify1>(request));
        }

        public Task NotifyAsync(OnvifEvent10.Notify1 request)
        {
            return new Task(() =>
            {
                //return something?
            });
        }
    }


    public class OnVif1Events : OnVifServiceBase
    {

        EventPortTypeClient m_EventPortTypeClient;
        //     OnVifDevice m_onVifDevice;
        CreatePullPointSubscriptionResponse m_PullPointResponse;
        GetEventPropertiesResponse m_EventPropertiesResponse;
        //     String m_ErrorMessage;
        PullPointSubscriptionClient m_PullPointSubscriptionClient;
        SubscriptionManagerClient m_SubscriptionManagerClient;
        //     bool m_initialised;
        private readonly TimeSpan m_subscriptionTerminationTime;


        public event EventHandler<DeviceRelaisEvent> EventReceived;

  

        public OnVif1Events(OnVifDevice onVifDevice):base(onVifDevice)
        {
         //   m_ErrorMessage = "";
            m_PullPointResponse = null;
         //   m_onVifDevice = onVifDevice;
            m_EventPropertiesResponse = null;
            m_EventPortTypeClient = null;
            
            m_PullPointSubscriptionClient = null;
            m_SubscriptionManagerClient = null;
     //       m_initialised = false;
            m_subscriptionTerminationTime = TimeSpan.FromSeconds(20);
        }

        public async Task PullPointAsync(System.Threading.CancellationToken cancellationToken)
        {


            if (m_PullPointSubscriptionClient==null)
            {

            }
            //   var pullPointSubscriptionClient = _onvifClientFactory.CreateClient<PullPointSubscription>(endPointAddress, _connectionParameters,
            //           MessageVersion.Soap12WSAddressing10);
            //   var subscriptionManagerClient = _onvifClientFactory.CreateClient<SubscriptionManager>(endPointAddress, _connectionParameters,
            //       MessageVersion.Soap12WSAddressing10);

            try
            {


            var pullRequest = new PullMessagesRequest("PT1S", 1024, null);
            pullRequest.MessageLimit = 1024;
            pullRequest.Timeout = "PT1S"; 


            //      int renewIntervalMs = (int)(m_subscriptionTerminationTime.TotalMilliseconds / 2);
            //      int lastTimeRenewMade = Environment.TickCount;


            TimeUtil timeutil = new TimeUtil(m_subscriptionTerminationTime.TotalMilliseconds / 2);

            while (!cancellationToken.IsCancellationRequested)
            {
                PullMessagesResponse response = await m_PullPointSubscriptionClient.PullMessagesAsync(pullRequest);

                foreach (var messageHolder in response.NotificationMessage)
                {
                    if (messageHolder.Message == null) continue;

                    string var = messageHolder.Message.Value;
                    var @event = new DeviceEvent(messageHolder.Message.Value);
                    OnEventReceived(@event);
                }

                
                
                if (timeutil.IsTimeover())
                {
                    timeutil.Time = System.DateTime.Now;
               //     lastTimeRenewMade = Environment.TickCount;
                    var renew = new Renew { TerminationTime = GetTerminationTime() };
               //     var renewrequ = new RenewRequest();
               //     renewrequ.Renew = renew;
                    await m_SubscriptionManagerClient.RenewAsync(renew);
                }
                
            }

            //new UnsubscribeRequest(new Unsubscribe())
            OnVifServices.OnvifEvent10.Unsubscribe unsub = new Unsubscribe();
            await m_SubscriptionManagerClient.UnsubscribeAsync(unsub);
            }

            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
         
            }

        }

        private void OnEventReceived(DeviceEvent @event)
        {
            throw new NotImplementedException();
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


                    if (service.Namespace == "http://www.onvif.org/ver10/events/wsdl")
                    {


                        

                        string serviceAdress = service.XAddr;

                        HttpTransportBindingElement httpBinding = new HttpTransportBindingElement();
                        httpBinding.AuthenticationScheme = AuthenticationSchemes.Digest;
                        var messageElement = new TextMessageEncodingBindingElement();
                        messageElement.MessageVersion = MessageVersion.CreateVersion(EnvelopeVersion.Soap12, AddressingVersion.WSAddressing10);
                        CustomBinding binding = new CustomBinding(messageElement, httpBinding);




                        /*
                        this.m_EventPortTypeClient = new EventPortTypeClient(binding, new EndpointAddress($"http://{ this.m_onVifDevice.Hostname}/onvif/event_service"));
                        this.m_EventPortTypeClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetPasswordDigestBehavior);
                        CreatePullPointSubscriptionRequest request = new CreatePullPointSubscriptionRequest();
                        request.InitialTerminationTime = "PT60S";
                        request.SubscriptionPolicy = new CreatePullPointSubscriptionSubscriptionPolicy();
                      

                        m_PullPointResponse = await this.m_EventPortTypeClient.CreatePullPointSubscriptionAsync(request);


           

                        PullMessagesRequest message = new PullMessagesRequest("PT1S", 1024, null);
                  
                        await m_PullPointSubscriptionClient.PullMessagesAsync(message);

                        GetEventPropertiesRequest eventrequest = new GetEventPropertiesRequest();
                        m_EventPropertiesResponse =await this.m_EventPortTypeClient.GetEventPropertiesAsync(eventrequest);
                        */
                        OnvifDevice.Capabilities response = await this.m_onVifDevice.GetDeviceCapabilitiesAsync();


             

                        NotificationConsumerClient consclient = new NotificationConsumerClient(binding, new EndpointAddress(serviceAdress));

                        consclient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetPasswordDigestBehavior);
                        consclient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetBasicBasicAuthBehaviour);

                        OnvifEvent10.Notify request = new OnvifEvent10.Notify();
                      


                        //  await consclient.NotifyAsync(request);




                        //     await consclient.OpenAsync();

                        NotificationConsumerService _notificationConsumerService = new NotificationConsumerService();
                //        _notificationConsumerService.NewNotification += _notificationConsumerService_NewNotification;
                 //       ServiceHost _notificationConsumerServiceHost = new ServiceHost(_notificationConsumerService, new Uri("http://localhost:8085/NotificationConsumerService"));
                 //       _notificationConsumerServiceHost.Open();


                        NotificationProducerClient client = new NotificationProducerClient(binding, new EndpointAddress(serviceAdress));
                        client.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetPasswordDigestBehavior);
                        client.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetBasicBasicAuthBehaviour);
   
                        var subScribe = new Subscribe()
                        {
                            ConsumerReference = new EndpointReferenceType
                            {
                                Address = new AttributedURIType { Value = consclient.Endpoint.Address.Uri.ToString() },

                            }

                        };
      

                        subScribe.InitialTerminationTime = "PT2H";
                        SubscribeResponse1 resp = await client.SubscribeAsync(subScribe);
         




                        if ((response.Events != null) && response.Events.WSSubscriptionPolicySupport)
                        {

                            m_PullPointSubscriptionClient = new PullPointSubscriptionClient(binding, new EndpointAddress(serviceAdress));
                            m_SubscriptionManagerClient = new SubscriptionManagerClient(binding, new EndpointAddress(serviceAdress));

                            m_PullPointSubscriptionClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetPasswordDigestBehavior);
                            m_PullPointSubscriptionClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetBasicBasicAuthBehaviour);
                            m_SubscriptionManagerClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetPasswordDigestBehavior);
                            m_SubscriptionManagerClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetBasicBasicAuthBehaviour);
                            return true;
                        }
                        else
                        {
                        //    throw new DeviceEventReceiverException("Device doesn't support pull point subscription");

                        }


                        if ((response.Events != null) && response.Events.WSPullPointSupport)
                        {
          
                            m_PullPointSubscriptionClient = new PullPointSubscriptionClient(binding, new EndpointAddress(serviceAdress));
                            m_SubscriptionManagerClient = new SubscriptionManagerClient(binding, new EndpointAddress(serviceAdress));

                            m_PullPointSubscriptionClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetPasswordDigestBehavior);
                            m_PullPointSubscriptionClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetBasicBasicAuthBehaviour);
                            m_SubscriptionManagerClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetPasswordDigestBehavior);
                            m_SubscriptionManagerClient.Endpoint.EndpointBehaviors.Add(this.m_onVifDevice.GetBasicBasicAuthBehaviour);
                            return true;
                        }
                        else
                        {
                           throw new DeviceEventReceiverException("Device doesn't support pull point subscription");
              
                        }

      
      

                    }
     
                }
            }
            catch (Exception ex)
            {
                m_ErrorMessage = ex.Message;
                return false;
            }

            return m_initialised;
        }

        private string GetTerminationTime()
        {
            return $"PT{(int)m_subscriptionTerminationTime.TotalSeconds}S";
        }
    }
}
