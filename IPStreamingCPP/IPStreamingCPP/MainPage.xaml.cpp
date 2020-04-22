//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include "StreamingPageParam.h"
#include "StreamingPage.xaml.h"

using namespace IPStreamingCPP;
using namespace AmcrestMotionDetection;

using namespace FFmpegInteropExtRT;
using namespace OnVifServicesRunTime;

using namespace concurrency;
using namespace Platform;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Core;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;


using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::UI::Xaml::Interop;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

	m_OnVifCameraViewModel = nullptr;


	m_displayRequest = ref new Windows::System::Display::DisplayRequest();
	m_displayRequestCnt = 0;


	m_StreamingPageParamControl = ref new StreamingPageParamControl();


	m_applicationSuspendingEventToken =
		Application::Current->Suspending += ref new SuspendingEventHandler(this, &MainPage::Application_Suspending);
	m_applicationResumingEventToken =
		Application::Current->Resuming += ref new EventHandler<Object^>(this, &MainPage::Application_Resuming);

	m_CodecReader = ref new FFmpegInteropExtRT::CodecReader();
	// srw2ho: crashin case of first call, but only one computer m_CodecReader->ReadInstalledVideoDecoderCodecsAsync();
	// readonly used Video codecs
	m_CodecReader->ReadUsedVideoDecoderCodecsAsync();
	m_CodecReader->ReadUsedAudioDecoderCodecsAsync();




}

MainPage::~MainPage()
{

	Application::Current->Suspending -= m_applicationSuspendingEventToken;
	Application::Current->Resuming -= m_applicationResumingEventToken;


	clearRecording();

	ReleaseAllRequestedDisplay();

	delete m_StreamingPageParamControl;

}




void MainPage::PivotMediaLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	StreamingPageParam^ streamingPageParam = dynamic_cast<StreamingPageParam^>(PivotCameras->SelectedItem);

	//StreamingPageParam ^ streamingPageParam = m_StreamingPageParamControl->getSelectedItem();

	if (streamingPageParam != nullptr) {

		MediaElement^ media = safe_cast<MediaElement^>(sender);
		if (media != nullptr) {
			streamingPageParam->MediaStreamElement = media;
			streamingPageParam->MediaCurrentStateChangedRegister = streamingPageParam->MediaStreamElement->CurrentStateChanged += ref new Windows::UI::Xaml::RoutedEventHandler(this, &MainPage::MediaElement_OnCurrentStateChanged);
			streamingPageParam->MediaFailedRegister = streamingPageParam->MediaStreamElement->MediaFailed += ref new Windows::UI::Xaml::ExceptionRoutedEventHandler(this, &MainPage::MediaElement_OnMediaFailed);
		}

	}
}



void MainPage::DisplayErrorMessage(Platform::String^ message)
{
	// Display error message
	this->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this, message]() {

		auto errorDialog = ref new MessageDialog(message);
		errorDialog->ShowAsync();


		}));



}


void MainPage::ActivateDisplay()
{
	//create the request instance if needed

	//make request to put in active state
	m_displayRequest->RequestActive();
	m_displayRequestCnt++;
}

void MainPage::ReleaseAllRequestedDisplay()
{
	//must be same instance, so quit if it doesn't exist
	if (m_displayRequest == nullptr)
		return;


	while (m_displayRequestCnt > 0) {
		//undo the request
		m_displayRequest->RequestRelease();
		m_displayRequestCnt--;
	}

}

void MainPage::ReleaseDisplay()
{
	//must be same instance, so quit if it doesn't exist
	if (m_displayRequest == nullptr)
		return;

	if (m_displayRequestCnt > 0) {
		//undo the request

		m_displayRequest->RequestRelease();
		m_displayRequestCnt--;
	}

}

void MainPage::WriteToAppData()
{


	for each (auto var in this->m_StreamingPageParamControl->Items)
	{
		var->WriteToAppData();
	}

}

void MainPage::Application_Suspending(Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e)
{
	// Handle global application events only if this page is active
	if (Frame->CurrentSourcePageType.Name == Interop::TypeName(MainPage::typeid).Name)
	{
		this->ClearRessources();
		this->clearRecording();
		this->ReleaseAllRequestedDisplay();

		this->WriteToAppData(); // write App data in case of Suspending



	}
}

