using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.System.Threading;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;

namespace OnVifServicesRunTime
{
    public sealed class WSEndpoint: System.ComponentModel.INotifyPropertyChanged
    {
        public WSEndpoint(string ID, string URL, string remoteAddr, string urlhostName, string DNSName)
        {
           m_remoteIPAdress = remoteAddr;
           m_ID = ID;
           m_URL = URL;
           m_DNSName = DNSName;
           m_UrlhostName = urlhostName;  // host is "www.contoso.com"

        }
        public string ID { get { return m_ID; }
            set {
                m_ID = value;
                RaisePropertyChanged("ID");
            }
        }
        public string URL { get { return m_URL; }
            set
            {
                m_URL = value;
                RaisePropertyChanged("URL");
            }
        }
        public string RemoteIPAdress { get { return m_remoteIPAdress; }
            set
            {
                m_remoteIPAdress = value;
                RaisePropertyChanged("RemoteIPAdress");
            }
        }
        public string UrlHostName { get { return m_UrlhostName; }
            set
            {
                m_UrlhostName = value;
                RaisePropertyChanged("UrlHostName");

            }
        }



        public string DNSName { get { return m_DNSName; }
            set {
                m_DNSName = value;
                
                RaisePropertyChanged("DNSName");

            }
        }


        private string m_ID;
        private string m_URL;
        private string m_remoteIPAdress;
        private string m_UrlhostName;
        private string m_DNSName;

        public event PropertyChangedEventHandler PropertyChanged = null;

        private void RaisePropertyChanged(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }
    }

    public sealed class DiscoveryClients : System.ComponentModel.INotifyPropertyChanged
    {
        private enum Waits {Undef, Cancel, Timeout, DataOk };

        private ObservableCollection<WSEndpoint> m_endPoints;

        private int m_ListenPort = 30001;
        private int m_SendPort = 30000;
        private int m_timeOut = 5; // seconds

        private SemaphoreSlim m_signal;
        int m_SelectedIndex;
        int m_status;

        public event PropertyChangedEventHandler PropertyChanged = null;

        private void RaisePropertyChanged(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }
        public DiscoveryClients(int DataGramlistenPort, int SendPort, int timeout)
        {
            m_ListenPort = DataGramlistenPort;
            m_SendPort = SendPort;
            m_endPoints = new ObservableCollection<WSEndpoint>();
            m_timeOut = timeout; // seconds
            m_status = -1;
            m_SelectedIndex = -1;
            m_signal = new SemaphoreSlim(0, 1);
        }

        ~DiscoveryClients()
        {
            CancelOperation();
        }

        public void  CancelOperation()
        {
            if (m_signal != null)
            {
    
                m_signal.Release();
                m_signal.Dispose();
                m_signal = null;
            }


        }
        public WSEndpoint getSelectedEndPoint()
        {

            if (this.SelectedIndex == -1)
            {
                return null; ;
            }
            if (this.SelectedIndex >= m_endPoints.Count) return null;
            return m_endPoints[SelectedIndex];

        }

        public bool IsDNSNameSelected
        {
            get {
                WSEndpoint endpoint = getSelectedEndPoint();
                if (endpoint != null) return !String.IsNullOrEmpty(endpoint.DNSName);
                return false;
            }

        }
        public bool IsUrlHostNameSelected
        {
            get
            {
                WSEndpoint endpoint = getSelectedEndPoint();
                if (endpoint != null) return !String.IsNullOrEmpty (endpoint.UrlHostName);
                return false;
            }

        }

        public int ListenPort
        {
            get { return this.m_ListenPort; }
            set
            {
                this.m_ListenPort = value;
                RaisePropertyChanged("ListenPort");

            }
        }
        public int SendPort
        {
            get { return this.m_SendPort; }
            set
            {
                this.m_SendPort = value;
                RaisePropertyChanged("SendPort");

            }
        }

        public int RequestTimeOut
        {
            get { return this.m_timeOut; }
            set
            {
                this.m_timeOut = value;
                RaisePropertyChanged("RequestTimeOut");

            }
        }




        public int SelectedIndex
        {
            get { return this.m_SelectedIndex; }
            set
            {
                if (value < this.m_endPoints.Count) this.m_SelectedIndex = value;
                else this.m_SelectedIndex = -1;
                RaisePropertyChanged("SelectedIndex");
                RaisePropertyChanged("IsUrlHostNameSelected");
                RaisePropertyChanged("IsDNSNameSelected");
            }
        }

