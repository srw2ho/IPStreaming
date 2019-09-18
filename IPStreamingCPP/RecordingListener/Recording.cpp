#include "pch.h"
#include "ServiceChunkReceiver.h"
#include "TimeConversion.h"

#include "Recording.h"


using namespace Platform;
using namespace Windows::Foundation;

using namespace Windows::Networking::Sockets;
using namespace Windows::UI::Core;
using namespace Windows::System::Threading;

using namespace StreamSocketComm;
using namespace concurrency;
using namespace GPIODriver;
using namespace RecordingListener;

const UINT64 nano100SecInSec = (UINT64)10000000 * 1;

Recording::Recording()
{
	m_pSocketListener = ref new StreamSocketComm::SocketListener();


	m_pGPIOEventPackageQueue = new GPIODriver::GPIOEventPackageQueue();

	m_GPIOClientInOut = ref new GPIODriver::GPIOInOut(m_pGPIOEventPackageQueue);

	//  m_pBME280DataQueue = new WeatherStationData::BME280DataQueue();



	m_bProcessingPackagesStarted = false;

	m_pPackageCancelTaskToken = nullptr;
	m_FailedEventRegister= m_pSocketListener->Failed += ref new Windows::Foundation::TypedEventHandler<Platform::Object ^, Platform::Exception ^>(this, &RecordingListener::Recording::OnFailed);

	m_OnClientConnected= m_pSocketListener->OnClientConnected += ref new Windows::Foundation::TypedEventHandler<Windows::Networking::Sockets::StreamSocket ^, int>(this, &RecordingListener::Recording::OnOnClientConnected);

	m_startStreamingEventRegister= m_pSocketListener->startStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object ^, Windows::Networking::Sockets::StreamSocket ^>(this, &RecordingListener::Recording::OnstartStreaming);

	m_stopStreamingEventRegister= m_pSocketListener->stopStreaming += ref new Windows::Foundation::TypedEventHandler<Platform::Object ^, Platform::Exception^ >(this, &RecordingListener::Recording::OnstopStreaming);

	m_FailedConnectionCount = 0;
	m_outputconfigoptions = nullptr;
	//m_inputconfigoptions = nullptr;
	m_Movement = false;

}

Recording::~Recording()
{
	// alle CallBacks zurücksetzen
	m_pSocketListener->Failed -= m_FailedEventRegister;
	m_pSocketListener->startStreaming -= m_startStreamingEventRegister;
	m_pSocketListener->stopStreaming -= m_stopStreamingEventRegister;
	m_pSocketListener->OnClientConnected -= m_OnClientConnected;

//	m_GPIOClientInOut->deleteAllGPIOPins();
	clearGPIOs();

	delete m_pSocketListener;
	delete m_pGPIOEventPackageQueue;
	m_pSocketListener = nullptr;
	m_pGPIOEventPackageQueue = nullptr;


}

void Recording::HostName::set(Platform::String ^ value) {
	m_HostName = value;
//	NotifyPropertyChanged("HostName");
//	NotifyPropertyChanged("VisibleKeyName");
}

void Recording::Port::set(int value) {
	m_port = value;
//	NotifyPropertyChanged("Port");
//	NotifyPropertyChanged("VisibleKeyName");
}

void Recording::IsMoment::set(bool value) {
	m_Movement = value;
}




void Recording::FailedConnectionCount::set(unsigned int value) {
	m_FailedConnectionCount = value;

//	NotifyPropertyChanged("FailedConnectionCount");
}

