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
using System.Threading;
using System.Threading.Tasks;
using System.Collections.ObjectModel;

using OnVifServices;
//using OnVifServices.OnvifMedia10;
using OnVifServices.OnvifPTZService;

using OnVifServicesRunTime;


//using DiscoveryIPAdresses;



// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace OnVifUniversal
{


    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }


    public sealed partial class MainPage : Page
    {

        //       OnVifServices.Media10Services mediaservice = null;

        List<Scenario> scenarios;
        public class Scenario
        {
            public string Title { get; set; }
            public Type ClassType { get; set; }
        }

        public MainPage()
        {
            this.InitializeComponent();
            //      this.ViewModel = new RecordingViewModel();
            this.CameraViewModel = new OnVifServicesRunTime.OnVifCameraViewModel();
            SampleTitle.Text = "Pivot";
     

            scenarios = new List<Scenario>
            {
                new Scenario() { Title="Scenario 1 - Basic", ClassType=typeof(OnVifCameraPage)},
                new Scenario() { Title="Scenario 2 - Headers", ClassType=typeof(OnVifCameraPage)},
                new Scenario() { Title="Scenario 3 - Tabs", ClassType=typeof(OnVifCameraPage)}
            };
        }
    

  //      public RecordingViewModel ViewModel { get; set; }
        public OnVifCameraViewModel CameraViewModel { get; set; }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ScenarioControl.ItemsSource = scenarios;
            ScenarioControl.SelectedIndex = 0;
            bool bok = true;
      
        }



        private void ScenarioControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Clear the status block when navigating scenarios.
            
            NotifyUser(String.Empty, NotifyType.StatusMessage);

            ListView scenarioListBox = sender as ListView;
            Scenario s = scenarioListBox.SelectedItem as Scenario;
            if (s != null)
            {
             //   CameraViewModel = new OnVifCameraViewModel();

                ScenarioFrame.Navigate(s.ClassType, this.CameraViewModel);
                if (Window.Current.Bounds.Width < 640)
                {
                    Splitter.IsPaneOpen = false;
                    StatusBorder.Visibility = Visibility.Collapsed;
                }
            }
            
        }

        public void NotifyUser(string strMessage, NotifyType type)
        {
            switch (type)
            {
                case NotifyType.StatusMessage:
                    StatusBorder.Background = new SolidColorBrush(Windows.UI.Colors.Green);
                    break;
                case NotifyType.ErrorMessage:
                    StatusBorder.Background = new SolidColorBrush(Windows.UI.Colors.Red);
                    break;
            }
            StatusBlock.Text = strMessage;

            // Collapse the StatusBlock if it has no text to conserve real estate.
            StatusPanel.Visibility = (StatusBlock.Text != String.Empty) ? Visibility.Visible : Visibility.Collapsed;
        }

        async void Footer_Click(object sender, RoutedEventArgs e)
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri(((HyperlinkButton)sender).Tag.ToString()));
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Splitter.IsPaneOpen = (Splitter.IsPaneOpen == true) ? false : true;
            StatusBorder.Visibility = Visibility.Collapsed;
        }


    }
    public enum NotifyType
    {
        StatusMessage,
        ErrorMessage
    };

    public class ScenarioBindingConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            //    Scenario s = value as Scenario;
            //    return (MainPage.Current.Scenarios.IndexOf(s) + 1) + ") " + s.Title;

            return "Scenario haaa...";
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return true;
        }
    }
}
