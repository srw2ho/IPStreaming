#include "pch.h"


//#include "DataSources.h"

//#include "StreamingPageParam.h"

#include "MainPage.xaml.h"

using namespace IPStreamingCPP;
using namespace FFmpegInteropExtRT;

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



StreamingPageParam::StreamingPageParam()
{
	m_mediaElement = nullptr;
	m_FFmpegMSS = nullptr;
	m_pCameraServer = nullptr;
	m_datasources = nullptr;
	m_restartStreamingTimer = nullptr;
	m_stream = nullptr;
	m_DataSourceparam = nullptr;
	m_MainFrame = nullptr;
	m_OnVifCamera = nullptr;
//	m_ScenarioFrame = nullptr;
	m_ScenarioViewControl = nullptr;
//	m_SplitView = nullptr;
	m_Muxerconfigoptions = ref new Windows::Foundation::Collections::PropertySet();
	m_Recording = nullptr;

}

StreamingPageParam::~StreamingPageParam()
{
	
	if (m_FFmpegMSS != nullptr) {
		m_FFmpegMSS->DestroyFFmpegAsync();
		//delete m_FFmpegMSS;
		m_FFmpegMSS = nullptr;
	}
	if (m_pCameraServer != nullptr) {
		delete m_pCameraServer;
	}
	
	if (m_Recording != nullptr)
	{
		delete m_Recording;
	}
	
	/*
	if (m_datasources != nullptr) {
		delete m_datasources;
	}
	*/

	
}

/*
void StreamingPageParam::NotifyPropertyChanged(Platform::String^ prop)
{

	PropertyChanged(this, ref new PropertyChangedEventArgs(prop));

}
*/



void StreamingPageParam::startMovementRecording(Windows::Foundation::Collections::PropertySet^ inputconfigoptions) {

	if (m_Recording != nullptr) {
	//	m_OnStartMovementStreaming = m_Recording->startStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object ^, Windows::Networking::Sockets::StreamSocket ^>(this, &IPStreamingCPP::StreamingPageParam::OnstartMovementStreaming);
	//	m_OnStopMovementStreaming = m_Recording->stopStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object ^, Platform::String ^>(this, &IPStreamingCPP::StreamingPageParam::OnstopMovementStreaming);

		m_Recording->startProcessingPackagesAsync(inputconfigoptions, this->m_Muxerconfigoptions);


	}
}		
void StreamingPageParam::stopMovementRecording() {



	m_Recording->stopProcessingPackagesAsync();
	

}


 Platform::String ^ StreamingPageParam::VisibleKeyName::get()
 {
		wchar_t buffer[200];
		swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s(%s)", this->m_KeyName->Data(), this->m_OnVifCamera->CameraIPAdress->Data());
		return ref new String(buffer);
}

void StreamingPageParam::ClearRessources()
{
	this->m_OnVifCamera->PropertyChanged -= m_PropertyChangedEventRegister;
	
	if (this->m_mediaElement != nullptr) {
		this->m_mediaElement->CurrentStateChanged -= m_MediaCurrentStateChangedRegister;
		this->m_mediaElement->MediaFailed -= m_MediaFailedRegister;
	}

	if (m_pCameraServer != nullptr) {

		this->m_pCameraServer->DestroyHTTPServerAsync();// Listener wird destroyed
		this->m_pCameraServer->Failed -= m_CameraServerFailedRegister;
	}

	if (m_Recording != nullptr) {
		m_Recording->stopStreaming -= m_OnStopMovementStreaming;
		m_Recording->startStreaming -= m_OnStartMovementStreaming;
		m_Recording->ChangeMovement -= m_OnChangeMovementStreaming;
		stopMovementRecording();

	}




}

StreamingPageParam ^ StreamingPageParam::createStreamingPageParam(Platform::String^ key, Frame^  Main)
{

	KeyName = key;
	m_FFmpegMSS = nullptr;

	m_pCameraServer = ref new FFmpegInteropExtRT::CameraServer();
	

	m_datasources = ref new IPStreamingCPP::DataSources(key);
	m_restartStreamingTimer = nullptr;
	m_DataSourceparam = ref new IPStreamingCPP::DataSourceparam();
	this->ReadFromAppData();
	m_MainFrame = Main;
	m_stream = nullptr;
//	m_SplitView = nullptr;
	m_Recording = ref new RecordingListener::Recording();
	return this;
}


