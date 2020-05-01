//
// MainPageOverView.xaml.cpp
// Implementierung der MainPageOverView-Klasse
//

#include "pch.h"
#include "MainPageOverView.xaml.h"

using namespace IPStreamingCPP;

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

// Die Elementvorlage "Leere Seite" wird unter https://go.microsoft.com/fwlink/?LinkId=234238 dokumentiert.
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

MainPageOverView::MainPageOverView()
{
	InitializeComponent();

	m_OnVifCameraViewModel = nullptr;


	m_displayRequest = ref new Windows::System::Display::DisplayRequest();
	m_displayRequestCnt = 0;


	m_StreamingPageParamControl = ref new StreamingPageParamControl();


	m_applicationSuspendingEventToken =
		Application::Current->Suspending += ref new SuspendingEventHandler(this, &MainPageOverView::Application_Suspending);
	m_applicationResumingEventToken =
		Application::Current->Resuming += ref new EventHandler<Object^>(this, &MainPageOverView::Application_Resuming);

	m_CodecReader = ref new FFmpegInteropExtRT::CodecReader();
	// srw2ho: crashin case of first call, but only one computer m_CodecReader->ReadInstalledVideoDecoderCodecsAsync();
	// readonly used Video codecs
	m_CodecReader->ReadUsedVideoDecoderCodecsAsync();
	m_CodecReader->ReadUsedAudioDecoderCodecsAsync();




}

MainPageOverView::~MainPageOverView()
{

	Application::Current->Suspending -= m_applicationSuspendingEventToken;
	Application::Current->Resuming -= m_applicationResumingEventToken;


	//	clearRecording();

	//	ReleaseAllRequestedDisplay();

	delete m_StreamingPageParamControl;

}


void MainPageOverView::buildMovementMap(MediaElement^ MediaStreamElement)
{


	for (unsigned int i = 0; i < CameraGrid->Children->Size; i++) {
		UIElement^ var = CameraGrid->Children->GetAt(i);

		StackPanel^ stackPanel = dynamic_cast<StackPanel^>(var);
		if (stackPanel != nullptr) {

			for (unsigned int j = 0; j < stackPanel->Children->Size; j++) {
				UIElement^ uivar = stackPanel->Children->GetAt(j);

				if (uivar == MediaStreamElement) {
					m_MovementMap[MediaStreamElement->GetHashCode()] = stackPanel;
					break;
				}

				Grid^ grid = dynamic_cast<Grid^>(uivar);
				if (grid != nullptr) {
					for (unsigned int k = 0; k < grid->Children->Size; k++) {
						UIElement^ uivar = grid->Children->GetAt(k);
						//Image^ image = dynamic_cast<Image^>(uivar);
						//if (image != nullptr) {
						//	m_Movement.m_MovementImage = image;
						//}

						if (uivar == MediaStreamElement) {
							m_MovementMap[MediaStreamElement->GetHashCode()] = stackPanel;
							break;
						}
					}
				}


			}
		}

	}

	return;
}



StackPanel^ MainPageOverView::getStackPanelByStreamingParam(IPStreamingCPP::StreamingPageParam^ data)
{
	MapMovementMap::iterator it;
	it = m_MovementMap.find(data->MediaStreamElement->GetHashCode());

	if (it != m_MovementMap.end()) {
		return it->second;
	}

	return nullptr;
}



void MainPageOverView::setMediaElem(unsigned int Idx) {

	StreamingPageParam^ streamingPageParam = nullptr;
	MediaElement^ media = nullptr;

	if (Idx < m_StreamingPageParamControl->Items->Size) {

		streamingPageParam = m_StreamingPageParamControl->Items->GetAt(Idx);
		switch (Idx) {
		case 0:
			media = media_0_0;
			break;
		case 1:
			media = media_1_0;
			break;
		case 2:
			media = media_0_1;
			break;
		case 3:
			media = media_1_1;
			break;
		case 4:
			media = media_0_2;
			break;
		case 5:
			media = media_1_2;
			break;
		case 6:
			media = media_0_3;
			break;
		case 7:
			media = media_1_3;
			break;
		case 8:
			media = media_0_4;
			break;
		case 9:
			media = media_1_4;
			break;

		}
	}

	if ((streamingPageParam != nullptr) && (media != nullptr)) {
		buildMovementMap(media);
		streamingPageParam->MediaStreamElement = media;
		streamingPageParam->MediaCurrentStateChangedRegister = streamingPageParam->MediaStreamElement->CurrentStateChanged += ref new Windows::UI::Xaml::RoutedEventHandler(this, &MainPageOverView::MediaElement_OnCurrentStateChanged);
		streamingPageParam->MediaFailedRegister = streamingPageParam->MediaStreamElement->MediaFailed += ref new Windows::UI::Xaml::ExceptionRoutedEventHandler(this, &MainPageOverView::MediaElement_OnMediaFailed);

	}




}


