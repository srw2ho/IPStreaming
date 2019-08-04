using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OnVifServicesRunTime
{
    public sealed class IsEnableConverter : Windows.UI.Xaml.Data.IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {

            var boolvalue = (bool)value;
            string param = parameter as string;
            if ((param != null) && (param == "Negation"))
            {
                return !boolvalue;
            }
            else return boolvalue;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            var boolvalue = (bool)value;
            string param = parameter as string;
            if ((param != null) && (param == "Negation"))
            {
                return !boolvalue;
            }
            else return boolvalue;
        }
    }

    public sealed class DictionaryConverter : Windows.UI.Xaml.Data.IValueConverter
    {
        public object Convert(object value, Type targetType,
          object parameter, string language)
        {
            var dictionary = value as Dictionary<string, string>;
            if (dictionary == null || !(parameter is string))
            {
                return null;
            }
            string result;
            dictionary.TryGetValue((string)parameter, out result);
            return result;
        }

        public object ConvertBack(object value, Type targetType,
          object parameter, string language)
        {
            return null;
        }
    }

}