bool StreamingPageParam::startUriStreaming()
{


	//StreamingCPP::DataSources ^ _datasources = safe_cast<IPStreamingCPP::DataSources^>(this->DataSources);
	m_DataSourceparam->SetParams(this->DataSources);

	//	StreamingPage ^ streamingpage = (StreamingPage ^)sender;
	if (this->DataSources != nullptr) {

		clearRecording();
		String^ uri = m_DataSourceparam->_inputUri->Value;
		// Read toggle switches states and use them to setup FFmpeg MSS
		bool forceDecodeAudio = m_DataSourceparam->_toggleSwitchAudioDecode->ValueasBool;
		bool forceDecodeVideo = m_DataSourceparam->_toggleSwitchVideoDecode->ValueasBool;

		// Set FFmpeg specific options. List of options can be found in https://www.ffmpeg.org/ffmpeg-protocols.html
		PropertySet^ options = ref new PropertySet();
		bool rtspflag = false;
		bool rtsliveStream = true;
		// Below are some sample options that you can set to configure RTSP streaming
		std::vector<std::wstring> arr = splitintoArray(uri->Data(), L":");
		if ((arr.size() > 0) && ((arr[0] == L"rtsp") || (arr[0] == L"rtp") || (arr[0] == L"rtmp"))) {
			rtspflag = true;
		}



		inputSource ^ inpSource = m_DataSourceparam->_inputSourceUri->getInputSourceBySourceUri(uri);
		if (inpSource != nullptr) {
			rtsliveStream = inpSource->LifeStream;

		}
		else rtsliveStream = false;
		//streamingpage->inputprobesize->IsEnabled = (inpSource != nullptr);
		//streamingpage->inputanalyzeduration->IsEnabled = (inpSource != nullptr);


		if (rtspflag) {// live-View
			options->Insert("rtsp_transport", "tcp");
			//		options->Insert("video_size", "640x480"); // does not function
			//		options->Insert("framerate", "10"); //does not function
			if (rtsliveStream) {
				int probesize = inpSource->FFmpegProbeSize;
				int analyzeduration = inpSource->FFmpegAnalyzeduration;
				options->Insert("analyzeduration", analyzeduration); // 5000000 default, now 50 ms
				options->Insert("probesize", probesize); // 5000000 defaut, smallest value = 32

		
				if (inpSource->InputVideo->VideoEnabled && !inpSource->InputAudio->AudioEnabled)
				{
					options->Insert("allowed_media_types", "video"); //  allow only video media type
				}
				else if (!inpSource->InputVideo->VideoEnabled && inpSource->InputAudio->AudioEnabled)
				{
					options->Insert("allowed_media_types", "audio"); //  allow only video media type
				}
				// sonst alles zulassen
	
				

				options->Insert("fflags", "nobuffer"); //  effect: no delay 
													   //			options->Insert("flags2", "+export_mvs"); // for movement vectors
													   //			av_dict_set(&opts, "flags2", "+export_mvs", 0);

													   //		options->Insert("tune", "zerolatency"); // 5000000 default, now 50 ms
													   //		options->Insert("max_delay", 200); // 5000000 defaut, smallest value = 32
				options->Insert("reorder_queue_size", 1);
				//		options->Insert("packet-buffering", 0);

			}

		}




		m_FFmpegMSS = FFmpegInteropMSS::CreateFFmpegInteropMSS();
		m_FFmpegMSS->startStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object ^, FFmpegInteropMSSInputParamArgs^>(this, &StreamingPageParam::OnstartStreaming); // callback to
	
		if (m_MainFrame != nullptr)
		{
			m_MainFrame->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this, inpSource, uri, options, forceDecodeAudio, forceDecodeVideo]() {
//				m_FFmpegMSS = m_FFmpegMSS->StartFFmpegInteropMSSFromStream(this->Stream, forceDecodeAudio, forceDecodeVideo, nullptr);
				m_FFmpegMSS = m_FFmpegMSS->StartFFmpegInteropMSSFromUri(uri, forceDecodeAudio, forceDecodeVideo, options);


				if (m_FFmpegMSS != nullptr)
				{
					MediaStreamSource^ mss = m_FFmpegMSS->GetMediaStreamSource();

					if (mss)
					{
						// Pass MediaStreamSource to Media Element
						m_mediaElement->SetMediaStreamSource(mss);
						restartStreamingTimer(inpSource); // Restart Start Streaming after time in adjustments
						// Close control panel after file open
						//	Splitter->IsPaneOpen = false;
						//				return true;
					}
					else
					{
						this->DisplayErrorMessage("Cannot open media","startUriStreaming");
					}
				}
				else
				{
					this->DisplayErrorMessage("Cannot open media","startUriStreaming");
				}
			}));

		}

	}

	return true;
}

