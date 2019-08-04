//*****************************************************************************
//
//	Copyright 2017  (Willi Schneider)
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//	http ://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.
//
//*****************************************************************************

#pragma once
#include <queue>
#include "FFmpegUty.h"
#include "FFmpegTimeouthandler.h"
#include <ppl.h>
extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

const __int64 nano100SecInWeek = (__int64)10000000 * 60 * 60 * 24 * 7;
const __int64 nano100SecInDay = (__int64)10000000 * 60 * 60 * 24;
const __int64 nano100SecInHour = (__int64)10000000 * 60 * 60;
const __int64 nano100SecInMin = (__int64)10000000 * 60;
const __int64 nano100SecInSec = (__int64)10000000;

const double  VideoTimeForOneHour = 1 * 60 * 60; // 1 hour

const double deletefilesOlderFiles = 48;


using namespace Windows::Storage::Streams;
using namespace Windows::Media::Core;



namespace FFmpegInteropExtRT
{
	class FramePacket {
		public:
			FramePacket() {
				m_pAvFrame = nullptr;
				m_pAvPacket= nullptr;
				m_stream_Index=-1;
				m_bCloned = false;
			};

			FramePacket(const FramePacket*packet) {
				m_pAvFrame = packet->m_pAvFrame;
				m_pAvPacket = packet->m_pAvPacket;
				m_stream_Index = packet->m_stream_Index;
				m_bCloned = false;
			};


			FramePacket(AVFrame*pframe, AVPacket*pAvPacket, int streamIdx) {
				m_pAvFrame = pframe;
				m_pAvPacket = pAvPacket;
				m_stream_Index = streamIdx;
				m_bCloned = false;
			};

			virtual ~FramePacket() {
				if (m_bCloned) {
					if (m_pAvPacket != nullptr) {
						av_packet_unref(m_pAvPacket);
						av_packet_free(&m_pAvPacket);
					}
					if (m_pAvFrame != nullptr) {
						av_frame_free(&m_pAvFrame);
					}
				}
			};


			FramePacket*CloneFramePacket() {

				//	toencppacket->m_pAVPacket = nullptr;
				if (m_pAvFrame != nullptr) {
					m_pAvFrame = av_frame_clone(m_pAvFrame);
				}
			//	else m_pAvFrame = nullptr;
				if (m_pAvPacket != nullptr) {
					m_pAvPacket = av_packet_clone(m_pAvPacket);
				}
		//		else m_pAvPacket = nullptr;

//				m_stream_Index = m_stream_Index;
				m_bCloned = true;
				return this;
			};

			AVFrame*getAVFrame() {return m_pAvFrame;};
			AVPacket*getAVPacket() { return m_pAvPacket; };
			int getstream_Index() { return m_stream_Index; };
			bool IsAvPacketInputSet() { return m_pAvPacket!=nullptr; };
			bool IsAvFrameSet() { return m_pAvFrame != nullptr; };

	protected:
		AVFrame *m_pAvFrame;
		AVPacket *m_pAvPacket;
		int m_stream_Index;
		bool m_bCloned;
	} ;



	template < typename T1>
	class PacketQueue {

	public:
		PacketQueue() {
			m_packetQueue = new std::vector<T1*>();
			InitializeCriticalSection(&m_CritLock);
			m_hWriteEvent = CreateEvent(
				NULL,               // default security attributes
				TRUE,               // manual-reset event
				FALSE,              // initial state is nonsignaled
				nullptr
				//TEXT("WriteEvent")  // object name
			);


		};
		virtual ~PacketQueue() {
			this->Flush();
			delete m_packetQueue;
			DeleteCriticalSection(&m_CritLock);
			CloseHandle(m_hWriteEvent);
		};

		virtual void unrefPacket(T1* Packet) {

		};
		virtual void cancelwaitForPacket() {
			::SetEvent(m_hWriteEvent);
		}

