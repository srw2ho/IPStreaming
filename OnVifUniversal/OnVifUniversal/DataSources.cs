using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OnVifServicesRunTime;
using System.Collections.ObjectModel;

using OnVifServices.OnvifMedia10;
using Windows.UI.Xaml.Controls;

namespace OnVifUniversal
{

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class Recording
    {
        public string ArtistName { get; set; }
        public string CompositionName { get; set; }
        public DateTime ReleaseDateTime { get; set; }
        public Recording()
        {
            this.ArtistName = "Wolfgang Amadeus Mozart";
            this.CompositionName = "Andante in C for Piano";
            this.ReleaseDateTime = new DateTime(1761, 1, 1);
        }
        public string OneLineSummary
        {
            get
            {
                return $"{this.CompositionName} by {this.ArtistName}, released: "
                    + this.ReleaseDateTime.ToString("d");
            }
        }
    }
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class RecordingViewModel
    {

        private ObservableCollection<Recording> recordings = new ObservableCollection<Recording>();
        public ObservableCollection<Recording> Recordings { get { return this.recordings; } }
        public RecordingViewModel()
        {
            this.recordings.Add(new Recording()
            {
                ArtistName = "Johann Sebastian Bach",
                CompositionName = "Mass in B minor",
                ReleaseDateTime = new DateTime(1748, 7, 8)
            });
            this.recordings.Add(new Recording()
            {
                ArtistName = "Ludwig van Beethoven",
                CompositionName = "Third Symphony",
                ReleaseDateTime = new DateTime(1805, 2, 11)
            });
            /*
            this.recordings.Add(new Recording()
            {
                ArtistName = "George Frideric Handel",
                CompositionName = "Serse",
                ReleaseDateTime = new DateTime(1737, 12, 3)
            });
            */
        }

    }
    [Windows.UI.Xaml.Data.Bindable]
    public  class OnvifMediaUri
    {
        protected OnVifServices.OnvifMedia10.MediaUri m_MediaUri;

        public string Uri { get { return this.m_MediaUri.Uri; } }
        public bool InvalidAfterConnect { get { return this.m_MediaUri.InvalidAfterConnect; } }
        public bool InvalidAfterReboot { get { return this.m_MediaUri.InvalidAfterReboot; } }

        public OnvifMediaUri(OnVifServices.OnvifMedia10.MediaUri url)
        {
            this.m_MediaUri = url;

        }
    }
    public sealed class MediaUrlViewModel
    {

        private ObservableCollection<OnvifMediaUri> m_OnvifMediaUris = new ObservableCollection<OnvifMediaUri>();
        public ObservableCollection<OnvifMediaUri> OnvifMediaUris { get { return this.m_OnvifMediaUris; } }
        public MediaUrlViewModel(List<MediaUri> urls)
        {
            foreach (MediaUri url in urls)
            {
                this.m_OnvifMediaUris.Add(new OnvifMediaUri(url));

            }
          
        }
    }



}