void StreamingPageParam::DisplayErrorMessage(Platform::String^ title, Platform::String^ message)
{
	// Display error message
	auto errorDialog = ref new MessageDialog(message, title);
	errorDialog->ShowAsync();
}

bool StreamingPageParam::startFileStreaming()
{


	//StreamingCPP::DataSources ^ _datasources = safe_cast<IPStreamingCPP::DataSources^>(this->DataSources);
	m_DataSourceparam->SetParams(this->DataSources);

	//	StreamingPage ^ streamingpage = (StreamingPage ^)sender;
	if (this->DataSources != nullptr) {

		bool forceDecodeAudio = m_DataSourceparam->_toggleSwitchAudioDecode->ValueasBool;
		bool forceDecodeVideo = m_DataSourceparam->_toggleSwitchVideoDecode->ValueasBool;

		m_FFmpegMSS = FFmpegInteropMSS::CreateFFmpegInteropMSS();
		m_FFmpegMSS->startStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object ^, FFmpegInteropMSSInputParamArgs^ >(this, &StreamingPageParam::OnstartStreaming);

		if (m_MainFrame != nullptr)
		{
			m_MainFrame->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this, forceDecodeAudio, forceDecodeVideo]() {
				m_FFmpegMSS = m_FFmpegMSS->StartFFmpegInteropMSSFromStream(this->Stream, forceDecodeAudio, forceDecodeVideo, nullptr);
				if (m_FFmpegMSS != nullptr)
				{
					MediaStreamSource^ mss = m_FFmpegMSS->GetMediaStreamSource();

					if (mss)
					{
						// Pass MediaStreamSource to Media Element
						m_mediaElement->SetMediaStreamSource(mss);

					}
					else
					{
						this->DisplayErrorMessage("Cannot open media","startFileStreaming");
					}
				}
				else
				{
					this->DisplayErrorMessage("Cannot open media", "startFileStreaming");
				}
			}));

		}

	}
	return true;
	
}
bool StreamingPageParam::stopStreaming()
{

	this->clearRecording();
	return true;
	

}


