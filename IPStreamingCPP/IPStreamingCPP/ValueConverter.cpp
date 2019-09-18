
#include "pch.h"
#include "ValueConverter.h"
using namespace Windows::Foundation;
namespace IPStreamingCPP
{

	Platform::Object^ ValueConverterToBool::Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
		Platform::Object^ parameter, Platform::String^ language)
	{
		bool boolvalue = safe_cast<IPropertyValue^>(value)->GetBoolean();

		Platform::String^param  = safe_cast<IPropertyValue^>(parameter)->GetString();
		if ((param != nullptr) && (param == "Negation"))
		{
			return !boolvalue;
		}
		else return boolvalue;


	}

	// No need to implement converting back on a one-way binding 
	Platform::Object^ ValueConverterToBool::ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
		Platform::Object^ parameter, Platform::String^ language)
	{
		
		bool boolvalue = safe_cast<IPropertyValue^>(value)->GetBoolean();

		Platform::String^param = safe_cast<IPropertyValue^>(parameter)->GetString();
		if ((param != nullptr) && (param == "Negation"))
		{
			return !boolvalue;
		}
		else return boolvalue;


	}
	
}