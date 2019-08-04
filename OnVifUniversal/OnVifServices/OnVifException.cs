using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OnVifServices
{


    

    public sealed class OnVifException : Exception
    {
        public OnVifException(string message) : base(message)
        {
         
        }
        public OnVifException(string message, Exception inner) : base(message, inner)
        {
        }

      }


}