//void MainPageOverView::PivotMediaLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
//{
//
//
//}



void MainPageOverView::DisplayErrorMessage(Platform::String^ message)
{
	// Display error message
	this->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this, message]() {

		auto errorDialog = ref new MessageDialog(message);
		errorDialog->ShowAsync();


		}));


}


void MainPageOverView::ActivateDisplay()
{
	//create the request instance if needed

	//make request to put in active state
	m_displayRequest->RequestActive();
	m_displayRequestCnt++;
}

void MainPageOverView::ReleaseAllRequestedDisplay()
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

void MainPageOverView::ReleaseDisplay()
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

void MainPageOverView::WriteToAppData()
{


	for each (auto var in this->m_StreamingPageParamControl->Items)
	{
		var->WriteToAppData();
	}

}

void MainPageOverView::Application_Suspending(Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e)
{
	// Handle global application events only if this page is active
	if (Frame->CurrentSourcePageType.Name == Interop::TypeName(MainPageOverView::typeid).Name)
	{
		this->ClearRessources();
		this->clearRecording();

		this->ReleaseAllRequestedDisplay();

		this->WriteToAppData(); // write App data in case of Suspending



	}
}

void MainPageOverView::Application_Resuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Handle global application events only if this page is active
	if (Frame->CurrentSourcePageType.Name == Interop::TypeName(MainPageOverView::typeid).Name)
	{
		//	this->ActivateDisplay();
	}
}


