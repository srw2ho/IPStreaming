using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;

namespace OnVifServicesRunTime
{
    class LocalStorageSettings : ILocalStorageSettings
    {
        private string m_DataSourceIDName;
        private string m_DataCompositeIDName;
        private Windows.Storage.ApplicationDataCompositeValue m_DataSourcecomposite;


        public LocalStorageSettings(string IDName)
        {
            m_DataSourceIDName = IDName;
            m_DataCompositeIDName = "";
            m_DataSourcecomposite = null;

        }

        public string SetContainerIDName(string IDName)
        {
            string composite = String.Format("{0}.{1}", IDName, m_DataSourceIDName);
            m_DataCompositeIDName = composite;
            return m_DataCompositeIDName;
        }


        public string SetSourceIDName(string IDName)
        {

            string composite = String.Format("{0}.{1}", m_DataSourceIDName, IDName);
            m_DataCompositeIDName = composite;
            return m_DataCompositeIDName;

        }

        public string getCompositePropertyIDName(string Property)
        {
            string composite = String.Format("{0}.{1}", m_DataCompositeIDName, Property);
            return composite;
        }

        public string SourceIDName
        {
            get
            {
                return m_DataSourceIDName;
            }
        }
        public string CompositeIDName
        {
            get
            {
                return m_DataCompositeIDName;
            }
        }




       public bool writeCompositeValuetoLocalStorage()
        {
            if (m_DataSourcecomposite == null) return false;

            ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
            bool keyok = localSettings.Values.ContainsKey(m_DataCompositeIDName);
            if (keyok)
            {
                localSettings.Values[m_DataCompositeIDName] = m_DataSourcecomposite;
            }
            else
            {
                localSettings.Values.Add(m_DataCompositeIDName, m_DataSourcecomposite);
            }

            //localSettings[m_DataCompositeIDName] = m_DataSourcecomposite;
            return true;


        }
        public Windows.Storage.ApplicationDataCompositeValue getCompositeValue()
        {

            ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;

            bool keyok = localSettings.Values.ContainsKey(m_DataCompositeIDName);


            if (keyok)
            {

                m_DataSourcecomposite = (ApplicationDataCompositeValue)localSettings.Values[m_DataCompositeIDName];
            }
            else
            {

                m_DataSourcecomposite = new Windows.Storage.ApplicationDataCompositeValue();
            }

            return m_DataSourcecomposite;

        }

        public Windows.Storage.ApplicationDataCompositeValue deleteCompositeValue()
        {
            ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;

            if (localSettings.Values.ContainsKey(m_DataCompositeIDName))
            {

                localSettings.Values.Remove(m_DataCompositeIDName);
                m_DataSourcecomposite = null;
            }

            return m_DataSourcecomposite;
        }
    }

    class LocalStorageItem : ILocalStorageItem
    {

        private string m_DataSourceIDName;
        private string m_DataCompositeIDName;

        public string DataSourceIDName
        {
            set
            {
                 m_DataSourceIDName = value;
            }
            get
            {
                return m_DataSourceIDName;
            }
        }
        public string DataCompositeIDName
        {
            set
            {
                m_DataCompositeIDName = value;
            }
            get
            {
                return m_DataCompositeIDName;
            }
        }

        public LocalStorageItem(string IDName)
        {
            m_DataSourceIDName = IDName;
            m_DataCompositeIDName = m_DataSourceIDName;



        }
        public string SetSourceIDName(string IDName, int Idx)
        {

            string composite = String.Format("{0}.{1:d3}", IDName, Idx);
            m_DataSourceIDName = composite;
            m_DataCompositeIDName = m_DataSourceIDName;
            return m_DataSourceIDName;

        }
        public void ResetCompositeName()
        {
            m_DataCompositeIDName = m_DataSourceIDName; // CompisiteIdName set to origin Value

        }

        public string getCompositePropertyIDName(string Property, int IDx)
        {

            string composite;

	        if (IDx == -1) {
                composite = String.Format("{0}.{1}", m_DataCompositeIDName, Property);


            }
	        else {
                composite = String.Format("{0}.{1:d3}.{2}", m_DataCompositeIDName, IDx, Property);
            }



            return composite;

        }




        public bool writeDoubleSettingsToLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, string key, double value)
        {
        
            if (!composite.ContainsKey(key)) composite.Add(key, value);
            else composite[key] = value;

            return true;
        }
        public bool writeIntegerSettingsToLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, string key, int value)
        {

            if (!composite.ContainsKey(key)) composite.Add(key, value);
            else composite[key] = value;

            return true;
        }
        public bool writeBoolSettingsToLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, string key, bool value)
        {

            if (!composite.ContainsKey(key)) composite.Add(key, value);
            else composite[key] = value;

            return true;
        }
        public bool writeFloatSettingsToLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, string key, float value)
        {

            if (!composite.ContainsKey(key)) composite.Add(key, value);
            else composite[key] = value;

            return true;
        }

        public bool writeStringSettingsToLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, string key, string value)
        {

            if (!composite.ContainsKey(key)) composite.Add(key, value);
            else composite[key] = value;

            return true;
        }


        public bool writeSettingsToLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, int Idx)
        {
            String  key = getCompositePropertyIDName("m_DataCompositeIDName", Idx);

            if (!composite.ContainsKey(key)) composite.Add(key, m_DataCompositeIDName);
            else composite[key] = m_DataCompositeIDName;

            return true;
        }

        public bool readDoubleSettingsfromLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, string Key, out double value)
        {
            value = 0;

            Object outvalue;

            bool bKey = composite.TryGetValue(Key, out outvalue);

            if (!bKey) return false;

            value = (double)outvalue;


            return true;
        }
        public bool readBoolSettingsfromLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, string Key, out bool value)
        {
            value = false;

            Object outvalue;

            bool bKey = composite.TryGetValue(Key, out outvalue);

            if (!bKey) return false;

            value = (bool)outvalue;


            return true;
        }
        public bool readFloatSettingsfromLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, string Key, out float value)
        {
            value = 0;

            Object outvalue;

            bool bKey = composite.TryGetValue(Key, out outvalue);

            if (!bKey) return false;

            value = (float)outvalue;


            return true;
        }


        public bool readIntegerSettingsfromLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, string Key, out int value)
        {
            value = 0;

            Object outvalue;

            bool bKey = composite.TryGetValue(Key, out outvalue);

            if (!bKey) return false;

            value = (int)outvalue;


            return true;
        }
        public bool readStringSettingsfromLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, string Key, out string value)
        {
            value = "";

            Object outvalue;

            bool bKey = composite.TryGetValue(Key, out outvalue);

            if (!bKey) return false;

            value = (string)outvalue;


            return true;
        }


        public bool readSettingsfromLocalStorage(Windows.Storage.ApplicationDataCompositeValue composite, int Idx)
        {

            String  key = getCompositePropertyIDName("m_DataCompositeIDName", Idx);

            Object outvalue;

            bool bKey = composite.TryGetValue(key, out  outvalue);
                   
            if (!bKey) return false;
            string retValue = (string)outvalue; 
            if (String.Compare(retValue, m_DataCompositeIDName) ==0)
            {
                return true;
            }

            m_DataCompositeIDName = (string) outvalue;


            return false;
        }

    }
}