Concurrency::task<void> Recording::doProcessPackages()
{
	auto token = m_pPackageCancelTaskToken->get_token();


	auto tsk = create_task([this, token]() -> void

	{
		bool dowhile = true;
		//	DWORD waitTime = INFINITE; // INFINITE ms Waiting-Time
		DWORD waitTime = 500; // 200 ms Waiting-Time

		// erst nach 2 sec mit dem Update loslegen, damit das System sich einschwingen kann
		UINT64 lastSendTime = ConversionHelpers::TimeConversion::getActualUnixTime() + 2 * nano100SecInSec;

	//	m_InletnextCycleFlushingTime = ConversionHelpers::TimeConversion::getActualUnixTime() + nano100SecInSec * 24 * 3600 * m_InletCycleFlushingTime;

		bool doInit =true;
		while (dowhile) {
			try {

				GPIOEventPackage* pPacket = nullptr;
				UINT64 actualTime = ConversionHelpers::TimeConversion::getActualUnixTime();

				size_t startsize = m_pGPIOEventPackageQueue->waitForPacket(&pPacket, waitTime); //wait for incoming Packet, INFINITE waiting for incoming Package
				if (pPacket != nullptr)
				{
					if (m_outputconfigoptions != nullptr) {
						GPIOPin* pGPIOPin = (GPIOPin*)pPacket->getAdditional();
						if (pGPIOPin != nullptr)
						{
							switch (pGPIOPin->getGPIOTyp()) {
								case GPIOTyp::input:
								{
									MapInputClientsPins::iterator it = m_MapInputClientsPins.find(pGPIOPin->getPinNumber());
									
									if (it != m_MapInputClientsPins.end()) {
										bool MovementActiv = true;
										MovementActiv = (it->second->getPinValue() != it->second->getSetValue());

										if (m_outputconfigoptions->HasKey("m_MovementActiv")) {
											Platform::Object^ isActivvalue = m_outputconfigoptions->Lookup("m_MovementActiv");
											if (isActivvalue != nullptr) {
												bool isActiv = safe_cast<IPropertyValue^>(isActivvalue)->GetBoolean();
												if ((MovementActiv != isActiv) || doInit) {// only in case of changes
													m_outputconfigoptions->Insert("m_MovementActiv", dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(MovementActiv)));
													m_Movement = MovementActiv;
													ChangeMovement(this, m_outputconfigoptions);
													doInit = false;
												}
											}
										}
										else
										{
											m_outputconfigoptions->Insert("m_MovementActiv", dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(MovementActiv)));
											m_Movement = MovementActiv;
											ChangeMovement(this, m_outputconfigoptions);
											doInit = false;
										}

									}
	
									break;
								}
							}
						}
					}
					delete pPacket; // Package deleten
				}

				if (token.is_canceled()) {
					cancel_current_task();
				}

			}
			catch (task_canceled&)
			{
				dowhile = false;

			}
			catch (const exception&)
			{
				dowhile = false;

			}

		}

	}, token);

	return tsk;
}

void Recording::startProcessingPackages(Windows::Foundation::Collections::PropertySet^ inputconfigoptions, Windows::Foundation::Collections::PropertySet^ outputconfigoptions)
{
	//m_inputconfigoptions = inputconfigoptions;
	m_outputconfigoptions = outputconfigoptions;
	

	if (m_bProcessingPackagesStarted) return;
	bool doStart = false;
	if (inputconfigoptions->HasKey("HostName") && inputconfigoptions->HasKey("Port") && inputconfigoptions->HasKey("GPIOTyp.input_1") && inputconfigoptions->HasKey("GPIOTyp.input_1.Activ")) {
		doStart = true;
	}
	if (!doStart) return;


	Platform::Object^ hostName = inputconfigoptions->Lookup("HostName");
	Platform::Object^ port = inputconfigoptions->Lookup("Port");
	if (hostName != nullptr) {
		this->m_HostName = safe_cast<IPropertyValue^>(hostName)->GetString();
	}
	if (port != nullptr) {
		this->m_port = safe_cast<IPropertyValue^>(port)->GetInt32();
	}


	m_bProcessingPackagesStarted = true;

	m_pGPIOEventPackageQueue->Flush();


	InitGPIOOutput(inputconfigoptions); // alle GPIO-Output erzeugen


	m_pSocketListener->StartClientAsync(m_HostName, m_port);



	if (m_pPackageCancelTaskToken != nullptr)
	{
		delete m_pPackageCancelTaskToken;

	}
	m_pPackageCancelTaskToken = new concurrency::cancellation_token_source();


	m_ProcessingPackagesTsk = create_task(doProcessPackages()).then([this](task<void> previous)
	{
		m_bProcessingPackagesStarted = false;
		try {
			previous.get();
		}
		catch (Exception^ exception)
		{

		}

	});




}

void Recording::cancelPackageAsync()
{
	if (m_pPackageCancelTaskToken != nullptr) {
		m_pPackageCancelTaskToken->cancel();
	}

	m_pGPIOEventPackageQueue->cancelwaitForPacket();




}

Windows::Foundation::IAsyncAction ^ Recording::startProcessingPackagesAsync(Windows::Foundation::Collections::PropertySet^ inputconfigoptions, Windows::Foundation::Collections::PropertySet^ outputconfigoptions) {


	return create_async([this, inputconfigoptions, outputconfigoptions]()
	{
		startProcessingPackages(inputconfigoptions, outputconfigoptions);

	});

}

Windows::Foundation::IAsyncAction ^ Recording::stopProcessingPackagesAsync() {

	return create_async([this]()
	{
		stopProcessingPackages();
	});
}


