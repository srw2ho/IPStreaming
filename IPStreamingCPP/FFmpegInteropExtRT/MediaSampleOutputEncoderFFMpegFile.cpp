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

#include "pch.h"
#include "MediaSampleOutputEncoderFFMpegFile.h"

using namespace concurrency;
using namespace FFmpegInteropExtRT;
using namespace std;
using namespace Windows::Storage;

extern "C"
{
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}






MediaSampleFFMpegEncoding::MediaSampleFFMpegEncoding(AVFormatContext* inputFormaCtx, double RecordingInHours) :
	MediaSampleEncoding(inputFormaCtx) {
	m_pAvOutFormatCtx = nullptr;
	m_pAvOutStream = nullptr;

	m_OverrunTimeinSec = 60*60; // 1h = 1min*60  = 60*60 sec
//	m_OverrunTimeinSec = 10; // 1h = 1min*60  = 60*60 sec
	m_bTimeForNewFile = false;
	m_pts_Overrun = 0;

	m_RecordingInHours = RecordingInHours;
	calculateRecordingTime();
//	m_OverrunTimeinSec = 10; // 1h = 1min*60  = 60*60 sec
};

MediaSampleFFMpegEncoding::~MediaSampleFFMpegEncoding() {
	if (m_pAvOutFormatCtx != nullptr) {
//		m_pAvEnCodecCtx = nullptr; // wird alles mit 	mit	avformat_free_context(m_pAvOutFormatCtx); in Destruktor von FFMpegOutputDevice erledigt

	//	av_freep(&m_pAvEnCodecCtx);
	//	m_pAvEnCodecCtx = nullptr;


	}



	

	if (m_pAvOutStream)
	{
		av_freep(&m_pAvOutStream);
		m_pAvOutStream = nullptr;

	}
};




int MediaSampleFFMpegEncoding::SetEncoding(AVFormatContext* pAvOutFormatCtx, int streamIdx) { // add encoding for output device

	m_pAvOutFormatCtx = pAvOutFormatCtx;
	m_streamIndex = streamIdx;
//	m_pAvCodecCtx = this->m_pAvFormatCtx->streams[streamIdx]->codec;
	m_pAvOutStream = m_pAvOutFormatCtx->streams[streamIdx];
//	m_pAvEnCodecCtx = m_pAvOutFormatCtx->streams[streamIdx]->codec;
	m_codec_id = m_pAvEnCodecCtx->codec_id;
	return PrepareEncodingContext();
}



void MediaSampleFFMpegEncoding::calculateRecordingTime() {

	double time = calculateOverrunTimeforfulHournanoSecs(); // Time in nsec
	if (fabs (time) > 1*nano100SecInSec) { // 1 sec in Toleranz
		m_OverrunTimeinSec = double(time / nano100SecInSec) ;// time in secs to full hour
	}
	else {
		m_OverrunTimeinSec = this->m_RecordingInHours * VideoTimeForOneHour + double(time / nano100SecInSec);
	}// time in secs to full hour

}


double MediaSampleFFMpegEncoding::calculateOverrunTimeforfulHournanoSecs()
{

	SYSTEMTIME st;
	FILETIME ft;
	FILETIME ftend;
	LARGE_INTEGER fttime, ftendtime;

	//DateTimeToSystemTime
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);

	if ((st.wMinute == 0) && (st.wSecond <= 1)) // 3 sec Toleranz für ganze Stunde
	{
		st.wMilliseconds = 0;
		st.wSecond = 0;
	}
	else if ((st.wMinute == 59) && (st.wSecond >= 59)) // 3 sec Toleranz für ganze Stunde
	{
		st.wMilliseconds = 999;
		st.wSecond = 59;
	}
	else
	{
		st.wMilliseconds = 999;
		st.wSecond = 59;
		st.wMinute = 59; // volle Stunde aufrunden 
	}

	SystemTimeToFileTime(&st, &ftend);

	fttime.LowPart = ft.dwLowDateTime;
	fttime.HighPart = ft.dwHighDateTime;

	ftendtime.LowPart = ftend.dwLowDateTime;
	ftendtime.HighPart = ftend.dwHighDateTime;

	LARGE_INTEGER deltatime;
	deltatime.QuadPart = ftendtime.QuadPart - fttime.QuadPart;
	double delta = double(deltatime.QuadPart);