void MainPage::Application_Resuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Handle global application events only if this page is active
	if (Frame->CurrentSourcePageType.Name == Interop::TypeName(MainPage::typeid).Name)
	{
		//	this->ActivateDisplay();
	}
}


void MainPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{



	OnVifCameraViewModel^ onVifCameraViewModel = safe_cast<OnVifCameraViewModel^>(e->Parameter);
	if (onVifCameraViewModel != nullptr) {
		this->m_OnVifCameraViewModel = onVifCameraViewModel;
	}
	if (this->m_OnVifCameraViewModel == nullptr) return;

	if (this->m_OnVifCameraViewModel->Cameras->Size == 0) // lesen aus local Storage
	{
		this->m_OnVifCameraViewModel->readDatafromLocalStorage();
	}
	m_StreamingPageParamControl->ClearRessources(); // all previous events unregister
	m_StreamingPageParamControl->Items->Clear();
	//int ncount = 0;
	wchar_t buffer[200];
	for (unsigned int ncount = 0; ncount < this->m_OnVifCameraViewModel->Cameras->Size; ncount++)
		//for each (auto var in this->m_OnVifCameraViewModel->Cameras)
	{
		OnVifCamera^ var = this->m_OnVifCameraViewModel->Cameras->GetAt(ncount);
		IPStreamingCPP::StreamingPageParam^ param = ref new StreamingPageParam();

		swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"Camera_%03d", ncount);

		param->createStreamingPageParam(ref new Platform::String(buffer), this->Frame);
		param->OnVifCamera = var; // OnVif-Camera

		param->PropertyChangedEventRegister = param->OnVifCamera->PropertyChanged += ref new PropertyChangedEventHandler(this, &MainPage::ScenarioPropertyChanged);
		param->CameraServerFailedRegister = param->CameraServer->Failed += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, FFmpegInteropExtRT::CameraServerFailedEventArgs^>(this, &MainPage::CameraServerOnFailed);
		// Movement-Recording On
		param->OnStartMovementStreaming = param->MovementRecording->startStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^>(this, &IPStreamingCPP::MainPage::OnstartMovementStreaming);
		param->OnStopMovementStreaming = param->MovementRecording->stopStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Platform::String^>(this, &IPStreamingCPP::MainPage::OnStopMovementStreaming);
		param->OnChangeMovementStreaming = param->MovementRecording->ChangeMovement += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^>(this, &IPStreamingCPP::MainPage::OnChangeMovement);

		param->OnStartAMCRESEventStreaming = param->AmcrestMotion->startStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^>(this, &IPStreamingCPP::MainPage::OnstartMovementStreaming);
		param->OnStopAMCRESEventStreaming = param->AmcrestMotion->stopStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Platform::String^>(this, &IPStreamingCPP::MainPage::OnStopMovementStreaming);
		param->OnChangeAMCRESEventStreaming = param->AmcrestMotion->ChangeMovement += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^>(this, &IPStreamingCPP::MainPage::OnChangeMovement);


		param->MovementRecording->HostName = "WilliRaspiPlus";
		param->MovementRecording->Port = 3000;

		param->CodecReader = this->m_CodecReader;
		// Movement-Recording On

		param->ScenarioView = ref new IPStreamingCPP::ScenarioViewControl();

		Windows::UI::Xaml::Interop::TypeName tt = Windows::UI::Xaml::Interop::TypeName(StreamingPage::typeid);
		ScenarioItem^ item = ref new ScenarioItem("Streaming", tt, Symbol::AttachCamera, param);
		param->ScenarioView->Items->Append(item);

		tt = Windows::UI::Xaml::Interop::TypeName(OnVifServicesRunTime::OnVifSingleCameraPage::typeid);
		item = ref new ScenarioItem("OnVifCamera", tt, Symbol::Edit, param->OnVifCamera);
		param->ScenarioView->Items->Append(item);


		param->takeParametersFromCamera(); // Camera-Parameter werden übernommen
		m_StreamingPageParamControl->Items->Append(param);

	}
	m_StreamingPageParamControl->readSettingsfromLocalStorage();

	if (this->m_OnVifCameraViewModel->Cameras->Size == 0) m_StreamingPageParamControl->SelectedIndex = -1;
	else if (m_StreamingPageParamControl->SelectedIndex < 0) m_StreamingPageParamControl->SelectedIndex = 0;


	//param->ScenarioView->SelectedIndex = 0;
	Page::OnNavigatedTo(e);

}
void MainPage::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
	// Handling of this event is included for completeness, as it will only fire when navigating between pages and this sample only includes one page
	this->ClearRessources();
	this->clearRecording();
	this->ReleaseAllRequestedDisplay();
		this->WriteToAppData();
	m_StreamingPageParamControl->writeSettingsToLocalStorage();
	Page::OnNavigatingFrom(e);
}




