//
// MainPage.xaml.h
// Declaration of the MainPage class.
//
#ifndef MAINPAGE
#define MAINPAGE
#pragma once

#include "MainPage.g.h"
#include "ScenarioViewControl.h"

#include "DataSources.h"

#include "StreamingPageParam.h"



using namespace IPStreamingCPP;

namespace IPStreamingCPP
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
		IPStreamingCPP::DataSources ^ m_datasources;
		IPStreamingCPP::StreamingPageParamControl^ m_StreamingPageParamControl;
		OnVifServicesRunTime::OnVifCameraViewModel ^ m_OnVifCameraViewModel;
		Windows::Foundation::EventRegistrationToken m_applicationSuspendingEventToken;
		Windows::Foundation::EventRegistrationToken m_applicationResumingEventToken;
		Windows::System::Display::DisplayRequest ^m_displayRequest;
		int m_displayRequestCnt;

		FFmpegInteropExtRT::CodecReader^ m_CodecReader;


	public:
		MainPage();
		virtual ~MainPage();

		property OnVifServicesRunTime::OnVifCameraViewModel^ CameraViewModel
		{
			OnVifServicesRunTime::OnVifCameraViewModel^ get() { return m_OnVifCameraViewModel; };
		}

		property IPStreamingCPP::StreamingPageParamControl^ StreamingPageControl
		{
			IPStreamingCPP::StreamingPageParamControl^ get() { return m_StreamingPageParamControl; };
		}



		void DisplayErrorMessage(Platform::String^ message);
	public:

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;

	private:
		void ScenarioControl_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void PivotCameras_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void PivotMediaLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	//	void MediaFailed(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e);

		void Application_Suspending(Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void Application_Resuming(Object^ sender, Object^ args);
		void CameraServerOnFailed(Platform::Object ^sender, FFmpegInteropExtRT::CameraServerFailedEventArgs ^args);
		void MediaElement_OnCurrentStateChanged(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	
		void MediaElement_OnMediaFailed(Platform::Object ^sender, Windows::UI::Xaml::ExceptionRoutedEventArgs ^e);
		void startUriStreaming(Platform::Object ^sender, IPStreamingCPP::StreamingPageParam ^ datasources);
		void startFileStreaming(Platform::Object ^sender, IPStreamingCPP::StreamingPageParam ^ datasources);
		void stopStreaming(Platform::Object ^sender, IPStreamingCPP::StreamingPageParam ^ datasources);

		void stopRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void startRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void ScenarioPropertyChanged(Platform::Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e);

		void clearRecording();

		void ActivateDisplay();
		void ReleaseDisplay();
		void ReleaseAllRequestedDisplay();

		void WriteToAppData();


		void ClearRessources();

		void OnstartMovementStreaming(Platform::Object ^sender, Windows::Foundation::Collections::PropertySet ^args);
	
		//
		void OnStopMovementStreaming(Platform::Object ^sender, Platform::String ^args);
		void OnChangeMovement(Platform::Object ^sender, Windows::Foundation::Collections::PropertySet ^args);


	//	void OnChangeAMCRESTEventMovement(Platform::Object^ sender, Windows::Foundation::Collections::PropertySet^ args);
	//	void OnStopAMCRESTEventStreaming(Platform::Object^ sender, Platform::String^ args);

	//	void OnStartAMCRESTEventStreaming(Platform::Object^ sender, Windows::Foundation::Collections::PropertySet^ args);
	//	bool checkForMovement(Windows::Foundation::Collections::PropertySet^ args);
		bool checkForEvents(StreamingPageParam^ streamingPageParam);


	};


}

#endif // MAINPAGE