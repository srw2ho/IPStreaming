using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceModel.Discovery;
using System.Text;
using System.Threading.Tasks;

namespace OnVifDiscovery
{
    public class DiscoveryEvent
    {
        public enum EventTypes { startEvent, endEvent, dataEvent };

 
        public EventTypes EventType
        { 
            get { return m_EventType; }
            set { m_EventType = value; }
        }
        public string Url
        {
            get { return m_url; }
            set { m_url = value; }
        }

        public DiscoveryEvent( string url, EventTypes eventType)
        {
                m_EventType = eventType;
                m_url = url;

        }
       
        private string m_url;
        private EventTypes m_EventType;
    }

  //  public delegate void RecoveryHandler(Object obj, string url);

    public class DiscoveryClients
    {
        public EventHandler<DiscoveryEvent> RecoveryHandler;


        public DiscoveryClients()
        {
            RecoveryHandler = null;
        }

        protected void OnRecoveryChange(string url, DiscoveryEvent.EventTypes eventType)
        {
            // Check if there are any Subscribers
            if (RecoveryHandler != null)
            {
                // Call the Event
                RecoveryHandler(this,new DiscoveryEvent(url, eventType) );
            }
        }


        public DiscoveryClient startDiscovery(TimeSpan span)
        {
            try
            {


                var endPoint = new UdpDiscoveryEndpoint(DiscoveryVersion.WSDiscoveryApril2005);

                var discoveryClient = new DiscoveryClient(endPoint);

                discoveryClient.FindCompleted += DiscoveryClient_FindCompleted;
                discoveryClient.FindProgressChanged += discoveryClient_FindProgressChanged;

                FindCriteria findCriteria = new FindCriteria();
                findCriteria.Duration = span;
                findCriteria.MaxResults = int.MaxValue;
                findCriteria.ContractTypeNames.Add(new System.Xml.XmlQualifiedName("NetworkVideoTransmitter", "http://www.onvif.org/ver10/network/wsdl"));
                discoveryClient.FindAsync(findCriteria);
    
                this.OnRecoveryChange("", DiscoveryEvent.EventTypes.startEvent);

                System.Diagnostics.Trace.WriteLine("DiscoveryClient.startDiscovery O.K ...");
    
                return discoveryClient;
            }
            catch(Exception ex)
            {
                System.Diagnostics.Trace.WriteLine("DiscoveryServer.startDiscovery: " + ex.Message);
                return null;
            }

        }

        private void DiscoveryClient_FindCompleted(object sender, FindCompletedEventArgs e)
        {

            this.OnRecoveryChange("", DiscoveryEvent.EventTypes.endEvent);
        }

        void discoveryClient_FindProgressChanged(object sender, FindProgressChangedEventArgs e)
        {
            //Check endpoint metadata here for required types.

            foreach (var u in e.EndpointDiscoveryMetadata.ListenUris)
            {

                string uri = u.OriginalString;

                 this.OnRecoveryChange(uri, DiscoveryEvent.EventTypes.dataEvent);

       
            }
        //    this.OnRecoveryChange("", DiscoveryEvent.EventTypes.endEvent);
        }
    }
}
