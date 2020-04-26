//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"
//#include "MainPage.xaml.h"

#include "AppShell.xaml.h"
#include "LandingPage.xaml.h"
#include "MainPage.xaml.h"
#include "SystemInformationHelpers.h"
#include <fstream>

using namespace IPStreamingCPP;
using namespace OnVifServicesRunTime;

using namespace FFmpegInteropExtRT;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
	
//	this->UnhandledException += ref new UnhandledExceptionEventHandler(this, &App::UnhandledException);

	FFmpegInteropExtRT::FFmpegInteropLogging::SetLogLevel(FFmpegInteropExtRT::LogLevel::Info);
	
	FFmpegInteropExtRT::FFmpegInteropLogging::SetLogProvider(this);
	
	//FFmpegInterop::FFmpegInteropLogging::SetDefaultLogProvider();

	Platform::String^ localfolder = Windows::Storage::ApplicationData::Current->LocalFolder->Path;	//for local saving for future

	m_StreamingFolder = localfolder;

	m_OnVifCameraViewModel = ref new OnVifServicesRunTime::OnVifCameraViewModel();

	// win32-file function can not access outside local App Strage createStreamingFolder();

	String^ fileName = L"ffmpegLogFile.txt";

																				
	wchar_t fullfileName[512];
	swprintf(fullfileName, sizeof(fullfileName) / sizeof(fullfileName[0]), L"%s\\%s", localfolder->Data(), fileName->Data());

	m_poutout = new std::wofstream();
	m_poutout->open(fullfileName);

	WriteLogMessage("Create ffmpeg-Debug LogFile\n");
}

App::~App()
{
	if (m_poutout != nullptr)
	{
		m_poutout->close();
		delete m_poutout;
		m_poutout = nullptr;
	}
	if (m_OnVifCameraViewModel != nullptr) {
		m_OnVifCameraViewModel->writeDatatoLocalStorage();
		delete m_OnVifCameraViewModel;
	}

}

// in Package.appanifest item "VideBibliothek" must be clicked on 
//void App::createStreamingFolder() {
//	auto folder = Windows::Storage::StorageLibrary::GetLibraryAsync(Windows::Storage::KnownLibraryId::Videos);
//	
//	create_task(folder).then([this](task<StorageLibrary^> ret) {
//		try {
//			auto storageLib = ret.get();
//
//			bool folderpresent = false;
//			auto folders = storageLib->Folders;
//			if (folders->Size > 0) {
//				auto singlefolder = folders->GetAt(0);
//				auto CreateFoldertsk = singlefolder->GetFolderAsync(L"IPStreaming");
//					create_task(CreateFoldertsk).then([=,this](task< StorageFolder^ > desiredtsk) {
//					try {
//						auto desired = desiredtsk.get();
//
//						if (desired ==nullptr)
//						{
//							auto CreateFoldertsk = singlefolder->CreateFolderAsync(L"IPStreaming");
//							create_task(CreateFoldertsk).then([this](task< StorageFolder^ > desiredtsk) {
//								try {
//									auto desired = desiredtsk.get();
//									m_StreamingFolder = desired->Path;
//								}
//								catch (Platform::Exception^ ex) {
//									bool bok = false;
//								}
//
//								});
//						}
//						else {
//								m_StreamingFolder = desired->Path;
//						}
//					
//					}
//					catch (Platform::Exception^ ex) {
//						bool bok = false;
//					}
//
//					});
//
//			}
//			/*
//			for (unsigned int i = 0; i < folders->Size; i++) {
//				auto folder = folders->GetAt(i);
//				Platform::String^ folderName = folder->Name;
//				if (folder->Name == L"IPStreaming") {
//					folderpresent = true;
//					m_StreamingFolder = folder->Path;
//					break;
//				}
//
//			}
//			
//			if (!folderpresent) {
//				if (folders->Size == 0) return;
//
//				auto singlefolder = folders->GetAt(0);
//				auto CreateFoldertsk = singlefolder->CreateFolderAsync(L"IPStreaming");
//				create_task(CreateFoldertsk).then([this](task< StorageFolder^ > desiredtsk) {
//					try {
//
//						auto desired = desiredtsk.get();
//						m_StreamingFolder = desired->Path;
//					}
//					catch (Platform::Exception^ ex) {
//						bool bok = false;
//					}
//
//					});
//
//
//
//			}
//			*/
//
//		}
//		catch (Platform::Exception^ ex) {
//			bool bok = false;
//		}
//
//		});
//
//}

void App::WriteLogMessage(String^ message)
{

	if ((m_poutout != nullptr) && m_poutout->good() && m_poutout->is_open())
	{
		/*
		// funktioniert nicht, da Messges gestückelt ankommen, diese aber
		// nur zusammen einen Simm ergeben
		time_t rawtime;
		struct tm  timeinfo;
		wchar_t buffer[100];
		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		wcsftime(buffer, sizeof(buffer), L"%d.%m.%Y %T : ", &timeinfo);


		String^ _message = ref new String(buffer) + message;

		*/
		m_poutout->write(message->Data(), wcslen(message->Data()));

		m_poutout->flush();


	}

}
void App::Log(LogLevel level, String^ message)
{
	WriteLogMessage(message);
	
	

//	OutputDebugString(message->Data());
}