void MainPageOverView::configureCameraGrid()
{

	auto scaleFactor = m_app->DisplayScaleFactor;	// realized in app.xaml.cpp
	auto size = m_app->DisplaySize;					// realized in app.xaml.cpp

	double fromTop = 40* scaleFactor; // distance from top
	double sizeHight;

	if (size.Height> fromTop) sizeHight = size.Height - fromTop;
	else sizeHight = size.Height;

	// Camera__Column_Row
	if (this->m_StreamingPageParamControl->Items->Size == 1) {
		bool bok = VisualStateManager::GoToState(this, SetCameraRowsTo0->Name, false);
	}
	else if ((this->m_StreamingPageParamControl->Items->Size > 1) && (this->m_StreamingPageParamControl->Items->Size <= 2)) {

		bool bok = VisualStateManager::GoToState(this, SetCameraRowsTo1->Name, false);
		media_0_0->MinHeight = sizeHight;
		media_1_0->MinHeight = sizeHight;

	}
	else
		if ((this->m_StreamingPageParamControl->Items->Size > 2) && (this->m_StreamingPageParamControl->Items->Size <= 4)) {
			bool bok = VisualStateManager::GoToState(this, SetCameraRowsTo2->Name, false);
			media_0_0->MinHeight = sizeHight / 2;
			media_1_0->MinHeight = sizeHight / 2;
			media_0_1->MinHeight = sizeHight / 2;
			media_1_1->MinHeight = sizeHight / 2;
		}
		else if ((this->m_StreamingPageParamControl->Items->Size > 4) && (this->m_StreamingPageParamControl->Items->Size <= 6)) {
			bool bok = VisualStateManager::GoToState(this, SetCameraRowsTo3->Name, false);
			media_0_0->MinHeight = sizeHight / 3;
			media_1_0->MinHeight = sizeHight / 3;
			media_0_1->MinHeight = sizeHight / 3;
			media_1_1->MinHeight = sizeHight / 3;
			media_0_2->MinHeight = sizeHight / 3;
			media_1_2->MinHeight = sizeHight / 3;
		}
		else
			if ((this->m_StreamingPageParamControl->Items->Size > 6) && (this->m_StreamingPageParamControl->Items->Size <= 8)) {
				bool bok = VisualStateManager::GoToState(this, SetCameraRowsTo4->Name, false);
				media_0_0->MinHeight = sizeHight / 4;
				media_1_0->MinHeight = sizeHight / 4;
				media_0_1->MinHeight = sizeHight / 4;
				media_1_1->MinHeight = sizeHight / 4;
				media_0_2->MinHeight = sizeHight / 4;
				media_1_2->MinHeight = sizeHight / 4;
				media_0_3->MinHeight = sizeHight / 4;
				media_1_3->MinHeight = sizeHight / 4;
			}
			else {
			}


}
void MainPageOverView::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{

	IPStreamingCPP::App^ app = safe_cast<IPStreamingCPP::App^>(e->Parameter);
	m_app = app;
	//OnVifCameraViewModel^ onVifCameraViewModel = safe_cast<OnVifCameraViewModel^>(e->Parameter);
	if (app != nullptr) {
		this->m_OnVifCameraViewModel = app->CameraViewModel;
	}
	if (this->m_OnVifCameraViewModel == nullptr) return;





	if (this->m_OnVifCameraViewModel->Cameras->Size == 0) // lesen aus local Storage
	{
		this->m_OnVifCameraViewModel->readDatafromLocalStorage();
	}
	m_StreamingPageParamControl->ClearRessources(); // all previous events unregister
	m_StreamingPageParamControl->Items->Clear();
	bool doSelction = (this->m_OnVifCameraViewModel->Cameras->Size > 1);
	//int ncount = 0;
	wchar_t buffer[200];
	for (unsigned int ncount = 0; ncount < this->m_OnVifCameraViewModel->Cameras->Size; ncount++)
		//for each (auto var in this->m_OnVifCameraViewModel->Cameras)
	{
		OnVifCamera^ var = this->m_OnVifCameraViewModel->Cameras->GetAt(ncount);
		if (doSelction && !var->IsCameraViewActiv) continue;

		IPStreamingCPP::StreamingPageParam^ param = ref new StreamingPageParam();

		swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"Camera_%03d", ncount);

		param->createStreamingPageParam(ref new Platform::String(buffer), this->Frame);
		param->OnVifCamera = var; // OnVif-Camera

		param->PropertyChangedEventRegister = param->OnVifCamera->PropertyChanged += ref new PropertyChangedEventHandler(this, &MainPageOverView::ScenarioPropertyChanged);
		param->CameraServerFailedRegister = param->CameraServer->Failed += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, FFmpegInteropExtRT::CameraServerFailedEventArgs^>(this, &MainPageOverView::CameraServerOnFailed);
		// Movement-Recording On
		param->OnStartMovementStreaming = param->MovementRecording->startStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^>(this, &IPStreamingCPP::MainPageOverView::OnstartMovementStreaming);
		param->OnStopMovementStreaming = param->MovementRecording->stopStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Platform::String^>(this, &IPStreamingCPP::MainPageOverView::OnStopMovementStreaming);
		param->OnChangeMovementStreaming = param->MovementRecording->ChangeMovement += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^>(this, &IPStreamingCPP::MainPageOverView::OnChangeMovement);

		param->OnStartAMCRESEventStreaming = param->AmcrestMotion->startStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^>(this, &IPStreamingCPP::MainPageOverView::OnstartMovementStreaming);
		param->OnStopAMCRESEventStreaming = param->AmcrestMotion->stopStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Platform::String^>(this, &IPStreamingCPP::MainPageOverView::OnStopMovementStreaming);
		param->OnChangeAMCRESEventStreaming = param->AmcrestMotion->ChangeMovement += ref new Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^>(this, &IPStreamingCPP::MainPageOverView::OnChangeMovement);


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

		setMediaElem(m_StreamingPageParamControl->Items->Size - 1);
	}

	m_selectedCameraPanel = nullptr;
	configureCameraGrid();

	m_StreamingPageParamControl->readSettingsfromLocalStorage();

	if (this->m_StreamingPageParamControl->Items->Size == 0) m_StreamingPageParamControl->SelectedIndex = -1;
	else if (m_StreamingPageParamControl->SelectedIndex < 0) m_StreamingPageParamControl->SelectedIndex = 0;





	Page::OnNavigatedTo(e);

}

