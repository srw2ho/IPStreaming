//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"


using namespace OnVifServicesRunTime;

using namespace OnVifUniversalCpp;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace Concurrency;



// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}


void OnVifUniversalCpp::MainPage::button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{







		try {

			String^ user = ref new String(L"admin");

			String^ pw = ref  new  String(L"willi");
			String^ host = ref new  String(L"192.168.1.230");
			OnVifCamera ^ camera = ref new OnVifCamera(host, user, pw);


			auto init = camera->connectAsync();
			auto initTask = create_task(init);
			initTask.then([this, camera](Boolean init)
			{
				if (init)
				{
				//	camera->
					auto Task = camera->getMediaUrisAsync();
					auto urlTask = create_task(Task);
					urlTask.then([this, camera](IVector<OnVifServicesRunTime::OnVifCameraProfile^> ^ profiles){}
					
					);

	

				}

			}); // end lambda


	//	IVector<String ^> ^ str = mediaServicesRunTime->getURIList();

	//	Windows::Foundation::Collections::IVector<String ^> ^ str  = dynamic_cast (Windows::Foundation::Collections::IVector<String ^>) (refptr->ReadlUrls()) ;
	}
	catch (...)
	{
		bool error = true;
	}

	
}
