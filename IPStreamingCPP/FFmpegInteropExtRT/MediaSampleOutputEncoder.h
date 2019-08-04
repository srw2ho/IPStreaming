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
//#include "..\..\..\..\FFmpegInterop\Source\IpCamera\Connection.h"
//#include "..\..\..\..\FFmpegInterop\Source\IpCamera\IPCamera.h"
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
//using namespace IpCamera;



namespace FFmpegInteropExtRT
{
	/*
	typedef struct {
		AVFrame *m_pAvFrame;
		AVPacket *m_pAvPacket;
		int m_stream_Index;
	} FramePacket;
	*/

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


	class MediaSampleEncoding
	{
	public:
			MediaSampleEncoding(AVFormatContext* inputFormaCtx);

			virtual ~MediaSampleEncoding();

			virtual int AddEncoding(AVMediaType codeType, AVCodecID codec_id, int fps, int height, int width, int64_t bit_rate);
			virtual int SetEncoding(AVFormatContext* m_pAvOutFormatCtx, int streamIdx) { return 0; };// add encoding for output device


		//	virtual int DecodeAVPacket(AVPacket* avPacket);
			virtual AVFrame* prepare_write_frame(FramePacket* Packet);
			virtual int EncodeAVPacket(AVFrame* pFrame, AVPacket**encpacket);
			virtual int flushEncoder(AVPacket**encpacket);

			virtual bool IsTimeForNewFile() { return false; }
			virtual bool IsInputStreamCopy() { return false; }; // should Input Stram be copied

			virtual bool WritePackage(AVPacket* encodePacket) {	return false;};
	
			AVCodecID getCodecId() {return m_codec_id;};
			AVMediaType getMediaType();
			void setCodecContext(AVCodecContext*pCodexCnt) { m_pAvCodecCtx = pCodexCnt; };

			AVCodecContext getCodecContext() { return *m_pAvCodecCtx; };

			void setStreamIdx(int steamIdx) {  m_streamIndex = steamIdx; };
			void setCodecId(AVCodecID codeID) {m_codec_id = codeID; };
			void setEncCodecContext(AVCodecContext*pCodexCnt) {m_pAvEnCodecCtx = pCodexCnt; };

			AVCodecContext* getEncCodecContext() { return m_pAvEnCodecCtx; };

			int getStreamIdx() { return m_streamIndex; };


			AVFrame* getEncodeFrame() { return m_pAvEncFrame; };
			SwsContext* getEncodeSwsCtx() { return m_pEncSwsCtx; };
			AVFrame* getDecodeFrame() { return m_pAvDecFrame; };


			int64_t getframeNumber() { return m_frameNumber; };
			void setframeNumber(int64_t frameNumber) { m_frameNumber = frameNumber; };
		//	int64_t getOverrunpts() { return m_Overrunpts; };
		//	void setOverrunpts(int64_t pts) { m_Overrunpts = pts; };
			int64_t getOutputpts() { return m_Outputpts; };
			void setState(int state) { m_state = state; }; //create-State: 0 = 0.K.
			int getState() { return m_state; };
			virtual void Setpts_Overrun() {};
		protected:

			virtual int encode_write_Videoframe(FramePacket* Packet, AVPacket**encpacket);
			virtual int encode_write_Audioframe(FramePacket* Packet, AVPacket**encpacket);
			virtual int PrepareEncodingContext();

			virtual int AddEncoding(int streamIdx, AVCodecID codec_id, int fps, int height, int width, int64_t bit_rate);
			virtual void rescaleEncodePacket(AVPacket*enc_pkt);

			int encode(AVCodecContext *avctx, AVPacket *pkt,  AVFrame *frame, int *got_packet);
			int encodeandProcess(AVCodecContext *avctx, AVFrame *frame);
			void UnLock();
			void Lock();
	
		protected:
			CRITICAL_SECTION m_CritLock;
			AVFormatContext* m_pAvFormatCtx;
			AVCodecContext* m_pAvCodecCtx;
			int m_streamIndex;

			AVCodecID m_codec_id;
			AVCodecContext *m_pAvEnCodecCtx;
			AVFrame* m_pAvEncFrame;
			AVFrame* m_pAvDecFrame;
			SwsContext* m_pEncSwsCtx;
			SwrContext* m_pEncSwrCtx;
			
			int64_t m_frameNumber;
			int64_t m_Inputpts;
			int64_t m_Outputpts;
	//		int64_t m_Overrunpts;
			int m_samples_count;
			int m_state;
					
	};

	class MediaSampleMpegEncoding:public MediaSampleEncoding
	{
	public:
		MediaSampleMpegEncoding(AVFormatContext* inputFormaCtx);
		virtual ~MediaSampleMpegEncoding();
	//	virtual bool WritePackage(AVPacket* encodePacket) ;

	};

	class MediaSampleMJpegEncoding :public MediaSampleEncoding
	{
	public:
		MediaSampleMJpegEncoding(AVFormatContext* inputFormaCtx);
		virtual ~MediaSampleMJpegEncoding();
	
	};

	class MediaSampleFFMpegEncoding :public MediaSampleEncoding
	{
	public:
		MediaSampleFFMpegEncoding(AVFormatContext* inputFormaCtx, double RecordingInHours);
		virtual ~MediaSampleFFMpegEncoding();

		virtual int SetEncoding(AVFormatContext* pAvOutFormatCtx, int streamIdx);// add encoding for output device

		virtual void SetAvOutStream(AVStream* pAvOutStream) { m_pAvOutStream = pAvOutStream; };
		virtual AVStream* getAvOutStream() { return m_pAvOutStream; };
		virtual bool IsTimeForNewFile();
		virtual void SetRecordingInHours(double RecordingInHours) { m_RecordingInHours = RecordingInHours; };

	//	virtual void Setpts_Overrun() { m_pts_Overrun = m_Outputpts;};


	protected:
		virtual void rescaleEncodePacket(AVPacket*enc_pkt);
		double calculateOverrunTimeforfulHournanoSecs();
		void calculateRecordingTime();

	protected:
		AVFormatContext* m_pAvOutFormatCtx;
		AVStream* m_pAvOutStream;
		double m_OverrunTimeinSec;
		int64_t m_pts_Overrun;
		bool m_bTimeForNewFile;
		double m_RecordingInHours; // Recording time = (m_RecordingInHours* 1hours ) per File
//		int64_t m_writeLastpts;

	};


	class MediaSampleFFMpegCopy :public MediaSampleFFMpegEncoding
	{
	public:
		MediaSampleFFMpegCopy(AVFormatContext* inputFormaCtx, double RecordingInHours);
		virtual ~MediaSampleFFMpegCopy();

		virtual AVFrame* prepare_write_frame(FramePacket* Packet);
		virtual void SetAvOutStream(AVStream* pAvOutStream) { };
		virtual bool IsInputStreamCopy() { return true; }; // should Input Stram be copied


	protected:
		//virtual void rescaleEncodePacket(AVPacket*enc_pkt);

	protected:


	};


	

}