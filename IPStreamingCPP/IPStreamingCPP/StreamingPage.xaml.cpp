//
// StreamingPage.xaml.cpp
// Implementation of the StreamingPage class
//

#include "pch.h"
#include "StreamingPage.xaml.h"

#include "MainPage.xaml.h"
//#include "DataSources.h"



using namespace IPStreamingCPP;
using namespace FFmpegInteropExtRT;
using namespace RecordingListener;

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





//using namespace MediaPlayerDataSources;

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




// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

StreamingPage::StreamingPage()
{
	InitializeComponent();
//	ReadFromAppData();
	_datasources = nullptr;
	m_StreamingPageParam = nullptr;

}

StreamingPage::~StreamingPage()
{



}
void StreamingPage::ReadFromAppData()
{
	if (_datasources == nullptr) return;


	this->_ffmpegoutputresolution = safe_cast <ResolutionViewModel^>(_datasources->getDataSource("_ffmpegoutputresolution"));

	this->_ffmpegoutputfps = safe_cast <FpsViewModel^>(_datasources->getDataSource("_ffmpegoutputfps"));


	this->_outputMJpegresolution = safe_cast <ResolutionViewModel^>(_datasources->getDataSource("_outputMJpegresolution"));

	this->_outputMJpefps = safe_cast <FpsViewModel^>(_datasources->getDataSource("_outputMJpefps"));


	this->_outputMPegresolution = safe_cast <ResolutionViewModel^>(_datasources->getDataSource("_outputMPegresolution"));


	this->_outputMPegfps = safe_cast <FpsViewModel^>(_datasources->getDataSource("_outputMPegfps"));

	this->_inputSourceUri = safe_cast <inputSourceViewModel^>(_datasources->getDataSource("_inputSourceUri"));


	this->_ffmpegoutputformat = safe_cast <OutputFormatViewModel^>(_datasources->getDataSource("_ffmpegoutputformat"));


	this->_toggleSwitchOutPutMpegStream = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_toggleSwitchOutPutMpegStream"));

	this->_toggleSwitchOutPutMJpegStream = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_toggleSwitchOutPutMJpegStream"));
	this->_toggleSwitchOutPutMuxer = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_toggleSwitchOutPutMuxer"));

	
	this->_toggleSwitchVideoDecode = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_toggleSwitchVideoDecode"));
	this->_toggleSwitchAudioDecode = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_toggleSwitchAudioDecode"));

	this->_outputMpegStreamUrl = safe_cast <ItemStringViewModel^>(_datasources->getDataSource("_outputMpegStreamUrl"));

	this->_outputMJpegStreamUrlPort = safe_cast <ItemStringViewModel^>(_datasources->getDataSource("_outputMJpegStreamUrlPort"));

	this->_ffmpegoutDeleteOlderFiles = safe_cast <HourViewModel^>(_datasources->getDataSource("_ffmpegoutDeleteOlderFiles"));
	this->_ffmpegoutRecordingHours = safe_cast <HourViewModel^>(_datasources->getDataSource("_ffmpegoutRecordingHours"));

	this->_inputUri = safe_cast <ItemStringViewModel^>(_datasources->getDataSource("_inputUri"));



	this->_toggleSwitchMovementWatcher = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_toggleSwitchMovementWatcher"));;

	this->_toggleSwitchMuxCopyInput = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_toggleSwitchMuxCopyInput"));;

	this->_HostNameMovementWatcher = safe_cast <ItemStringViewModel^>(_datasources->getDataSource("_HostNameMovementWatcher"));;

	this->_PortMovementWatcher= safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_PortMovementWatcher"));;

	this->_InputPin1MovementWatcher= safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_InputPin1MovementWatcher"));;

	this->_InputPin1MovementWatcherActiv = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_InputPin1MovementWatcherActiv"));;

	this->_RecordingOnMovement = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_RecordingOnMovement"));;

	this->_CameraEvents = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_CameraEvents"));;


	this->_MovementRecordingTimeSecs = safe_cast <HourViewModel^>(_datasources->getDataSource("_MovementRecordingTimeSecs"));


	

	this->outputMpegStreamUrl->Text = L"Willi-PC:3000";
	this->_localHostName = L"localhost";

	auto hostNames = NetworkInformation::GetHostNames();
	for (unsigned int i = 0; i < hostNames->Size; i++) {
		HostName^ host = hostNames->GetAt(i);
		//		auto hostName = hostNames->GetAt(i)->ToString();
		if ((host->Type == HostNameType::DomainName)) {
			auto ss = host->DisplayName;
			this->_localHostName = ss;
			break;
		}


	}

	this->OutputfileFolder->Text = ApplicationData::Current->LocalFolder->Path;


	if (this->_outputMJpegStreamUrlPort->Value->IsEmpty())
	{
		this->_outputMJpegStreamUrlPort->Value = this->_localHostName + ":8000";
	}
	if (this->_outputMpegStreamUrl->Value->IsEmpty())
	{
		this->_outputMpegStreamUrl->Value = L"Willi-PC:3000";
	}


	//this->outputMJpegStreamUrlPort->Text = this->_localHostName + ":8000";


}
void StreamingPage::WriteToAppData()
{
	if (this->_datasources != nullptr) {
		this->_datasources->writeSettingsToLocalStorage();
	}


}

