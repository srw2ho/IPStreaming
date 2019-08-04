

using System;

namespace OnVifServices
{
    public class TimeUtil
    {
        public DateTime time;
        double timeover;

        public DateTime Time
        {
            set
            {
                time = value;
            }
            get
            {
                return time;
            }
        }

        public double Timeover
        {
            set {
                timeover = value;
            }
            get{
                return timeover;
            }
        }


        public TimeUtil(double timeover)
        {
            this.time = DateTime.Now;
            this.timeover = timeover;
        }

        public bool IsTimeover()
        {
            TimeSpan timeSpan = DateTime.Now.Subtract(this.time);
            double time = this.timeover - timeSpan.TotalSeconds;
            return time < 0;
        }

    }

}
