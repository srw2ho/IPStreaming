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
using static OnVifServicesRunTime.DiscoveryClients;


// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace OnVifServicesRunTime
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class OnVifCameraPage : Page
    {
        public OnVifCameraViewModel CameraViewModel { get; set; }
        public DiscoveryClients DiscoveryClients { get; set; }
        public OnVifCameraPage()
        {
            this.InitializeComponent();
 //           this.CameraViewModel = new OnVifCameraViewModel();
            this.CameraViewModel = null;
            this.DiscoveryClients = new DiscoveryClients(30001,30000, 20); // Datagram listener to 30001, Send-Port, 20 sec timeout

        }



        
        public void ButtonAddOnVifSelectedDevice(object sender, RoutedEventArgs e)
        {
            if (this.CameraViewModel == null) return;
            AppBarButton button = sender as AppBarButton;
            if (button == null) return;
            string devideIPAdress = "";
            string deviceUser = "admin";
            string devicePassword = "admin";

            WSEndpoint endpoint = this.DiscoveryClients.getSelectedEndPoint();
            if (endpoint == null) return;

            if (button.Name == "AddOnVifDNS")
            {
                devideIPAdress = endpoint.DNSName;
            }
            else if (button.Name == "AddOnVifIP")
            {
                devideIPAdress = endpoint.UrlHostName;
            }
            else devideIPAdress = endpoint.UrlHostName;


            this.CameraViewModel.addDevice(devideIPAdress, deviceUser, devicePassword);


        }


        public void ButtonRecoveryDevices(object sender, RoutedEventArgs e)
        {
            if (this.DiscoveryClients == null) return;
   
            Windows.Foundation.IAsyncOperation<int> endpoints = this.DiscoveryClients.GetAvailableWSEndpointsAsync();
            return;

        }

        public void ButtonAddDevice(object sender, RoutedEventArgs e)
        {
            if (this.CameraViewModel == null) return;
            this.CameraViewModel.addDevice(this.devideIPAdress.Text, this.deviceUser.Text, this.devicePassword.Password);
  

        }

        public void OnVifDisoverySelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ListView listView = sender as ListView;
            if (listView != null)
            {
                this.DiscoveryClients.SelectedIndex = listView.SelectedIndex;
            }


        }

        public void CameraSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            GridView gridView = sender as GridView;

            if (gridView != null)
            {
                if (gridView.ItemsSource ==  this.CameraViewModel.Cameras)
                {
                    CameraViewModel.SelectionChanged(sender, e);
                    OnVifCamera camera = CameraViewModel.getSelectedCamera();
                    if (camera != null)
                    {

                        CameraFrame.Navigate(typeof(OnVifSingleCameraPage), camera);

                    }

                }

            }

        }

        public void ButtonDeleteDevice(object sender, RoutedEventArgs e)
        {
       //     OnVifCamera camera = sender as OnVifCamera;
       //     if (camera == null) return;
            this.CameraViewModel.deleteSelectedDevice();

        }


     
        public void ButtonEditDevice(object sender, RoutedEventArgs e)
        {
            this.CameraViewModel.editDevice(this.devideIPAdress.Text, this.deviceUser.Text, this.devicePassword.Password);
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            if (this.CameraViewModel != null)
            {
                this.CameraViewModel.writeDatatoLocalStorage();
            }
            this.DiscoveryClients.CancelOperation();
            base.OnNavigatingFrom(e);

        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
  
            if (e.Parameter is OnVifCameraViewModel)
            {
                OnVifCameraViewModel paramcameraViewModel = e.Parameter as OnVifCameraViewModel;
                if (paramcameraViewModel != null)
                {
                    this.CameraViewModel = paramcameraViewModel;
                    if (this.CameraViewModel.Cameras.Count == 0) // if no set then read from Loxcal Storage
                    {
                        this.CameraViewModel.readDatafromLocalStorage();
                    }
                    if (this.CameraViewModel.Cameras.Count>0) this.CameraViewModel.SelectedIndex = 0;
                    
                }
            }

          

            base.OnNavigatedTo(e);
        }
        /*
        private void CameraFrameLoaded(object sender, RoutedEventArgs e)
        {
        
            Frame frame = sender as Frame;
            if (frame!=null)
            {
                OnVifCamera camera = frame.DataContext as OnVifCamera;
                if (camera != null) // Navigate to OnVifSingleCameraPage with Parameter OnVifCamera
                {
                    frame.Navigate(typeof(OnVifSingleCameraPage), camera);
                }
            }

        }
        */

        private void cameraGridViewTapped(object sender, TappedRoutedEventArgs e) // selection for GridView
        {
            GridView gridView = sender as GridView;

            if (gridView != null)
            {
                Grid cameraGrid = e.OriginalSource as Grid;
                if (cameraGrid != null)
                {
                    OnVifCamera camera = cameraGrid.DataContext as OnVifCamera;
                    if (camera != null && (gridView.SelectedItem != camera)) // already selected
                    {
                        gridView.SelectedItem = camera;
                    }
          
                }


            }

        }
    }
}