void MainPageOverView::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
	//e->Cancel = !ressourdel;
	// Handling of this event is included for completeness, as it will only fire when navigating between pages and this sample only includes one page
	this->ClearRessources();
	this->clearRecording();

	this->ReleaseAllRequestedDisplay();
	this->WriteToAppData();
	m_StreamingPageParamControl->writeSettingsToLocalStorage();
	Page::OnNavigatingFrom(e);


}



void MainPageOverView::startUriStreaming(Platform::Object^ sender, IPStreamingCPP::StreamingPageParam^ data)
{
	IPStreamingCPP::StreamingPageParam^ streamingPageParam = safe_cast<IPStreamingCPP::StreamingPageParam^>(data);

	auto tsk = streamingPageParam->startUriStreaming();
	startCameraViewing(streamingPageParam);

	Splitter->IsPaneOpen = false;
}

void MainPageOverView::startFileStreaming(Platform::Object^ sender, IPStreamingCPP::StreamingPageParam^ data)
{
	IPStreamingCPP::StreamingPageParam^ streamingPageParam = safe_cast<IPStreamingCPP::StreamingPageParam^>(data);

	auto boK = streamingPageParam->startFileStreaming();
	startCameraViewing(streamingPageParam);

	Splitter->IsPaneOpen = false;

}

void MainPageOverView::stopStreaming(Platform::Object^ sender, IPStreamingCPP::StreamingPageParam^ data)
{
	IPStreamingCPP::StreamingPageParam^ streamingPageParam = safe_cast<IPStreamingCPP::StreamingPageParam^>(data);
	streamingPageParam->clearMediaElem();
	streamingPageParam->stopStreaming();


	StackPanel^ panel = getStackPanelByStreamingParam(streamingPageParam);

	if (panel != nullptr) {
		panel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}


}


void MainPageOverView::stopallRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto tsk = clearRecordingAsync();
	tsk.then([this]()->bool {

		this->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this]() {

			for each (auto var in m_StreamingPageParamControl->Items)
			{

				StackPanel^ panel = getStackPanelByStreamingParam(var);

				if (panel != nullptr) {
					panel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
				}
			}

			}));

		return true;


		}

	);


}

void MainPageOverView::startallRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{


	for each (auto var in m_StreamingPageParamControl->Items)
	{
		var->startUriStreaming();
		startCameraViewing(var);

	}

	Splitter->IsPaneOpen = false;


}
void MainPageOverView::stopRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	StreamingPageParam^ streamingPageParam = m_StreamingPageParamControl->getSelectedItem();
	if (streamingPageParam != nullptr) {
		streamingPageParam->clearMediaElem();
		auto tsk = streamingPageParam->stopStreaming();

		tsk.then([=, this]()->bool {

			this->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([=, this]() {
				StackPanel^ panel = getStackPanelByStreamingParam(streamingPageParam);
				if (panel != nullptr) {
					panel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

				}

				}));

			return true;


			}

		);

	}

}



void MainPageOverView::startRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	StreamingPageParam^ streamingPageParam = m_StreamingPageParamControl->getSelectedItem();
	if (streamingPageParam != nullptr) {
		auto tsk = streamingPageParam->startUriStreaming();
		startCameraViewing(streamingPageParam);
		Splitter->IsPaneOpen = false;


	}

}


void MainPageOverView::PivotCameras_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)

{

	Pivot^ pivot = dynamic_cast<Pivot^>(sender); // Pivot
	if (pivot != nullptr) {

		StreamingPageParam^ param = dynamic_cast<StreamingPageParam^>(pivot->SelectedItem); // Property-Changed by OnVifCamera-Page
		if (param != nullptr) {
			ScenarioControl->ItemsSource = param->ScenarioView->Items;
			ScenarioControl->SelectedIndex = 0;

			StackPanel^ panel = getStackPanelByStreamingParam(param);
			if (panel != nullptr) {
				selectCameraPanel(panel);
				//	panel->Focus = true;
			}


		}
		else {
		}

	}

}