void StreamingPageParam::OnstartStreaming(Platform::Object ^sender, FFmpegInteropMSSInputParamArgs^ args)
{
	FFmpegInteropMSS ^ _senderFFmpegMSS = (FFmpegInteropMSS ^)sender;
	FFmpegInteropMSS ^ _FFmpegMSS = (FFmpegInteropMSS ^)this->getFFmpegInteropMSS();

	if (_senderFFmpegMSS != _FFmpegMSS) {


	}


	if (_FFmpegMSS != nullptr) {

//IPStreamingCPP::DataSources ^ _datasources = safe_cast<IPStreamingCPP::DataSources^>(this->DataSources);
		m_DataSourceparam->SetParams(this->DataSources);

		//	StreamingPage ^ streamingpage = (StreamingPage ^)sender;
		if (this->DataSources != nullptr) {


			App ^ app = (App ^)Application::Current;
			app->WriteLogMessage("Restart ffmpeg Streaming \n"); // Restart Streaming

																 //	_pIPServer->closeAllConnections();
																 //return;
			double inputfps = 0;
			int inputheight = 0;
			int intputwidth = 0;

			if (args != nullptr)// parameter coming from input stream
			{
				inputfps = args->Fps;
				inputheight = args->Height;
				intputwidth = args->Width;
			}


			bool doClientEncoding = false;
			if (m_DataSourceparam->_toggleSwitchOutPutMJpegStream->ValueasBool) {
				if (this->m_pCameraServer->Port == -1)
				{
					int port = 8000;
					std::wstring wstr = m_DataSourceparam->_outputMJpegStreamUrlPort->Value->Data();
					std::vector<std::wstring> arr = splitintoArray(wstr, L":");
					if (arr.size() > 1) {
						port = _wtoi(arr[1].c_str());
					}
					m_pCameraServer->CreateHTTPServerAsync(port);
				}
				doClientEncoding = true;
				m_pCameraServer->OpenHTTPServerAsync();


				int fps = -1;
				int height = 480;
				int width = 640;
				int64_t bit_rate = 800000;

				Fps^ selectedfps = m_DataSourceparam->_outputMJpefps->getSelectedFps();
				if (selectedfps != nullptr)fps = selectedfps->FPS;
				Resolution^ selectedResolution = m_DataSourceparam->_outputMJpegresolution->getSelectedResolution();
				if (selectedResolution != nullptr) {
					height = selectedResolution->Height;
					width = selectedResolution->Width;
				}
				bit_rate = CalculateBitrate(inputfps, fps, height, width, 1);
				PropertySet^ configoptions = ref new PropertySet();
				configoptions->Insert("m_fps", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(fps))); configoptions->Insert("m_height", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(height)));
				configoptions->Insert("m_width", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(width))); configoptions->Insert("m_bit_rate", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt64(bit_rate)));

				_FFmpegMSS->PrepareHttpMJpegClientEncoding(m_pCameraServer, "Camera_1_Http", configoptions);
			}
			else
			{
				m_pCameraServer->CloseHTTPServerAsync();
			}
			if (m_DataSourceparam->_toggleSwitchOutPutMpegStream->ValueasBool) {
				std::wstring wstr = m_DataSourceparam->_outputMpegStreamUrl->Value->Data();
				std::vector<std::wstring> arr = splitintoArray(wstr, L":");
				Platform::String^ streamhost = "";
				int port = 3000;
				if (arr.size() > 1) {
					port = _wtoi(arr[1].c_str());
				}
				if (arr.size() > 1) {
					std::wstring wstr = std::wstring(arr[0].begin(), arr[0].end());
					streamhost = ref new Platform::String(wstr.c_str());
				}
				if (streamhost->Length() > 0)
				{
					m_pCameraServer->CreateHTTPClientAsync(streamhost, port);
					doClientEncoding = true;
				}
				int fps = -1; // take input stream fps
				int height = 480;
				int width = 640;
				int64_t bit_rate = 800000;


				Fps^ selectedfps = m_DataSourceparam->_outputMPegfps->getSelectedFps();
				if (selectedfps != nullptr)fps = selectedfps->FPS;
				Resolution^ selectedResolution = m_DataSourceparam->_outputMPegresolution->getSelectedResolution();
				if (selectedResolution != nullptr) {
					height = selectedResolution->Height;
					width = selectedResolution->Width;
				}
				bit_rate = CalculateBitrate(inputfps, fps, height, width, 1);
				
				PropertySet^ configoptions = ref new PropertySet();

				configoptions->Insert("m_fps", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(fps))); configoptions->Insert("m_height", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(height)));
				configoptions->Insert("m_width", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(width))); configoptions->Insert("m_bit_rate", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt64(bit_rate)));
				_FFmpegMSS->PrepareHttpMpegClientEncoding(m_pCameraServer, "Camera_1_Http", configoptions);

			}
			else
			{
				m_pCameraServer->CloseHTTPClientAsync();
			}

			if (m_DataSourceparam->_toggleSwitchOutPutMuxer->ValueasBool) {
				String ^ folderPath = ApplicationData::Current->LocalFolder->Path;
				int fps = -1; // take input stream fps
				int height = 600;
				int width = 800;
				int64_t bit_rate = 800000;
				double deletefilesOlderFiles = 48; //delte older files after 48 h 
				double RecordingInHours = 1; // recording 1 hour 
				Platform::String^  outputFormat = L"mp4";
				Fps^ selectedfps = m_DataSourceparam->_ffmpegoutputfps->getSelectedFps();
				if (selectedfps != nullptr)fps = selectedfps->FPS;
				Resolution^ selectedResolution = m_DataSourceparam->_ffmpegoutputresolution->getSelectedResolution();
				if (selectedResolution != nullptr) {
					height = selectedResolution->Height;
					width = selectedResolution->Width;
				}
				bit_rate = CalculateBitrate(inputfps, fps, height, width, 1);
				ItemValue^ selectedRecordingHopurs = m_DataSourceparam->_ffmpegoutRecordingHours->getSelectedHours();
				ItemValue^ selectedDeleteOlderFiles = m_DataSourceparam->_ffmpegoutDeleteOlderFiles->getSelectedHours();
				outPutFormat^ outformat = m_DataSourceparam->_ffmpegoutputformat->getSelectedOutPutFormat();
				if (selectedRecordingHopurs != nullptr)RecordingInHours = selectedRecordingHopurs->Value;
				if (selectedDeleteOlderFiles != nullptr)deletefilesOlderFiles = selectedDeleteOlderFiles->Value;
				if (outformat != nullptr)outputFormat = outformat->Format;
				PropertySet^ outputoptions = ref new PropertySet();
				//	outputoptions->Insert("bufsize", "2000000");
			//	PropertySet^ configoptions = ref new PropertySet();

				m_Muxerconfigoptions->Insert("m_fps", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(fps))); m_Muxerconfigoptions->Insert("m_height", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(height)));
				m_Muxerconfigoptions->Insert("m_width", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(width))); m_Muxerconfigoptions->Insert("m_bit_rate", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt64(bit_rate)));
			
				m_Muxerconfigoptions->Insert("m_strFolder", dynamic_cast<PropertyValue^>(PropertyValue::CreateString(folderPath))); m_Muxerconfigoptions->Insert("m_outputformat", dynamic_cast<PropertyValue^>(PropertyValue::CreateString(outputFormat)));
				m_Muxerconfigoptions->Insert("m_outputFormat", dynamic_cast<PropertyValue^>(PropertyValue::CreateString(outputFormat)) ); m_Muxerconfigoptions->Insert("m_deletefilesOlderFilesinHours", dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(deletefilesOlderFiles)));
				m_Muxerconfigoptions->Insert("m_RecordingInHours", dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(RecordingInHours)));

				bool CopyInput = m_DataSourceparam->_toggleSwitchMuxCopyInput->ValueasBool;
				m_Muxerconfigoptions->Insert("m_MuxCopyInput", dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(CopyInput)));

				ItemValue^ selectedRecordingActivTimeinSec = m_DataSourceparam->_MovementRecordingTimeSecs->getSelectedHours();

				double RecordingActivTimeinSec = selectedRecordingActivTimeinSec->Value;
				m_Muxerconfigoptions->Insert("m_RecordingActivTimeinSec", dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(RecordingActivTimeinSec)));
				bool recordingOnMovement = m_DataSourceparam->_RecordingOnMovement->ValueasBool;

				bool MovementActiv = m_DataSourceparam->_toggleSwitchMovementWatcher->ValueasBool;
				if (MovementActiv) {
					PropertySet^ inputOptions = ref new PropertySet();
					inputOptions->Insert("HostName", dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_DataSourceparam->_HostNameMovementWatcher->Value)));
					inputOptions->Insert("Port", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(m_DataSourceparam->_PortMovementWatcher->Value)));
					inputOptions->Insert("GPIOTyp.input_1", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(m_DataSourceparam->_InputPin1MovementWatcher->Value)));
					inputOptions->Insert("GPIOTyp.input_1.Activ", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(m_DataSourceparam->_InputPin1MovementWatcherActiv->Value)));
					this->startMovementRecording(inputOptions);
				}

				if (recordingOnMovement)
					m_Muxerconfigoptions->Insert("m_MovementActivated", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(1)));
				else {
					m_Muxerconfigoptions->Insert("m_MovementActivated", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(-1)));
					//RecordingInHours = 72;
					//m_Muxerconfigoptions->Insert("m_RecordingInHours", dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(RecordingInHours)));
				}


				m_Muxerconfigoptions->Insert("m_MovementActiv", dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(MovementActiv)));

				_FFmpegMSS->PrepareFFMPegOutPutEncoding(this->KeyName, m_Muxerconfigoptions, outputoptions);
				
				// only for testing
				/*
				TimeSpan delay;
				delay.Duration = RecordingActivTimeinSec * 10000000;

				ThreadPoolTimer^timer = ThreadPoolTimer::CreatePeriodicTimer(ref new TimerElapsedHandler([this](ThreadPoolTimer^  source)
				{
					bool isActiv;
					Object^ value = m_Muxerconfigoptions->Lookup("m_MovementActiv");
					if (value != nullptr)
					{
						isActiv = safe_cast<IPropertyValue^>(value)->GetBoolean();
					}
					else isActiv = false;
				
	
					isActiv = !isActiv;
					m_Muxerconfigoptions->Insert("m_MovementActiv", dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(isActiv)));


				}), delay);
				// only for testing
				*/
