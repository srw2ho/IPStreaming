using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace OnVifServicesRunTime
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class OnVifSingleCameraPage : Page
    {
        public OnVifCamera Camera {
            get { return m_Camera; }
            set { m_Camera = value; } 
        }

        private OnVifCamera m_Camera;
        public OnVifSingleCameraPage()
        {
            this.InitializeComponent();
            m_Camera = null;
        }

        public async void ConnectAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
          
            if (m_Camera!=null)
            {
                Button send = sender as Windows.UI.Xaml.Controls.Button;
                if (send != null) send.IsEnabled = false;

                await m_Camera.connectAsync();

                if (send != null) send.IsEnabled = true;

            }


        }
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {

            base.OnNavigatingFrom(e);

        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter is OnVifCamera)
            {
                OnVifCamera paramcamera = e.Parameter as OnVifCamera;
                if (paramcamera != null)
                {
                    this.Camera = paramcamera;
                }
            }



            base.OnNavigatedTo(e);
        }
    }
}