        public bool IsConnected
        {
            get { return m_status == 1; }
        }

        public int Status
        {
            set
            {
                m_status = value;
                RaisePropertyChanged("Status");
                RaisePropertyChanged("IsConnected");
            }
            get { return m_status; }
        }

        public IList<WSEndpoint> EndPoints
        {
            get { return m_endPoints; }
        }

        public Windows.Foundation.IAsyncOperation<int> GetAvailableWSEndpointsAsync()
        {
          //  m_SendPort = port;
            Task<int> from = GetAvailableWSEndpointsInternal();

            Windows.Foundation.IAsyncOperation<int> to = from.AsAsyncOperation();

            return to;


        }



        /// <summary>
        /// Get available Webservices
        /// </summary>
        /// 
        internal async Task<int> GetAvailableWSEndpointsInternal()
        {
            int ret = 0;
            try
            {
                this.Status = -1;
                m_endPoints.Clear();
            //    m_signal.CurrentCount
                using (var socket = new Windows.Networking.Sockets.DatagramSocket())
                {
                    // Set the callback for servers' responses
                    socket.MessageReceived += SocketOnMessageReceived;
                    // Start listening for servers' responses
                    await socket.BindServiceNameAsync(m_ListenPort.ToString());

                    // Send a search message
                    await SendMessage(socket);
                    bool bTimeOut = false;
                    ThreadPoolTimer timer = ThreadPoolTimer.CreateTimer((t) =>
                    {
                        bTimeOut = true;
                        m_signal.Release();
                    }, TimeSpan.FromSeconds(m_timeOut));

                    await m_signal.WaitAsync();
                    timer.Cancel();
                    timer = null;

                    if (bTimeOut)
                    {
                        ret = -1;
                        await socket.CancelIOAsync();
                    }
                    else ret = 1;
                 }
        
            }
            catch (Exception)
            {
                m_signal.Release();
                ret = -1;
            }
            this.Status  = ret;
            return ret;
        }

        /// <summary>
        /// Sends a broadcast message searching for available Webservices
        /// </summary>
        private async Task SendMessage(DatagramSocket socket)
        {

            HostName hostName = new HostName("255.255.255.255"); // to all listeners

            using (var stream = await socket.GetOutputStreamAsync(hostName, m_SendPort.ToString()))
            {
                using (var writer = new Windows.Storage.Streams.DataWriter(stream))
                { // <Command>;<Timeout>
                    string sendstr = String.Format("{0};{1}", "OnVifUniversal.SEARCHWS",this.m_timeOut);
                    
                    var data = Encoding.UTF8.GetBytes(sendstr);
                    writer.WriteBytes(data);
                    writer.WriteByte(0); // String 0
                    await writer.StoreAsync();
                }
            }
        }



        private async void SocketOnMessageReceived(DatagramSocket sender, DatagramSocketMessageReceivedEventArgs args)
        {
            // Creates a reader for the incoming message
            var resultStream = args.GetDataStream().AsStreamForRead(1024);
            using (var reader = new StreamReader(resultStream))
            {
                // Get the message received
                string message = await reader.ReadToEndAsync();
                // Cheks if the message is a response from a server
                if (message.StartsWith("OnVifUniversal.WSRESPONSE", StringComparison.CurrentCultureIgnoreCase))
                {
                    var dispatcher = Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher;
                    // Spected format: WSRESPONSE;<ID>;RemoteADR;<HTTP ADDRESS>

                    await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        string[] lines = message.Split(Environment.NewLine.ToCharArray());
                        foreach (var line in lines)
                        {
                            var splitedMessage = line.Split(';');
                            if (splitedMessage.Length >= 5)
                            {
                                var id = splitedMessage[1];
                                var url = splitedMessage[2];
                                var UrlHostName = splitedMessage[3];
                                var dnsName = splitedMessage[4];
                                m_endPoints.Add(new WSEndpoint(id, url, args.RemoteAddress.CanonicalName, UrlHostName, dnsName));
                            }

                        }
                    });
                }
                m_signal.Release(); // Signal that data arrived

            }
        }


    }
}