void MainPage::startUriStreaming(Platform::Object^ sender, IPStreamingCPP::StreamingPageParam^ data)
{
	IPStreamingCPP::StreamingPageParam^ streamingPageParam = safe_cast<IPStreamingCPP::StreamingPageParam^>(data);
	bool boK = streamingPageParam->startUriStreaming();
	Splitter->IsPaneOpen = !boK;
}

void MainPage::startFileStreaming(Platform::Object^ sender, IPStreamingCPP::StreamingPageParam^ data)
{
	IPStreamingCPP::StreamingPageParam^ streamingPageParam = safe_cast<IPStreamingCPP::StreamingPageParam^>(data);
	bool boK = streamingPageParam->startFileStreaming();
	Splitter->IsPaneOpen = !boK;

}

void MainPage::stopStreaming(Platform::Object^ sender, IPStreamingCPP::StreamingPageParam^ data)
{
	IPStreamingCPP::StreamingPageParam^ streamingPageParam = safe_cast<IPStreamingCPP::StreamingPageParam^>(data);
	streamingPageParam->stopStreaming();

}

//this->startRecording->IsEnabled = true;
//this->stopRecording->IsEnabled = true;

void MainPage::stopRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	StreamingPageParam^ streamingPageParam = m_StreamingPageParamControl->getSelectedItem();
	if (streamingPageParam != nullptr) {
		streamingPageParam->stopStreaming();
	}

}



void MainPage::startRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	StreamingPageParam^ streamingPageParam = m_StreamingPageParamControl->getSelectedItem();
	if (streamingPageParam != nullptr) {
		bool boK = streamingPageParam->startUriStreaming();
		Splitter->IsPaneOpen = !boK;
	}

}




void MainPage::PivotCameras_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)

{
	//StreamingPageParam ^ streamingPageParam = m_StreamingPageParamControl->getSelectedItem();
	Pivot^ pivot = dynamic_cast<Pivot^>(sender); // Pivot
	if (pivot != nullptr) {
		StreamingPageParam^ param = dynamic_cast<StreamingPageParam^>(pivot->SelectedItem); // Property-Changed by OnVifCamera-Page
		if (param != nullptr) {
			ScenarioControl->ItemsSource = param->ScenarioView->Items;
			ScenarioControl->SelectedIndex = 0;
			if (param->MovementRecording->IsMoment || param->AmcrestMotion->IsMoment) {
				this->detectMovement->Visibility = Windows::UI::Xaml::Visibility::Visible;
			}
			else
			{
				this->detectMovement->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			}


		}

	}

}



void MainPage::ScenarioPropertyChanged(Platform::Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e)
{
	OnVifCamera^ camera = dynamic_cast<OnVifCamera^>(sender); // Property-Changed by OnVifCamera-Page
	if (camera != nullptr) {
		if (e->PropertyName == "IsProfilesReaded")
		{
			if (camera->IsProfilesReaded) { // only when reading
				StreamingPageParam^ param = this->m_StreamingPageParamControl->getItemByOnVifCamera(camera);
				if (param != nullptr) {

					param->takeParametersFromCamera(); // Camera-Parameter werden übernommen

				}
			}


		}

	}



}