		virtual size_t waitForPacket(T1** Packet, DWORD waitTime = INFINITE) {

			this->Lock();
			*Packet = nullptr;
			size_t size = m_packetQueue->size();
			if (size == 0) {//no packet then wait for
				this->UnLock();
				DWORD dwWaitResult = WaitForSingleObject(m_hWriteEvent, // event handle
					waitTime);    // indefinite wait
				if (dwWaitResult == WAIT_OBJECT_0) {}
				this->Lock();
				size = m_packetQueue->size();
				if (size > 0) {
					*Packet = m_packetQueue->front();
					m_packetQueue->erase(m_packetQueue->begin());

				}
				::ResetEvent(m_hWriteEvent);
				this->UnLock();


			}
			else {
				*Packet = m_packetQueue->front();
				m_packetQueue->erase(m_packetQueue->begin());


				this->UnLock();
			}

			return size;
		};




		virtual void Flush()
		{
			this->Lock();
			while (!m_packetQueue->empty())
			{
				T1* Packet = PopPacket();
				unrefPacket((T1*)Packet);
			}
			this->UnLock();
		};

		virtual void PushPacket(T1* ppacket) {

			this->Lock();
			//		AVPacket * pClonedPacket = av_packet_clone(ppacket);
			m_packetQueue->push_back((T1*)ppacket);
			::SetEvent(m_hWriteEvent);
			this->UnLock();
		};

		virtual void DeleteFirstPackets(size_t size)
		{
			this->Lock();
			while (m_packetQueue->size() > size)
			{
				T1* Packet = PopPacket();
				unrefPacket((T1*)Packet);
			}
			this->UnLock();
		}

		virtual size_t getPacketSize() {
			size_t size = 0;

			this->Lock();
			size = m_packetQueue->size();
			this->UnLock();
			return size;

		};

		virtual T1* PopPacket() {
			T1*pPacketRet = nullptr;
			this->Lock();
			if (!m_packetQueue->empty())
			{
				pPacketRet = m_packetQueue->front();
				//	avPacket = m_packetQueue.front();
				m_packetQueue->erase(m_packetQueue->begin());

				//		m_packetQueue->pop();
				::ResetEvent(m_hWriteEvent);
			}
			this->UnLock();
			return pPacketRet;
		};

		void Lock() {
			EnterCriticalSection(&m_CritLock);
		}

		void UnLock() {
			LeaveCriticalSection(&m_CritLock);
		}

	protected:
		HANDLE m_hWriteEvent;

		std::vector<T1*>* m_packetQueue;
		CRITICAL_SECTION m_CritLock;
		//	std::vector<AVPacket> m_packetQueue;

	};


	class AVPacketQueue :public PacketQueue <AVPacket> {

	public:
		AVPacketQueue() {};
		virtual ~AVPacketQueue() {};

		void PushPacket(AVPacket* ppacket)
		{
			this->Lock();
			AVPacket * pClonedPacket = av_packet_clone(ppacket);
			m_packetQueue->push_back(pClonedPacket);
			::SetEvent(m_hWriteEvent);
			this->UnLock();
		}

		void unrefPacket(AVPacket* Packet)
		{
			if (Packet != nullptr) {

				av_packet_free(&Packet);
			}
		}
	protected:

	};


	class AVFrameQueue :public PacketQueue <FramePacket> {
	public:
		AVFrameQueue() {};
		virtual ~AVFrameQueue() {};

		void PushPacket(FramePacket* ppacket)
		{
			this->Lock();
			FramePacket* toencppacket = new FramePacket(ppacket);
			toencppacket->CloneFramePacket();
/*

			if (ppacket->m_pAvFrame != nullptr) {
				toencppacket->m_pAvFrame = av_frame_clone(ppacket->m_pAvFrame);
			}
			else toencppacket->m_pAvFrame = nullptr;
			if (ppacket->m_pAvPacket != nullptr) {
				toencppacket->m_pAvPacket = av_packet_clone(ppacket->m_pAvPacket);
			}
			else toencppacket->m_pAvPacket = nullptr;

			toencppacket->m_stream_Index = ppacket->m_stream_Index;
			*/
			m_packetQueue->push_back(toencppacket);
			::SetEvent(m_hWriteEvent);
			this->UnLock();
		}



		void unrefPacket(FramePacket* Packet)
		{
			if (Packet != nullptr) {
				/*
				if (Packet->m_pAvPacket != nullptr) {
					av_packet_unref(Packet->m_pAvPacket);
					av_packet_free(&Packet->m_pAvPacket);
				}
				
				if (Packet->m_pAvFrame != nullptr) {
					av_frame_free(&Packet->m_pAvFrame);
				}
				*/
				delete Packet;

			};
		}



	protected:


	};



	

}