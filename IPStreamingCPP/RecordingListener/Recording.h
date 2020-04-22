#pragma once

#include <queue>
#include <map>

#include "SocketListener.h"
#include "GPIOInOut.h"


typedef std::map<int, GPIODriver::GPIOInputPin* > MapInputClientsPins;

namespace RecordingListener
{
    public ref class Recording sealed
    {
		StreamSocketComm::SocketListener^  m_pSocketListener;
		concurrency::cancellation_token_source* m_pPackageCancelTaskToken;

		GPIODriver::GPIOInOut^ m_GPIOClientInOut;
		MapInputClientsPins m_MapInputClientsPins;
		GPIODriver::GPIOEventPackageQueue* m_pGPIOEventPackageQueue;

		bool m_bProcessingPackagesStarted;
		concurrency::task<void> m_ProcessingPackagesTsk;
		Windows::Foundation::EventRegistrationToken m_startStreamingEventRegister;
		Windows::Foundation::EventRegistrationToken m_stopStreamingEventRegister;
		Windows::Foundation::EventRegistrationToken m_FailedEventRegister;
		Windows::Foundation::EventRegistrationToken m_OnClientConnected;

		Platform::String ^ m_HostName;
		int m_port;
		unsigned int m_FailedConnectionCount;
		Windows::Foundation::Collections::PropertySet^ m_outputconfigoptions;

		bool m_Movement;
    public:
		Recording();
		virtual ~Recording();
		event Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^  >^ ChangeMovement;

		event Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^  >^ startStreaming;
		event Windows::Foundation::TypedEventHandler<Platform::Object^, Platform::String ^>^ stopStreaming;
		event Windows::Foundation::TypedEventHandler<Platform::Object^, Platform::String ^> ^ Failed;
		Windows::Foundation::IAsyncAction ^ startProcessingPackagesAsync(Windows::Foundation::Collections::PropertySet^ inputconfigoptions, Windows::Foundation::Collections::PropertySet^ outputconfigoptions);
		Windows::Foundation::IAsyncAction ^ stopProcessingPackagesAsync();


		property Platform::String ^ HostName {
			Platform::String ^  get() { return m_HostName; };
			void set(Platform::String ^ value);
		}

		property int Port {
			int  get() { return m_port; };
			void set(int value);
		}

		property bool IsMoment {
			bool get() { return m_Movement; };
			void set(bool value);
		}


		property unsigned int  FailedConnectionCount {
			unsigned int   get() { return m_FailedConnectionCount; };
			void set(unsigned int   value);
		}

	private:
		Concurrency::task<void> doProcessPackages();
		void cancelPackageAsync();
		void OnFailed(Platform::Object ^sender, Platform::Exception ^args);
		void OnOnClientConnected(Windows::Networking::Sockets::StreamSocket ^sender, int args);
		void OnstartStreaming(Platform::Object ^sender, Windows::Networking::Sockets::StreamSocket ^args);
		void OnstopStreaming(Platform::Object ^sender, Platform::Exception ^exception);
		bool InitGPIOOutput(Windows::Foundation::Collections::PropertySet^ inputconfigoptions);
		void clearGPIOs();
		void startProcessingPackages(Windows::Foundation::Collections::PropertySet^ inputconfigoptions, Windows::Foundation::Collections::PropertySet^ outputconfigoptions);
		void stopProcessingPackages();
		void DisableMovementActivated();
    };
}
