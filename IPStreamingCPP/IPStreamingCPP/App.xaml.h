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
	ref class App sealed :
		public FFmpegInteropExtRT::ILogProvider
	{

	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		App();

	public:
		virtual void Log(FFmpegInteropExtRT::LogLevel level, Platform::String^ message);
		void App::WriteLogMessage(Platform::String^ message);
		virtual ~App();
	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
	//	void UnhandledException(Platform::Object ^ sender, Windows::UI::Xaml::UnhandledExceptionEventArgs^ e);
		wofstream* m_poutout;
		OnVifServicesRunTime::OnVifCameraViewModel ^ m_OnVifCameraViewModel;
	};
}
