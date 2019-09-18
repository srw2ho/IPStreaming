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






MediaSampleFFMpegEncoding::MediaSampleFFMpegEncoding(AVFormatContext* inputFormaCtx, PropertySet^ configOptions) :
	MediaSampleEncoding(inputFormaCtx) {
	m_pAvOutFormatCtx = nullptr;
	m_pAvOutStream = nullptr;

	m_OverrunTimeinSec = 60*60; // 1h = 1min*60  = 60*60 sec
//	m_OverrunTimeinSec = 10; // 1h = 1min*60  = 60*60 sec
	m_bTimeForNewFile = false;
	m_pts_Overrun = 0;
	double RecordingInHours = 10;

	m_Outpts = 0;
	m_Outdts = 0;
	int isMovementActived = -1;

	if (configOptions->HasKey("m_RecordingInHours"))
	{
		Platform::Object^ recordingHours = configOptions->Lookup("m_RecordingInHours");
		RecordingInHours = safe_cast<IPropertyValue^>(recordingHours)->GetDouble();
	}

	if (configOptions->HasKey("m_MovementActivated"))
	{
		Platform::Object^ isActatedvalue = configOptions->Lookup("m_MovementActivated");
		isMovementActived = safe_cast<IPropertyValue^>(isActatedvalue)->GetInt32();

	}
	m_RecordingInHours = RecordingInHours;

	if (isMovementActived > 0) {
		m_OverrunTimeinSec = this->m_RecordingInHours * VideoTimeForOneHour;
	}
	else {
		calculateRecordingTime();
	}

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
	if (fabs(time) > 1 * nano100SecInSec) { // 1 sec in Toleranz
		m_OverrunTimeinSec = double(time / nano100SecInSec);// time in secs to full hour
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

void MediaSampleFFMpegEncoding::writeNewHeader() {
	m_Outpts = 0;
	m_Outdts = 0;
}

void MediaSampleFFMpegEncoding::rescaleEncodePacket(AVPacket*enc_pkt) {


	if (enc_pkt->pts != AV_NOPTS_VALUE)
	{
		av_packet_rescale_ts(enc_pkt, m_pAvFormatCtx->streams[m_streamIndex]->time_base, this->m_pAvOutStream->time_base);

	//	m_pts = enc_pkt->pts;

		int64_t delta_pts;
		int64_t delta_dts ;
		if (m_Outpts > 0) {
			delta_pts = enc_pkt->pts - m_pts;
			delta_dts = enc_pkt->dts - m_dts;
			//		double detaTimeinSec = (m_Outpts - encodePacket->pts)  * av_q2d(pEncoding->getAvOutStream()->time_base); 
			double delta_ptsinSec = (delta_pts)* av_q2d(getAvOutStream()->time_base);

			if (delta_ptsinSec > 5) { // Live-Recording is 5 sec further in time
				if (enc_pkt->duration > 0) {
					delta_pts = enc_pkt->duration;
					delta_dts = enc_pkt->duration;
				}
				else {
					delta_pts = 1;
					delta_dts = 1;
				}
			}
		}
		else {
		
			double detaTimeinSec = (enc_pkt->pts)  * av_q2d(getAvOutStream()->time_base);
			if (detaTimeinSec < 5) { // input stream is started starten
				delta_pts = enc_pkt->pts;
				delta_dts = enc_pkt->dts;
			}
			else 
			{ //new file is created, we start at position 0

				if (enc_pkt->duration > 0) {
					delta_pts = enc_pkt->duration;
					delta_dts = enc_pkt->duration;
				}
				else {
					delta_pts = 1;
					delta_dts = 1;
				}
				// dies ist nicht sauber, kann passieren, dass cur_dts nicht mehr unterstützt wird
				// dann muss nach einer anderen Lösung gesucht werden
				// siehe mux.c
				//if (st->cur_dts && st->cur_dts != AV_NOPTS_VALUE &&
				//	((!(s->oformat->flags & AVFMT_TS_NONSTRICT) &&
				//		st->codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE &&
				//		st->codecpar->codec_type != AVMEDIA_TYPE_DATA &&
				//		st->cur_dts >= pkt->dts) || st->cur_dts > pkt->dts)) {
				//	av_log(s, AV_LOG_ERROR,
				//		"Application provided invalid, non monotonically increasing dts to muxer in stream %d: %s >= %s\n",
				//		st->index, av_ts2str(st->cur_dts), av_ts2str(pkt->dts));
				//	return AVERROR(EINVAL);
				//}
			//	getAvOutStream()->cur_dts = AV_NOPTS_VALUE;
			}
			
		}

		m_Outpts = m_Outpts + delta_pts;
		m_Outdts = m_Outdts + delta_dts;

		m_pts = enc_pkt->pts;
		m_dts = enc_pkt->dts;

		enc_pkt->pts = m_Outpts;
		enc_pkt->dts = m_Outdts;


	}
	else
	{
		enc_pkt->pts = 0;
		enc_pkt->dts = 0;
	}

	enc_pkt->stream_index = m_streamIndex;


	
	m_frameNumber++;


}



bool MediaSampleFFMpegEncoding::IsTimeForNewFile()
{

	if (getMediaType() == AVMEDIA_TYPE_VIDEO) { // only for Video-Files

		
	//	double pts = (m_pts - m_pts_Overrun)  * av_q2d(m_pAvOutStream->time_base); //-m_pAvOutFormatCtx->start_time;
		double pts = (m_Outpts - m_pts_Overrun)  * av_q2d(m_pAvOutStream->time_base); //-m_pAvOutFormatCtx->start_time;

		if (pts > m_OverrunTimeinSec)
		{
			m_OverrunTimeinSec = m_RecordingInHours * VideoTimeForOneHour; // m_RecordingInHours* 1h = 1min*60  = 60*60 sec

																		 //	m_OverrunTimeinSec = 10;
		   // for testing
		   //m_OverrunTimeinSec = 10;
		   // for testing
			m_pts_Overrun = m_Outpts;
			m_bTimeForNewFile = true;
			//		m_frameNumber = 0;
		}
		else m_bTimeForNewFile = false;

	}




	return m_bTimeForNewFile;
}








 MediaSampleFFMpegCopy::MediaSampleFFMpegCopy(AVFormatContext* inputFormaCtx, PropertySet^ configOptions) :
	 MediaSampleFFMpegEncoding(inputFormaCtx, configOptions) {

 };

 MediaSampleFFMpegCopy::~MediaSampleFFMpegCopy() {
	

 };




 int MediaSampleFFMpegCopy::flushEncoder(AVPacket**encpacket) {

	 *encpacket = nullptr;
	 return -1;
 }






 AVFrame* MediaSampleFFMpegCopy::prepare_write_frame(FramePacket* Packet)
 {
	 rescaleEncodePacket(Packet->getAVPacket());
	 return nullptr;

 }