void MainPageOverView::ScenarioPropertyChanged(Platform::Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e)
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

void MainPageOverView::ScenarioControl_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
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
				page->startUriStreaming += ref new  Windows::Foundation::TypedEventHandler<Platform::Object^, IPStreamingCPP::StreamingPageParam^>(this, &MainPageOverView::startUriStreaming);
				page->startFileStreaming += ref new  Windows::Foundation::TypedEventHandler<Platform::Object^, IPStreamingCPP::StreamingPageParam^>(this, &MainPageOverView::startFileStreaming);
				page->stopStreaming += ref new  Windows::Foundation::TypedEventHandler<Platform::Object^, IPStreamingCPP::StreamingPageParam^>(this, &MainPageOverView::stopStreaming);

			}

		}
	}

}


void MainPageOverView::CameraServerOnFailed(Platform::Object^ sender, FFmpegInteropExtRT::CameraServerFailedEventArgs^ args)
{
	Platform::String^ message = args->Message;
	DisplayErrorMessage(message);


	//	throw ref new Platform::NotImplementedException();
}


void MainPageOverView::MediaElement_OnCurrentStateChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

void MainPageOverView::MediaElement_OnMediaFailed(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e)
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

void MainPageOverView::clearRecording()
{
	m_StreamingPageParamControl->clearRecording();
	//for each (auto var in this->m_StreamingPageParamControl->Items)
	//{
	//	auto tsk = var->clearRecording();
	//}

}

concurrency::task<void> MainPageOverView::clearRecordingAsync()
{

	for each (auto var in this->m_StreamingPageParamControl->Items)
	{
		var->clearMediaElem();
	}

	auto tsk = create_task([this]()->void {
		try {

			for each (auto var in this->m_StreamingPageParamControl->Items)
			{
				auto tsk = var->clearRecording();
				tsk.wait();
			}
		}
		catch (Exception^ exception)
		{
			bool bexception = true;
		}

		});

	return tsk;


	//	this->detectMovement->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

void MainPageOverView::ClearRessources()
{
	m_StreamingPageParamControl->ClearRessources(); // all previous events unregister

}



void IPStreamingCPP::MainPageOverView::OnStopMovementStreaming(Platform::Object^ sender, Platform::String^ args)
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

void IPStreamingCPP::MainPageOverView::OnstartMovementStreaming(Platform::Object^ sender, Windows::Foundation::Collections::PropertySet^ args)
{
	//	throw ref new Platform::NotImplementedException();
}



//void IPStreamingCPP::MainPageOverView::OnStopAMCRESTEventStreaming(Platform::Object^ sender, Platform::String^ args)
//{
//	if (args != nullptr) { // stop movement with error
//		Platform::String^ message = "AMCREST-Event Watcher: " + args;
//		DisplayErrorMessage(message);
//	}
//
//}

void MainPageOverView::startCameraViewing(IPStreamingCPP::StreamingPageParam^ data) {
	StackPanel^ panel = getStackPanelByStreamingParam(data);
	if (panel != nullptr) {
		panel->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}

	//Windows::UI::Xaml::Controls::Image^ movementimage = getMovementImageByStreamingParam(data);
	//if (movementimage != nullptr) {
	//	movementimage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	//}
}


void IPStreamingCPP::MainPageOverView::OnChangeMovement(Platform::Object^ sender, Windows::Foundation::Collections::PropertySet^ args)
{

	//AmcrestMotion^ recording = dynamic_cast<AmcrestMotion^>(sender);
	bool dodetect = false;
	StreamingPageParam^ streamingPageParam = nullptr;

	{
		for each (auto var in this->m_StreamingPageParamControl->Items)
		{
			if (var->AmcrestMotion == sender) {
				streamingPageParam = var;
				break;
			}
			if (var->MovementRecording == sender) {
				streamingPageParam = var;
				break;
			}
		}

		if (streamingPageParam != nullptr) {
			//if (streamingPageParam->AmcrestMotion == recording)
			{
				Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
					CoreDispatcherPriority::Normal,
					ref new Windows::UI::Core::DispatchedHandler([this, streamingPageParam, args]()
						{
							Windows::UI::Xaml::Controls::StackPanel^ mvmPanel= getStackPanelByStreamingParam(streamingPageParam);
							if (mvmPanel != nullptr) {
								bool IsMoment = (streamingPageParam->MovementRecording->IsMoment || streamingPageParam->AmcrestMotion->IsMoment);
								if (IsMoment) {
									this->selectMovementCameraPanel(mvmPanel);
								}
								else
								{
									this->unselectMovementCameraPanel(mvmPanel);
								}
							}

						}));

			}
		}
	}

}
bool IPStreamingCPP::MainPageOverView::checkForEvents(StreamingPageParam^ streamingPageParam) {

	Platform::String^ eventset = ref new Platform::String(L"");

	Windows::Foundation::Collections::IObservableVector<Platform::String^ >^ events = streamingPageParam->AmcrestMotion->Events;
	for (unsigned int i = 0; i < events->Size; i++) {
		eventset = events->GetAt(i);
		eventset += "\\r\\n";
	}

	if (eventset->Length() > 0) {
		DisplayErrorMessage(eventset);
	}

	return (events->Size > 0);


}

