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


#include "MediaSampleOutputEncoderBase.h"

namespace FFmpegInteropExtRT
{


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


		virtual bool WritePackage(AVPacket* encodePacket) { return false; };

		AVCodecID getCodecId() { return m_codec_id; };
		AVMediaType getMediaType();
		void setCodecContext(AVCodecContext*pCodexCnt) { m_pAvCodecCtx = pCodexCnt; };

		AVCodecContext getCodecContext() { return *m_pAvCodecCtx; };

		void setStreamIdx(int steamIdx) { m_streamIndex = steamIdx; };
		void setCodecId(AVCodecID codeID) { m_codec_id = codeID; };
		void setEncCodecContext(AVCodecContext*pCodexCnt) { m_pAvEnCodecCtx = pCodexCnt; };

		AVCodecContext* getEncCodecContext() { return m_pAvEnCodecCtx; };

		int getStreamIdx() { return m_streamIndex; };

		virtual AVStream* getAvOutStream() { return nullptr; };

		AVFrame* getEncodeFrame() { return m_pAvEncFrame; };
		SwsContext* getEncodeSwsCtx() { return m_pEncSwsCtx; };
		AVFrame* getDecodeFrame() { return m_pAvDecFrame; };


		int64_t getframeNumber() { return m_frameNumber; };
		void setframeNumber(int64_t frameNumber) { m_frameNumber = frameNumber; };
		//	int64_t getOverrunpts() { return m_Overrunpts; };
		//	void setOverrunpts(int64_t pts) { m_Overrunpts = pts; };
	//	int64_t getOutputpts() { return m_Outputpts; };
		//	int64_t getOutputdts() { return m_Outputdts; };
		void setState(int state) { m_state = state; }; //create-State: 0 = 0.K.
		int getState() { return m_state; };
		virtual void Setpts_Overrun() {};
		virtual void writeNewHeader() {};

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
			//int64_t m_Inputpts;
			int64_t m_pts;
			int64_t m_dts;

		//	int64_t m_Outputpts;
		//	int64_t m_Outputdts;
	//		int64_t m_Overrunpts;
			int m_samples_count;
			int m_state;

					
	};
	

	

}