//
		
			}
		}


	}
}

void StreamingPageParam::restartStreamingTimer(inputSource ^ inpSource)
{

	if (inpSource == nullptr) return;
	if (!inpSource->LifeStream) return;


	double timedelayinhour = inpSource->RestartStreamingTimeinHour;
	if (timedelayinhour == -1) return; // Restart Function is disabled
	double deltatoFullHour = 0;
	if (timedelayinhour >= 1) {
		// beim ersten Starten wird die Restart-Zeit auf eine volle Stunde erhöht
		SYSTEMTIME st;
		FILETIME ft;
		FILETIME ftend;
		LARGE_INTEGER fttime, ftendtime;

		//DateTimeToSystemTime
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &ft);

		WORD minMinute;
		bool doDeltaTime = true;
		WORD maxMinute;

		minMinute = 29; // recording starts always in half hour steps
		maxMinute = 30;
	
		if ((st.wMinute == maxMinute) && (st.wSecond <= 1) || (st.wMinute == minMinute) && (st.wSecond >= 59)) // 2 sec tolerance for whole hour
		{
			doDeltaTime = false;
		}
		
		if (doDeltaTime) {
			st.wMilliseconds = 0;
			st.wMinute = 30; // round to half hour, because all full hour a recording file are new created
			st.wSecond = 0;

			SystemTimeToFileTime(&st, &ftend);

			fttime.LowPart = ft.dwLowDateTime;
			fttime.HighPart = ft.dwHighDateTime;

			ftendtime.LowPart = ftend.dwLowDateTime;
			ftendtime.HighPart = ftend.dwHighDateTime;

			LARGE_INTEGER deltatime;
			deltatime.QuadPart = ftendtime.QuadPart - fttime.QuadPart;
			deltatoFullHour = double(deltatime.QuadPart);
		}
	}
	TimeSpan delay;
	delay.Duration = timedelayinhour * 60 * 60 * 10000000 + deltatoFullHour;


	// for testing 	delay.Duration = 10000000*20;

	m_restartStreamingTimer = ThreadPoolTimer::CreateTimer(ref new TimerElapsedHandler([this](ThreadPoolTimer^  source)
	{
		if (m_MainFrame != nullptr)
		{
			m_MainFrame->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this]() {
				this->startUriStreaming();
			}));

		}

	}), delay);

}
void StreamingPageParam::WriteToAppData()
{
	if (this->m_datasources != nullptr) {
		this->m_datasources->writeSettingsToLocalStorage();
	}
}