//	if (delta < 0) delta = 0;
	return delta;
};

void MediaSampleFFMpegEncoding::rescaleEncodePacket(AVPacket*enc_pkt) {


	if (enc_pkt->pts != AV_NOPTS_VALUE)
	{
		av_packet_rescale_ts(enc_pkt, m_pAvFormatCtx->streams[m_streamIndex]->time_base, this->m_pAvOutStream->time_base);
	}
	else
	{
		enc_pkt->pts = 0;
		enc_pkt->dts = 0;
	}

	enc_pkt->stream_index = m_streamIndex;
	m_Outputpts = enc_pkt->pts;

	//enc_pkt->pts = m_frameNumber;
	//enc_pkt->dts = m_frameNumber;
	//av_packet_rescale_ts(enc_pkt, m_pAvFormatCtx->streams[m_streamIndex]->time_base, this->m_pAvOutStream->time_base);
	m_frameNumber++;

	/*
	av_packet_rescale_ts(enc_pkt, m_pAvFormatCtx->streams[m_streamIndex]->time_base, this->m_pAvOutStream->time_base);
//	av_packet_rescale_ts(enc_pkt, m_pAvOutFormatCtx->streams[this->m_streamIndex]->time_base, m_pAvOutFormatCtx->streams[this->m_streamIndex]->time_base);
	m_frameNumber++;
	m_Outputpts = enc_pkt->pts;
	enc_pkt->stream_index = m_streamIndex;
	*/

}




 bool MediaSampleFFMpegEncoding::IsTimeForNewFile()
 {
	 
	 if (getMediaType() == AVMEDIA_TYPE_VIDEO) { // only for Video-Files
		 double pts = (m_Outputpts - m_pts_Overrun)  * av_q2d(m_pAvOutFormatCtx->streams[this->m_streamIndex]->time_base); //-m_pAvOutFormatCtx->start_time;
		 if (pts > m_OverrunTimeinSec)
		 {
			 m_OverrunTimeinSec = m_RecordingInHours*VideoTimeForOneHour; // m_RecordingInHours* 1h = 1min*60  = 60*60 sec

																		  //	m_OverrunTimeinSec = 10;
			m_pts_Overrun = m_Outputpts;
			m_bTimeForNewFile = true;
			 //		m_frameNumber = 0;
		 }
		 else m_bTimeForNewFile = false;
	 }

	return m_bTimeForNewFile;
}








 MediaSampleFFMpegCopy::MediaSampleFFMpegCopy(AVFormatContext* inputFormaCtx, double RecordingInHours) :
	 MediaSampleFFMpegEncoding(inputFormaCtx, RecordingInHours) {

 };

 MediaSampleFFMpegCopy::~MediaSampleFFMpegCopy() {
	

 };









 AVFrame* MediaSampleFFMpegCopy::prepare_write_frame(FramePacket* Packet)
 {
	 rescaleEncodePacket(Packet->getAVPacket());
	 return nullptr;

 }

 /*
 void MediaSampleFFMpegCopy::rescaleEncodePacket(AVPacket*enc_pkt) {

	 m_frameNumber++;
	 if (enc_pkt->pts != AV_NOPTS_VALUE)
	 {
		 av_packet_rescale_ts(enc_pkt, m_pAvFormatCtx->streams[m_streamIndex]->time_base, this->m_pAvOutStream->time_base);
	 }
	 else
	 {
		 enc_pkt->pts = 0;
		 enc_pkt->dts = 0;
	 }

	 enc_pkt->stream_index = m_streamIndex;
	 m_Outputpts = enc_pkt->pts;

 }
 */








