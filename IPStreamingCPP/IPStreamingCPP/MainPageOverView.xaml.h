//
// MainPageOverView.xaml.h
// Deklaration der MainPageOverView-Klasse
//

#pragma once

#include "MainPageOverView.g.h"
#include "ScenarioViewControl.h"

#include "DataSources.h"

#include "StreamingPageParam.h"



using namespace IPStreamingCPP;

namespace IPStreamingCPP
{
	//class Movement {
	//public:
	//	Windows::UI::Xaml::Controls::StackPanel^ m_StackPanel;
	////	Windows::UI::Xaml::Controls::Image^ m_MovementImage;
	//};

	
	typedef std::map<int, Windows::UI::Xaml::Controls::StackPanel^> MapMovementMap;

	/// <summary>
	/// Eine leere Seite, die eigenständig verwendet oder zu der innerhalb eines Rahmens navigiert werden kann.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class MainPageOverView sealed
	{
		IPStreamingCPP::DataSources^ m_datasources;
		IPStreamingCPP::StreamingPageParamControl^ m_StreamingPageParamControl;
		OnVifServicesRunTime::OnVifCameraViewModel^ m_OnVifCameraViewModel;
		Windows::Foundation::EventRegistrationToken m_applicationSuspendingEventToken;
		Windows::Foundation::EventRegistrationToken m_applicationResumingEventToken;
		Windows::System::Display::DisplayRequest^ m_displayRequest;
		int m_displayRequestCnt;

		FFmpegInteropExtRT::CodecReader^ m_CodecReader;
		IPStreamingCPP::App^ m_app;

		Windows::UI::Xaml::Controls::StackPanel^ m_selectedCameraPanel;
		MapMovementMap m_MovementMap;
		MapMovementMap m_SelectedMovementMap;
	public:
		MainPageOverView();
		virtual ~MainPageOverView();

		property OnVifServicesRunTime::OnVifCameraViewModel^ CameraViewModel
		{
			OnVifServicesRunTime::OnVifCameraViewModel^ get() { return m_OnVifCameraViewModel; };
		}

		property IPStreamingCPP::StreamingPageParamControl^ StreamingPageControl
		{
			IPStreamingCPP::StreamingPageParamControl^ get() { return m_StreamingPageParamControl; };
		}

		

		void DisplayErrorMessage(Platform::String^ message);
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;

	private:
		void ScenarioControl_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void PivotCameras_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
//		void PivotMediaLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);


		void Application_Suspending(Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void Application_Resuming(Object^ sender, Object^ args);
		void CameraServerOnFailed(Platform::Object^ sender, FFmpegInteropExtRT::CameraServerFailedEventArgs^ args);
		void MediaElement_OnCurrentStateChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void MediaElement_OnMediaFailed(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e);
		void startUriStreaming(Platform::Object^ sender, IPStreamingCPP::StreamingPageParam^ datasources);
		void startFileStreaming(Platform::Object^ sender, IPStreamingCPP::StreamingPageParam^ datasources);
		void stopStreaming(Platform::Object^ sender, IPStreamingCPP::StreamingPageParam^ datasources);

		void stopRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void startRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void stopallRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void startallRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void ScenarioPropertyChanged(Platform::Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e);

		concurrency::task<void> clearRecordingAsync();
		void clearRecording();

		void ActivateDisplay();
		void ReleaseDisplay();
		void ReleaseAllRequestedDisplay();

		void WriteToAppData();


		void ClearRessources();

		void OnstartMovementStreaming(Platform::Object^ sender, Windows::Foundation::Collections::PropertySet^ args);

		//
		void OnStopMovementStreaming(Platform::Object^ sender, Platform::String^ args);
		void OnChangeMovement(Platform::Object^ sender, Windows::Foundation::Collections::PropertySet^ args);
		bool checkForEvents(StreamingPageParam^ streamingPageParam);
		void configureCameraGrid();
		void setMediaElem(unsigned int Idx);
		Windows::UI::Xaml::Controls::StackPanel^ getStackPanelByStreamingParam(IPStreamingCPP::StreamingPageParam^ data);
	
	//	Windows::UI::Xaml::Controls::Image^ getMovementImageByStreamingParam(IPStreamingCPP::StreamingPageParam^ data);
		void buildMovementMap(MediaElement^ MediaStreamElement);
		void startCameraViewing(IPStreamingCPP::StreamingPageParam^ data);
		void CameraPicture_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void unselectCameraPanel(StackPanel^ panel);
		void selectCameraPanel(StackPanel^ panel);
		void unselectMovementCameraPanel(StackPanel^ panel);
		void selectMovementCameraPanel(StackPanel^ panel);


	};
}