void StreamingPageParam::takeParametersFromCamera()
{
	m_DataSourceparam->SetParams(this->DataSources);

	if (this->OnVifCamera->ProfileEncodings->Size == 0) return;

	inputSourceViewModel^ uris = this->m_DataSourceparam->_inputSourceUri;
	if (uris == nullptr) return;
	inputSource^ source;
	Platform::Collections::Vector<inputSource^>^ foundedItems = ref new Platform::Collections::Vector<inputSource^>();

	for each (auto var in this->OnVifCamera->ProfileEncodings) {

		if (var->StreamUrlOrigin->IsEmpty()) continue;

		source = uris->getInputSourceByInputKey(var->StreamUrlOrigin); // Origin Url used as key for input source
		
		if (source == nullptr)
		{// OverWrite
			source = ref new inputSource(var->Name, this->OnVifCamera->CameraIPAdress, 0, this->OnVifCamera->User, this->OnVifCamera->Password);
			uris->Items->Append(source);
		}
		if (source != nullptr) {
			foundedItems->Append(source); // not delete
			IMap<String^, String^> ^ cameradata = this->OnVifCamera->CameraData; // get Camera-Data
			if (cameradata->HasKey("FirmwareVersion")) source->CameraVersion = cameradata->Lookup("FirmwareVersion");
			if (cameradata->HasKey("HardwareId")) source->HardwareId = cameradata->Lookup("HardwareId");
			if (cameradata->HasKey("Manufacturer")) source->CameraManufactorer = cameradata->Lookup("Manufacturer");
			if (cameradata->HasKey("Model")) source->CameraModel = cameradata->Lookup("Model");
			if (cameradata->HasKey("SerialNumber")) source->SerialNumber = cameradata->Lookup("SerialNumber");

			source->InputsourceUri = var->StreamUrl;
			source->InputKey = var->StreamUrlOrigin; // origin Url used as Key for refinding
			source->HostName = this->OnVifCamera->CameraIPAdress;
			source->InputVideo->Bitrate = var->VideoProfileEncoding->BitrateLimit;
			source->InputVideo->FPS = var->VideoProfileEncoding->FrameRateLimit;
			source->InputVideo->Resolution->Height = var->VideoProfileEncoding->Resolution->Height;
			source->InputVideo->Resolution->Width = var->VideoProfileEncoding->Resolution->Width;

			source->InputAudio->BitRate = var->AudioProfileEncoding->Bitrate;
			source->InputAudio->SampleRate = var->AudioProfileEncoding->SampleRate;

		}

	}

	if (foundedItems->Size == 0) return;

	unsigned int Idx = 0;


	while ((source = uris->getInputSourceByIdx(Idx)) != nullptr)
	{
		bool toDelete = true;
		for each (auto var in foundedItems)
		{
			if (source == var) {
				toDelete = false;
				break;
			}
		}

		if (toDelete) {
			uris->deleteInputSource(source);
		}
		else {
			Idx++;
		}

	}


}


void StreamingPageParam::clearRecording()
{
	stopMovementRecording();

	if (m_FFmpegMSS != nullptr)
	{
		m_mediaElement->Stop();
		MediaStreamSource^ mss = m_FFmpegMSS->GetMediaStreamSource();
		if (mss != nullptr) {
			m_mediaElement->Source = nullptr;
// wsc in this case no changings are fired			m_mediaElement->ClearValue(m_mediaElement->SourceProperty);
		}
		m_FFmpegMSS->DestroyFFmpegAsync();
	//	delete m_FFmpegMSS;
		m_FFmpegMSS = nullptr;
	}

	if (m_restartStreamingTimer != nullptr) {

		m_restartStreamingTimer->Cancel();
		m_restartStreamingTimer = nullptr;
	}


}