void MainPage::ScenarioControl_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{

	//NotifyUser(String.Empty, NotifyType.StatusMessage);

	ListView^ scenarioListBox = dynamic_cast<ListView^>(sender);

	ScenarioItem^ item = dynamic_cast<ScenarioItem^>(scenarioListBox->SelectedItem);
	if (item != nullptr) {

		//		if (ScenarioFrame->CurrentSourcePageType.Name != item->TypeClassName.Name)
		{

			if (item->TypeClassName.Name == Windows::UI::Xaml::Interop::TypeName(OnVifSingleCameraPage::typeid).Name) {

				VisualStateManager::GoToState(this, "SetOpenPaneBig", true);
			}
			else {
				VisualStateManager::GoToState(this, "SetOpenPaneDefault", true);
			}

			ScenarioFrame->Navigate(item->TypeClassName, item->Object);
			StreamingPage^ page = dynamic_cast<StreamingPage^>(ScenarioFrame->Content);

			if (page != nullptr) {
				page->startUriStreaming += ref new  Windows::Foundation::TypedEventHandler<Platform::Object^, IPStreamingCPP::StreamingPageParam^>(this, &MainPage::startUriStreaming);
				page->startFileStreaming += ref new  Windows::Foundation::TypedEventHandler<Platform::Object^, IPStreamingCPP::StreamingPageParam^>(this, &MainPage::startFileStreaming);
				page->stopStreaming += ref new  Windows::Foundation::TypedEventHandler<Platform::Object^, IPStreamingCPP::StreamingPageParam^>(this, &MainPage::stopStreaming);

			}

		}
	}

}


void MainPage::CameraServerOnFailed(Platform::Object^ sender, FFmpegInteropExtRT::CameraServerFailedEventArgs^ args)
{
	Platform::String^ message = args->Message;
	DisplayErrorMessage(message);


	//	throw ref new Platform::NotImplementedException();
}


void MainPage::MediaElement_OnCurrentStateChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	MediaElement^ mediaElement = (MediaElement^)sender;
	if (mediaElement != nullptr && mediaElement->IsAudioOnly == false)
	{
		if (mediaElement->CurrentState == Windows::UI::Xaml::Media::MediaElementState::Playing)
		{
			this->ActivateDisplay();
		}
		else // CurrentState is Buffering, Closed, Opening, Paused, or Stopped. 
		{


			bool bRelease = true;
			if (mediaElement->CurrentState == Windows::UI::Xaml::Media::MediaElementState::Opening) {
				bRelease = false;
			}
			if (mediaElement->CurrentState == Windows::UI::Xaml::Media::MediaElementState::Paused) {

			}
			if (mediaElement->CurrentState == Windows::UI::Xaml::Media::MediaElementState::Closed) {

			}
			if (mediaElement->CurrentState == Windows::UI::Xaml::Media::MediaElementState::Stopped) {

			}
			if (mediaElement->CurrentState == Windows::UI::Xaml::Media::MediaElementState::Buffering) {
				bRelease = false;
			}

			if (bRelease) {
				this->ReleaseDisplay();
			}




		}
	}

	//	throw ref new Platform::NotImplementedException();
}

