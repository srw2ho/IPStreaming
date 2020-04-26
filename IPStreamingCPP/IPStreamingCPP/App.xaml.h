//
// App.xaml.h
// Declaration of the App class.
//
using namespace std;
#pragma once

#include "App.g.h"

namespace IPStreamingCPP
{
	/// <summary>
	/// Provides application-specific behavior to supplement the default Application class.
	/// </summary>


	public ref class App sealed :
		public FFmpegInteropExtRT::ILogProvider
	{

		wofstream* m_poutout;
		OnVifServicesRunTime::OnVifCameraViewModel^ m_OnVifCameraViewModel;
		Platform::String^ m_StreamingFolder;


	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ args) override;

	internal:
	

	public:
		App();
		virtual void Log(FFmpegInteropExtRT::LogLevel level, Platform::String^ message);
		void App::WriteLogMessage(Platform::String^ message);
		property OnVifServicesRunTime::OnVifCameraViewModel^ CameraViewModel
		{
			OnVifServicesRunTime::OnVifCameraViewModel^ get() { return m_OnVifCameraViewModel; };
		}
		property Platform::String^ StreamingFolder
		{
			Platform::String^ get() { return m_StreamingFolder; };
		}


		virtual ~App();
	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
	//	void UnhandledException(Platform::Object ^ sender, Windows::UI::Xaml::UnhandledExceptionEventArgs^ e);
	//	void createStreamingFolder();

	};
}
