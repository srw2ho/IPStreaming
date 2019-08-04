using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OnVifServices
{
    interface IOnVifService
    {
        bool Isinitalized { get; }
        string ErrorMessage { get; }
        Task<bool> InitalizeAsync();

    }

    public class OnVifServiceBase: IOnVifService
    {
        protected String m_ErrorMessage;
        protected OnVifDevice m_onVifDevice;
        protected bool m_initialised;

        public Boolean IsInitialized
        {
            get
            {
                return m_initialised;

            }
        }

        public string ErrorMessage
        {
            get { return this.m_ErrorMessage; }
        }

        public bool Isinitalized => throw new NotImplementedException();

        public OnVifServiceBase(OnVifDevice onVifDevice)
        {

            m_onVifDevice = onVifDevice;
            m_initialised = false;
            m_ErrorMessage = "";
        }

        public virtual Task<bool> InitalizeAsync()
        {
            throw new NotImplementedException();
        }
    }
}
