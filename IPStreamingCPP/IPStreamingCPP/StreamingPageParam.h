#pragma once
#ifndef STREAMINGPAGEPARAM
#define STREAMINGPAGEPARAM

#include "ScenarioViewControl.h"
#include "StreamingPageParam.h"

#include "DataSources.h"

#include "CodecReader.h"


//#include "MainPage.xaml.h"


using namespace Platform;
using namespace IPStreamingCPP;
using namespace FFmpegInteropExtRT;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Controls;

namespace IPStreamingCPP
{

	public ref class  DataSourceparam sealed {

	internal:
		ResolutionViewModel^ _ffmpegoutputresolution;
		FpsViewModel^ _ffmpegoutputfps;
		ResolutionViewModel^ _outputMJpegresolution;
		FpsViewModel^ _outputMJpefps;
		ResolutionViewModel^ _outputMPegresolution;
		FpsViewModel^ _outputMPegfps;
		inputSourceViewModel^ _inputSourceUri;
		OutputFormatViewModel^ _ffmpegoutputformat;
		ItemValueViewModel^ _toggleSwitchOutPutMpegStream;
		ItemValueViewModel^ _toggleSwitchOutPutMJpegStream;
		ItemValueViewModel^ _toggleSwitchOutPutMuxer;
		ItemValueViewModel^ _toggleSwitchVideoDecode;
		ItemValueViewModel^ _toggleSwitchAudioDecode;
		ItemStringViewModel^ _outputMpegStreamUrl;
		ItemStringViewModel^ _outputMJpegStreamUrlPort;
		HourViewModel^ _ffmpegoutDeleteOlderFiles;
		HourViewModel^ _ffmpegoutRecordingHours;
		ItemStringViewModel^ _inputUri;

		ItemValueViewModel ^ _toggleSwitchMovementWatcher;

		ItemValueViewModel ^ _toggleSwitchMuxCopyInput;

		ItemStringViewModel^ _HostNameMovementWatcher;

		ItemValueViewModel^ _PortMovementWatcher;
		ItemValueViewModel^ _InputPin1MovementWatcher;
		ItemValueViewModel^ _InputPin1MovementWatcherActiv;
		ItemValueViewModel^ _RecordingOnMovement;
		ItemValueViewModel^ _CameraEvents;
		HourViewModel ^ _MovementRecordingTimeSecs;

		bool _init;
		DataSourceparam()
		{
			_init = false;
			this->_ffmpegoutputresolution = nullptr;

			this->_ffmpegoutputfps =  nullptr;


			this->_outputMJpegresolution = nullptr;

			this->_outputMJpefps = nullptr;


			this->_outputMPegresolution = nullptr;


			this->_outputMPegfps = nullptr;

			this->_inputSourceUri = nullptr;


			this->_ffmpegoutputformat = nullptr;


			this->_toggleSwitchOutPutMpegStream = nullptr;

			this->_toggleSwitchOutPutMJpegStream = nullptr;
			this->_toggleSwitchOutPutMuxer = nullptr;

			this->_toggleSwitchVideoDecode = nullptr;
			this->_toggleSwitchAudioDecode = nullptr;

			this->_outputMpegStreamUrl = nullptr;

			this->_outputMJpegStreamUrlPort = nullptr;

			this->_ffmpegoutDeleteOlderFiles = nullptr;
			this->_ffmpegoutRecordingHours = nullptr;

			this->_inputUri = nullptr;

			this->_toggleSwitchMovementWatcher = nullptr;
			this->_toggleSwitchMuxCopyInput = nullptr;

			this->_HostNameMovementWatcher = nullptr;

			this->_PortMovementWatcher = nullptr;

			this->_InputPin1MovementWatcher = nullptr;
			this->_InputPin1MovementWatcherActiv = nullptr;
			this->_MovementRecordingTimeSecs = nullptr;
			this->_RecordingOnMovement = nullptr;
			this->_CameraEvents = nullptr;

		}
		void SetParams(IPStreamingCPP::DataSources ^ _datasources)
		{
			if (_init) return;

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

			this->_PortMovementWatcher = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_PortMovementWatcher"));;

			this->_InputPin1MovementWatcher = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_InputPin1MovementWatcher"));;

			this->_InputPin1MovementWatcherActiv = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_InputPin1MovementWatcherActiv"));;

			this->_MovementRecordingTimeSecs = safe_cast <HourViewModel^>(_datasources->getDataSource("_MovementRecordingTimeSecs"));

			this->_RecordingOnMovement = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_RecordingOnMovement"));;

			this->_CameraEvents = safe_cast <ItemValueViewModel^>(_datasources->getDataSource("_CameraEvents"));;;

			_init = true;
		}

		
	

	};

	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]

	public ref class StreamingPageParam sealed //: public Windows::UI::Xaml::Data::INotifyPropertyChanged
	{

		Platform::String^ m_KeyName;
		Windows::UI::Xaml::Controls::MediaElement ^ m_mediaElement;
		Frame^ m_MainFrame;
		FFmpegInteropExtRT::FFmpegInteropMSS^ m_FFmpegMSS;
		FFmpegInteropExtRT::CameraServer ^ m_pCameraServer;
		IPStreamingCPP::DataSources ^ m_datasources;
		RecordingListener::Recording ^ m_Recording;
		AmcrestMotionDetection::AmcrestMotion^ m_AmcrestMotion;

		Windows::System::Threading::ThreadPoolTimer ^ m_restartStreamingTimer;
		IPStreamingCPP::DataSourceparam ^ m_DataSourceparam;
		Windows::Storage::Streams::IRandomAccessStream^ m_stream;
		OnVifServicesRunTime::OnVifCamera ^ m_OnVifCamera;
		Windows::Foundation::EventRegistrationToken m_PropertyChangedEventRegister;
		Windows::Foundation::EventRegistrationToken m_MediaCurrentStateChangedRegister;
		Windows::Foundation::EventRegistrationToken m_MediaFailedRegister;
		Windows::Foundation::EventRegistrationToken m_CameraServerFailedRegister;

		//Windows::Foundation::EventRegistrationToken m_OnFailedMovementStreaming;
		Windows::Foundation::EventRegistrationToken m_OnStartMovementStreaming;
		Windows::Foundation::EventRegistrationToken m_OnStopMovementStreaming;
		Windows::Foundation::EventRegistrationToken m_OnChangeMovementStreaming;

		Windows::Foundation::EventRegistrationToken m_OnStartAMCRESEventStreaming;
		Windows::Foundation::EventRegistrationToken m_OnStopAMCRESEventStreaming;
		Windows::Foundation::EventRegistrationToken m_OnChangeAMCRESEventStreaming;

		IPStreamingCPP::ScenarioViewControl ^  m_ScenarioViewControl;
		
		Windows::Foundation::Collections::PropertySet^ m_Muxerconfigoptions;
//		Platform::String^  m_RecordingState;
		FFmpegInteropExtRT::CodecReader^ m_CodecReader;

	internal:

		property Windows::Foundation::EventRegistrationToken PropertyChangedEventRegister
		{
			void set(Windows::Foundation::EventRegistrationToken value) { this->m_PropertyChangedEventRegister = value; };
		}

		property Windows::Foundation::EventRegistrationToken MediaCurrentStateChangedRegister
		{
			void set(Windows::Foundation::EventRegistrationToken value) { this->m_MediaCurrentStateChangedRegister = value; };
		}

		property Windows::Foundation::EventRegistrationToken MediaFailedRegister
		{
			void set(Windows::Foundation::EventRegistrationToken value) { this->m_MediaFailedRegister = value; };
		}

		property Windows::Foundation::EventRegistrationToken CameraServerFailedRegister
		{
			void set(Windows::Foundation::EventRegistrationToken value) { this->m_CameraServerFailedRegister = value; };
		}
		property Windows::Foundation::EventRegistrationToken OnStartMovementStreaming
		{
			void set(Windows::Foundation::EventRegistrationToken value) { this->m_OnStartMovementStreaming = value; };
		}

		property Windows::Foundation::EventRegistrationToken OnStopMovementStreaming
		{
			void set(Windows::Foundation::EventRegistrationToken value) { this->m_OnStopMovementStreaming = value; };
		}

		property Windows::Foundation::EventRegistrationToken OnChangeMovementStreaming
		{
			void set(Windows::Foundation::EventRegistrationToken value) { this->m_OnChangeMovementStreaming = value; };
		}

		property Windows::Foundation::EventRegistrationToken OnStartAMCRESEventStreaming
		{
			void set(Windows::Foundation::EventRegistrationToken value) { this->m_OnStartAMCRESEventStreaming = value; };
		}

		property Windows::Foundation::EventRegistrationToken OnStopAMCRESEventStreaming
		{
			void set(Windows::Foundation::EventRegistrationToken value) { this->m_OnStopAMCRESEventStreaming = value; };
		}

		property Windows::Foundation::EventRegistrationToken OnChangeAMCRESEventStreaming
		{
			void set(Windows::Foundation::EventRegistrationToken value) { this->m_OnChangeAMCRESEventStreaming = value; };
		}



	
	public:
	//	virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;


		void ClearRessources();
	

		
		property ScenarioViewControl^ ScenarioView
		{
			ScenarioViewControl^ get() { return this->m_ScenarioViewControl; }
			void set(ScenarioViewControl^ value) { this->m_ScenarioViewControl = value; };
		}


		property Platform::String ^ VisibleKeyName{	Platform::String ^ get(); }

		property Platform::String ^ KeyName
		{
			Platform::String^ get() { return this->m_KeyName; }
			void set(Platform::String^ value) { this->m_KeyName = value; };
		}

		property FFmpegInteropExtRT::CodecReader^ CodecReader
		{
			FFmpegInteropExtRT::CodecReader^ get() { return this->m_CodecReader; }
			void set(FFmpegInteropExtRT::CodecReader^ value) { this->m_CodecReader = value; };
		}


		property IPStreamingCPP::DataSources ^ DataSources
		{
			IPStreamingCPP::DataSources ^ get() { return this->m_datasources; }
			void set(IPStreamingCPP::DataSources ^ value) { this->m_datasources = value; };
		}


		property FFmpegInteropExtRT::CameraServer ^ CameraServer
		{
			FFmpegInteropExtRT::CameraServer ^ get() { return this->m_pCameraServer; }
			void set(FFmpegInteropExtRT::CameraServer ^ value) { this->m_pCameraServer = value; };
		}

		property RecordingListener::Recording ^ MovementRecording
		{
			RecordingListener::Recording ^ get() { return this->m_Recording; }
			void set(RecordingListener::Recording ^ value) { this->m_Recording = value;  };
		}
	

		property AmcrestMotionDetection::AmcrestMotion^ AmcrestMotion
		{
			AmcrestMotionDetection::AmcrestMotion^ get() { return this->m_AmcrestMotion; }
			void set(AmcrestMotionDetection::AmcrestMotion^ value) { this->m_AmcrestMotion = value; };
		}


		property IPStreamingCPP::DataSourceparam ^ DataSourceparam
		{
			IPStreamingCPP::DataSourceparam ^ get() { return this->m_DataSourceparam; }
			void set(IPStreamingCPP::DataSourceparam ^ value) { this->m_DataSourceparam = value; };
		}

		property OnVifServicesRunTime::OnVifCamera^ OnVifCamera
		{
			OnVifServicesRunTime::OnVifCamera^ get() { return this->m_OnVifCamera; }
			void set(OnVifServicesRunTime::OnVifCamera^ value) { this->m_OnVifCamera = value; };
		}

		property IRandomAccessStream^ Stream
		{
			IRandomAccessStream^ get() { return this->m_stream; }
			void set(IRandomAccessStream^ value) { this->m_stream = value; };
		}

		property Windows::UI::Xaml::Controls::MediaElement^ MediaStreamElement
		{
			Windows::UI::Xaml::Controls::MediaElement^ get() { return this->m_mediaElement; }
			void set(Windows::UI::Xaml::Controls::MediaElement^ value) { this->m_mediaElement = value; };
		}


		StreamingPageParam ^ createStreamingPageParam(Platform::String^ key, Frame^  Main);

		FFmpegInteropExtRT::FFmpegInteropMSS^ getFFmpegInteropMSS() { return  m_FFmpegMSS; };

		StreamingPageParam();
		virtual ~StreamingPageParam();


		void restartStreamingTimer(inputSource ^ inpSource);
		void OnstartStreaming(Platform::Object ^sender, FFmpegInteropMSSInputParamArgs^ args);
		bool startUriStreaming();
	
		bool startFileStreaming();
		bool stopStreaming();

		void WriteToAppData();
		void ReadFromAppData();
		void takeParametersFromCamera();
		void DisplayErrorMessage(Platform::String^ title, Platform::String^ message);

		void startMovementRecording(Windows::Foundation::Collections::PropertySet^ inputconfigoptions);
		Windows::Foundation::IAsyncAction^ stopMovementRecording();
		void startAMCRESTEventRecording(Windows::Foundation::Collections::PropertySet^ inputconfigoptions);
		Windows::Foundation::IAsyncAction^ stopAMCRESTEventRecording();

	internal:

		concurrency::task<void>  clearRecording();
		void SetStream(IRandomAccessStream^ stream) { this->m_stream = stream; }
		std::vector<std::wstring> splitintoArray(const std::wstring& s, const std::wstring& delim);
		int64_t CalculateBitrate(double inputfps, int fps, int height, int width, int motion);


	private:
		bool setstartUriStreaming();
		
	//	void OnstartMovementStreaming(Platform::Object ^sender, Windows::Networking::Sockets::StreamSocket ^args);
	//	void OnstopMovementStreaming(Platform::Object ^sender, Platform::String ^args);
	//	void OnChangeMovement(Platform::Object ^sender, Windows::Foundation::Collections::PropertySet ^args);
	
	//protected:
	//	void NotifyPropertyChanged(Platform::String^ prop);

	
};




	[Windows::UI::Xaml::Data::Bindable]
	public ref class  StreamingPageParamControl sealed //: Windows::UI::Xaml::DependencyObject
	{
		Platform::Collections::Vector<StreamingPageParam^>^ m_Items;
		unsigned int m_SelectedIndex;
		Platform::String^ m_DataCompositeIDName;

	public:
		StreamingPageParamControl();
		virtual ~StreamingPageParamControl();
		void SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void ClearRessources();

		property Windows::Foundation::Collections::IVector<StreamingPageParam^>^ Items {
			Windows::Foundation::Collections::IVector<StreamingPageParam^>^ Items::get() { return this->m_Items; };
		}

		StreamingPageParam^ getSelectedItem();

		StreamingPageParam^ getItemByOnVifCamera(OnVifServicesRunTime::OnVifCamera^ camera);
		property int SelectedIndex
		{
			int get();
			void set(int value);
		}
		void writeSettingsToLocalStorage();
		void readSettingsfromLocalStorage();

		Windows::Storage::ApplicationDataCompositeValue^ getCompositeValue();
		Windows::Storage::ApplicationDataCompositeValue^ deleteCompositeValue();

	};
}
#endif // A_H