void Recording::stopProcessingPackages()
{
	if (!m_bProcessingPackagesStarted) return;
	try {

		m_bProcessingPackagesStarted = false;
		m_pSocketListener->CancelConnections();// alle Connections schliessen

		cancelPackageAsync();


	//	Sleep(100);
		// Darf nicht in UI-Thread aufgerufen werden-> Blockiert UI-Thread-> gibt Exception
		m_ProcessingPackagesTsk.wait();
		m_Movement = false;
		m_outputconfigoptions->Insert("m_MovementActivated", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(0)));
		ChangeMovement(this, m_outputconfigoptions);

//		clearGPIOs();


	}
	catch (Exception^ exception)
	{
		bool b = true;

	}


}
void Recording::clearGPIOs() { // cancel all pending timeouts
	m_Movement = false;
	MapInputClientsPins::iterator it;
	m_GPIOClientInOut->deleteAllGPIOPins();
	for (it = m_MapInputClientsPins.begin(); it != m_MapInputClientsPins.end(); ) {
//		GPIOPin*pPin = it->second;
//		delete pPin;
		it = m_MapInputClientsPins.erase(it);
	}
}


bool Recording::InitGPIOOutput(Windows::Foundation::Collections::PropertySet^ inputconfigoptions) {

	clearGPIOs();

	if (inputconfigoptions == nullptr) return false;

	int input1 = -1;
	int input1Activ = -1;
	Platform::Object^ inputPin1 = inputconfigoptions->Lookup("GPIOTyp.input_1");
	Platform::Object^ inputPin1Activ = inputconfigoptions->Lookup("GPIOTyp.input_1.Activ");
	if (inputPin1 != nullptr) {
		input1 = safe_cast<IPropertyValue^>(inputPin1)->GetInt32();
	}
	if (inputPin1Activ != nullptr) {
		input1Activ = safe_cast<IPropertyValue^>(inputPin1Activ)->GetInt32();
	}

	if (input1 != -1) {
		m_MapInputClientsPins[input1] = new GPIODriver::GPIOInputPin(m_pGPIOEventPackageQueue, input1, (input1Activ>0) ? 0 : 1);

	}

	for (MapInputClientsPins::iterator it = m_MapInputClientsPins.begin(); it != m_MapInputClientsPins.end(); it++) {
		it->second->setActivateOutputProcessing(true);
		it->second->setSetValue((input1Activ>0) ? 0 : 1);
		m_GPIOClientInOut->addGPIOPin(it->second);
	}
	

	//m_pRecordingOnSensor = new GPIODriver::GPIOInputPin(m_pGPIOEventPackageQueue, m_RedordingPinNr, 0); // BME280_I2C_ADDR_PRIM
	//m_pRecordingOnSensor->setActivateOutputProcessing(true);
	//m_GPIOClientInOut->addGPIOPin(m_pRecordingOnSensor);
	m_Movement = false;
	return true;
}

void RecordingListener::Recording::OnFailed(Platform::Object ^sender, Platform::Exception ^args)
{// Trying to connect failed
//	throw ref new Platform::NotImplementedException();
	Platform::String^ err = ref new String();
	err = args->Message;
	m_FailedConnectionCount = m_FailedConnectionCount + 1;
	this->Failed(this, err);
}


void RecordingListener::Recording::OnOnClientConnected(Windows::Networking::Sockets::StreamSocket ^sender, int args)
{// Client connection created --> Add Connection to Listener
	//throw ref new Platform::NotImplementedException();

	StreamSocket^ socket = sender;

	StreamSocketComm::SocketChunkReceiver* pServiceListener = new ServiceChunkReceiver(socket, this->m_GPIOClientInOut);

	SocketChunkReceiverWinRT ^ pBME280ChunkReceiverWinRT = ref new SocketChunkReceiverWinRT(pServiceListener); // wrapper for SocketChunkReceiver and its derived
	m_pSocketListener->AddChunkReceiver(pBME280ChunkReceiverWinRT);
	pBME280ChunkReceiverWinRT->geSocketChunkReceiver()->StartService(); // send "BME280Server.Start" - Copmmand to Client Station-> Start with Communication
//		this->m_FailedConnectionCount = 0; // wieder 0 setzen, damit Fehler wieder neu erfasst werden kann.




}


void RecordingListener::Recording::OnstartStreaming(Platform::Object ^sender, Windows::Networking::Sockets::StreamSocket ^args)
{
	m_FailedConnectionCount = 0;
	this->startStreaming(this, args);
	//throw ref new Platform::NotImplementedException();
}


void RecordingListener::Recording::OnstopStreaming(Platform::Object ^sender, Platform::Exception ^exception)
{
	Platform::String^ err = ref new String();
	if (exception != nullptr) {
		err = exception->Message;
		m_FailedConnectionCount = m_FailedConnectionCount + 1;
	}
	else err = "";

	this->stopStreaming(this, err);
	//Connection  error by remote or stopped by user (CancelConnections)
	//throw ref new Platform::NotImplementedException();
}