void StreamingPage::OnPropertyChanged(Platform::String^ info)
{
	
	PropertyChanged(this, ref new PropertyChangedEventArgs(info));


	
}

void StreamingPage::OnLoaded(Object ^sender, RoutedEventArgs ^e)
{
}

void StreamingPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	StreamingPageParam^ streamingParam = safe_cast<StreamingPageParam^>(e->Parameter);
	if (streamingParam != nullptr) {

		m_StreamingPageParam = streamingParam;
		this->_datasources = m_StreamingPageParam->DataSources;
		ReadFromAppData();

	}
	else
	{
		this->_datasources = nullptr;
		m_StreamingPageParam = nullptr;
	}


	Page::OnNavigatedTo(e);
}

void StreamingPage::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{
	// Handling of this event is included for completeness, as it will only fire when navigating between pages and this sample only includes one page

	Page::OnNavigatingFrom(e);

}

void StreamingPage::inputURIBoxKeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{

	String^ uri = safe_cast<TextBox^>(sender)->Text;
	//	uri = L"rtsp://admin:willi@1AMC000UY_89WJ0B:554/cam/realmonitor?channel=1&subtype=0";

	// Only respond when the text box is not empty and after Enter key is pressed
	if (e->Key == Windows::System::VirtualKey::Enter && !uri->IsEmpty())
	{
		// srw2ho, 10.04.2020, overtake input
		IPStreamingCPP::inputSource^ inpSource = this->_inputSourceUri->geSelectedInputSource();
		if (inpSource != nullptr) {
			inpSource->InputsourceUri =this->inpurturl->Text;
		}
		starturiRecording();
		e->Handled = true;
	}

}


void StreamingPage::starturiRecording()
{
	Inpurturl->Value = this->inpurturl->Text; // Set Uri

	this->startUriStreaming(this, this->m_StreamingPageParam); // Callback to MediaPlayder Source


}


void StreamingPage::stopRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

		this->stopStreaming(this, this->m_StreamingPageParam); // Callback to MediaPlayder Source

}


void StreamingPage::startRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	starturiRecording();
	//this->inpurturl->Text

}

void StreamingPage::OpenLocalFile(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	FileOpenPicker^ filePicker = ref new FileOpenPicker();
	filePicker->ViewMode = PickerViewMode::Thumbnail;
	filePicker->SuggestedStartLocation = PickerLocationId::VideosLibrary;
	filePicker->FileTypeFilter->Append("*");

	// Show file picker so user can select a file
	create_task(filePicker->PickSingleFileAsync()).then([this](StorageFile^ file)
	{
		if (file != nullptr)
		{

			this->stopStreaming(this, this->m_StreamingPageParam); // Callback to MediaPlayder Source
			create_task(file->OpenAsync(FileAccessMode::Read)).then([this, file](task<IRandomAccessStream^> stream)
			{
				try
				{
		
					IRandomAccessStream^ readStream = stream.get();
					this->m_StreamingPageParam->Stream = readStream;

					this->startFileStreaming(this, this->m_StreamingPageParam); // Callback to MediaPlayder Source
	
				}
				catch (COMException^ ex)
				{
					DisplayErrorMessage(ex->Message);
				}
			});
		}
	});
}
/*
void StreamingPage::MediaFailed(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e)
{
	DisplayErrorMessage(e->ErrorMessage);
}
*/

void StreamingPage::DisplayErrorMessage(Platform::String^ message)
{
	// Display error message
	auto errorDialog = ref new MessageDialog(message);
	errorDialog->ShowAsync();
}


void StreamingPage::outputMpegStreamUrlKeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	String^ outputMpegStreamUrl = safe_cast<TextBox^>(sender)->Text;
	//	uri = L"rtsp://admin:willi@1AMC000UY_89WJ0B:554/cam/realmonitor?channel=1&subtype=0";

	// Only respond when the text box is not empty and after Enter key is pressed
	if (e->Key == Windows::System::VirtualKey::Enter && !outputMpegStreamUrl->IsEmpty())
	{
		std::wstring wstr = outputMpegStreamUrl->Data();
		
		std::vector<std::wstring> arr = this->m_StreamingPageParam->splitintoArray(wstr, L":");
		if (arr.size() > 1) {
			int port = _wtoi(arr[1].c_str());
		}
		// Mark event as handled to prevent duplicate event to re-triggered
		e->Handled = true;

	}
}

void StreamingPage::outputMJpegStreamUrlPortKeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	String^ outputMpegStreamPort = safe_cast<TextBox^>(sender)->Text;
	//	uri = L"rtsp://admin:willi@1AMC000UY_89WJ0B:554/cam/realmonitor?channel=1&subtype=0";

	// Only respond when the text box is not empty and after Enter key is pressed
	if (e->Key == Windows::System::VirtualKey::Enter && !outputMpegStreamPort->IsEmpty())
	{
		// Mark event as handled to prevent duplicate event to re-triggered
		e->Handled = true;

	}
}

