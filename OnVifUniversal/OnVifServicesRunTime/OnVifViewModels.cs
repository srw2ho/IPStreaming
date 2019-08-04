using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OnVifServices.OnvifMedia10;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using OnVifServices.OnvifMedia20;
using OnVifServices;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Core;

namespace OnVifServicesRunTime
{



    [Windows.UI.Xaml.Data.Bindable]
    public sealed class ComboItemValue
    {
        public double Value { get; set; }

        public ComboItemValue(double ItemValue)
        {
            Value = ItemValue;
        }

        public string VisibleValue
        {
            get
            {
                string str = string.Format("{0}", Value);
                return str;
            }
        }
    }
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class AudioEncodingItem
    {
        private string m_Value;

        public string Value
        {
            get
            {
                return  m_Value;
            }
        }

        internal AudioEncodingItem(string ItemValue)
        {
            m_Value = ItemValue;
        }

        public string VisibleValue
        {
            get
            {
               return  m_Value.ToString();

            }
        }
    }



    [Windows.UI.Xaml.Data.Bindable]
    public sealed class ComboViewModelAudioEncodingBase : INotifyPropertyChanged
    {

        private ObservableCollection<AudioEncodingItem> m_Items;
        private int m_SelectedIndex;


        public IList<AudioEncodingItem> Items
        {
            get { return m_Items; }
        }

