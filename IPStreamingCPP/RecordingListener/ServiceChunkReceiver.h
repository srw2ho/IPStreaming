#pragma once
#ifndef SERVICE_SOCKETCHUNK_RECEIVER_H_
#define SERVICE_SOCKETCHUNK_RECEIVER_H_

#include <collection.h>
#include <ppltasks.h>
#include <wrl.h>
#include <robuffer.h>

#include "SocketListener.h"

#include "SocketChunkReceiver.h"


#include "GPIOInOut.h"

namespace RecordingListener
{


	class ServiceChunkReceiver: public StreamSocketComm::SocketChunkReceiver
	{
		enum ConsumeDataType {
			Undef,
			Binary,
			String
		};

		GPIODriver::GPIOInOut^ m_pGPIOClientInOut;



		int m_ConsumeDataType;
	public:
		ServiceChunkReceiver(Windows::Networking::Sockets::StreamSocket^, GPIODriver::GPIOInOut^);
		virtual ~ServiceChunkReceiver();

		virtual Platform::String^ getStartServiceString();


	private:
		virtual void DoProcessChunk(Windows::Storage::Streams::DataReader^ reader);
		virtual void DoProcessChunk_(Windows::Storage::Streams::DataReader^ reader);
		Windows::Storage::Streams::IBuffer^ createBufferfromDataQueue();
		Platform::String^ DoCommands();
		int SearchForChunkMsgLen(Windows::Storage::Streams::DataReader^ reader);
		int DoProcessChunkMsg(Windows::Storage::Streams::DataReader^ reader);
		int DoProcessPayloadMsg(Windows::Storage::Streams::DataReader^ reader);
		int DoProcessStringMsg(Windows::Storage::Streams::DataReader^ reader);
	};

}
#endif /**/