int64_t StreamingPageParam::CalculateBitrate(double inputfps, int fps, int height, int width, int motion) //motion: 1:low, 2:medium 4:hight
{
	double calcfps;
	if (fps == -1) {
		calcfps = inputfps;
	}
	else calcfps = fps;

	//(921, 600 x 5) x 1 x 0.07 = 322, 560 bps / 1000 = 322 kbps bitrate
	//	int64_t bitrate = (double)height*width*calcfps*motion*0.1;
	//	int64_t bitrate = (double)height*width*calcfps*motion*0.09;
	//	int64_t bitrate = (double)height*width*calcfps*motion*0.08;
	int64_t bitrate = (int64_t) (double)height*width*calcfps*motion*0.07;
	//	int64_t bitrate = (double)height*width*calcfps*motion*0.05;

	while (bitrate > 0) {
		if (((int64_t)bitrate % 16) != 0) {
			bitrate--;
		}
		else break;

	}

	//	if (bitrate == 0)bitrate = 800000;
	if (bitrate <  800000)bitrate = 800000;

	return (int64_t)bitrate;

}

void StreamingPageParam::ReadFromAppData()
{
	IPStreamingCPP::DataSources^ _datasources = this->DataSources;

	ResolutionViewModel^ _ffmpegoutputresolution = ref new ResolutionViewModel("_ffmpegoutputresolution");
	FpsViewModel^ _ffmpegoutputfps = ref new FpsViewModel("_ffmpegoutputfps");


	ResolutionViewModel^ _outputMJpegresolution = ref new ResolutionViewModel("_outputMJpegresolution");
	FpsViewModel^ _outputMJpefps = ref new FpsViewModel("_outputMJpefps");

	ResolutionViewModel^_outputMPegresolution = ref new ResolutionViewModel("_outputMPegresolution");
	FpsViewModel^_outputMPegfps = ref new FpsViewModel("_outputMPegfps");

	inputSourceViewModel^ _inputSourceUri = ref new inputSourceViewModel("_inputSourceUri");

	OutputFormatViewModel^ _ffmpegoutputformat = ref new OutputFormatViewModel("_ffmpegoutputformat");

	ItemValueViewModel^ _toggleSwitchOutPutMpegStream = ref new ItemValueViewModel("_toggleSwitchOutPutMpegStream");
	ItemValueViewModel^ _toggleSwitchOutPutMJpegStream = ref new ItemValueViewModel("_toggleSwitchOutPutMJpegStream");
	ItemValueViewModel^ _toggleSwitchOutPutMuxer = ref new ItemValueViewModel("_toggleSwitchOutPutMuxer");
	ItemValueViewModel^ _toggleSwitchVideoDecode = ref new ItemValueViewModel("_toggleSwitchVideoDecode");
	ItemValueViewModel^ _toggleSwitchAudioDecode = ref new ItemValueViewModel("_toggleSwitchAudioDecode");



	ItemStringViewModel^ _outputMpegStreamUrl = ref new ItemStringViewModel("_outputMpegStreamUrl");
	ItemStringViewModel^_outputMJpegStreamUrlPort = ref new ItemStringViewModel("_outputMJpegStreamUrlPort");
	ItemStringViewModel^ _inputUri = ref new ItemStringViewModel("_inputUri");

	HourViewModel^ _ffmpegoutDeleteOlderFiles = ref new HourViewModel("_ffmpegoutDeleteOlderFiles");
	HourViewModel^ _ffmpegoutRecordingHours = ref new HourViewModel("_ffmpegoutRecordingHours");

	ItemValueViewModel^ _toggleSwitchMovementWatcher = ref new ItemValueViewModel("_toggleSwitchMovementWatcher");

	ItemValueViewModel^ _toggleSwitchMuxCopyInput = ref new ItemValueViewModel("_toggleSwitchMuxCopyInput");

	ItemStringViewModel^ _HostNameMovementWatcher = ref new ItemStringViewModel("_HostNameMovementWatcher");

	ItemValueViewModel^ _PortMovementWatcher = ref new  ItemValueViewModel("_PortMovementWatcher");

	ItemValueViewModel^ _InputPin1MovementWatcher = ref new  ItemValueViewModel("_InputPin1MovementWatcher");

	ItemValueViewModel^ _InputPin1MovementWatcherActiv = ref new  ItemValueViewModel("_InputPin1MovementWatcherActiv");

	ItemValueViewModel^ _RecordingOnMovement = ref new  ItemValueViewModel("_RecordingOnMovement");


	HourViewModel^ _MovementRecordingTimeSecs = ref new  HourViewModel("_MovementRecordingTimeSecs");




	_outputMpegStreamUrl->Value = L"Willi-PC:3000";


	//this->_datasources = ref new DataSources("Camera_1");

	_datasources->AddDataSource(_ffmpegoutputresolution);
	_datasources->AddDataSource(_ffmpegoutputfps);
	_datasources->AddDataSource(_outputMJpegresolution);
	_datasources->AddDataSource(_outputMJpefps);
	_datasources->AddDataSource(_outputMPegresolution);
	_datasources->AddDataSource(_outputMPegfps);
	_datasources->AddDataSource(_inputSourceUri);
	_datasources->AddDataSource(_ffmpegoutputformat);

	_datasources->AddDataSource(_toggleSwitchOutPutMpegStream);
	_datasources->AddDataSource(_toggleSwitchOutPutMJpegStream);
	_datasources->AddDataSource(_toggleSwitchOutPutMuxer);
	_datasources->AddDataSource(_toggleSwitchVideoDecode);
	_datasources->AddDataSource(_toggleSwitchAudioDecode);

	_datasources->AddDataSource(_outputMpegStreamUrl);
	_datasources->AddDataSource(_outputMJpegStreamUrlPort);

	_datasources->AddDataSource(_inputUri);

	_datasources->AddDataSource(_ffmpegoutDeleteOlderFiles);
	_datasources->AddDataSource(_ffmpegoutRecordingHours);

	_datasources->AddDataSource(_toggleSwitchMovementWatcher);
	_datasources->AddDataSource(_toggleSwitchMuxCopyInput);
	_datasources->AddDataSource(_HostNameMovementWatcher);
	_datasources->AddDataSource(_PortMovementWatcher);
	_datasources->AddDataSource(_InputPin1MovementWatcher);
	_datasources->AddDataSource(_InputPin1MovementWatcherActiv);
	_datasources->AddDataSource(_RecordingOnMovement);
	
	_datasources->AddDataSource(_MovementRecordingTimeSecs);

	_datasources->readSettingsfromLocalStorage();




}

