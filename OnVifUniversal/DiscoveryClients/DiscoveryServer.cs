using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Net;
using System.Net.NetworkInformation;
using System.Threading;
//using System.ServiceModel.Discovery;

namespace OnVifDiscovery
{

    public class DiscoveryServer
    {
        protected enum Waits { Cancel, Timeout, DataOk};

        Socket m_serverSocket;
        private byte[] m_dataStream;
        string m_Id;
        DiscoveryClients m_DiscoveryClients;
        List<string> m_RecoveryData;
        string m_localIPAdress;
        int m_ServerPort;
        int m_RemoteServerPort;
        ManualResetEvent m_DiscoverySearchEvent;
        ManualResetEvent m_DiscoveryCancelEvent;
     //   bool m_ReceivingIsBusy;
   
        public DiscoveryServer()
        {
            m_Id = "WSDiscoveryServer";
            m_ServerPort = 30000;
            m_RemoteServerPort = m_ServerPort;
            m_serverSocket = null;
            m_dataStream = new byte[4096];
            m_dataStream[0] = 0;
            m_DiscoveryClients = new DiscoveryClients();
            m_DiscoveryClients.RecoveryHandler += DiscoveryClients_RecoveryHandler;
          
            m_localIPAdress = GetLocalIPv4(NetworkInterfaceType.Ethernet);
            m_DiscoverySearchEvent = new ManualResetEvent(false);
            m_DiscoveryCancelEvent = new ManualResetEvent(false);

            m_RecoveryData = new List<string>();
   //         m_ReceivingIsBusy = false;

        }
        public void ThreadStart(Object obj)
        {
            System.Threading.CancellationTokenSource ct = (System.Threading.CancellationTokenSource) (obj);
            startServer();
            if (m_serverSocket == null) return;
            while (!ct.IsCancellationRequested) // Socket Receice Thread can be stopped by Socket.Close()
            {
                Thread.Sleep(500);
                if (m_serverSocket==null)
                {
                    startServer();
                }
                else if (m_ServerPort != m_RemoteServerPort)
                {
                    m_ServerPort = m_RemoteServerPort;
                    startServer();// Restart Server with new ListenPort
                }
            }
            stopDiscoveryServer();


        }
        protected Waits Wait(WaitHandle yourEvent, WaitHandle cancelEvent, TimeSpan timeOut)
        {
            WaitHandle[] handles = new WaitHandle[] { yourEvent, cancelEvent };

            // WaitAny returns the index of the event that got the signal
            var waitResult = WaitHandle.WaitAny(handles, timeOut);

            if (waitResult == 1)
            {
                return Waits.Cancel; // cancel!
            }

            if (waitResult == WaitHandle.WaitTimeout)
            {
                return Waits.Timeout; // timeout
            }

            return Waits.DataOk;
        }

        protected void stopDiscoveryServer() {
            m_DiscoveryCancelEvent.Set();
            if (m_serverSocket != null)
            {
                m_serverSocket.Close(); // Cancel receive thread
            }
        }
        private void DiscoveryClients_RecoveryHandler(object obj, DiscoveryEvent @event)
        {
            DiscoveryClients discoveryClients = obj as DiscoveryClients;
            if (discoveryClients != null)
            {
   
                if (@event.EventType == DiscoveryEvent.EventTypes.startEvent)
                {
                    m_RecoveryData.Clear();
                    m_DiscoverySearchEvent.Reset();
         
                }
                else
                if (@event.EventType== DiscoveryEvent.EventTypes.dataEvent)
                {
                    m_RecoveryData.Add(@event.Url);
                }
                else if (@event.EventType == DiscoveryEvent.EventTypes.endEvent)
                {
                    m_DiscoverySearchEvent.Set();
                }


            }

    
        }

