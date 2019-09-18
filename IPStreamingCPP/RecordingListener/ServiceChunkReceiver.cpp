#include "pch.h"
#include <ppl.h>
#include "ServiceChunkReceiver.h"




using namespace GPIODriver;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;


using namespace StreamSocketComm;

namespace RecordingListener
{


	ServiceChunkReceiver::ServiceChunkReceiver(Windows::Networking::Sockets::StreamSocket^socket, GPIODriver::GPIOInOut^ pGPIOInOut) :SocketChunkReceiver(socket)
	{
		m_pGPIOClientInOut = pGPIOInOut;

	}


	ServiceChunkReceiver::~ServiceChunkReceiver()
	{

	}


	Windows::Storage::Streams::IBuffer^ ServiceChunkReceiver::createBufferfromDataQueue() {



		DataWriter ^ writer = ref new DataWriter();
		

		return writer->DetachBuffer();
	}




	Platform::String^ ServiceChunkReceiver::DoCommands() {
		std::wstring doProcessCmd;

		return ref new Platform::String (doProcessCmd.data()) ;

	}


	Platform::String^ ServiceChunkReceiver::getStartServiceString()
	{

		Platform::String^ stringToSend = ref new Platform::String(L"GPIOServiceClient.Start");
		return stringToSend;
	}

	int ServiceChunkReceiver::DoProcessChunkMsg(DataReader^ reader)
	{
		if (m_chunkBufferSize == -1) return -1;

		unsigned int _len = reader->UnconsumedBufferLength;

		if (m_chunkBufferSize == 0)
		{
			return 0;
		}
		if (reader->UnconsumedBufferLength < m_chunkBufferSize) {
			return 0;
		}

		int retValue = -1;
		if (m_ConsumeDataType == ConsumeDataType::String)
		{
			retValue = DoProcessStringMsg(reader);
		}
		else
		{
			retValue = DoProcessPayloadMsg(reader);
		}

		return retValue; // read new data
	}
	int ServiceChunkReceiver::DoProcessPayloadMsg(DataReader^ reader)
	{
		unsigned int len = reader->UnconsumedBufferLength;
		Platform::String^  rec = reader->ReadString(m_chunkBufferSize);
		m_pGPIOClientInOut->DoProcessCommand(rec);
		return 1;

	}
	
	int ServiceChunkReceiver::DoProcessStringMsg(DataReader^ reader)
	{
		Platform::String^  rec = reader->ReadString(m_chunkBufferSize);
		if (rec == ("GPIOServiceClient.Started")) {

			Platform::String^ command = m_pGPIOClientInOut->GetGPIClientSendState();
			
			Windows::Storage::Streams::IBuffer^ buf = SocketHelpers::createPayloadBufferfromSendData(command);
			m_acceptingData = true;
			this->SendData(buf);

		}
		else if (rec == ("GPIOServiceClient.Stopped")) {
		//	Platform::String^ command = DoCommands();
		//	Windows::Storage::Streams::IBuffer^ buf = SocketHelpers::createBufferfromSendData(command);
			m_acceptingData = false;
		//	this->SendData(buf);
		}
		else
		if (rec == ("GPIOServiceClient.GetData")) {


		}
		else
		if (rec == ("GPIOServiceClient.LifeCycle")) {
			std::string command = "GPIOServiceClient.LifeCycle";
			Windows::Storage::Streams::IBuffer^ buf = SocketHelpers::createBufferfromSendData(command);
			this->SendData(buf);
		}


		//	m_chunkBufferSize = 0;
		return 1;

	}

