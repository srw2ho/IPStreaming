using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;



namespace OnVifServicesRunTime
{
    [Windows.UI.Xaml.Data.Bindable]


    public sealed class OnVifCameraViewModel 
    {

        private System.Collections.ObjectModel.ObservableCollection<OnVifCamera> m_Cameras = new ObservableCollection<OnVifCamera>();

        private int m_SelectedIndex;
        private LocalStorageSettings m_LocalStorageSettings;

        public int SelectedIndex
        {
            get { return this.m_SelectedIndex; }
            set
            {
                if (value < this.Cameras.Count) this.m_SelectedIndex = value;
                else this.m_SelectedIndex = -1;
            }
        }

        public IList<OnVifCamera> Cameras { get { return this.m_Cameras; } }



        public async void ConnectAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            OnVifCamera camera = getSelectedCamera();
            if (camera == null)
            {
                Button send = sender as Windows.UI.Xaml.Controls.Button;
                if (send != null) send.IsEnabled = false;

                await camera.connectAsync();

                if (send != null) send.IsEnabled = true;

            }


        }

        public bool editDevice(string IPAdress, string User, string Password)
        {
            OnVifCamera camera = getCameraByIPAdress(IPAdress); // wenn  IP-Adrresse passt
            if (camera == null) camera = getSelectedCamera(); // actual Selected
            if (camera!=null)
            {
                camera.setConnectionData(IPAdress, User, Password);
              
            }

            return true;

        }
        public bool moveUpDevice()
        {
            OnVifCamera camera= getSelectedCamera(); // actual Selected
            if (camera != null)
            {
                int Idx = this.SelectedIndex;
                if (Idx > 0)
                {
                    this.Cameras.RemoveAt(Idx);
                    this.Cameras.Insert(Idx - 1, camera);
                    this.SelectedIndex = Idx - 1;
                }


            }

            return true;

        }
        public bool moveDownDevice()
        {
            OnVifCamera camera = getSelectedCamera(); // actual Selected
            if (camera != null)
            {
                int Idx = this.SelectedIndex;
                if (Idx + 1 < this.Cameras.Count)
                {
                    this.Cameras.RemoveAt(Idx);
                    this.Cameras.Insert(Idx + 1, camera);
                    this.SelectedIndex = Idx + 1;

                }

            }

            return true;

        }

        public bool addDevice(string IPAdress, string User, string Password)
        {
            OnVifCamera camera = getCameraByIPAdress(IPAdress);
            if (camera == null) //only for testing
            {
                camera = new OnVifCamera(IPAdress, User, Password);
                camera.CameraName = IPAdress;
                this.Cameras.Add(camera);
            }
            
              
            return true;

        }

        public OnVifCamera getCameraByIPAdress(string IpAdress)
        {

            for (int i = 0; i < this.Cameras.Count; i++)
            {
                OnVifCamera camera = this.Cameras[i];
                if (camera.CameraIPAdress == IpAdress)
                {
                    return camera;

                }
            }
            return null;

        }
        public bool deleteSelectedDevice()
        {
            OnVifCamera device = getSelectedCamera();
            if (device == null) return false;

            return deleteDevice(device);
  

        }

        public bool deleteDevice(OnVifCamera device)
        {

            for (int i = 0; i < this.Cameras.Count; i++)
            {
                if (this.Cameras[i] == device)
                {

                    this.Cameras.RemoveAt(i);
                    return true;
                }

            }
            return false;

        }


        public OnVifCamera getSelectedCamera()
        {

            if (this.SelectedIndex == -1)
            {
                return null; ;
            }
            if (this.SelectedIndex >= Cameras.Count) return null;
            return Cameras[SelectedIndex];

        }
        public OnVifCamera getCameraByIdx(int Idx)
        {
            if (Idx >= Cameras.Count) return null;
            return Cameras[Idx];

        }


        public void SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            GridView gridview = sender as GridView;
            if (gridview != null)
            {
                this.SelectedIndex = gridview.SelectedIndex;
            }


        }


        public OnVifCameraViewModel()
        {
            m_LocalStorageSettings = new LocalStorageSettings("OnVifCameraViewModel");
            m_LocalStorageSettings.SetContainerIDName("OnVifCamera");

            SelectedIndex = -1;
          




        }
        public bool writeDatatoLocalStorage()
        {
            OnVifCamera camera;
            m_LocalStorageSettings.deleteCompositeValue(); // vor jedem Schreiben alles löschen

            Windows.Storage.ApplicationDataCompositeValue composite =  m_LocalStorageSettings.getCompositeValue();



            for (int i = 0; i < this.Cameras.Count; i++)
            {
                camera = this.Cameras[i];
                camera.writeSettingsToLocalStorage(composite, i);
            }
            if (composite.ContainsKey("SelectedIndex"))
            {
                composite["SelectedIndex"] = this.SelectedIndex;
            }
            else
            {
                composite.Add("SelectedIndex", this.SelectedIndex);
            }

            m_LocalStorageSettings.writeCompositeValuetoLocalStorage();

            return true;

        }
        public OnVifCamera createDummyCamera()
        {
            string CameraName = "AMCREST";
            string CameraIPAdress = "192.168.1.xxx";
            string User = "admin";
            string Password = "admin";
            OnVifCamera camera = new OnVifCamera(CameraIPAdress, User, Password);
            camera.CameraName = CameraName;

            return camera;
        }
        public bool readDatafromLocalStorage()
        {
            this.Cameras.Clear();
            Windows.Storage.ApplicationDataCompositeValue composite = m_LocalStorageSettings.getCompositeValue();
            int Idx = 0;
            while (true)
            {
                OnVifCamera camera = new OnVifCamera("","","");
                bool bdata = camera.readSettingsfromLocalStorage(composite, Idx);
                if (bdata)
                {
                    this.Cameras.Add(camera);
                }
                else break;
                Idx++;

            }
  

            if (this.Cameras.Count==0) // Add Dummy
            {
                OnVifCamera camera = createDummyCamera();
                this.Cameras.Add(camera);
                this.SelectedIndex = 0;
            }
            else
            {
                Object outvalue;
                bool bKey = composite.TryGetValue("SelectedIndex", out outvalue);
                if (bKey) this.SelectedIndex = (int)outvalue;
            }
 

            return (this.Cameras.Count>0);

        }
    }
 
}