        bool startServer()
        {
            try
            {
                bool isNetworkAvailable = System.Net.NetworkInformation.NetworkInterface.GetIsNetworkAvailable();
                if (!isNetworkAvailable) return false;
                if (m_serverSocket != null)
                {
                    m_serverSocket.Close();
                    m_serverSocket = null;
                }
                m_serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
 
                // Initialise the IPEndPoint for the server and listen on port 30000
                IPEndPoint server = new IPEndPoint(IPAddress.Any, m_ServerPort);
                // Associate the socket with this IP address and port
                m_serverSocket.Bind(server);
                // Initialise the IPEndPoint for the clients
                IPEndPoint clients = new IPEndPoint(IPAddress.Any, 0);
                // Initialise the EndPoint for the clients
                EndPoint epSender = (EndPoint)clients;
                // Start listening for incoming data
                m_serverSocket.BeginReceiveFrom(this.m_dataStream, 0, this.m_dataStream.Length, SocketFlags.None, ref epSender, new AsyncCallback(ReceiveData), epSender);
                System.Diagnostics.Trace.WriteLine("DiscoveryServer.startServer O.K ...");

        
                return true;
            }
            catch (SocketException ex )
            {
                System.Diagnostics.Trace.WriteLine(ex.Message);
                if (m_serverSocket != null) m_serverSocket.Close();
                m_serverSocket = null;
                return false;
            }

        }

        
        private void ReceiveData(IAsyncResult asyncResult)
        {
            try
            {

         //       m_MessageReceive.Wait();

                // Initialise the IPEndPoint for the clients
                IPEndPoint clients = new IPEndPoint(IPAddress.Any, 0);
                // Initialise the EndPoint for the clients
                EndPoint epSender = (EndPoint)clients;
      
                // Receive all data. Sets epSender to the address of the caller
                int bytetoread = m_serverSocket.EndReceiveFrom(asyncResult, ref epSender);
 
                string message = Encoding.UTF8.GetString(m_dataStream);
                System.Diagnostics.Trace.WriteLine("DiscoveryServer.Request from: " + epSender.ToString() + " " + message);
                // Get the message received

                // Check if it is a search ws message
                Waits toDo = Waits.DataOk;
           //     if (!m_ReceivingIsBusy)
                {
                    if (message.StartsWith("OnVifUniversal.SEARCHWS", StringComparison.CurrentCultureIgnoreCase))
                    { // <"OnVifUniversal.SEARCHWS"> ; <time> ; <listenport> 
                        int time = 20;
                        int listenPort = this.m_ServerPort;
                        var splitedMessage = message.Split(';');
                        if (splitedMessage.Length > 0) // first parameter = timeout
                        {
                            try
                            {
                                time = Int32.Parse(splitedMessage[1]);
                            }
                            catch (Exception)
                            {

                            }
                        }
                        if (splitedMessage.Length > 1) // second parmeter = listenPort 
                        {
                            try
                            {
                                listenPort = Int32.Parse(splitedMessage[2]);
                                m_RemoteServerPort = listenPort;
                            }
                            catch (Exception)
                            {

                            }
                        }

                        TimeSpan span = TimeSpan.FromSeconds(time);
                        TimeSpan CancelEventspan = TimeSpan.FromSeconds(time+2);

                        System.ServiceModel.Discovery.DiscoveryClient discoveryClient = m_DiscoveryClients.startDiscovery(span); // Start Discovery Search
                        if (discoveryClient != null)
                        {
                        //    m_ReceivingIsBusy = true;
                            toDo = Wait(this.m_DiscoverySearchEvent, m_DiscoveryCancelEvent, CancelEventspan); // Wait for Discovery-Result, Timeout or Cancel

                            if (toDo == Waits.Cancel)
                            {
                                System.Diagnostics.Trace.WriteLine("DiscoveryServer.DiscoveryClient: Cancel");
                            }
                            else if (toDo == Waits.Timeout)
                            {
                                System.Diagnostics.Trace.WriteLine("DiscoveryServer.DiscoveryClient: Timeout");

                            }
                            else if (toDo == Waits.DataOk)
                            {
                                byte[] data = getSendData(); // Discovery-Result send back to requester
                                if (data.Length > 0)
                                {
                                    // Send the response message to the client who was searching
                                    string result = System.Text.Encoding.UTF8.GetString(data);
                                    string sendtrace = String.Format("DiscoveryServer.SendData to {0}: {1} ", epSender.ToString(), result);
                                    System.Diagnostics.Trace.WriteLine(sendtrace);
                                    if (m_serverSocket.IsBound)
                                    {
                                        m_serverSocket.BeginSendTo(data, 0, data.Length, SocketFlags.None, epSender, new AsyncCallback(this.SendData), epSender);

                                    }

                                }

                            }

                            discoveryClient.Close();
           //                 m_ReceivingIsBusy = false;

                        }

                    }
                }

                // ready to receive next message
                // receive next message
                m_serverSocket.BeginReceiveFrom(this.m_dataStream, 0, this.m_dataStream.Length, SocketFlags.None, ref epSender, new AsyncCallback(this.ReceiveData), epSender);

                //        m_MessageReceive.Release();
            }

            catch (System.ObjectDisposedException ex)
            {
                // the read was canceled. This is expected. Socket.Close
                System.Diagnostics.Trace.WriteLine(ex.Message);
                if (m_serverSocket != null) m_serverSocket.Close();
                m_serverSocket = null;
                return;
            }
            catch (SocketException ex )
            {
                System.Diagnostics.Trace.WriteLine(ex.Message);
                if (m_serverSocket != null) m_serverSocket.Close();
                m_serverSocket = null;
                return;
            }

        }