        public ComboViewModelAudioEncodingBase()
        {
            m_Items = new ObservableCollection<AudioEncodingItem>();
            m_SelectedIndex = -1;


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
        public AudioEncodingItem getSelectecItem()

        {
            if (this.SelectedIndex == -1) return null;
            return m_Items[this.SelectedIndex];
        }

        public void SelectionChanged(object sender, SelectionChangedEventArgs e)
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


    [Windows.UI.Xaml.Data.Bindable]
    public sealed class ComboViewModelItemBase : INotifyPropertyChanged
    {

        private ObservableCollection<ComboItemValue> m_Items;
        private int m_SelectedIndex;


        public IList<ComboItemValue> Items {
            get { return m_Items; }
        }

        public ComboViewModelItemBase()
        {
            m_Items = new ObservableCollection<ComboItemValue>();
            m_SelectedIndex = -1;


        }
        public int SelectedIndex
        {

            get { return m_SelectedIndex; }
            set {
                m_SelectedIndex = value;
                RaisePropertyChanged("SelectedIndex");
            }

        }
        public ComboItemValue getSelectecItem()

        {
            if (m_Items == null) return null;
            if (m_Items.Count == 0) return null;
            if (this.SelectedIndex == -1) return null;
            return m_Items[this.SelectedIndex];
        }

        public void SelectionChanged(object sender, SelectionChangedEventArgs e)
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

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class OnVifResolution : INotifyPropertyChanged
    {
        private int m_Height;
        private int m_Width;
        public OnVifResolution(int height, int width)
        {
            m_Height = height;
            m_Width = width;
        }
        public int Height {
            get { return m_Height; }
            set { m_Height = value; RaisePropertyChanged("Height"); }
        }

        public int Width
        {
            get { return m_Width; }
            set { m_Width = value; RaisePropertyChanged("Width"); }
        }


        public string VisibleItem
        {
            get
            {
                string str = string.Format("{0} x {1}", Height, Width);
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
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class ComboViewModelResolution : INotifyPropertyChanged
    {

        private ObservableCollection<OnVifResolution> m_Items;
        private int m_SelectedIndex;


        public IList<OnVifResolution> Items
        {
            get { return m_Items; }
        }

        public ComboViewModelResolution()
        {
            m_Items = new ObservableCollection<OnVifResolution>();
            m_SelectedIndex = -1;


        }
        public int SelectedIndex
        {

            get {
                return m_SelectedIndex;
            }
            set {
                m_SelectedIndex = value;
                RaisePropertyChanged("SelectedIndex");
            }

        }
        public OnVifResolution getSelectecItem()

        {
            if (this.SelectedIndex == -1) return null;
            return m_Items[this.SelectedIndex];
        }

        public void SelectionChanged(object sender, SelectionChangedEventArgs e)
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


    [Windows.UI.Xaml.Data.Bindable]
    public sealed class OnVifRange : INotifyPropertyChanged
    {
        private int m_Min;
        private int m_Max;

        public OnVifRange(int min, int max)
        {
            m_Min = min;
            m_Max = max;
        }

        public int Max
        {
            get { return m_Max; }
            set { m_Max = value; RaisePropertyChanged("Max"); }
        }

        public int Min
        {
            get { return m_Min; }
            set { m_Min = value; RaisePropertyChanged("Min"); }
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

    [Windows.UI.Xaml.Data.Bindable]
    //public sealed class OnVifAudioProfileEncoding : ILocalStorageItem, INotifyPropertyChanged
    public sealed class OnVifAudioProfileEncoding :  INotifyPropertyChanged
    {

        private string m_encodingField;

        private int m_bitrateField;

        private int m_sampleRateField;

        private OnVifServices.OnvifMedia10.AudioEncoderConfiguration m_AudioEncoderConfiguration;
       

        private OnVifServices.OnvifMedia20.AudioEncoder2Configuration[] m_AudioEncoder2Configuration;

        LocalStorageItem m_localStorage;

        public OnVifAudioProfileEncoding()
        {
            //m_AudioEncoderConfiguration = new OnVifServices.OnvifMedia10.AudioEncoderConfiguration();
            //m_AudioEncoder2Configuration = new OnVifServices.OnvifMedia20.AudioEncoder2Configuration[1];

            m_AudioEncoderConfiguration = null;
            m_AudioEncoder2Configuration = null;


            m_encodingField = "";
            m_bitrateField = 0;
            m_sampleRateField = 0;

        }
        internal LocalStorageItem LocalStorageItem
        {
            set { this.m_localStorage = value; }
            get { return this.m_localStorage; }
        }
        // Store settings to local storage

        internal bool writeSettingsToLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, int Idx) 
        {
            if (m_localStorage == null) return false;
            bool bok = m_localStorage.writeSettingsToLocalStorage(composite, Idx);
            bok = m_localStorage.writeIntegerSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_bitrateField", Idx), m_bitrateField);
            bok = m_localStorage.writeIntegerSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_sampleRateField", Idx), m_sampleRateField);
            bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_encodingField", Idx), m_encodingField);

            return bok;
        }
        // read settings from local storage
        internal bool readSettingsfromLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, int Idx)
        {
            if (m_localStorage == null) return false;
            bool bStoreok = m_localStorage.readSettingsfromLocalStorage(composite, Idx);
            if (bStoreok)
            {
                bool bok = m_localStorage.readIntegerSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_bitrateField", Idx), out m_bitrateField);
                bok = m_localStorage.readIntegerSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_sampleRateField", Idx), out m_sampleRateField);
                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_encodingField", Idx), out  m_encodingField);
            }

            return bStoreok;
        }
        
        internal OnVifServices.OnvifMedia10.AudioEncoderConfiguration AudioEncoderConfiguration
        {

            set
            {
                m_AudioEncoderConfiguration = value;
                RaisePropertyChanged("AudioEncoderConfiguration");
            }
        }

        internal OnVifServices.OnvifMedia20.AudioEncoder2Configuration[] AudioEncoder2Configuration
        {
            set
            {
                m_AudioEncoder2Configuration = value;
                RaisePropertyChanged("AudioEncoder2Configuration");
            }
        }


        public int Bitrate
        {
            get
            {
                return this.m_bitrateField;
            }
            set
            {
                m_bitrateField = value;
                RaisePropertyChanged("Bitrate");
            }
        }

        public int SampleRate
        {
            get
            {
                return this.m_sampleRateField;
            }
            set
            {
                m_sampleRateField = value;
                RaisePropertyChanged("SampleRate");
            }
        }

        internal string Encoding
        {
            get
            {
                return this.m_encodingField;
            }
            set
            {
                m_encodingField = value;
                RaisePropertyChanged("Encoding");
            }
        }


        internal bool buildConfigurations()
        {

            if (m_AudioEncoderConfiguration != null)
            {
                this.Encoding = m_AudioEncoderConfiguration.Encoding.ToString();
                this.Bitrate = m_AudioEncoderConfiguration.Bitrate;
                this.SampleRate = m_AudioEncoderConfiguration.SampleRate;
            }
            else
            if (m_AudioEncoder2Configuration != null)
            {
                OnVifServices.OnvifMedia20.AudioEncoder2Configuration encoding = null;
                if (m_AudioEncoder2Configuration.Length > 0)
                {
                    encoding = m_AudioEncoder2Configuration[0];
                }
                /*
                foreach (var item in m_AudioEncoder2Configuration)
                {
                    if (item.Encoding=="AAA")
                    {
                        encoding = item;
                        break;
                    }

                }
                */
                if (encoding != null)
                {
                    this.Encoding = encoding.Encoding;
                    this.Bitrate = encoding.Bitrate;
                    this.SampleRate = encoding.SampleRate;
                }

            }



            return true;
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


    [Windows.UI.Xaml.Data.Bindable]
    public sealed class OnVifVideoProfileEncoding :  INotifyPropertyChanged
    //public sealed class OnVifVideoProfileEncoding : ILocalStorageItem, INotifyPropertyChanged
    {

        OnVifServices.OnvifMedia10.VideoEncoderConfiguration m_VideoEncoderConfiguration;
        OnVifServices.OnvifMedia20.VideoEncoder2Configuration[] m_VideoEncoder2Configuration;

        LocalStorageItem m_localStorage;
        OnVifResolution m_Resolution;
        float m_FrameRateLimit;
        int m_BitrateLimit;
        int m_EncodingInterval;
        float m_Quality;
        bool m_ConstantBitRate;
        int m_GovLength;
        bool m_GovLengthSpecified;
        bool m_ConstantBitRateSpecified;
        string m_Encoding;

        public OnVifVideoProfileEncoding()
        {
            //          m_localStorage = new LocalStorageItem("OnVifVideoProfileEncoding");
            //  m_VideoEncoderConfiguration = new OnVifServices.OnvifMedia10.VideoEncoderConfiguration();
            //  m_VideoEncoder2Configuration = new OnVifServices.OnvifMedia20.VideoEncoder2Configuration[1]; ;
            m_VideoEncoderConfiguration = null;
            m_VideoEncoder2Configuration = null;
            m_Resolution = new OnVifResolution(0, 0);

            m_FrameRateLimit = 0;
            m_BitrateLimit = 0;
            m_EncodingInterval = 0;
            m_Quality = 0;
            m_ConstantBitRate = false;
            m_GovLength = 0;
            m_GovLengthSpecified = false;
            m_ConstantBitRateSpecified = false;
            m_Encoding = "";
        }

        // Store settings to local storage
        internal LocalStorageItem LocalStorageItem
        {
            set { this.m_localStorage = value; }
            get { return this.m_localStorage; }
        }

        internal bool writeSettingsToLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, int Idx)
        {
            if (m_localStorage == null) return false;
            bool bok = m_localStorage.writeSettingsToLocalStorage(composite, Idx);
            bok = m_localStorage.writeFloatSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_FrameRateLimit", Idx), m_FrameRateLimit);
            bok = m_localStorage.writeIntegerSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_BitrateLimit", Idx), m_BitrateLimit);

            bok = m_localStorage.writeIntegerSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Resolution.Height", Idx), m_Resolution.Height);
            bok = m_localStorage.writeIntegerSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Resolution.Width", Idx), m_Resolution.Width);

            bok = m_localStorage.writeIntegerSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_EncodingInterval", Idx), m_EncodingInterval);

            bok = m_localStorage.writeFloatSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Quality", Idx), m_Quality);

            bok = m_localStorage.writeBoolSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_ConstantBitRate", Idx), m_ConstantBitRate);

            bok = m_localStorage.writeIntegerSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_GovLength", Idx), m_GovLength);

            bok = m_localStorage.writeBoolSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_GovLengthSpecified", Idx), m_GovLengthSpecified);

            bok = m_localStorage.writeBoolSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_ConstantBitRateSpecified", Idx), m_ConstantBitRateSpecified);

            bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Encoding", Idx), m_Encoding);

            return bok;
        }
        // read settings from local storage
        internal bool readSettingsfromLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, int Idx)
        {
            if (m_localStorage == null) return false;
            bool bStoreok = m_localStorage.readSettingsfromLocalStorage(composite, Idx);
            if (bStoreok)
            {
                bool bok = m_localStorage.readFloatSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_FrameRateLimit", Idx), out m_FrameRateLimit);
                bok = m_localStorage.readIntegerSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_BitrateLimit", Idx), out m_BitrateLimit);
                int Height;
                int Width;
                bok = m_localStorage.readIntegerSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Resolution.Height", Idx), out Height);
                if (bok) m_Resolution.Height = Height;
                bok = m_localStorage.readIntegerSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Resolution.Width", Idx), out Width);
                if (bok) m_Resolution.Width = Width;
                bok = m_localStorage.readIntegerSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_EncodingInterval", Idx), out m_EncodingInterval);

                bok = m_localStorage.readFloatSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Quality", Idx), out m_Quality);

                bok = m_localStorage.readBoolSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_ConstantBitRate", Idx), out m_ConstantBitRate);

                bok = m_localStorage.readIntegerSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_GovLength", Idx), out m_GovLength);

                bok = m_localStorage.readBoolSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_GovLengthSpecified", Idx), out m_GovLengthSpecified);

                bok = m_localStorage.readBoolSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_ConstantBitRateSpecified", Idx), out m_ConstantBitRateSpecified);

                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Encoding", Idx), out m_Encoding);

            }

            return bStoreok;
        }
        
        internal OnVifServices.OnvifMedia10.VideoEncoderConfiguration VideoEncoderConfiguration
        {
    
            set
            {
                m_VideoEncoderConfiguration = value;
                RaisePropertyChanged("VideoEncoderConfiguration");
            }
        }

        internal OnVifServices.OnvifMedia20.VideoEncoder2Configuration[] VideoEncoder2Configuration
        {

            set
            {
                m_VideoEncoder2Configuration = value;
                RaisePropertyChanged("VideoEncoder2Configuration");
            }
        }



        public int GovLength
        {
            get
            {
                return this.m_GovLength;
            }
            set
            {
                m_GovLength = value;
                RaisePropertyChanged("GovLength");
            }
        }

        public Boolean GovLengthSpecified
        {
            get
            {
                return this.m_GovLengthSpecified;
            }
            set
            {
                m_GovLengthSpecified = value;
                RaisePropertyChanged("GovLengthSpecified");
            }
        }

        


        public Boolean ConstantBitRateSpecified
        {
            get
            {
                return this.m_ConstantBitRateSpecified;
            }
            set
            {
                m_ConstantBitRateSpecified = value;
                RaisePropertyChanged("ConstantBitRateSpecified");
            }
        }
        public Boolean ConstantBitRate
        {
            get
            {
                return this.m_ConstantBitRate;
            }
            set
            {
                m_ConstantBitRate = value;
                RaisePropertyChanged("ConstantBitRate");
            }
        }
        public string Encoding
        {
            get
            {
                return this.m_Encoding;
            }
            set
            {
                m_Encoding = value;
                RaisePropertyChanged("Encoding");
            }
        }

        public float Quality
        {
            get
            {
                return this.m_Quality;
            }
            set
            {
                m_Quality = value;
                RaisePropertyChanged("Quality");
            }
        }

        public OnVifResolution Resolution
        {
            get
            {
                return this.m_Resolution;
            }
            set
            {
                m_Resolution = value;
                RaisePropertyChanged("Resolution");
            }
        }

        public float FrameRateLimit {
            get {
                return this.m_FrameRateLimit;
            }
            set {
                m_FrameRateLimit = value;
                RaisePropertyChanged("FrameRateLimit");
            }
        }

        public int BitrateLimit
        {
            get
            {
                return this.m_BitrateLimit;
            }
            set
            {
                m_BitrateLimit = value;
                RaisePropertyChanged("BitrateLimit");
            }
        }
        public int EncodingInterval
        {
            get
            {
                return this.m_EncodingInterval;
            }
            set
            {
                m_EncodingInterval = value;
                RaisePropertyChanged("EncodingInterval");
            }
        }


        internal bool buildConfigurations()
        {
            if (this.m_VideoEncoderConfiguration != null)
            {

                this.Encoding = "";
                if (m_VideoEncoderConfiguration.H264!=null)
                {
                    this.GovLength = m_VideoEncoderConfiguration.H264.GovLength;
                    this.Encoding = "H264";
                }
                else
                if (m_VideoEncoderConfiguration.MPEG4 != null)
                {
                    this.GovLength = m_VideoEncoderConfiguration.MPEG4.GovLength;
                    this.Encoding = "MPEG4";
                }

                this.Quality = this.m_VideoEncoderConfiguration.Quality;

                this.BitrateLimit = m_VideoEncoderConfiguration.RateControl.BitrateLimit;
                this.FrameRateLimit = m_VideoEncoderConfiguration.RateControl.FrameRateLimit;
                this.EncodingInterval = m_VideoEncoderConfiguration.RateControl.EncodingInterval;
                this.Resolution.Height = m_VideoEncoderConfiguration.Resolution.Height;
                this.Resolution.Width = m_VideoEncoderConfiguration.Resolution.Width;
                this.GovLengthSpecified = false;
                this.ConstantBitRate = false;
                this.ConstantBitRateSpecified = false;
         

            }
            else if (this.m_VideoEncoder2Configuration != null)
            {
                OnVifServices.OnvifMedia20.VideoEncoder2Configuration findencoding = null;
                if (m_VideoEncoder2Configuration.Length > 0)
                {
                    findencoding = m_VideoEncoder2Configuration[0];
                }
                if (findencoding != null)
                {
                    this.Encoding = findencoding.Encoding;
                    this.Quality = findencoding.Quality;
                    this.GovLength = findencoding.GovLength;
                    this.GovLengthSpecified = findencoding.GovLengthSpecified;
                    this.ConstantBitRate = findencoding.RateControl.ConstantBitRate;
                    this.ConstantBitRateSpecified = findencoding.RateControl.ConstantBitRateSpecified;
                    this.BitrateLimit = findencoding.RateControl.BitrateLimit;
                    this.FrameRateLimit = findencoding.RateControl.FrameRateLimit;
                    this.EncodingInterval = 0;
                    this.Resolution.Height = findencoding.Resolution.Height;
                    this.Resolution.Width = findencoding.Resolution.Width;
                }
 
            }


            return true;
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

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class OnVifAudioProfileEncodingConfiguration : INotifyPropertyChanged
    { 


  
        ComboViewModelItemBase m_SampleRates;
        ComboViewModelItemBase m_BitRates;
        ComboViewModelAudioEncodingBase m_AudioEncodings;


        private AudioEncoderConfigurationOption[] m_optionsField;

        private AudioEncoder2ConfigurationOptions[] m_options2Field;


        public OnVifAudioProfileEncodingConfiguration()
        {

            m_SampleRates = new ComboViewModelItemBase();
            m_BitRates = new ComboViewModelItemBase();
            m_AudioEncodings = new ComboViewModelAudioEncodingBase();
            //   m_optionsField = new AudioEncoderConfigurationOption[1];
            //   m_options2Field = new AudioEncoder2ConfigurationOptions[1];
            m_optionsField = null;
            m_options2Field = null;

        }

        internal AudioEncoderConfigurationOption[] Options
        {
            get { return m_optionsField; }
            set
            {
                m_optionsField = value;
                RaisePropertyChanged("Options");
            }
        }
        internal AudioEncoder2ConfigurationOptions[] Options2
        {
            get { return m_options2Field; }
            set
            {
                m_options2Field = value;
                RaisePropertyChanged("Options2");
            }
        }



        public ComboViewModelItemBase BitRates
        {
        get { return m_BitRates; }
        set
        {
            m_BitRates = value;
            RaisePropertyChanged("Resolutions");
        }
    }

    public ComboViewModelItemBase SampleRates
        {
        get { return m_SampleRates; }
        set
        {
            m_SampleRates = value;
            RaisePropertyChanged("SampleRates");
        }
    }


        public ComboViewModelAudioEncodingBase AudioEncodings
        {
            get { return m_AudioEncodings; }
            set
            {
                m_AudioEncodings = value;
                RaisePropertyChanged("AudioEncodings");
            }
        }



    internal bool buildConfigurations()
    {
        m_BitRates.Items.Clear();
        m_SampleRates.Items.Clear();
        m_AudioEncodings.Items.Clear();

            if (m_optionsField != null)
            {
                foreach (AudioEncoderConfigurationOption item in m_optionsField)
                {

                    m_AudioEncodings.Items.Add(new AudioEncodingItem(item.Encoding.ToString()));
                    foreach (var bitrate in item.BitrateList)
                    {
                        m_BitRates.Items.Add(new ComboItemValue(bitrate));
                    }
                    foreach (var samplerate in item.SampleRateList)
                    {
                        m_SampleRates.Items.Add(new ComboItemValue(samplerate));
                    }


                }

            }
            else if (this.m_options2Field != null)
            {
                foreach (AudioEncoder2ConfigurationOptions item in m_options2Field)
                {
                    //item.Encoding

                    m_AudioEncodings.Items.Add(new AudioEncodingItem(item.Encoding.ToString()));

                    foreach (var bitrate in item.BitrateList)
                    {
                        m_BitRates.Items.Add(new ComboItemValue(bitrate));
                    }
                    foreach (var samplerate in item.SampleRateList)
                    {
                        m_SampleRates.Items.Add(new ComboItemValue(samplerate));
                    }

                }

            }

        return true;
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

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class OnVifVideoProfileEncodingConfiguration : INotifyPropertyChanged
    {

        ComboViewModelResolution m_Resolutions;
        OnVifRange m_QualityRange;
        OnVifRange m_FrameRateRange;
        OnVifRange m_EncodingIntervalRange;
        ComboViewModelItemBase m_FrameRates;
        ComboViewModelItemBase m_QualityRanges;
        ComboViewModelItemBase m_BitRates;

        OnVifServices.OnvifMedia10.VideoEncoderConfigurationOptions m_VideoEncoderConfigurationOptions;
        OnVifServices.OnvifMedia20.VideoEncoder2ConfigurationOptions[] m_VideoEncoder2ConfigurationOptions;
        public OnVifVideoProfileEncodingConfiguration()
        {

            //      m_VideoEncoderConfigurationOptions = new OnVifServices.OnvifMedia10.VideoEncoderConfigurationOptions();
            //      m_VideoEncoder2ConfigurationOptions = new OnVifServices.OnvifMedia20.VideoEncoder2ConfigurationOptions[1];
            m_VideoEncoderConfigurationOptions = null;
            m_VideoEncoder2ConfigurationOptions = null;
            m_Resolutions = new ComboViewModelResolution();
            m_QualityRange = new OnVifRange(0, 0);
            m_FrameRateRange = new OnVifRange(0, 0);
            m_EncodingIntervalRange = new OnVifRange(0, 0);
            m_FrameRates = new ComboViewModelItemBase();
            m_QualityRanges = new ComboViewModelItemBase();
            m_BitRates = new ComboViewModelItemBase();



        }

        internal VideoEncoderConfigurationOptions VideoEncoderConfigurationOptions
        {
            get { return m_VideoEncoderConfigurationOptions; }
            set
            {
                m_VideoEncoderConfigurationOptions = value;
                RaisePropertyChanged("VideoEncoderConfigurationOptions");
            }
        }
        internal VideoEncoder2ConfigurationOptions[] VideoEncoder2ConfigurationOptions
        {
            get { return m_VideoEncoder2ConfigurationOptions; }
            set
            {
                m_VideoEncoder2ConfigurationOptions = value;
                RaisePropertyChanged("VideoEncoder2ConfigurationOptions");
            }
        }

        public ComboViewModelResolution Resolutions {
            get { return m_Resolutions; }
            set { m_Resolutions = value;
                RaisePropertyChanged("Resolutions");
            }
        }

        public ComboViewModelItemBase FrameRates
        {
            get { return m_FrameRates; }
            set
            {
                m_FrameRates = value;
                RaisePropertyChanged("FrameRates");
            }
        }
        public ComboViewModelItemBase BitRates
        {
            get { return m_BitRates; }
            set
            {
                m_BitRates = value;
                RaisePropertyChanged("BitRates");
            }
        }


        public ComboViewModelItemBase QualityRanges
        {
            get { return m_QualityRanges; }
            set
            {
                m_QualityRanges = value;
                RaisePropertyChanged("QualityRanges");
            }
        }

        public OnVifRange QualityRange
        {
            get { return m_QualityRange; }
            set
            {
                m_QualityRange = value;
                RaisePropertyChanged("QualityRange");
            }
        }


        public OnVifRange FrameRateRange
        {
            get { return m_FrameRateRange; }
            set
            {
                m_FrameRateRange = value;
                RaisePropertyChanged("FrameRateRange");
            }
        }

        public OnVifRange EncodingIntervalRange
        {
            get { return m_EncodingIntervalRange; }
            set
            {
                m_EncodingIntervalRange = value;
                RaisePropertyChanged("EncodingIntervalRange");
            }
        }



        internal bool buildConfigurations(string activevideoencoding)
        {
            this.Resolutions.Items.Clear();
            if (m_VideoEncoderConfigurationOptions != null)
            {

                this.QualityRange.Max = m_VideoEncoderConfigurationOptions.QualityRange.Max;
                this.QualityRange.Min = m_VideoEncoderConfigurationOptions.QualityRange.Min;

                if (activevideoencoding== "H264")
                {
                    if (m_VideoEncoderConfigurationOptions.H264 != null) // H264 wird bevorzugt
                    {
                        foreach (var item in m_VideoEncoderConfigurationOptions.H264.ResolutionsAvailable)
                        {
                            this.Resolutions.Items.Add(new OnVifResolution(item.Height, item.Width));
                        }
                        this.EncodingIntervalRange.Max = m_VideoEncoderConfigurationOptions.H264.EncodingIntervalRange.Max;
                        this.EncodingIntervalRange.Min = m_VideoEncoderConfigurationOptions.H264.EncodingIntervalRange.Min;
                        this.FrameRateRange.Max = m_VideoEncoderConfigurationOptions.H264.FrameRateRange.Max;
                        this.FrameRateRange.Min = m_VideoEncoderConfigurationOptions.H264.FrameRateRange.Min;


                    }
                }
                else if (activevideoencoding == "JPEG")
                {
                    if (m_VideoEncoderConfigurationOptions.JPEG != null)
                    {
                        foreach (var item in m_VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable)
                        {
                            this.Resolutions.Items.Add(new OnVifResolution(item.Height, item.Width));
                        }

                        this.EncodingIntervalRange.Max = m_VideoEncoderConfigurationOptions.JPEG.EncodingIntervalRange.Max;
                        this.EncodingIntervalRange.Min = m_VideoEncoderConfigurationOptions.JPEG.EncodingIntervalRange.Min;
                        this.FrameRateRange.Max = m_VideoEncoderConfigurationOptions.JPEG.FrameRateRange.Max;
                        this.FrameRateRange.Min = m_VideoEncoderConfigurationOptions.JPEG.FrameRateRange.Min;
                    }
                }
                else if (activevideoencoding == "MPEG4")
                {
                    if (m_VideoEncoderConfigurationOptions.MPEG4 != null)
                    {
                        foreach (var item in m_VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable)
                        {
                            this.Resolutions.Items.Add(new OnVifResolution(item.Height, item.Width));
                        }
                        this.EncodingIntervalRange.Max = m_VideoEncoderConfigurationOptions.MPEG4.EncodingIntervalRange.Max;
                        this.EncodingIntervalRange.Min = m_VideoEncoderConfigurationOptions.MPEG4.EncodingIntervalRange.Min;
                        this.FrameRateRange.Max = m_VideoEncoderConfigurationOptions.MPEG4.FrameRateRange.Max;
                        this.FrameRateRange.Min = m_VideoEncoderConfigurationOptions.MPEG4.FrameRateRange.Min;
                    }
                }

              


                buildFrameRates();
                buildQualityRanges();
            }
            else if (m_VideoEncoder2ConfigurationOptions!=null)
            {
                FrameRates.Items.Clear();
    
                VideoEncoder2ConfigurationOptions encoding = null;
                foreach (var item in m_VideoEncoder2ConfigurationOptions)
                {
                    if (item.Encoding == activevideoencoding) // Search for active encoding and takke settings
                    {
                        encoding = item;
                        break;
                    }
                }
           
    
                if (encoding != null)
                {

                    this.QualityRange.Max = (int) encoding.QualityRange.Max;
                    this.QualityRange.Min = (int) encoding.QualityRange.Min;

                 
                    foreach (var item in encoding.ResolutionsAvailable)
                    {
                        this.Resolutions.Items.Add(new OnVifResolution(item.Height, item.Width));
                    }

                    foreach (var item in encoding.FrameRatesSupported)
                    {
                        this.FrameRates.Items.Add(new ComboItemValue(item));
                    }


                    buildQualityRanges();

                }
            }
            else{ // settings from local storage

            }
            return true;
        }


        private bool buildFrameRates()
        {
            FrameRates.Items.Clear();

            for (int i = FrameRateRange.Min; i <= FrameRateRange.Max; i++ ){
                FrameRates.Items.Add(new ComboItemValue(i));
            }
            return true;
        }
        private bool buildQualityRanges()
        {
            QualityRanges.Items.Clear();
            for (int i = QualityRange.Min; i <= QualityRange.Max; i++)
            {
                QualityRanges.Items.Add(new ComboItemValue(i));
            }
            return true;
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

    [Windows.UI.Xaml.Data.Bindable]

    public sealed class OnVifCameraProfile :  INotifyPropertyChanged
    {

        private OnVifVideoProfileEncoding m_VideoProfileEncoding;
        private OnVifVideoProfileEncodingConfiguration m_VideoProfileEncodingConfiguration;
        private OnVifAudioProfileEncoding m_AudioProfileEncoding;
        private OnVifAudioProfileEncodingConfiguration m_AudioProfileEncodingConfiguration;

        private string m_Name;
        private string m_StreamUrl;
        private string m_StreamUrlOrigin;
        private int m_ProfileIdx;
        Boolean m_IsVideoProfileChanged;
        Boolean m_IsAudioProfileChanged;

        LocalStorageItem m_localStorage;

        public OnVifCameraProfile(int nProfileIdx)
        {
            this.m_VideoProfileEncoding = new OnVifVideoProfileEncoding();
            this.m_VideoProfileEncodingConfiguration = new OnVifVideoProfileEncodingConfiguration();
            this.m_AudioProfileEncoding = new OnVifAudioProfileEncoding ();
            this.m_AudioProfileEncodingConfiguration = new OnVifAudioProfileEncodingConfiguration();

       //     this.m_localStorage = new LocalStorageItem($"OnVifCameraProfile_{nProfileIdx}");
            this.m_localStorage = null;
            m_ProfileIdx = nProfileIdx;
            m_StreamUrl = "";
            m_StreamUrlOrigin = "";
            m_Name = "";
            m_IsVideoProfileChanged = false;
            m_IsAudioProfileChanged = false;

        }


        internal LocalStorageItem LocalStorageItem
        {
            set {this.m_localStorage = value;}
            get{return this.m_localStorage ;}
        }


        internal bool writeSettingsToLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, int Idx)
        {
            if (m_localStorage == null) return false;
            bool bok = m_localStorage.writeSettingsToLocalStorage(composite, Idx);
            bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Name", Idx), m_Name);
            bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_StreamUrl", Idx), m_StreamUrl);
            bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_StreamUrlOrigin", Idx), m_StreamUrlOrigin);

            

            string VideoProfileDataSourceName = m_localStorage.getCompositePropertyIDName("m_VideoProfileEncoding", Idx);
            string AudioProfileDataSourceName = m_localStorage.getCompositePropertyIDName("m_AudioProfileEncoding", Idx);
          

            m_localStorage.DataCompositeIDName = VideoProfileDataSourceName;
            m_VideoProfileEncoding.LocalStorageItem = m_localStorage;

            m_VideoProfileEncoding.writeSettingsToLocalStorage(composite, -1);

            m_localStorage.DataCompositeIDName = AudioProfileDataSourceName;
            m_AudioProfileEncoding.LocalStorageItem = m_localStorage;
            m_AudioProfileEncoding.writeSettingsToLocalStorage(composite, -1);
            return bok;
        }
        // read settings from local storage

        internal bool readSettingsfromLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, int Idx)
        {
            if (m_localStorage == null) return false;
            bool bStoreok = m_localStorage.readSettingsfromLocalStorage(composite, Idx);
            if (bStoreok)
            {
                bool bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Name", Idx), out m_Name);
                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_StreamUrl", Idx), out m_StreamUrl);

                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_StreamUrlOrigin", Idx), out m_StreamUrlOrigin);

                string VideoProfileDataSourceName = m_localStorage.getCompositePropertyIDName("m_VideoProfileEncoding", Idx);
                string AudioProfileDataSourceName = m_localStorage.getCompositePropertyIDName("m_AudioProfileEncoding", Idx);

                m_localStorage.DataCompositeIDName = VideoProfileDataSourceName;
                this.m_VideoProfileEncoding.LocalStorageItem = m_localStorage;
                bok = this.m_VideoProfileEncoding.readSettingsfromLocalStorage(composite, -1);

                m_localStorage.DataCompositeIDName = AudioProfileDataSourceName;
                this.m_AudioProfileEncoding.LocalStorageItem = m_localStorage;
                bok = this.m_AudioProfileEncoding.readSettingsfromLocalStorage(composite, -1);

            }

            return bStoreok;
        }
        

        private String getUrlByUserPassword(string url, string User, string Password)
        {
          if (!String.IsNullOrEmpty(User) && !String.IsNullOrEmpty(Password))
          {
             String pwUser = String.Format("//{0}:{1}@", User, Password);
             String newUrl = url.Replace("//", pwUser);
             return newUrl;
          }
          else return url;

        }

        internal bool setStreamurl(IDictionary<string, IList<string>> StreamUris, string User, string Password)
        {

            var fields = StreamUris.Values.ToList();

            if (fields.Count > 0)
            {
                var itemlist = fields[0];
                var url = itemlist[0];
                this.StreamUrlOrigin = url; // Origin Stream Url without Password and User
                this.StreamUrl = getUrlByUserPassword(url, User, Password);

            }
            return (fields.Count>0) ;
        }


        internal bool buildVideoBitRates()
        {
            m_VideoProfileEncodingConfiguration.BitRates.Items.Clear();

            double pixels_per_second = m_VideoProfileEncoding.Resolution.Width * m_VideoProfileEncoding.Resolution.Height * m_VideoProfileEncoding.FrameRateLimit;
            double Bits_per_pixelMax = 0.16;
            long video_bitrateMax = (long)(pixels_per_second * Bits_per_pixelMax + 0.5) / 1000; //int kBits
            video_bitrateMax = (long)DividedByCalculator.getNumberDividedBy((long)video_bitrateMax, 512);

            double Bits_per_pixelMin = 0.02;
            long video_bitrateMin = (long)(pixels_per_second * Bits_per_pixelMin + 0.5) / 1000; // in KBits
            video_bitrateMin = (long)DividedByCalculator.getNumberDividedBy((long)video_bitrateMin, 16);

            int counts = 10;
            long video_bitratediff = (long)(video_bitrateMax - Bits_per_pixelMin) / counts;

            video_bitratediff = (long)DividedByCalculator.getNumberDividedBy((long)video_bitratediff, 16);

            long video_bitrate = video_bitrateMin;

            m_VideoProfileEncodingConfiguration.BitRates.Items.Add(new ComboItemValue(m_VideoProfileEncoding.BitrateLimit));

            while (counts >= 0)
            {
                m_VideoProfileEncodingConfiguration.BitRates.Items.Add(new ComboItemValue(video_bitrate));
                video_bitrate += video_bitratediff;
                counts--;

            }


            return true;
        }
        internal bool setSelectedIndices()
        {
            //Set Video Encodings Indexes
            if ((m_VideoProfileEncodingConfiguration.VideoEncoder2ConfigurationOptions == null) && (m_VideoProfileEncodingConfiguration.VideoEncoderConfigurationOptions == null) ){
                m_VideoProfileEncodingConfiguration.FrameRates.Items.Add(new ComboItemValue(this.m_VideoProfileEncoding.FrameRateLimit));
                m_VideoProfileEncodingConfiguration.Resolutions.Items.Add(new OnVifResolution(this.m_VideoProfileEncoding.Resolution.Height, this.m_VideoProfileEncoding.Resolution.Width));

                m_VideoProfileEncodingConfiguration.BitRates.Items.Add(new ComboItemValue(this.m_VideoProfileEncoding.BitrateLimit));
                m_VideoProfileEncodingConfiguration.QualityRanges.Items.Add(new ComboItemValue(this.m_VideoProfileEncoding.Quality));
            }

            if ((m_AudioProfileEncodingConfiguration.Options == null) && (m_AudioProfileEncodingConfiguration.Options2 == null))
            {
                m_AudioProfileEncodingConfiguration.BitRates.Items.Add(new ComboItemValue(this.m_AudioProfileEncoding.Bitrate));
                m_AudioProfileEncodingConfiguration.SampleRates.Items.Add(new ComboItemValue(this.m_AudioProfileEncoding.SampleRate));
                m_AudioProfileEncodingConfiguration.AudioEncodings.Items.Add(new AudioEncodingItem(this.m_AudioProfileEncoding.Encoding));

            }


            int Idx = 0;
            foreach (var item in m_VideoProfileEncodingConfiguration.FrameRates.Items)
            {
                if (item.Value == this.m_VideoProfileEncoding.FrameRateLimit)
                {
                    m_VideoProfileEncodingConfiguration.FrameRates.SelectedIndex = Idx;

                    break;
                }
                Idx++;
            }
            Idx = 0;
            foreach (var item in m_VideoProfileEncodingConfiguration.Resolutions.Items)
            {
                if (item.Height == this.m_VideoProfileEncoding.Resolution.Height && item.Width == this.m_VideoProfileEncoding.Resolution.Width)
                {
                    m_VideoProfileEncodingConfiguration.Resolutions.SelectedIndex = Idx;

                    break;
                }
                Idx++;
            }

            Idx = 0;
            foreach (var item in m_VideoProfileEncodingConfiguration.QualityRanges.Items)
            {
                if (item.Value == this.m_VideoProfileEncoding.Quality)
                {
                    m_VideoProfileEncodingConfiguration.QualityRanges.SelectedIndex = Idx;
      
                    break;
                }
                Idx++;
            }
            Idx = 0;
            foreach (var item in m_VideoProfileEncodingConfiguration.BitRates.Items)
            {
                if (item.Value == this.m_VideoProfileEncoding.BitrateLimit)
                {
                    m_VideoProfileEncodingConfiguration.BitRates.SelectedIndex = Idx;

                    break;
                }
                Idx++;
            }

            // Set Audio Encodings Indexes
            Idx = 0;
            foreach (var item in m_AudioProfileEncodingConfiguration.BitRates.Items)
            {
                if (item.Value == this.m_AudioProfileEncoding.Bitrate)
                {
                    m_AudioProfileEncodingConfiguration.BitRates.SelectedIndex = Idx;

                    break;
                }
                Idx++;
            }
            Idx = 0;
            foreach (var item in m_AudioProfileEncodingConfiguration.SampleRates.Items)
            {
                if (item.Value == this.m_AudioProfileEncoding.SampleRate)
                {
                    m_AudioProfileEncodingConfiguration.SampleRates.SelectedIndex = Idx;

                    break;
                }
                Idx++;
            }
 
     //       m_AudioProfileEncodingConfiguration.AudioEncodings.Items.Add(new AudioEncodingItem (this.m_AudioProfileEncoding.Encoding));
            Idx = 0;
            foreach (var item in m_AudioProfileEncodingConfiguration.AudioEncodings.Items)
            {
                if (item.Value == this.m_AudioProfileEncoding.Encoding)
                {
                    m_AudioProfileEncodingConfiguration.AudioEncodings.SelectedIndex = Idx;

                    break;
                }
                Idx++;
            }

            return true;
        }


        public Boolean IsVideoProfileChanged
        {
            get { return m_IsVideoProfileChanged; }
            set
            {
                m_IsVideoProfileChanged = value;
                RaisePropertyChanged("IsVideoProfileChanged");
            }
        }

        public Boolean IsAudioProfileChanged
        {
            get { return m_IsAudioProfileChanged; }
            set
            {
                m_IsAudioProfileChanged = value;
                RaisePropertyChanged("IsAudioProfileChanged");
            }
        }

        public void VideoSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox combo = sender as ComboBox;
            if (combo != null)
            {
                if (combo.ItemsSource == this.m_VideoProfileEncodingConfiguration.Resolutions.Items)
                {
                    this.m_VideoProfileEncodingConfiguration.Resolutions.SelectionChanged(sender, e);
                    OnVifResolution value = this.m_VideoProfileEncodingConfiguration.Resolutions.getSelectecItem();
                    if (value != null)
                    {
                        if ((value.Height != m_VideoProfileEncoding.Resolution.Height) && (value.Width != m_VideoProfileEncoding.Resolution.Width))
                        {
                            m_VideoProfileEncoding.Resolution.Height = value.Height;
                            m_VideoProfileEncoding.Resolution.Width = value.Width;
                            this.buildVideoBitRates();
                            this.IsVideoProfileChanged = true;
                        }


                    }



                }
                if (combo.ItemsSource == this.m_VideoProfileEncodingConfiguration.BitRates.Items)
                {
                    this.m_VideoProfileEncodingConfiguration.BitRates.SelectionChanged(sender, e);
                    ComboItemValue value = this.m_VideoProfileEncodingConfiguration.BitRates.getSelectecItem();
                    if (value != null)
                    {
                        if (m_VideoProfileEncoding.BitrateLimit != (int)value.Value)
                        {
                            m_VideoProfileEncoding.BitrateLimit = (int)value.Value;
                            this.IsVideoProfileChanged = true;
                        }

                    }

                }
                if (combo.ItemsSource == this.m_VideoProfileEncodingConfiguration.QualityRanges.Items)
                {
                    this.m_VideoProfileEncodingConfiguration.QualityRanges.SelectionChanged(sender, e);
                    ComboItemValue value = this.m_VideoProfileEncodingConfiguration.QualityRanges.getSelectecItem();
                    if (value != null)
                    {
                        if (m_VideoProfileEncoding.Quality != (float)value.Value)
                        {
                            m_VideoProfileEncoding.Quality = (float)value.Value;
                            this.IsVideoProfileChanged = true;
                        }

                    }



                }
                if (combo.ItemsSource == this.m_VideoProfileEncodingConfiguration.FrameRates.Items)
                {
                    this.m_VideoProfileEncodingConfiguration.FrameRates.SelectionChanged(sender, e);
                    ComboItemValue value = this.m_VideoProfileEncodingConfiguration.FrameRates.getSelectecItem();
                    if (value != null)
                    {
       
                        if (m_VideoProfileEncoding.FrameRateLimit != (int)value.Value)
                        {
                            m_VideoProfileEncoding.FrameRateLimit = (int)value.Value;
                            this.IsVideoProfileChanged = true;
                        }
                    }


                }


            }

        }
        public void AudioSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox combo = sender as ComboBox;
            if (combo != null)
            {
                if (combo.ItemsSource == this.m_AudioProfileEncodingConfiguration.BitRates.Items)
                {
                    this.m_AudioProfileEncodingConfiguration.BitRates.SelectionChanged(sender, e);
                    var value = this.m_AudioProfileEncodingConfiguration.BitRates.getSelectecItem();
                    if (value != null)
                    {
                        if (m_AudioProfileEncoding.Bitrate != (int)value.Value)
                        {
                            m_AudioProfileEncoding.Bitrate = (int)value.Value;
                            this.IsAudioProfileChanged = true;
                        }


                    }

                }

                if (combo.ItemsSource == this.m_AudioProfileEncodingConfiguration.SampleRates.Items)
                {
                    this.m_AudioProfileEncodingConfiguration.SampleRates.SelectionChanged(sender, e);
                    var value = this.m_AudioProfileEncodingConfiguration.SampleRates.getSelectecItem();
                    if (value != null)
                    {
                        if (m_AudioProfileEncoding.SampleRate != (int)value.Value)
                        {
                            m_AudioProfileEncoding.SampleRate = (int)value.Value;
                            this.IsAudioProfileChanged = true;
                        }

                    }

                }

                if (combo.ItemsSource == this.m_AudioProfileEncodingConfiguration.AudioEncodings.Items)
                {
                    this.m_AudioProfileEncodingConfiguration.AudioEncodings.SelectionChanged(sender, e);
                    var value = this.m_AudioProfileEncodingConfiguration.AudioEncodings.getSelectecItem();
                    if (value != null)
                    {
                        if (m_AudioProfileEncoding.Encoding != value.Value)
                        {
                            m_AudioProfileEncoding.Encoding = value.Value;
                            this.IsAudioProfileChanged = true;
                        }


                    }

                }



            }

        }

        public string Name
        {
            get {
                return m_Name;
            }

            set
            {
                if (value != this.m_Name)
                {
                    this.m_Name = value;
                    RaisePropertyChanged("Name");
                    RaisePropertyChanged("ProfileName");
                }


            }
        }

        public string ProfileName
        {
            get
            {
                string str;
                string[] arry = this.m_Name.Split('_');
                if (arry.Length > 1)
                {
                    str = String.Format("{0}_{1}", arry[arry.Length - 1], m_ProfileIdx);
                }
                else str = String.Format("Profile_{0}", m_ProfileIdx);
                return str;
            }

          
        }

        public string StreamUrlOrigin // Origin StreamUrl without Password/User
        {
            get
            {
                return m_StreamUrlOrigin;
            }

            set
            {
                if (value != this.m_StreamUrlOrigin)
                {
                    this.m_StreamUrlOrigin = value;
                }

                RaisePropertyChanged("StreamUrlOrigin");
            }
        }

        public string StreamUrl
        {
            get {
                return m_StreamUrl;
            }

            set
            {
                if (value != this.m_StreamUrl)
                {
                    this.m_StreamUrl = value;
                }

                RaisePropertyChanged("StreamUrl");
            }
        }

        public OnVifVideoProfileEncoding VideoProfileEncoding
        {
            get { return this.m_VideoProfileEncoding; }
            set
            {
                if (value != this.m_VideoProfileEncoding)
                {
                    this.m_VideoProfileEncoding = value;
                }
  
              RaisePropertyChanged("VideoProfileEncoding");
            }
        }
        public OnVifVideoProfileEncodingConfiguration VideoProfileEncodingConfiguration
        {
            get { return this.m_VideoProfileEncodingConfiguration; }
            set
            {
                if (value != this.m_VideoProfileEncodingConfiguration)
                {
                    this.m_VideoProfileEncodingConfiguration = value;
                }
 
              RaisePropertyChanged("VideoProfileEncodingConfiguration");

            }
        }

        public OnVifAudioProfileEncoding AudioProfileEncoding
        {
            get { return this.m_AudioProfileEncoding; }
            set
            {
                if (value != this.m_AudioProfileEncoding)
                {
                    this.m_AudioProfileEncoding = value;
                }

                RaisePropertyChanged("AudioProfileEncoding");
            }
        }
        public OnVifAudioProfileEncodingConfiguration AudioProfileEncodingConfiguration
        {
            get { return this.m_AudioProfileEncodingConfiguration; }
            set
            {
                if (value != this.m_AudioProfileEncodingConfiguration)
                {
                    this.m_AudioProfileEncodingConfiguration = value;
                }

                RaisePropertyChanged("AudioProfileEncodingConfiguration");

            }
        }

        public int ProfileIdx
        {
            get
            {
                return m_ProfileIdx;
            }

            set
            {
                if (value != this.m_ProfileIdx)
                {
                    this.m_ProfileIdx = value;
                }

                RaisePropertyChanged("ProfileIdx");
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
 

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class OnVifCamera :  INotifyPropertyChanged
    {

        public Object CameraObjKey
        {
            get { return this; }

        }

        public String CameraImage
        {
            get { return m_CameraImage; }
            set
            {
                if (m_CameraImage != value)
                {
                    m_CameraImage = value;
                    RaisePropertyChanged("CameraImage");
                    RaisePropertyChanged("CameraImageSource");
                }

            }
        }

        public Windows.UI.Xaml.Media.ImageSource CameraImageSource
        {
            get
            {
                //  URI: ms - appx://Assets/Images/back.png
                BitmapImage bitmapImage = new BitmapImage(new Uri("ms-appx:///" + this.CameraImage));
                return bitmapImage;
            }
        }

        public string CameraIPAdress {
            get { return m_CameraIPAdress; }
            set
            {
                if (m_CameraIPAdress != value)
                {
                    m_CameraIPAdress = value;
                    RaisePropertyChanged("CameraIPAdress");
                }

            }
        }

        public string CameraName {
            get { return m_CameraName; }
            set
            {
                if (m_CameraName != value)
                {
                    m_CameraName = value;
                    RaisePropertyChanged("CameraName");
                }

            }
        }
        public string Password {
            get { return m_Password; }
            set {
                if (m_Password != value)
                {
                    m_Password = value;
                    RaisePropertyChanged("Password");
                }

            }
        }
        public string User {
            get { return m_User; }
            set
            {
                if (m_User != value)
                {
                    m_User = value;
                    RaisePropertyChanged("User");
                }

            }
        }



        private IList<OnVifCameraProfile> m_ProfileEncodings;
        private IDictionary<string, string> m_CameraData;
        

 
        private OnVifPTZControllerRunTime m_OnVifPTZControllerRunTime;


        private string m_CameraIPAdress;
        private string m_CameraName;
        private string m_Password;
        private string m_User;

    //    private int m_SelectedProfileIndex;
        private Boolean m_IsConnected;
        private Boolean m_IsUrlsReaded;
        private Boolean m_IsProfilesReaded;
        private Boolean m_IsProfilesChanged;

        LocalStorageItem m_localStorage;

        private IOnVifMedia m_OnVifMedia;
        private OnVifDevice m_onVifDevice;
        private String m_CameraImage;
        public event PropertyChangedEventHandler PropertyChanged = null;


        public OnVifCamera(string CameraIPAdress, string User, string Password)
        {
            this.m_CameraName = "";
            this.m_CameraIPAdress = CameraIPAdress;
            this.m_User = User;
            this.m_Password = Password;
   
            this.m_CameraImage = "images/camera_icon.png";

            m_CameraData = new Dictionary<string, string>(); // leere liste;

            m_localStorage = new LocalStorageItem("OnVifCamera");
  
            m_onVifDevice = new OnVifDevice(CameraIPAdress, User, Password);

            this.m_OnVifMedia = new OnVifMedia(m_onVifDevice);


            m_ProfileEncodings = this.m_OnVifMedia.ProfileEncodings;

            this.m_OnVifPTZControllerRunTime = new OnVifPTZControllerRunTime(this.m_onVifDevice, this.m_OnVifMedia);

            m_IsConnected = false;
            m_IsUrlsReaded = false;
            m_IsProfilesReaded = false;
            m_IsProfilesChanged = false;


        }
        public void setConnectionData(string Hostname, string User, string Password)
        {

            this.CameraIPAdress = Hostname;
            this.User = User;
            this.Password = Password;

            this.m_onVifDevice.setConnectionData(m_CameraIPAdress, m_User, m_Password); //  // seet PW and User to Device
        }

        internal bool writeSettingsToLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, int CameraIdx)
        {
            if (m_localStorage == null) return false;
        //    m_localStorage.DataSourceIDName = "OnVifCamera";
            m_localStorage.SetSourceIDName("OnVifCamera", CameraIdx);

            int Idx = -1;

            bool bok = m_localStorage.writeSettingsToLocalStorage(composite, Idx);
     
        
            bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraIPAdress", Idx), m_CameraIPAdress);
            bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraName", Idx), m_CameraName);

            bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Password", Idx), m_Password);
            bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_User", Idx), m_User);

            if (this.m_CameraData.ContainsKey("Name")){
                bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.Name", Idx), this.m_CameraData["Name"]);
            }
            if (this.m_CameraData.ContainsKey("FirmwareVersion")){
                bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.FirmwareVersion", Idx), this.m_CameraData["FirmwareVersion"]);
            }
            if (this.m_CameraData.ContainsKey("HardwareId")){
                bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.HardwareId", Idx), this.m_CameraData["HardwareId"]);
            }
            if (this.m_CameraData.ContainsKey("Manufacturer")){
                bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.Manufacturer", Idx), this.m_CameraData["Manufacturer"]);
            }
            if (this.m_CameraData.ContainsKey("Model")){
                bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.Model", Idx), this.m_CameraData["Model"]);
            }
            if (this.m_CameraData.ContainsKey("SerialNumber")){
                bok = m_localStorage.writeStringSettingsToLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.SerialNumber", Idx), this.m_CameraData["SerialNumber"]);
            }




            int ProfileIdx = 0;
            foreach (var profile in this.m_ProfileEncodings)
            {
                m_localStorage.ResetCompositeName(); 
                m_localStorage.DataCompositeIDName = m_localStorage.getCompositePropertyIDName("OnVifCameraProfile", -1);
                profile.LocalStorageItem = m_localStorage;
                profile.writeSettingsToLocalStorage(composite, ProfileIdx);
                ProfileIdx++;
            }
     

            return bok;
        }


        // read settings from local storage
        internal bool readSettingsfromLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, int CameraIdx)
        {
            if (m_localStorage == null) return false;

            m_localStorage.SetSourceIDName("OnVifCamera", CameraIdx);
            int Idx = -1;
            bool bStoreOk = m_localStorage.readSettingsfromLocalStorage(composite, Idx);
            
            if (bStoreOk)
            {
              
                bool bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraIPAdress", Idx), out m_CameraIPAdress);
         
                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraName", Idx), out m_CameraName);
           
                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_Password", Idx), out m_Password);
        
                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_User", Idx), out m_User);

                this.CameraData.Clear();
                string Value;

                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.Name", Idx), out Value);
                if (bok) this.m_CameraData.Add("Name", Value);

                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.FirmwareVersion", Idx), out Value);
                if (bok) this.m_CameraData.Add("FirmwareVersion", Value);

                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.HardwareId", Idx), out Value);
                if (bok) this.m_CameraData.Add("HardwareId", Value);
                
                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.Manufacturer", Idx), out Value);
                if (bok) this.m_CameraData.Add("Manufacturer", Value);

                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.Model", Idx), out Value);
                if (bok) this.m_CameraData.Add("Model", Value);


                bok = m_localStorage.readStringSettingsfromLocalStorage(composite, m_localStorage.getCompositePropertyIDName("m_CameraData.SerialNumber", Idx), out Value);
                if (bok) this.m_CameraData.Add("SerialNumber", Value);

                this.CameraData = this.m_CameraData; // for property eventing

                this.setConnectionData(m_CameraIPAdress, m_User, m_Password); //  // seet PW and User to Device



                int ProfileIdx = 0;
                while (true) // vorhandene Profiles aus den Settings lesen
                {
                    OnVifCameraProfile profile = new OnVifCameraProfile(ProfileIdx);
                    m_localStorage.ResetCompositeName();
                    m_localStorage.DataCompositeIDName = m_localStorage.getCompositePropertyIDName("OnVifCameraProfile", -1) ;
                    profile.LocalStorageItem = m_localStorage;

                    bool bfound = profile.readSettingsfromLocalStorage(composite, ProfileIdx);
                    if (!bfound) break;
                    profile.AudioProfileEncoding.buildConfigurations();
                    profile.AudioProfileEncodingConfiguration.buildConfigurations();

                    profile.VideoProfileEncoding.buildConfigurations();
                    profile.VideoProfileEncodingConfiguration.buildConfigurations(profile.VideoProfileEncoding.Encoding);

                    profile.setSelectedIndices();
                    this.m_ProfileEncodings.Add(profile);
                    ProfileIdx++;
                }
          

            }

            return bStoreOk;
        }
        
        public IList<OnVifCameraProfile> ProfileEncodings {
            get { return m_ProfileEncodings; }
            set
            {
                if (value != this.m_ProfileEncodings)
                {
                    this.m_ProfileEncodings = value;

                }
                RaisePropertyChanged("ProfileEncodings");

            }

        }

        public IDictionary<string, string> CameraData {
            get { return m_CameraData; }

            set
            {
                if (value!= m_CameraData)
                {
                    m_CameraData = value;
                }
                RaisePropertyChanged("CameraData");

            }
        }
        /*
        public int SelectedProfileIndex
        {
            get { return this.m_SelectedProfileIndex; }
            set
            {
                this.m_SelectedProfileIndex = value;
                RaisePropertyChanged("SelectedProfileIndex");
            }
        }




        public OnVifCameraProfile getSelectedProfile()
        {

            if (this.SelectedProfileIndex == -1)
            {
                return null; ;
            }
            return m_ProfileEncodings[SelectedProfileIndex];

        }
           */

        public Boolean IsProfilesChanged
        {
            get { return m_IsProfilesChanged; }
            set
            {
                if (value != this.IsProfilesChanged)
                {
                    this.m_IsProfilesChanged = value;
                    RaisePropertyChanged("IsProfilesChanged");
                }


            }
        }

        public Boolean IsProfilesReaded
        {
            get { return m_IsProfilesReaded; }
            set
            {
                if (value != this.m_IsProfilesReaded)
                {
                    this.m_IsProfilesReaded = value;
                    RaisePropertyChanged("IsProfilesReaded");
                }


            }
        }


        public Boolean IsUrlsReaded  {


            get { return m_IsUrlsReaded; }
            set
            {
                if (value != this.IsUrlsReaded)
                {
                    this.m_IsUrlsReaded = value;
                    RaisePropertyChanged("IsUrlsReaded");
                }

       
            } 
        }
        public Boolean IsConnected
        {
            get { return m_IsConnected; }
            set
            {
                if (value != this.IsConnected)
                {
                    this.m_IsConnected = value;
                    RaisePropertyChanged("IsConnected");
                }


            }
        }

    


       private void RaisePropertyChanged(string name)
       {
           if (PropertyChanged != null)
           {
               PropertyChanged(this, new PropertyChangedEventArgs(name));
           }
       }


     
        public OnVifPTZControllerRunTime OnVifPTZControllerRunTime
        {
            get
            {
                return m_OnVifPTZControllerRunTime;
            }
        }

        public string CameraIdent
        {
            get
            {
                string ret = String.Format("{0}.<{1}> ", CameraName, CameraIPAdress);
                return ret;
            }
        }

  
        public Windows.Foundation.IAsyncOperation<Boolean> setActDateandTimeAsync()
        {
            Task<Boolean> from = setActDateandTimeInternalAsync();

            Windows.Foundation.IAsyncOperation<Boolean> to = from.AsAsyncOperation();

            return to;

        }

        public Windows.Foundation.IAsyncOperation<Boolean> connectAsync()
        {
            Task<Boolean> from =  connectInternalAsync();

            Windows.Foundation.IAsyncOperation<Boolean> to = from.AsAsyncOperation();

            return to;

        }

        public Windows.Foundation.IAsyncOperation<Boolean> connectPTZAsync()
        {
            Task<Boolean> from = this.connectPTZInternalAsync();

            Windows.Foundation.IAsyncOperation<Boolean> to = from.AsAsyncOperation();
            
            return to;

        }


        public Windows.Foundation.IAsyncOperation<IList<OnVifCameraProfile>> getEncodingsAsync()
        {
     
            Task<IList<OnVifCameraProfile>> from = getEncodingsInternalAsync();
            Windows.Foundation.IAsyncOperation<IList<OnVifCameraProfile>> result = from.AsAsyncOperation();

            return result;
        }

        public Windows.Foundation.IAsyncOperation<Boolean> writeEncodingsAsync()
        {

            Task<Boolean> from = writeEncodingsInternalAsync();
            Windows.Foundation.IAsyncOperation<Boolean> result = from.AsAsyncOperation();

            return result;
        }




        public Windows.Foundation.IAsyncOperation<IList<OnVifCameraProfile>> getMediaUrisAsync()
        {

            Task<IList<OnVifCameraProfile>> from = getInternalMediaUrisAsync();
            Windows.Foundation.IAsyncOperation<IList<OnVifCameraProfile>> result = from.AsAsyncOperation();

            return result;
        }

     

        public Windows.Foundation.IAsyncOperation<OnVifCameraProfile> getVideoEncodingAsync(int nProfileIdx)
        {
            Task<OnVifCameraProfile> from = this.m_OnVifMedia.getVideoEncodingInternalAsync(nProfileIdx);
            Windows.Foundation.IAsyncOperation<OnVifCameraProfile> result = from.AsAsyncOperation();

            return result;
        }

        public Windows.Foundation.IAsyncOperation<Boolean> writeVideoEncodingAsync(int nProfileIdx)
        {
            Task<Boolean> from = this.m_OnVifMedia.writeVideoEncodingInternalAsync(nProfileIdx);
            Windows.Foundation.IAsyncOperation<Boolean> result = from.AsAsyncOperation();

            return result;
        }

        public Windows.Foundation.IAsyncOperation<OnVifCameraProfile> getVideoEncodingConfigurationOptionsAsync(int nProfileIdx)
        {
            Task<OnVifCameraProfile> from = this.m_OnVifMedia.getVideoEncodingConfigurationOptionsInternalAsync(nProfileIdx);
            Windows.Foundation.IAsyncOperation<OnVifCameraProfile> result = from.AsAsyncOperation();

            return result;
        }




        public void ButtonconnectAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            
            Button send = sender as Button;
            if (send != null) send.IsEnabled = false;

            Windows.Foundation.IAsyncOperation<Boolean> from = connectAsync();

            if (send != null) send.IsEnabled = true;
   
        }


        // Buttons Read Video-Encodings
        public void ButtonconnectPTZAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Button send = sender as Button;
            if (send != null) send.IsEnabled = false;

            Windows.Foundation.IAsyncOperation<Boolean> from = connectPTZAsync();

            if (send != null) send.IsEnabled = true;
     
        }


        // Buttons Read Video-Encodings
        public void ButtonReadProfilesAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Button send = sender as Button;
            if (send != null) send.IsEnabled = false;
            Windows.Foundation.IAsyncOperation<IList<OnVifCameraProfile>> from = getEncodingsAsync();
            if (send != null) send.IsEnabled = true;
   //         return from;
        }
        // Buttons Read Video-Encodings

        public void ButtonWriteProfilesAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Button send = sender as Button;
            if (send != null) send.IsEnabled = false;
            Windows.Foundation.IAsyncOperation<Boolean> from = writeEncodingsAsync();
            if (send != null) send.IsEnabled = true;
      //      return from;
        }



        // Button Read Media Uris
        public void ButtonMediaUrisAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Button send = sender as Button;
            if (send != null) send.IsEnabled = false;
            Windows.Foundation.IAsyncOperation<IList<OnVifCameraProfile>> from = getMediaUrisAsync();
            if (send != null) send.IsEnabled = true;
  //          return from;
        }

        public void ButtonSetSyncTimeWithPCAsync(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            Button send = sender as Button;
            if (send != null) send.IsEnabled = false;
            Windows.Foundation.IAsyncOperation<Boolean> from = setActDateandTimeAsync();
            if (send != null) send.IsEnabled = true;
            //          return from;
        }








        public OnVifCameraProfile getOnVifProfile(int nProfileIdx)
        {
            return this.m_OnVifMedia.getOnVifProfile(nProfileIdx);

        }


        public IDictionary<string, string> GetCameraData()
        {

            this.m_onVifDevice.GetCameraData(m_CameraData);
            CameraData = m_CameraData; // Update Notification

            return m_CameraData;
        }

        public int getNummberOfProfileTokens()
        {
            if (this.m_OnVifMedia == null) return 0;

            return this.m_OnVifMedia.getNummberOfProfileTokens();

        }


        public string getProfileToken(int nProfileIdx)
        {
            if (this.m_OnVifMedia == null) return "";

            return this.m_OnVifMedia.getProfileToken(nProfileIdx);


        }

        public string getProfileName(int nProfileIdx)
        {
            if (this.m_OnVifMedia == null) return "";

            return this.m_OnVifMedia.getProfileName(nProfileIdx);


        }


        // Reading Profiles from Camera
        internal async Task<IList<OnVifCameraProfile>> getEncodingsInternalAsync()
        {
            try
            {
                if (!this.IsConnected)
                {
                    await connectInternalAsync();
                }

                this.IsProfilesReaded = false;
                this.IsUrlsReaded = false;

                if (this.IsConnected)
                {
                    for (int i = 0; i < getNummberOfProfileTokens(); i++)
                    {
                        OnVifCameraProfile profile = this.getOnVifProfile(i);
                        await this.m_OnVifMedia.getVideoEncodingInternalAsync(i);
                        await this.m_OnVifMedia.getVideoEncodingConfigurationOptionsInternalAsync(i);
                        await this.m_OnVifMedia.getAudioEncodingInternalAsync(i);
                        await this.m_OnVifMedia.getAudioEncodingConfigurationOptionsInternalAsync(i);
                        IDictionary<string, IList<string>> StreamUris = await ReadInternalMediaUrisAsync(i);
                        profile.Name = getProfileName(i);
                        profile.VideoProfileEncoding.buildConfigurations();
                        profile.VideoProfileEncodingConfiguration.buildConfigurations(profile.VideoProfileEncoding.Encoding);
                        profile.AudioProfileEncodingConfiguration.buildConfigurations();
                        profile.AudioProfileEncoding.buildConfigurations();
                        profile.setStreamurl(StreamUris, this.User, this.Password);

                        profile.buildVideoBitRates();
                        profile.setSelectedIndices();
                        profile.PropertyChanged += OnVifCameraProfile_PropertyChanged;

                    }
                    this.IsUrlsReaded = (this.ProfileEncodings.Count > 0);
                    this.IsProfilesReaded = (this.ProfileEncodings.Count > 0);// Property change notification
                    ProfileEncodings = this.m_ProfileEncodings;
                }
 
            }
            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();

            }
            return this.m_ProfileEncodings;
        }

        private void OnVifCameraProfile_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            OnVifCameraProfile profile = sender as OnVifCameraProfile;
            if (profile != null)
            {
                if (e.PropertyName== "IsAudioProfileChanged")
                {
                    if (profile.IsAudioProfileChanged)
                    {
                        profile.setSelectedIndices();
                        this.IsProfilesChanged = true;
                    }

                }
                else
                if (e.PropertyName== "IsVideoProfileChanged")
                {
                    if (profile.IsVideoProfileChanged)
                    {
                        profile.setSelectedIndices();
                        this.IsProfilesChanged = true;
                    }

                }



            }

        }

        internal async Task<Boolean> writeEncodingsInternalAsync()
        {
            bool retValue = false;
            try
            {
                for (int i = 0; i < getNummberOfProfileTokens(); i++)
                {
                    OnVifCameraProfile profile = this.getOnVifProfile(i);
                    if (profile.IsVideoProfileChanged)
                    {

                        retValue = await this.m_OnVifMedia.writeVideoEncodingInternalAsync(i) || retValue;
                        profile.IsVideoProfileChanged = !retValue;
                    }
                    if (profile.IsAudioProfileChanged)
                    {
                        retValue = await this.m_OnVifMedia.writeAudioEncodingInternalAsync(i) || retValue;
                        profile.IsAudioProfileChanged = !retValue;
                    }

                }
                this.IsProfilesChanged = false;
            }
            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();

            }

            return retValue;
        }

        internal async Task<IList<OnVifCameraProfile>> getInternalMediaUrisAsync()
        {
            this.IsUrlsReaded = false;
            try
            {
                for (int i = 0; i < getNummberOfProfileTokens(); i++)
                {
                    OnVifCameraProfile profile = this.getOnVifProfile(i);
              
                    if (profile == null) continue;

                    IDictionary<string, IList<string>> StreamUris = await ReadInternalMediaUrisAsync(i);

                    profile.setStreamurl(StreamUris, this.User,this.Password);

                 }
                this.IsUrlsReaded = true;
            }
            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();

            }

            return this.m_ProfileEncodings;
        }


        internal async Task<Boolean> connectPTZInternalAsync()
        {
            Boolean ret = false;
            try
            {
                if (!this.IsConnected)
                {
                    await connectInternalAsync();
                }
                if (this.IsConnected)
                {
                    ret = await this.OnVifPTZControllerRunTime.InitalizeAsync();
                }

       
            }
            catch (OnVifException ex)
            {
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();

            }
            return ret;
        }

        internal async Task<Boolean> connectInternalAsync()
        {
            Boolean ret = false;
            try
            {
                this.IsConnected = false;
                this.IsUrlsReaded = false;
                this.IsProfilesReaded = false;
                this.IsProfilesChanged = false;
                this.OnVifPTZControllerRunTime.IsInitialized = false;
 
                ret = await this.m_onVifDevice.InitalizeDeviceAsync();

                if (ret)
                {
                    ret = await this.m_OnVifMedia.InitalizeAsync();

                    GetCameraData(); // Get Camera-Online-Data
                }
                this.IsConnected = ret;
            }
            catch (OnVifException ex )
            {

                //      ex.ExceptionHandled
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();



            }

            return ret;
        }

        internal async Task<Boolean> setActDateandTimeInternalAsync()
        {
            Boolean ret = false;
            try
            {

                if (!this.IsConnected)
                {
                    await connectInternalAsync();
                }
                if (this.IsConnected)
                {
                    ret = await this.m_onVifDevice.setActDateandTimeAsync();
                }
            }
            catch (OnVifException ex)
            {
                //      ex.ExceptionHandled
                OnVifMsgbox msg = new OnVifMsgbox(ex);
                msg.Show();
            }

            return ret;
        }

        internal async Task<IDictionary<string, IList<string>>> ReadInternalMediaUrisAsync(int nProfileIdx)
        {
            IDictionary<string, IList<string>> StreamUris = new Dictionary<string, IList<string>>();

            StreamUris = await this.m_OnVifMedia.ReadMediaUrisAsync(nProfileIdx);

            return StreamUris;
        }
  


    }
    
    

}

