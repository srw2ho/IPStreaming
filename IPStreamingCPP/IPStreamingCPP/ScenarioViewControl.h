#pragma once

#ifndef SCENARIOVIEWCONTROL
#define SCENARIOVIEWCONTROL


using namespace Platform;
using namespace IPStreamingCPP;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
namespace IPStreamingCPP
{

	typedef  void  ScenarioNotificationFunction(Platform::Object^ obj1, Platform::Object^ obj2);


	//delegate void ScnenrarioNotificationhandler(IPStreamingCPP::DataSources^, Platform::Object^ );


	interface struct ScenarioEventInterface {
	public:
	//	event ScnenrarioNotificationhandler ^ eventhandler;
		event  Windows::Foundation::TypedEventHandler<Platform::Object^, Platform::Object^ >^ eventhandler;
		void fire(Platform::Object^ obj, Platform::Object^obj1);
	};

	// class that implements the interface event and function  
	
	ref class ScenarioEventSource : public ScenarioEventInterface {
	public:

	//	virtual event ScnenrarioNotificationhandler^ eventhandler;
		virtual event  Windows::Foundation::TypedEventHandler<Platform::Object^, Platform::Object^ >^ eventhandler;

		virtual void fire(Platform::Object^ obj, Platform::Object^obj1)
		{
			eventhandler(obj, obj1);

		}
	};


	// interface that has an event and a function to invoke the event  



	//typedef std::function<void(const Event&, int reason)> eventcallback;

	//delegate void NotificationEventhandler(Platform::Object^, Platform::Object^);

	[Windows::UI::Xaml::Data::Bindable]
	public ref  class  ScenarioItem sealed //: Windows::UI::Xaml::DependencyObject
	{
	private :
	//	Windows::Foundation::Collections::IVector<ScenarioEventSource^>^ m_callBackHandler;

	
		//IMap<String^, int>

		Windows::Foundation::Collections::IMap<Platform::String ^, ScenarioEventSource^>^ m_callBackHandler;
		Windows::UI::Xaml::Controls::Symbol m_Symbol;
		Platform::String^ m_ScenarioName;
		Platform::Object^ m_Object;
		//		Platform::Type^ m_ClassType;
		Windows::UI::Xaml::Interop::TypeName m_TypeName;
		Page ^ m_page;
	internal:

		void AddEvent(Platform::String ^ key, ScenarioEventSource^ item)
		{
	//		ScenarioEventSource^ item = ref new ScenarioEventSource();
	//		item->eventhandler += ref new  Windows::Foundation::TypedEventHandler<Platform::Object^, Platform::Object^>(handler);
			m_callBackHandler->Insert(key, item);

			//	m_callBackHandler->Append(item);
		}

		property Windows::Foundation::Collections::IMap<Platform::String ^, ScenarioEventSource^>^ CallBackHandler
		{
			Windows::Foundation::Collections::IMap<Platform::String ^, ScenarioEventSource^>^ get() { return m_callBackHandler; };

		}

	public:
		ScenarioItem(Platform::String^ IDName, Windows::UI::Xaml::Interop::TypeName TypeName, Windows::UI::Xaml::Controls::Symbol, Platform::Object^ object);

		virtual ~ScenarioItem();


		property Platform::String^ SymbolAsChar
		{
			Platform::String^ get();
		}


		property Platform::Object^ Object
		{
			Platform::Object^ get();
			void set(Platform::Object^ value);
		}

		property Platform::String^ ScenarioName
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}


		property Windows::UI::Xaml::Interop::TypeName TypeClassName
		{
			Windows::UI::Xaml::Interop::TypeName get();
			void set(Windows::UI::Xaml::Interop::TypeName value);
		}

	protected:


	
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class  ScenarioViewControl sealed //: Windows::UI::Xaml::DependencyObject
	{
		Platform::Collections::Vector<ScenarioItem^>^ m_Items;
		unsigned int m_SelectedIndex;

	public:
		ScenarioViewControl();
		//	~ScenarioViewControl();
		void ScenarioViewControl::SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

		property Windows::Foundation::Collections::IVector<ScenarioItem^>^ Items {
			Windows::Foundation::Collections::IVector<ScenarioItem^>^ Items::get() { return this->m_Items; };
		}

		ScenarioItem^ getSelectedItem();
		property int SelectedIndex
		{
			int get();
			void set(int value);
		}
	};

}
#endif // SCENARIOVIEWCONTROL