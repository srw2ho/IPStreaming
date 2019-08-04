using System.ServiceModel.Description;

namespace OnVifServices
{
    public class PasswordDigestBehavior : IEndpointBehavior
    {
        public double TimeOffset { get; set; }
        public string Username { get; set; }
        public string Password { get; set; }

        public PasswordDigestBehavior(string username, string password, double timeOffset)
        {
            Username = username;
            Password = password;
            TimeOffset = timeOffset;
        }

        #region IEndpointBehavior Members

        public void AddBindingParameters(ServiceEndpoint endpoint, System.ServiceModel.Channels.BindingParameterCollection bindingParameters)
        {
  

        }

        public void ApplyClientBehavior(ServiceEndpoint endpoint, System.ServiceModel.Dispatcher.ClientRuntime clientRuntime)
        {
            clientRuntime.ClientMessageInspectors.Add(new PasswordDigestMessageInspector(Username, Password, TimeOffset));
     //       clientRuntime.MessageInspectors.Add(new PasswordDigestMessageInspector(Username, Password, TimeOffset));
        }

        public void ApplyDispatchBehavior(ServiceEndpoint endpoint, System.ServiceModel.Dispatcher.EndpointDispatcher endpointDispatcher)
        {

        }

        public void Validate(ServiceEndpoint endpoint)
        {

        }

        #endregion
    }
}