void MainPage::MediaElement_OnMediaFailed(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e)
{
	if (m_displayRequest != nullptr)
	{
		// Deactivate the display request and set the var to null.
		this->ReleaseDisplay();
	}
	Platform::String^ message = e->ErrorMessage;
	DisplayErrorMessage(message);

	//	throw ref new Platform::NotImplementedException();
}
void MainPage::clearRecording()
{

	for each (auto var in this->m_StreamingPageParamControl->Items)
	{
		var->clearRecording();
	}
	//	this->detectMovement->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

void MainPage::ClearRessources()
{
	m_StreamingPageParamControl->ClearRessources(); // all previous events unregister

}



void IPStreamingCPP::MainPage::OnStopMovementStreaming(Platform::Object^ sender, Platform::String^ args)
{
	AmcrestMotion^ Amcrestrecording = dynamic_cast<AmcrestMotion^>(sender);
	RecordingListener::Recording^ recording = dynamic_cast<RecordingListener::Recording^>(sender);


	if (args != nullptr) { // stop movement with error
		Platform::String^ message = "undefinded Watcher: " + args;
		if (Amcrestrecording) {
			message = "AMCREST-Event Watcher: " + args;
		}
		else {
			message = "Movement-Watcher: " + args;
		}
	
		DisplayErrorMessage(message);
	}

	//	throw ref new Platform::NotImplementedException();
}

void IPStreamingCPP::MainPage::OnstartMovementStreaming(Platform::Object^ sender, Windows::Foundation::Collections::PropertySet^ args)
{
	//	throw ref new Platform::NotImplementedException();
}



//void IPStreamingCPP::MainPage::OnStopAMCRESTEventStreaming(Platform::Object^ sender, Platform::String^ args)
//{
//	if (args != nullptr) { // stop movement with error
//		Platform::String^ message = "AMCREST-Event Watcher: " + args;
//		DisplayErrorMessage(message);
//	}
//
//}


void IPStreamingCPP::MainPage::OnChangeMovement(Platform::Object^ sender, Windows::Foundation::Collections::PropertySet^ args)
{

	//AmcrestMotion^ recording = dynamic_cast<AmcrestMotion^>(sender);
	bool dodetect = false;
	//if (recording != nullptr)
	{
		StreamingPageParam^ streamingPageParam = m_StreamingPageParamControl->getSelectedItem();
		if (streamingPageParam != nullptr) {
			//if (streamingPageParam->AmcrestMotion == recording)
			{
				Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
					CoreDispatcherPriority::Normal,
					ref new Windows::UI::Core::DispatchedHandler([this, streamingPageParam, args]()
						{	
							bool IsMoment = (streamingPageParam->MovementRecording->IsMoment || streamingPageParam->AmcrestMotion->IsMoment);
							if (IsMoment) {
								this->detectMovement->Visibility = Windows::UI::Xaml::Visibility::Visible;
							}
							else
							{
								this->detectMovement->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
							}
							// not activated checkForEvents(streamingPageParam);

						}));

			}
		}
	}

}
bool IPStreamingCPP::MainPage::checkForEvents(StreamingPageParam^ streamingPageParam) {

	Platform::String^ eventset = ref new Platform::String(L"");

	Windows::Foundation::Collections::IObservableVector<Platform::String^ >^ events = streamingPageParam->AmcrestMotion->Events;
	for (unsigned int i = 0; i < events->Size; i++) {
		eventset = events->GetAt(i);
		eventset+= "\\r\\n";
	}

	if (eventset->Length() > 0) {
		DisplayErrorMessage(eventset);
	}

	return (events->Size>0);


}

//bool IPStreamingCPP::MainPage::checkForMovement(Windows::Foundation::Collections::PropertySet^ args) {
//
//	if (args->HasKey("m_MovementActiv") && args->HasKey("m_MovementActivated") && args->HasKey("m_RecordingActivTimeinSec")) {
//		Platform::Object^ isActivvalue = args->Lookup("m_MovementActiv");
//		//Platform::Object^ isActatedvalue = args->Lookup("m_MovementActivated");
//		Platform::Object^ RecordingTime = args->Lookup("m_RecordingActivTimeinSec");
//		bool isActiv = safe_cast<IPropertyValue^>(isActivvalue)->GetBoolean();
//		//	int isActived = safe_cast<IPropertyValue^>(isActatedvalue)->GetInt32();
//			//if (isActived > 0) 
//		{
//			return (isActiv);
//		}
//	}
//
//	return false;
//
//}


//
//void IPStreamingCPP::MainPage::OnChangeMovement(Platform::Object^ sender, Windows::Foundation::Collections::PropertySet^ args)
//{
//
//	RecordingListener::Recording^ recording = dynamic_cast<RecordingListener::Recording^>(sender);
//	bool dodetect = false;
//	if (recording != nullptr) {
//		StreamingPageParam^ streamingPageParam = m_StreamingPageParamControl->getSelectedItem();
//		if (streamingPageParam != nullptr) {
//			if (streamingPageParam->MovementRecording == recording)
//			{
//				Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
//					CoreDispatcherPriority::Normal,
//					ref new Windows::UI::Core::DispatchedHandler([this, recording, args]()
//						{
//							bool IsMoment = recording->IsMoment;
//								
//						//		checkForMovement(args);
//							if (IsMoment) {
//								this->detectMovement->Visibility = Windows::UI::Xaml::Visibility::Visible;
//							}
//							else
//							{
//								this->detectMovement->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
//							}
//
//						}));
//
//			}
//		}
//	}
//
//}


