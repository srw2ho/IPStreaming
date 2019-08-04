using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OnVifServicesRunTime
{
    interface IOnVifMedia
    {
        bool Isinitalized { get; }
        IList<OnVifCameraProfile> ProfileEncodings { get; }
        OnVifCameraProfile getOnVifProfile(int nProfileIdx);


        string ProfileRefToken { get; }
        string getProfileToken(int nProfileIdx);
        int getNummberOfProfileTokens();
        string getProfileName(int nProfileIdx);
        Task<IDictionary<string, IList<string>>> ReadMediaUrisAsync(int nProfileIdx);
        Task<bool> InitalizeAsync();
        Task<Boolean> writeVideoEncodingInternalAsync(int nProfileIdx);
        Task<Boolean> writeAudioEncodingInternalAsync(int nProfileIdx);
        Task<OnVifCameraProfile> getAudioEncodingInternalAsync(int nProfileIdx);
        Task<OnVifCameraProfile> getVideoEncodingInternalAsync(int nProfileIdx);
        Task<OnVifCameraProfile> getAudioEncodingConfigurationOptionsInternalAsync(int nProfileIdx);
        Task<OnVifCameraProfile> getVideoEncodingConfigurationOptionsInternalAsync(int nProfileIdx);
    }
}
