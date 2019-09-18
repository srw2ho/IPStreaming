#pragma once 
namespace IPStreamingCPP
{
	public ref class ValueConverterToBool  sealed : Windows::UI::Xaml::Data::IValueConverter
	{
		// This converts the DateTime object to the Platform::String^ to display.
	public:
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language);


		// No need to implement converting back on a one-way binding 
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language);

	};
}