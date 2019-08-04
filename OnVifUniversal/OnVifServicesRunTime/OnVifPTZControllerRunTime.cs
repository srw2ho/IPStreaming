using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OnVifServices.OnvifMedia10;
using System.Numerics;
using System.ServiceModel;
using System.ComponentModel;
using OnVifServices.OnvifPTZService;
using Windows.UI.Xaml.Controls;
using OnVifServices;


namespace OnVifServicesRunTime
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class PTZPresetItem : INotifyPropertyChanged
    {

        private string m_nameField;

        private PTZVector m_pTZPositionField;

        private string m_tokenField;
        private int m_nIdx;


        internal PTZPresetItem( string name, string token, PTZVector position, int nIdx)
        //internal PTZPresetItem(string name, string token, int nIdx)
        {
            m_nameField = name;
            m_tokenField = token;
            m_pTZPositionField = position;
            m_nIdx = nIdx;

        }
        public string Name
        {
            get { return m_nameField; }
            set
            {
                m_nameField = value;
                RaisePropertyChanged("Name");
            }

        }
        public string Token
        {
            get { return m_tokenField; }
            set
            {
                m_tokenField = value;
                RaisePropertyChanged("Token");
            }

        }
        internal PTZVector PTZPosition
        {
            get { return m_pTZPositionField; }
            set
            {
                m_pTZPositionField = value;
                RaisePropertyChanged("PTZPosition");
            }

        }
        public string VisibleItem
        {
            get
            {
                string str;
                if (m_pTZPositionField != null)
                {
                    str = string.Format("{0}_{1}_xPos/y_Pos({2}/{3}", Name, Token, m_pTZPositionField.PanTilt.x, m_pTZPositionField.PanTilt.y);
                }
                else
                {
                    str = string.Format("{0}_{1}", Name, Token);
                }
          
                return str;
            }
        }


        public event PropertyChangedEventHandler PropertyChanged = null;

        private void RaisePropertyChanged(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }
    }


    public sealed class ComboViewModelPTZPreset: INotifyPropertyChanged
    {

        private System.Collections.ObjectModel.ObservableCollection<PTZPresetItem> m_Items;
        private int m_SelectedIndex;


        public IList<PTZPresetItem> Items
        {
            get { return m_Items; }
        }

        public ComboViewModelPTZPreset()
        {
            m_Items = new System.Collections.ObjectModel.ObservableCollection<PTZPresetItem>();
            m_SelectedIndex = -1;
        }


        public bool deletePresetItem(PTZPresetItem item)
        {
            return true;

        }
        public int SelectedIndex
        {

            get { return m_SelectedIndex; }
            set
            {
                m_SelectedIndex = value;
                RaisePropertyChanged("SelectedIndex");
            }

        }
        public PTZPresetItem getSelectecItem()

        {
            if (this.SelectedIndex == -1) return null;
            return m_Items[this.SelectedIndex];
        }

        public void SelectionChanged(object sender, Windows.UI.Xaml.Controls.SelectionChangedEventArgs e)
        {
            ComboBox combo = sender as ComboBox;
            if (combo != null)
            {
                this.SelectedIndex = combo.SelectedIndex;
            }
        }
        public event PropertyChangedEventHandler PropertyChanged = null;

        private void RaisePropertyChanged(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }
    }

    public sealed class OnVifPTZControllerRunTime : System.ComponentModel.INotifyPropertyChanged
    {
        OnVifServices.OnVifPTZController m_PTZController;

        bool m_Isinitalized;
        bool m_IsStopped;

        ComboViewModelPTZPreset m_Presets;

        IOnVifMedia m_OnVifMedia;
        public bool IsInitialized
        {
            get { return m_Isinitalized; }
            set {
                m_Isinitalized = value;
                RaisePropertyChanged("IsInitialized");
            }
        }

        public ComboViewModelPTZPreset Presets
        {
            get { return m_Presets; }
            set
            {
                m_Presets = value;
                RaisePropertyChanged("Presets");
            }
        }
        public bool IsStopped
        {
            get { return m_IsStopped; }
            set
            {
                if (value != m_IsStopped)
                {
                    m_IsStopped = value;
                    RaisePropertyChanged("IsStopped");
                }

            }
        }

   
        public int PanIncrements {
            get { return m_PTZController.PanIncrements; }
            set { m_PTZController.PanIncrements = value;
                RaisePropertyChanged("PanIncrements");
            }
        }

        public int TiltIncrements
        {
            get { return m_PTZController.TiltIncrements; }
            set { m_PTZController.TiltIncrements = value;
                RaisePropertyChanged("TiltIncrements");
            }
        }



        public Windows.Foundation.IAsyncOperation<Boolean> InitalizeAsync()
        {
            Task<Boolean> from = InitializeInternalAsync();

            Windows.Foundation.IAsyncOperation<Boolean> to = from.AsAsyncOperation();

            return to;


        }
 

        public Windows.Foundation.IAsyncOperation<Boolean> RemovePresetAsync(string presettoken)
        {
            Task<Boolean> ret = RemovePresetInternalAsync(presettoken);

            return ret.AsAsyncOperation();

        }

        public Windows.Foundation.IAsyncOperation<Boolean> setPTZPresetAsync(string presettoken)
        {
            Task<Boolean> ret = setPTZPresetInternalAsync(presettoken);

            return ret.AsAsyncOperation();

        }


        public void ButtonPanTiltClickAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Windows.Foundation.IAsyncAction ret = PanTiltClickInternalAsync(sender, e).AsAsyncAction();
            //     return ret;
        }

        // Buttons Read Video-Encodings
        //public Windows.Foundation.IAsyncOperation<IList<PTZPresetItem>> ButtonreadPTZPresetsAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        public void ButtonreadPTZPresetsAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Button send = sender as Button;
            if (send != null) send.IsEnabled = false;

            Windows.Foundation.IAsyncOperation<IList<PTZPresetItem>> from = readPTZPresetsAsync();

            if (send != null) send.IsEnabled = true;
 //           return from;
        }

 


        //public Windows.Foundation.IAsyncOperation<Boolean> ButtonGoToPTZPresetAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        public void ButtonGoToPTZPresetAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Windows.Foundation.IAsyncOperation<Boolean> from = null;
            Button send = sender as Button;
            if (send != null) send.IsEnabled = false;

            PTZPresetItem preset = this.Presets.getSelectecItem();
            if (preset == null) return ;

            /*if (preset.Token == "HomeToken")
            {
                from = this.SetHomePositionAsync();
            }
            else
            */
            {
                if (preset == null) return ;

                from = this.GotoPresetAsync(preset.Token);
            }


       
   

            if (send != null) send.IsEnabled = true;

            return ;

        }

        public void ButtonDeletePTZPresetAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)

   //     public Windows.Foundation.IAsyncOperation<Boolean> ButtonDeletePTZPresetAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Windows.Foundation.IAsyncOperation<Boolean> ret = null;
           Button send = sender as Button;
            if (send != null) send.IsEnabled = false;

         //   int Idx = this.Presets.SelectedIndex;

            PTZPresetItem preset = this.Presets.getSelectecItem();
            if (preset == null) return ;

            ret = this.RemovePresetAsync(preset.Token);
   

            if (send != null) send.IsEnabled = true;
            return ;

        }
        public  void ButtonAddPTZPresetAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {

            Button send = sender as Button;
            if (send != null) send.IsEnabled = false;

          //  int Idx = this.Presets.SelectedIndex;

       
            var ret = this.setPTZPresetAsync("");


            if (send != null) send.IsEnabled = true;
    //        return ret;

        }

        public void ButtonSetPTZPresetAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Windows.Foundation.IAsyncOperation<Boolean> ret = null;
            Button send = sender as Button;
            if (send != null) send.IsEnabled = false;

           // int Idx = this.Presets.SelectedIndex;
            PTZPresetItem preset = this.Presets.getSelectecItem();
            /*if (preset.Token == "HomeToken")
            {
                ret = this.SetHomePositionAsync();
            }
            else
            */
            {
                if (preset == null) return ;

                ret = this.setPTZPresetAsync(preset.Token);
            }
            if (send != null) send.IsEnabled = true;
     //       return ret;

        }



        public Windows.Foundation.IAsyncAction StopAsync()
        {
            Windows.Foundation.IAsyncAction ret = StopInternalAsync().AsAsyncAction();
            return ret;

        }

        public Windows.Foundation.IAsyncOperation<Boolean> SetHomePositionAsync()
        {

            Task<Boolean> from = SetHomePositionInternalAsync();
            Windows.Foundation.IAsyncOperation<Boolean> ret = from.AsAsyncOperation();

            return ret;
        }



        public Windows.Foundation.IAsyncOperation<Boolean> GotoHomePositionAsync()
        {

            Task<Boolean> from = GotoHomePositionInternalAsync();
            Windows.Foundation.IAsyncOperation<Boolean> ret = from.AsAsyncOperation();

            return ret;
        }



        public Windows.Foundation.IAsyncOperation<Boolean> GotoPresetAsync(string presettoken)
        {

            Task<Boolean> from = GotoPresetInternalAsync(presettoken);
            Windows.Foundation.IAsyncOperation<Boolean> ret = from.AsAsyncOperation();

            return ret;
        }

   
        public Windows.Foundation.IAsyncOperation<IList<PTZPresetItem>> readPTZPresetsAsync()
        {
            Task<IList<PTZPresetItem>> ret = readPTZPresetsInternalAsync();

            Windows.Foundation.IAsyncOperation<IList<PTZPresetItem>> from = ret.AsAsyncOperation();
            return from;
        }

       

        internal OnVifPTZControllerRunTime(OnVifDevice OnVifDevice, IOnVifMedia OnVifMedia)
        {
            m_OnVifMedia = OnVifMedia;
            m_PTZController = new OnVifServices.OnVifPTZController(OnVifDevice);
            m_Isinitalized = false;
            m_IsStopped = true;
            m_Presets = new ComboViewModelPTZPreset();
        }

        internal async Task<Boolean> GotoPresetInternalAsync(string presettoken)
        {
            Boolean ret = false;
            try
            {
                ret = await m_PTZController.GotoPresetAsync(presettoken);
            }
            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();
            }
            return ret;
        }

        internal async Task<IList<PTZPresetItem>> readPTZPresetsInternalAsync()
        {
            try
            {
                m_Presets.Items.Clear();
                var presets = await this.m_PTZController.readPTZPresetsAsync();
                if (presets == null) return null;
                int j = 0;
                foreach (var preset in presets.Preset)
                {
                    m_Presets.Items.Add( new PTZPresetItem (preset.Name, preset.token, preset.PTZPosition,j++));
                }
                m_Presets.SelectedIndex = 0;
            }
            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();
            }
            return m_Presets.Items;
 
        }

        internal async Task<Boolean> RemovePresetInternalAsync(string presettoken)
        {
            Boolean ret = false;
            try
            {
                ret = await this.m_PTZController.RemovePresetAsync(presettoken);
                await readPTZPresetsInternalAsync();
            }

            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();
            }

            return ret;

        }

        internal async Task<Boolean> setPTZPresetInternalAsync(string presettoken)
        {
            Boolean ret = false;
            try
            {
                ret = await this.m_PTZController.setPTZPresetAsync(presettoken);
                await readPTZPresetsInternalAsync();
            }
            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();
            }

            return ret;

        }


        internal async Task<Boolean> SetHomePositionInternalAsync()
        {
            Boolean ret = false;
            try
            {
                ret = await this.m_PTZController.SetHomePositionAsync();
            }

            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();
            }
            return ret;

        }

        internal async Task<Boolean> GotoHomePositionInternalAsync()
        {
            Boolean ret = false;
            try
            {
                ret = await this.m_PTZController.GotoHomePositionAsync();
            }

            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();
            }
            return ret;
        }


        internal async Task<bool> InitializeInternalAsync()
        {
            this.IsInitialized = false;
            bool ret = false;
            try
            {
                if (!this.m_OnVifMedia.Isinitalized)
                {
                    await this.m_OnVifMedia.InitalizeAsync();
                }

                ret = await m_PTZController.InitalizeAsync();
                m_PTZController.ProfileRefToken = m_OnVifMedia.ProfileRefToken;
                var presets = await readPTZPresetsInternalAsync();
                this.IsInitialized = ret;
            }
            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();
            }
            return ret;
        }

        internal async Task StopInternalAsync()
        {
            try
            {
                await m_PTZController.StopAsync();
            }

            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();
            }

        }


        internal async Task PanTiltClickInternalAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if ((m_PTZController != null) && m_PTZController.IsInitialized)
            {
                Windows.UI.Xaml.Controls.Button send = sender as Windows.UI.Xaml.Controls.Button;
                try
                {

                    switch (send.Name)
                    {
                        case "Stop":
                            await StopInternalAsync();
                            break;

                        case "PanLeft":
                            m_PTZController.PanLeft();
                            break;
                        case "PanRight":
                            m_PTZController.PanRight();
                            break;
                        case "TiltUp":
                            m_PTZController.TiltUp();
                            break;
                        case "TiltDown":
                            m_PTZController.TiltDown();
                            break;

                        case "PanLeftTiltDown":
                            m_PTZController.PanLeftTiltDown();
                            break;
                        case "PanLeftTiltUp":
                            m_PTZController.PanLeftTiltUp();
                            break;
                        case "PanRightTiltUp":
                            m_PTZController.PanRightTiltUp();
                            break;
                        case "PanRightTiltDown":
                            m_PTZController.PanRightTiltDown();
                            break;
                        case "ZoomIn":
                            m_PTZController.ZoomIn();
                            break;
                        case "ZoomOut":
                            m_PTZController.ZoomOut();
                            break;


                    }
                    this.IsStopped = false;
                }
      
                catch (OnVifException ex)
                {
                    this.IsStopped = false;
                    OnVifMsgbox msg = new OnVifMsgbox(ex);
                    msg.Show();
                }

            }


        }
        public event PropertyChangedEventHandler PropertyChanged = null;

        private void RaisePropertyChanged(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }
    }
}
