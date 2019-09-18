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

#include "MediaSampleOutputEncoder.h"

using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;

namespace FFmpegInteropExtRT
{

	class MediaSampleFFMpegEncoding :public MediaSampleEncoding
	{
	public:
		MediaSampleFFMpegEncoding(AVFormatContext* inputFormaCtx, PropertySet^ configOptions);
		virtual ~MediaSampleFFMpegEncoding();

		virtual int SetEncoding(AVFormatContext* pAvOutFormatCtx, int streamIdx);// add encoding for output device

		virtual void SetAvOutStream(AVStream* pAvOutStream) { m_pAvOutStream = pAvOutStream; };
		virtual AVStream* getAvOutStream() { return m_pAvOutStream; };
		virtual bool IsTimeForNewFile();
		virtual void SetRecordingInHours(double RecordingInHours) { m_RecordingInHours = RecordingInHours; };

		virtual void writeNewHeader();

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
	//	int64_t m_pts;
	//	int64_t m_dts;

		int64_t m_Outpts;
		int64_t m_Outdts;

	};


	class MediaSampleFFMpegCopy :public MediaSampleFFMpegEncoding
	{
	public:
		MediaSampleFFMpegCopy(AVFormatContext* inputFormaCtx, PropertySet^ configOptions);
		virtual ~MediaSampleFFMpegCopy();

		virtual AVFrame* prepare_write_frame(FramePacket* Packet);
		virtual void SetAvOutStream(AVStream* pAvOutStream) { };
		virtual bool IsInputStreamCopy() { return true; }; // should Input Stram be copied

		virtual int flushEncoder(AVPacket**encpacket);
	protected:
		//virtual void rescaleEncodePacket(AVPacket*enc_pkt);

	protected:


	};


	

}