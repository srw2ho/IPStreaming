using System;
using System.ServiceModel.Channels;
using System.ServiceModel.Dispatcher;
using System.Text;

namespace OnVifServices
{
    public class BasicAuthMessageInspector : IClientMessageInspector
    {
        public string Username { get; set; }
        public string Password { get; set; }

        public BasicAuthMessageInspector(string username, string password)
        {
            Username = username;
            Password = password;
        }

        #region IClientMessageInspector Members

        public void AfterReceiveReply(ref Message reply, object correlationState)
        {
   
        }

        public object BeforeSendRequest(ref Message request, System.ServiceModel.IClientChannel channel)
        {
            var utf8 = Encoding.UTF8;
           
            String usernameandPW = Username + ":" + Password;
            byte[] utfBytes = utf8.GetBytes(usernameandPW);
            //     string encoded = Convert.ToBase64String((Username + ":" + Password).ToUtf8());
             string encoded = Convert.ToBase64String(utfBytes);

            var httpRequestMessage = new HttpRequestMessageProperty();
      //      httpRequestMessage.Headers.Add("Authorization", "Basic " + encoded);
            httpRequestMessage.Headers["Authorization"] = "Basic " + encoded;

//            httpRequestMessage.Headers.Add("Authorization", "Basic " + encoded);
            request.Properties.Add(HttpRequestMessageProperty.Name, httpRequestMessage);

          //  return Convert.DBNull;
            return null;
        }
        #endregion
    }
}