        private byte[] getSendData()
        {
            using (MemoryStream ms = new MemoryStream())
            {
                using (StreamWriter sw = new StreamWriter(ms, Encoding.UTF8))
                {
                    string sendItem = "";
                    if (m_RecoveryData.Count == 0)
                    {
                        sendItem = string.Format("{0}", "OnVifUniversal.NODATA");
                        sw.WriteLine(sendItem);
                    }
                    else
                    for (int i = 0; i < m_RecoveryData.Count; i++)
                    {
                  
                        string hostName = "";
                        string DnshostName = "";
                        try
                        {
                            Uri myUri = new Uri(m_RecoveryData[i]);
                            hostName = myUri.Host;
                      //      IPHostEntry ip = Dns.GetHostEntry(hostName); // name resolving
                      //      DnshostName = ip.HostName;
                        }
                        catch (Exception ex)
                        {
                            System.Diagnostics.Trace.WriteLine(ex.Message);
                        }

                        sendItem = string.Format("{0};{1};{2};{3};{4}", "OnVifUniversal.WSRESPONSE", this.m_Id, m_RecoveryData[i], hostName, DnshostName);
                        sw.WriteLine(sendItem);
                    }
                }
                return ms.ToArray();
            }

        }



        private void SendData(IAsyncResult asyncResult)
        {
            if (m_serverSocket == null) return;
            m_serverSocket.EndSend(asyncResult);
        }
        protected static string GetLocalIPv4(NetworkInterfaceType _type)
        {
            string output = "";
            foreach (NetworkInterface item in NetworkInterface.GetAllNetworkInterfaces())
            {
                if (item.NetworkInterfaceType == _type && item.OperationalStatus == OperationalStatus.Up)
                {
                    IPInterfaceProperties adapterProperties = item.GetIPProperties();

                    if (adapterProperties.GatewayAddresses.FirstOrDefault() != null)
                    {
                        foreach (UnicastIPAddressInformation ip in adapterProperties.UnicastAddresses)
                        {
                            if (ip.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
                            {
                                output = ip.Address.ToString();
                            }
                        }
                    }
                }
            }

            return output;
        }

        protected static string GetLocalIPAddress()
        {
            var host = Dns.GetHostEntry(Dns.GetHostName());
            foreach (var ip in host.AddressList)
            {
                if (ip.AddressFamily == AddressFamily.InterNetwork)
                {
                    return ip.ToString();
                }
            }
            System.Diagnostics.Trace.WriteLine("No network adapters with an IPv4 address in the system!");
            return "";
            //      throw new Exception("No network adapters with an IPv4 address in the system!");
        }


    }


}