void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{

#if _DEBUG
	// Show graphics profiling information while debugging.
	if (IsDebuggerPresent())
	{
		// Display the current frame rate counters
		//DebugSettings->EnableFrameRateCounter = true;
	}
#endif

	// Change minimum window size
	ApplicationView::GetForCurrentView()->SetPreferredMinSize(Size(320, 200));

	// Darken the window title bar using a color value to match app theme
	ApplicationViewTitleBar^ titleBar = ApplicationView::GetForCurrentView()->TitleBar;
	if (titleBar != nullptr)
	{
		Color color = safe_cast<Color>(this->Resources->Lookup(L"SystemChromeMediumColor"));
		titleBar->BackgroundColor = color;
		titleBar->ButtonBackgroundColor = color;
	}

	if (SystemInformationHelpers::IsTenFootExperience())
	{
		// Apply guidance from https://msdn.microsoft.com/windows/uwp/input-and-devices/designing-for-tv
		ApplicationView::GetForCurrentView()->SetDesiredBoundsMode(ApplicationViewBoundsMode::UseCoreWindow);

		ResourceDictionary^ TenFootDictionary = ref new ResourceDictionary();
		TenFootDictionary->Source = ref new Uri("ms-appx:///styles/TenFootStylesheet.xaml");
		this->Resources->MergedDictionaries->Append(TenFootDictionary);
	}

	auto shell = dynamic_cast<AppShell^>(Window::Current->Content);

	// Do not repeat app initialization when the Window already has content,
	// just ensure that the window is active
	if (shell == nullptr)
	{
		// read Camera settings from local storage
		m_OnVifCameraViewModel->readDatafromLocalStorage(); 

		
		// Create a AppShell to act as the navigation context and navigate to the first page
		shell = ref new AppShell(this);

		shell->AppFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

		if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
		{
			// TODO: Restore the saved session state only when appropriate, scheduling the
			// final launch steps after the restore is complete

		}
	}

	// Place our app shell in the current Window
	Window::Current->Content = shell;

	if (shell->AppFrame->Content == nullptr)
	{
		// When the navigation stack isn't restored navigate to the first page,
		// suppressing the initial entrance animation and configuring the new
		// page by passing required information as a navigation parameter
	
		shell->AppFrame->Navigate(TypeName(IPStreamingCPP::MainPage::typeid), this, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo());
//		shell->AppFrame->Navigate(TypeName(IPStreamingCPP::Views::LandingPage::typeid), e->Arguments, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo());

	}

	// Ensure the current window is active
	Window::Current->Activate();
}
/*
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
#if _DEBUG
	// Show graphics profiling information while debugging.
	if (IsDebuggerPresent())
	{
		// Display the current frame rate counters
		DebugSettings->EnableFrameRateCounter = true;
	}
#endif


	auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

	// Do not repeat app initialization when the Window already has content,
	// just ensure that the window is active
	if (rootFrame == nullptr)
	{
		// Create a Frame to act as the navigation context and associate it with
		// a SuspensionManager key
		rootFrame = ref new Frame();

		rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

		if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
		{
			// TODO: Restore the saved session state only when appropriate, scheduling the
			// final launch steps after the restore is complete

		}

		if (e->PrelaunchActivated == false)
		{
			if (rootFrame->Content == nullptr)
			{
				// When the navigation stack isn't restored navigate to the first page,
				// configuring the new page by passing required information as a navigation
				// parameter
				rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
			}
			// Place the frame in the current Window
			Window::Current->Content = rootFrame;
			// Ensure the current window is active
			Window::Current->Activate();
		}
	}
	else
	{
		if (e->PrelaunchActivated == false)
		{
			if (rootFrame->Content == nullptr)
			{
				// When the navigation stack isn't restored navigate to the first page,
				// configuring the new page by passing required information as a navigation
				// parameter
				rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
			}
			// Ensure the current window is active
			Window::Current->Activate();
		}
	}
}
*/

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
    (void) sender;  // Unused parameter
    (void) e;   // Unused parameter

	auto deferral = e->SuspendingOperation->GetDeferral();


	if (m_OnVifCameraViewModel != nullptr) {
		m_OnVifCameraViewModel->writeDatatoLocalStorage();
		delete m_OnVifCameraViewModel;
		m_OnVifCameraViewModel = nullptr;
	}

	if (m_poutout != nullptr)
	{
		m_poutout->close();
		delete m_poutout;
		m_poutout = nullptr;
	}

	deferral->Complete();
    //TODO: Save application state and stop any background activity
}
/*
void App::UnhandledException(Object^ sender, UnhandledExceptionEventArgs^ e)
{
	(void)sender;  // Unused parameter
	(void)e;   // Unused parameter


	//TODO: Save application state and stop any background activity
}
*/
/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
    throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}