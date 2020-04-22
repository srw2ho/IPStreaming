//
// StreamingPage.xaml.h
// Declaration of the StreamingPage class
//

#pragma once

#include "StreamingPage.g.h"

//#include "DataSources.h"
#include "StreamingPageParam.h"
#include "ValueConverter.h"

namespace IPStreamingCPP
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class StreamingPage sealed: Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:



	public:
		StreamingPage();
		virtual ~StreamingPage();

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;


		event Windows::Foundation::TypedEventHandler<Platform::Object^, IPStreamingCPP::StreamingPageParam ^  > ^ startUriStreaming;
		event Windows::Foundation::TypedEventHandler<Platform::Object^, IPStreamingCPP::StreamingPageParam^ > ^ startFileStreaming;
		event Windows::Foundation::TypedEventHandler<Platform::Object^, IPStreamingCPP::StreamingPageParam ^  > ^ stopStreaming;
	

		
		property ItemStringViewModel ^ Inpurturl
		{
			IPStreamingCPP::ItemStringViewModel^ get() { return _inputUri; };
		}

		property IPStreamingCPP::ResolutionViewModel^ FFmpegoutputresolution
		{
			IPStreamingCPP::ResolutionViewModel^ get() { return _ffmpegoutputresolution; };
		}

		property IPStreamingCPP::FpsViewModel^ FFmpegoutputFps
		{
			IPStreamingCPP::FpsViewModel^ get() { return _ffmpegoutputfps; };
		}

		property IPStreamingCPP::ResolutionViewModel^ OutputMJpegresolution
		{
			IPStreamingCPP::ResolutionViewModel^ get() { return _outputMJpegresolution; };
		}

		property IPStreamingCPP::FpsViewModel^ OutputMJpegFps
		{
			IPStreamingCPP::FpsViewModel^ get() { return _outputMJpefps; };
		}
		property IPStreamingCPP::ResolutionViewModel^ OutputMPegresolution
		{
			IPStreamingCPP::ResolutionViewModel^ get() { return _outputMPegresolution; };
		}

		property IPStreamingCPP::FpsViewModel^ OutputMPegFps
		{
			IPStreamingCPP::FpsViewModel^ get() { return _outputMPegfps; };
		}

		property IPStreamingCPP::inputSourceViewModel^ InputSourceUri
		{
			IPStreamingCPP::inputSourceViewModel^ get() { return _inputSourceUri; };
		}

		property IPStreamingCPP::OutputFormatViewModel^ FFmpegoutputformat
		{
			IPStreamingCPP::OutputFormatViewModel^ get() { return _ffmpegoutputformat; };
		}
		property IPStreamingCPP::ItemValueViewModel^ ToggleSwitchOutPutMpegStream
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _toggleSwitchOutPutMpegStream; };
		}
		property IPStreamingCPP::ItemValueViewModel^ ToggleSwitchOutPutMJpegStream
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _toggleSwitchOutPutMJpegStream; };
		}

		property IPStreamingCPP::ItemValueViewModel^ ToggleSwitchOutPutMuxer
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _toggleSwitchOutPutMuxer; };
		}

		property IPStreamingCPP::ItemValueViewModel^ ToggleSwitchVideoDecode
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _toggleSwitchVideoDecode; };
		}

		property IPStreamingCPP::ItemValueViewModel^ ToggleSwitchAudioDecode
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _toggleSwitchAudioDecode; };
		}

		property IPStreamingCPP::ItemStringViewModel^ OutputMpegStreamUrl
		{
			IPStreamingCPP::ItemStringViewModel^ get() { return _outputMpegStreamUrl; };
		}

		property IPStreamingCPP::ItemStringViewModel^ OutputMJpegStreamUrlPort
		{
			IPStreamingCPP::ItemStringViewModel^ get() { return _outputMJpegStreamUrlPort; };
		}

		property IPStreamingCPP::HourViewModel^ FFmpegoutDeleteOlderFiles
		{
			IPStreamingCPP::HourViewModel^ get() { return _ffmpegoutDeleteOlderFiles; };
		}

		property IPStreamingCPP::HourViewModel^ FFmpegoutRecordingHours
		{
			IPStreamingCPP::HourViewModel^ get() { return _ffmpegoutRecordingHours; };
		}


		property IPStreamingCPP::ItemValueViewModel^ ToggleSwitchMovementWatcher
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _toggleSwitchMovementWatcher; };
		}

		property IPStreamingCPP::ItemValueViewModel^ ToggleSwitchMuxCopyInput
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _toggleSwitchMuxCopyInput; };
		}

		property IPStreamingCPP::ItemStringViewModel^ HostNameMovementWatcher
		{
			IPStreamingCPP::ItemStringViewModel^ get() { return _HostNameMovementWatcher; };
		}

		property IPStreamingCPP::ItemValueViewModel^ PortMovementWatcher
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _PortMovementWatcher; };
		}


		property IPStreamingCPP::ItemValueViewModel^ InputPin1MovementWatcher
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _InputPin1MovementWatcher; };
		}
		property IPStreamingCPP::ItemValueViewModel^ InputPin1MovementWatcherActiv
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _InputPin1MovementWatcherActiv; };
		}

		property IPStreamingCPP::HourViewModel^ MovementRecordingTimeSecs
		{
			IPStreamingCPP::HourViewModel^ get() { return _MovementRecordingTimeSecs; };
		}
		property IPStreamingCPP::ItemValueViewModel^ RecordingOnMovement
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _RecordingOnMovement; };
		}

		property IPStreamingCPP::ItemValueViewModel^ CameraEvents
		{
			IPStreamingCPP::ItemValueViewModel^ get() { return _CameraEvents; };
		}

	


	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e); ;

	private:

		void OnPropertyChanged(Platform::String^ info);


		void outputMJpegStreamUrlPortKeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void inputURIBoxKeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);

		void outputMpegStreamUrlKeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
	
	
		void stopRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void startRecording_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void OpenLocalFile(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void starturiRecording();
	
		void DisplayErrorMessage(Platform::String^ message);
		void ReadFromAppData();
		void WriteToAppData();
	private:

		IPStreamingCPP::StreamingPageParam ^ m_StreamingPageParam;
		IPStreamingCPP::DataSources ^ _datasources;

		Platform::String^ _localHostName;
		IPStreamingCPP::ItemStringViewModel^ _inputUri;

		IPStreamingCPP::ResolutionViewModel ^ _ffmpegoutputresolution;
		IPStreamingCPP::FpsViewModel ^ _ffmpegoutputfps;
		IPStreamingCPP::ResolutionViewModel ^ _outputMJpegresolution;
		IPStreamingCPP::FpsViewModel ^ _outputMJpefps;
		IPStreamingCPP::ResolutionViewModel ^ _outputMPegresolution;
		IPStreamingCPP::FpsViewModel ^ _outputMPegfps;
		IPStreamingCPP::OutputFormatViewModel ^ _ffmpegoutputformat;

		IPStreamingCPP::ItemValueViewModel ^ _toggleSwitchOutPutMpegStream;
		IPStreamingCPP::ItemValueViewModel ^ _toggleSwitchOutPutMJpegStream;
		IPStreamingCPP::ItemValueViewModel ^ _toggleSwitchOutPutMuxer;
		IPStreamingCPP::ItemValueViewModel ^ _toggleSwitchVideoDecode;
		IPStreamingCPP::ItemValueViewModel ^ _toggleSwitchAudioDecode;
		IPStreamingCPP::ItemStringViewModel ^ _outputMpegStreamUrl;
		IPStreamingCPP::ItemStringViewModel ^ _outputMJpegStreamUrlPort;
		IPStreamingCPP::HourViewModel ^ _ffmpegoutDeleteOlderFiles;
		IPStreamingCPP::HourViewModel ^ _ffmpegoutRecordingHours;


		IPStreamingCPP::inputSourceViewModel^ _inputSourceUri;
		IPStreamingCPP::ItemValueViewModel ^ _toggleSwitchMovementWatcher;

		IPStreamingCPP::ItemValueViewModel ^ _toggleSwitchMuxCopyInput;

		IPStreamingCPP::ItemStringViewModel^ _HostNameMovementWatcher;

		IPStreamingCPP::ItemValueViewModel^ _PortMovementWatcher;
		IPStreamingCPP::ItemValueViewModel^ _InputPin1MovementWatcher;
		IPStreamingCPP::ItemValueViewModel^ _InputPin1MovementWatcherActiv;
		IPStreamingCPP::ItemValueViewModel^ _RecordingOnMovement;
		IPStreamingCPP::ItemValueViewModel^ _CameraEvents;
		IPStreamingCPP::HourViewModel ^ _MovementRecordingTimeSecs;

		Windows::System::Threading::ThreadPoolTimer ^ _restartStreamingTimer;


		
	};
}
