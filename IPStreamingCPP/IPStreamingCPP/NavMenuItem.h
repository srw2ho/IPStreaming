#pragma once
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;

namespace IPStreamingCPP
{
    /// <summary>
    /// Data to represent an item in the nav menu.
    /// </summary>
    public ref class NavMenuItem sealed : public INotifyPropertyChanged
    {
		Platform::String^ m_Label;
		Symbol m_Symbol;
		TypeName m_DestPage;
		Platform::Object^ m_Arguments;
    public:
        NavMenuItem(Platform::String^, Windows::UI::Xaml::Controls::Symbol, TypeName);
        NavMenuItem(Platform::String^, Windows::UI::Xaml::Controls::Symbol, TypeName, Platform::Object^);



		property Platform::String^ Label {
			Platform::String^ get() {return m_Label;};
			void set(Platform::String^ value) { m_Label = value; };
		};

		property Windows::UI::Xaml::Controls::Symbol Symbol {
			Windows::UI::Xaml::Controls::Symbol get() { return m_Symbol; };
			void set(Windows::UI::Xaml::Controls::Symbol value) { m_Symbol = value; };
		};

		property Windows::UI::Xaml::Interop::TypeName DestPage {
			Windows::UI::Xaml::Interop::TypeName get() { return m_DestPage; };
			void set(Windows::UI::Xaml::Interop::TypeName value) { m_DestPage = value; };
		};

		property Platform::Object^ Arguments {
			Platform::Object^ get() { return m_Arguments; };
			void set(Platform::Object^ value) { m_Arguments = value; };
		};
	
        property Platform::String^ SymbolAsChar
        {
			Platform::String^ get();
        }

        property bool IsSelected
        {
            bool get();
            void set(bool value);
        }

        property Visibility SelectedVis
        {
            Visibility get();
            void set(Visibility value);
        }

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

    protected:
        void NotifyPropertyChanged(Platform::String^ prop);

    private:
        bool m_isSelected;
        Visibility m_selectedVisibility;
    };
}