	int ServiceChunkReceiver::SearchForChunkMsgLen(DataReader^ reader)
	{
		unsigned int len = reader->UnconsumedBufferLength;
		m_chunkBufferSize = -1;
		unsigned int nread = 6;
		if (reader->UnconsumedBufferLength >= nread)
		{
			m_ConsumeDataType = ConsumeDataType::Undef;
			unsigned int readBufferLen = reader->ReadUInt32();

			byte checkByte1 = reader->ReadByte();
			byte checkByte2 = reader->ReadByte();

			if ((checkByte1 == 0x55) && ((checkByte2 == 0x55) || (checkByte2 == 0x50)))  // prufen, ob stream richtig ist
			// Do Processing - Message
			{

				if (reader->UnconsumedBufferLength >= readBufferLen) { // notwendige Daten sind da, ansonsten müssen weitere empfangen werden
					m_chunkBufferSize = readBufferLen;
					if (checkByte2 == 0x55) {
						m_ConsumeDataType = ConsumeDataType::String;
					}
					else if (checkByte2 == 0x50) {
						m_ConsumeDataType = ConsumeDataType::Binary;
					}
				}

			}
			else
			{ // alles auslesen, neu aufsetzen
				IBuffer^  chunkBuffer = reader->ReadBuffer(reader->UnconsumedBufferLength);
			}

		}

		len = reader->UnconsumedBufferLength;
		return m_chunkBufferSize;
	}

	void ServiceChunkReceiver::DoProcessChunk(DataReader^ reader)
	{
		int chunkMsgLen = 0;
		bool doReadNewData = false;
		while (!doReadNewData)
		{
			switch (m_ChunkProcessing)
			{

			case 0: // search Chunk-Msg-Len
			{

				int nChunkProcess = SearchForChunkMsgLen(reader);

				if (nChunkProcess > 0) {
					m_ChunkProcessing = 1;
					int64 time = 0;
					m_DeltaChunkReceived = time - m_StartChunkReceived;
					m_StartChunkReceived = time;


				}
				else if (nChunkProcess == -1) { // read no buffer
					doReadNewData = true;
				}
				break;
			}
			case 1: // Process-Chunk
			{
				int nChunkProcess = DoProcessChunkMsg(reader);
				if (nChunkProcess == 1) // read new data use same buffer
				{
					m_ChunkProcessing = 0;
				}
				else if (nChunkProcess == -1) // read new buffer
				{
					m_ChunkProcessing = 0;
					doReadNewData = true;
				}
				else if (nChunkProcess == 0) // 
				{// read further data
					doReadNewData = true;
				}
				break;


			}
			default:
			{

			}

			}
		}



	}

	void ServiceChunkReceiver::DoProcessChunk_(DataReader^ reader)
	{
		bool doReadBuffer = false;

		unsigned int nread = 6;
		if (reader->UnconsumedBufferLength > nread)
			// 4 bytes for length and 2 bytes for verifikation
		{
			unsigned int readBufferLen = reader->ReadUInt32();

			byte checkByte1 = reader->ReadByte();
			byte checkByte2 = reader->ReadByte();

			if ((checkByte1 == 0x55) && (checkByte2 == 0x55)) { // prufen, ob stream richtig ist
			// Do Processing - Message

				unsigned int _len = reader->UnconsumedBufferLength;

				if (reader->UnconsumedBufferLength < readBufferLen) doReadBuffer = true;
				else {
					Platform::String^  rec = reader->ReadString(readBufferLen);
					if (rec == ("GPIOServiceClient.Started")) {
						Platform::String^ command = DoCommands();
						Windows::Storage::Streams::IBuffer^ buf = SocketHelpers::createBufferfromSendData(command);
						m_acceptingData = true;
						this->SendData(buf);

					}
					else if (rec == ("GPIOServiceClient.Stopped")) {
						Platform::String^ command = DoCommands();
						Windows::Storage::Streams::IBuffer^ buf = SocketHelpers::createBufferfromSendData(command);
						m_acceptingData = false;
						this->SendData(buf);
					}
					else
					if (rec == ("GPIOServiceClient.GetData")) {


					}
					else
					if (rec == ("GPIOServiceClient.LifeCycle")) {
						std::string command = "GPIOServiceClient.LifeCycle";
						Windows::Storage::Streams::IBuffer^ buf = SocketHelpers::createBufferfromSendData(command);
						this->SendData(buf);
					}
					else
					{
						m_pGPIOClientInOut->DoProcessCommand(rec);
	
			//			String^ state = m_pGPIOInOut->GetGPIState();

					}

				}
			}
			else doReadBuffer = true;
			if (doReadBuffer)
			{ // alles auslesen
				IBuffer^  chunkBuffer = reader->ReadBuffer(reader->UnconsumedBufferLength);
			}

		}

	}

}