void MainPageOverView::unselectCameraPanel(StackPanel^ panel) {
	//return;
	panel->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::White);
//	panel->BorderThickness =  Thickness(2);
	
}
void MainPageOverView::unselectMovementCameraPanel(StackPanel^ panel) {
	
	auto it = m_SelectedMovementMap.find(panel->GetHashCode());
	if (it != m_SelectedMovementMap.end()) {
		m_SelectedMovementMap.erase(it);
	}

	if (panel == m_selectedCameraPanel) {
		selectCameraPanel(panel);
	}
	else {
		unselectCameraPanel(panel);
	}


}

void MainPageOverView::selectMovementCameraPanel(StackPanel^ panel) {
	panel->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::DarkBlue);
	m_SelectedMovementMap[panel->GetHashCode()] = panel;

}


void MainPageOverView::selectCameraPanel(StackPanel^ panel) {

	MapMovementMap::iterator it = m_SelectedMovementMap.find(panel->GetHashCode());
	bool bMovementPanel = (it != m_SelectedMovementMap.end());


	if (bMovementPanel) {// selected hat movement
		m_selectedCameraPanel = panel;
	}
	else {
		bool bSelectedMovementPane = false;
		if (m_selectedCameraPanel != nullptr) {
			MapMovementMap::iterator selectedit = m_SelectedMovementMap.find(m_selectedCameraPanel->GetHashCode());
			bSelectedMovementPane = (selectedit != m_SelectedMovementMap.end());

			if (!bSelectedMovementPane) {
				unselectCameraPanel(m_selectedCameraPanel);
			}

			m_selectedCameraPanel = nullptr;
		}

		panel->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::DarkGray);

		//	panel->BorderThickness =  Thickness(2);
		m_selectedCameraPanel = panel;
	}


}




void IPStreamingCPP::MainPageOverView::CameraPicture_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{

	StackPanel^ panel = dynamic_cast<StackPanel^> (sender);
	bool bfound = false;
	if (panel != nullptr) {
		MapMovementMap::iterator it = m_MovementMap.begin();
		int panelhasCode = -1;
		while (it != m_MovementMap.end())
		{
			StackPanel^ panel_ = it->second;
			if (panel == panel_) {
				panelhasCode = it->first;
				break;
			}
			it++;
		}

		// Iterate over the map using Iterator till end.
		if (panelhasCode == -1)return;

		int mediaelemhasCode;
		for (unsigned int i = 0; i < m_StreamingPageParamControl->Items->Size; i++)
		{
			MapMovementMap::iterator it = m_MovementMap.begin();
			auto var = m_StreamingPageParamControl->Items->GetAt(i);
			mediaelemhasCode = var->MediaStreamElement->GetHashCode();
			if (mediaelemhasCode == panelhasCode) {
				PivotCameras->SelectedIndex = i;
				return;
			}

		}

	}

}