std::vector<std::wstring> StreamingPageParam::splitintoArray(const std::wstring& s, const std::wstring& delim)
{
	std::vector<std::wstring> result;
	wchar_t*pbegin = (wchar_t*)s.c_str();
	wchar_t * next_token = nullptr;
	wchar_t* token;
	token = wcstok_s(pbegin, delim.c_str(), &next_token);
	while (token != nullptr) {
		// Do something with the tok
		result.push_back(token);
		token = wcstok_s(NULL, delim.c_str(), &next_token);
	}

	return result;
}





StreamingPageParamControl::StreamingPageParamControl()
{
	this->m_Items = ref new Platform::Collections::Vector<StreamingPageParam^>();
	m_SelectedIndex = -1;
}

StreamingPageParamControl::~StreamingPageParamControl()
{
	for each (auto var in this->Items)
	{
		delete var;

	}

	delete Items;
	m_SelectedIndex = -1;
}



void StreamingPageParamControl::SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	Windows::UI::Xaml::Controls::ListView^ inputsourceurl = safe_cast<ListView^>(sender);
	if (inputsourceurl->SelectedIndex == -1) return;

	auto selected = inputsourceurl->Items->GetAt(inputsourceurl->SelectedIndex);
	this->SelectedIndex = inputsourceurl->SelectedIndex;



}



int StreamingPageParamControl::SelectedIndex::get() {
	return this->m_SelectedIndex;
}
void StreamingPageParamControl::SelectedIndex::set(int value) {
	this->m_SelectedIndex = value;
}



void StreamingPageParamControl::ClearRessources()
{
	for each (auto var in this->Items)
	{
		var->ClearRessources();

	}
}

StreamingPageParam^ StreamingPageParamControl::getItemByOnVifCamera(OnVifServicesRunTime::OnVifCamera^ camera)
{
	for each (auto var in this->Items)
	{
		if (var->OnVifCamera == camera) {

			return var;
		}
	}

	return nullptr;

}


StreamingPageParam^ StreamingPageParamControl::getSelectedItem()
{
	if (this->SelectedIndex == -1)
	{
		return nullptr;
	}
	if (this->SelectedIndex < Items->Size)	return(StreamingPageParam^)Items->GetAt((unsigned int)SelectedIndex);
	else return nullptr;

}




