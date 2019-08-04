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


#include "MediaSampleOutputDeviceBase.h"
#include "MediaSampleOutputCamera.h"
#include "MediaSampleOutputFFMpegFile.h"
#include "MediaSampleOutputMuxer.h"


extern "C"
{
#include <libavutil/motion_vector.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>


}

using namespace concurrency;
using namespace FFmpegInteropExtRT;
using namespace std;
using namespace Windows::Storage;



MediaSampleOutputEncoding::MediaSampleOutputEncoding(AVFormatContext* inputFormaCtx) {
	InitializeCriticalSection(&m_CritLock);
	m_pAvFormatCtx = inputFormaCtx;
	m_pOutputDevices = new MediaSampleOutputDevices();

//	m_pEncodings = new MediaSampleEncodingList();
	m_packetQueue = new AVFrameQueue();
	m_pCancelTaskToken = new concurrency::cancellation_token_source();
	m_btaskStarted = false;

	//avcodec_init();
//	avcodec_register_all();

	m_pAvVideoCtx = nullptr;
	m_pAvAudioCtx = nullptr;
	InitCodecContext(m_pAvFormatCtx);

}

MediaSampleOutputEncoding::~MediaSampleOutputEncoding() {
	stopEncoding();
	DeleteAllDevices();
	m_packetQueue->Flush();


	if (m_pAvVideoCtx != nullptr) {
		avcodec_close(m_pAvVideoCtx);

		avcodec_free_context(&m_pAvVideoCtx);
		m_pAvVideoCtx = nullptr;
	}
	if (m_pAvAudioCtx != nullptr) {
		avcodec_close(m_pAvAudioCtx);

		avcodec_free_context(&m_pAvAudioCtx);
		m_pAvAudioCtx = nullptr;
	}



	delete m_pOutputDevices;
	//	delete m_pEncodings;
	delete m_packetQueue;
	delete m_pCancelTaskToken;
	DeleteCriticalSection(&m_CritLock);


};

MediaSampleOutputDevice* MediaSampleOutputEncoding::getDeviceByName(Platform::String^ deviceName) 
{
	std::list<MediaSampleOutputDevice*>::iterator it;
	for (it = this->m_pOutputDevices->begin(); it != this->m_pOutputDevices->end();it++)
	{
		MediaSampleOutputDevice*pDevice = *it;
		if (pDevice->getDeviceName() == deviceName)
		{
			return pDevice;
		}

	}
	return nullptr;


}

void MediaSampleOutputEncoding::InitCodecContext(AVFormatContext* inputFormaCtx) {

	AVCodecParameters *dec_ctx;
	AVFormatContext *ifmt_ctx = this->m_pAvFormatCtx;
	AVStream *in_stream;
	AVCodec* instreamcodec;

	int ret = -1;

	for (size_t streamIdx = 0; streamIdx < ifmt_ctx->nb_streams; streamIdx++) {
		in_stream = ifmt_ctx->streams[streamIdx];
		dec_ctx = in_stream->codecpar;

		bool ret = true;
	//	AVFormatContext *ifmt_ctx = this->m_pAvFormatCtx;



		//m_pAvCodecCtx = ifmt_ctx->streams[streamIdx]->codec;

		in_stream = ifmt_ctx->streams[streamIdx];
		int audioStreamIndex = av_find_best_stream(ifmt_ctx, dec_ctx->codec_type, (int) streamIdx, -1, &instreamcodec, 0);

		if (instreamcodec==nullptr)
//		if (audioStreamIndex!= streamIdx)
		{
			ret = false;
		}

		AVCodecContext* pAvCodecCtx = avcodec_alloc_context3(instreamcodec);
		if (avcodec_parameters_to_context(pAvCodecCtx, in_stream->codecpar) < 0)
		{
			ret = false;
		}
	//	pAvCodecCtx->time_base.den = in_stream->r_frame_rate.num;
	//	pAvCodecCtx->time_base.num = in_stream->r_frame_rate.den;
		pAvCodecCtx->time_base = av_inv_q(in_stream->r_frame_rate);

		if (avcodec_open2(pAvCodecCtx, nullptr, NULL) < 0)

	//	if (avcodec_open2(pAvCodecCtx, instreamcodec, NULL) < 0)
		{
			ret = false;
		}

		if (ret) {
			if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
				m_pAvVideoCtx = pAvCodecCtx;
			}
			if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
				m_pAvAudioCtx = pAvCodecCtx;
			}
		}


	}


}






CameraOutputDevice* MediaSampleOutputEncoding::CreateCameraOutputDevice(Platform::String^ deviceName,CameraServer^ pCameraServer) {

	CameraOutputDevice*pRet = new CameraOutputDevice(deviceName,m_pAvFormatCtx, pCameraServer);
	m_pOutputDevices->push_back(pRet);
	return pRet;

}

FFMpegOutputDevice* MediaSampleOutputEncoding::CreateFFMpegOutputDevice(Platform::String^ deviceName, Platform::String^ folder, int fps, int height, int width, int64_t bit_rate, PropertySet^ ffmpegOutputOptions, Platform::String^ outputformat, double deletefilesOlderFilesinHours, double RecordingInHours) {

	
	bool bcreateCpyDevice = false;

	std::vector<std::wstring> outputarray;

	outputarray = splitintoArray(outputformat->Data(), L".");
	if (outputarray.size() > 1) {
		if (outputarray[0] == L"cpyinput") {// copy input stream to muxer
			outputformat = ref new  Platform::String(outputarray[1].c_str());
			bcreateCpyDevice = true;
		}
	}

	FFMpegOutputDevice*pRet = nullptr;
	if (bcreateCpyDevice) {
		pRet = new FFMpegOutputCopyDevice(deviceName, m_pAvFormatCtx, folder, fps, height, width, bit_rate, ffmpegOutputOptions, outputformat, deletefilesOlderFilesinHours, RecordingInHours);

	}
	else {
		pRet = new FFMpegOutputDevice(deviceName, m_pAvFormatCtx, folder, fps, height, width, bit_rate, ffmpegOutputOptions,outputformat, deletefilesOlderFilesinHours, RecordingInHours);

	}
	m_pOutputDevices->push_back(pRet);
	return pRet;

}







MediaSampleEncoding* CameraOutputDevice::AddMpegEncoding(int fps, int height, int width, int64_t bit_rate) {

	MediaSampleEncoding * encoding = new MediaSampleMpegEncoding(this->m_pAvFormatCtx);
	int ret = encoding->AddEncoding(AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_MPEG1VIDEO, fps, height, width, bit_rate);
	if (ret < 0) {
		delete  encoding;
		return nullptr;
	}

	this->m_pEncodings->push_back(encoding);
	//	psampleprovider->SetOutputEncoding(this);
	//	psampleprovider->PrepareEncoding();
	return encoding;

}

MediaSampleEncoding* CameraOutputDevice::AddMJpegEncoding(int fps, int height, int width, int64_t bit_rate) {

	MediaSampleEncoding * encoding = new MediaSampleMJpegEncoding(this->m_pAvFormatCtx);
	int ret = encoding->AddEncoding(AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_MJPEG, fps, height, width, bit_rate);
	if (ret < 0) {
		delete  encoding;
		return nullptr;
	}

	this->m_pEncodings->push_back(encoding);
	//psampleprovider->SetOutputEncoding(this);
	//psampleprovider->PrepareEncoding();
	return encoding;

}







bool MediaSampleOutputEncoding::EncodeAndWriteFrames(FramePacket* avPacket) {
	bool found = false;

	std::list<MediaSampleOutputDevice*>::iterator it;
	for (it = this->m_pOutputDevices->begin(); it != this->m_pOutputDevices->end();it++)
	{
		MediaSampleOutputDevice*pDevice = *it;
		AVPacket*encpacket = nullptr;
		if (avPacket->IsAvPacketInputSet())
		{
			if (pDevice->IsCopyOutPutDevice())
			{
				found = pDevice->CopyAndWriteFrames(avPacket) | found; // Copy Packet
			}

		}
		else if (avPacket->IsAvFrameSet())
		{
			if (pDevice->IsEncodingOutPutDevice()) {
				found = pDevice->EncodeAndWriteFrames(avPacket) | found; // Encoding Packet
			}

		}

	}

	return found;

}

void MediaSampleOutputEncoding::WriteAVPacketToMuxer(AVPacket* avPacket)
{
	if (m_btaskStarted) {
		//	this->Lock();
		int encType = EncodeTypes::EncodeNothing;
		int streamIdx = avPacket->stream_index;
		bool bOK = IsStreamEncodingOutput(streamIdx, encType); // should Packet be encoded by output, is there an AVMEDIA_TYPE_VIDEO/AVMEDIA_TYPE_AUDIO-Endoder

		if (!bOK) return;

		if ((encType & EncodeTypes::EncodePacket) == EncodeTypes::EncodePacket) {
			DecodeandProcessAVPacket(avPacket);
		}

		if ((encType & EncodeTypes::CopyPacket) == EncodeTypes::CopyPacket)
		{
			ProcessPacket(streamIdx, avPacket);// Packet without decoding copy to muxer
		}


		//	this->UnLock();
	}
	else {
		//	Flush();
	}

}

void MediaSampleOutputEncoding::WriteAVFrameToMuxer(AVFrame* pavFame, AVPacket* avPacket, int streamIdx)
{

	//	this->Lock();
	if (m_btaskStarted) {
		/*
		AVFrameSideData *sd;
		sd = av_frame_get_side_data(pavFame, AV_FRAME_DATA_MOTION_VECTORS);
		if (sd) {
			const AVMotionVector *mvs = (const AVMotionVector *)sd->data;
			for (size_t i = 0; i < sd->size / sizeof(*mvs); i++) {
				const AVMotionVector *mv = &mvs[i];
				char buffer[100];
	
		//		sprintf_s(filebuffer, sizeof(filebuffer), "%s_%s", buffer, filename);

				sprintf_s(buffer, sizeof (buffer), "%d,%2d,%2d,%2d,%4d,%4d,%4d,%4d,%llx\n",
					0, mv->source,
					mv->w, mv->h, mv->src_x, mv->src_y,
					mv->dst_x, mv->dst_y, mv->flags);
				av_log(nullptr, 32, buffer);
				
			}
		}
		*/

		int encType = EncodeTypes::EncodeNothing;

		bool bOK = IsStreamEncodingOutput(streamIdx, encType); // should Packet be encoded by output, is there an AVMEDIA_TYPE_VIDEO/AVMEDIA_TYPE_AUDIO-Endoder

		if (!bOK) return;


		if ((encType & EncodeTypes::EncodePacket) == EncodeTypes::EncodePacket) {
			{
				if (pavFame->pkt_dts != AV_NOPTS_VALUE) {
					pavFame->pts = pavFame->best_effort_timestamp;
					//pavFame->pts = av_frame_get_best_effort_timestamp(pavFame);
				}
				else
				{
					pavFame->pts = 0;
				}
				this->ProcessDecodedFrame(streamIdx, pavFame);
			}


		}

		if ((encType & EncodeTypes::CopyPacket) == EncodeTypes::CopyPacket)
		{// copying input packet for direct muxin
			ProcessPacket(streamIdx, avPacket);// Packet without decoding copy to muxer
		}

		//	this->UnLock();
	}
}





Concurrency::task<void> MediaSampleOutputEncoding::DoEncoding()
{
	auto token = m_pCancelTaskToken->get_token();
	auto tsk = create_task([this, token]()
	{	
		m_btaskStarted = true;
		m_packetQueue->Flush();
	
		bool dowhile = true;
		DWORD waitTime = INFINITE;
		while (dowhile) {
			try {
	
				FramePacket* toEncPaket = nullptr;
				size_t startsize = m_packetQueue->waitForPacket(&toEncPaket, waitTime); //wait for incoming Packet, 50 ms
				if (toEncPaket != nullptr)
				{
					Trace("DoEncoding: getPacketSize() = %d", startsize);
					EncodeAndWriteFrames(toEncPaket);
					m_packetQueue->unrefPacket(toEncPaket);
				}

				if (token.is_canceled()) { cancel_current_task(); }

			}
			catch (const exception& )
			{
				dowhile = false;
			}
		}

	}, token);

	return tsk;

}

void MediaSampleOutputEncoding::startEncoding()
{
	if (m_pOutputDevices->size() == 0) return;// only processing if some do to
	if (m_btaskStarted) return;

	m_btaskStarted = true;
	m_doEncodingTsk = create_task(DoEncoding()).then([this](task<void> previous) 
	{
		m_btaskStarted = false;
		try {
			previous.get();
		}
		catch (const exception& )
		{

		}

	});


}
void MediaSampleOutputEncoding::stopEncoding()
{
	if (!m_btaskStarted) return;
	try {
		m_packetQueue->cancelwaitForPacket();
		m_pCancelTaskToken->cancel();
		Sleep(100);
		m_doEncodingTsk.wait();
	}
	catch (const exception&  )
	{
		bool b = true;
	
	}


}

Concurrency::task<int> MediaSampleOutputEncoding::DostopEncoding()
{
	m_pCancelTaskToken->cancel();
	auto ret = create_task([this]()
	{
		m_doEncodingTsk.wait();
		return 1;
	}).then([this](task<int> previous) // warten auf Ende Task
	{
		int ret = 0;
		try {
			m_btaskStarted = false;
			ret = previous.get();
	
		}
		catch (const task_canceled& )
		{
			ret = -1;

		}
		return ret;
//		return task_from_result(ret);
	});

	return ret;
 }





HRESULT MediaSampleOutputEncoding::GetFrameFromFFmpegDecoder(AVCodecContext*pAvCodecCtx, AVPacket* avPacket)
{
	HRESULT hr = S_OK;
	int decodeFrame = 0;

	if (avPacket != nullptr)
	{
		int sendPacketResult = avcodec_send_packet(pAvCodecCtx, avPacket);
		if (sendPacketResult == AVERROR(EAGAIN))
		{
			// The decoder should have been drained and always ready to access input
			_ASSERT(FALSE);
			hr = E_UNEXPECTED;
		}
		else if (sendPacketResult < 0)
		{
			// We failed to send the packet
			hr = E_FAIL;
			TraceError("#####GetFrameFromFFmpegDecoder-Decoder: failed on the sample");

		//	DebugMessage(L"Decoder failed on the sample\n");
		}
	}
	if (SUCCEEDED(hr))
	{
		AVFrame *pFrame = av_frame_alloc();
		// Try to get a frame from the decoder.
		decodeFrame = avcodec_receive_frame(pAvCodecCtx, pFrame);

		// The decoder is empty, send a packet to it.
		if (decodeFrame == AVERROR(EAGAIN))
		{
			// The decoder doesn't have enough data to produce a frame,
			// return S_FALSE to indicate a partial frame
			hr = S_FALSE;
			av_frame_free(&pFrame);
		//	av_frame_unref(pFrame);
		//	av_freep(pFrame);
		}
		else if (decodeFrame < 0)
		{
			hr = E_FAIL;
			//av_frame_unref(pFrame);
			//av_freep(pFrame);
			av_frame_free(&pFrame);
		//	Trace("#####GetFrameFromFFmpegDecoder-Error()");
			TraceError("#####GetFrameFromFFmpegDecoder-Decoder: Failed to get a frame from the decoder");
		//	DebugMessage(L"Failed to get a frame from the decoder\n");
		}
		else
		{
			m_pAvDecFrame = pFrame;
		}
	}

	return hr;
}

void MediaSampleOutputEncoding::ProcessPacket(int streamIdx,AVPacket* pPacket)
{

	if (m_btaskStarted) {

		if (m_packetQueue->getPacketSize() < 100)  // to avoid buffer ovverun
		{

			FramePacket toencppacket(nullptr, pPacket, streamIdx);
			//		toencppacket.m_pAvPacket = pPacket;
			//		toencppacket.m_pAvFrame = nullptr;
			//		toencppacket.m_stream_Index = streamIdx;
			m_packetQueue->PushPacket(&toencppacket);
		}


	

	}

}

void MediaSampleOutputEncoding::ProcessDecodedFrame(int streamIdx, AVFrame* pFrame)
{

	if (m_btaskStarted) {

	//	FramePacket toencppacket;


		if (m_packetQueue->getPacketSize() < 100) // to avoid buffer ovverun
		{
			FramePacket toencppacket(pFrame, nullptr, streamIdx);

			//toencppacket.m_pAvPacket = nullptr;
			//toencppacket.m_pAvFrame = pFrame;
			//toencppacket.m_stream_Index = streamIdx;
			m_packetQueue->PushPacket(&toencppacket);
		}


	}

}

int MediaSampleOutputEncoding::DecodeandProcessAVPacket(AVPacket* packet) {
	int streamIdx = packet->stream_index;
	int ret = 0;

	AVCodecContext* pAvCodecCtx = nullptr;

	if (m_pAvFormatCtx->streams[streamIdx]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
		pAvCodecCtx = m_pAvVideoCtx;
	}
	else if (m_pAvFormatCtx->streams[streamIdx]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
		pAvCodecCtx = m_pAvAudioCtx;
	}
	else return -1;

	AVPacket*pClonePaket = av_packet_clone(packet);
	AVPacket*pdecoderPacket = pClonePaket;

	HRESULT hr = S_OK;
	bool fGotFrame = false;


//	av_packet_rescale_ts(pClonePaket,	m_pAvFormatCtx->streams[streamIdx]->time_base,	pAvCodecCtx->time_base);

	while (SUCCEEDED(hr))
	{
		hr = GetFrameFromFFmpegDecoder(pAvCodecCtx, pdecoderPacket);
		pdecoderPacket = nullptr;// nur einmalig an den Decoder übergeben
		if (SUCCEEDED(hr))
		{
			if (hr == S_FALSE)
			{
				// If the decoder didn't give an initial frame we still need
				// to feed it more frames. Keep S_FALSE as the result
				if (fGotFrame)
				{
					hr = S_OK;
				}
				break;
			}
			else if (m_pAvDecFrame->pkt_dts != AV_NOPTS_VALUE) {
				m_pAvDecFrame->pts = m_pAvDecFrame->best_effort_timestamp;
	//			m_pAvDecFrame->pts = av_frame_get_best_effort_timestamp(m_pAvDecFrame);
			}
			else
			{
				m_pAvDecFrame->pts = 0;
			}

			// Update the timestamp if the packet has one
			/*
			else if (m_pAvDecFrame->pts != AV_NOPTS_VALUE)
			{
				framePts = m_pAvFrame->pts;
				frameDuration = m_pAvFrame->pkt_duration;
			}*/


	
			fGotFrame = true;

			ProcessDecodedFrame(streamIdx, m_pAvDecFrame);
			av_frame_free(&m_pAvDecFrame);
		}
	}
	av_packet_free(&pClonePaket);
	return fGotFrame ? 1:0;
}


bool MediaSampleOutputEncoding::IsMediaTypeEncodingOutput(AVPacket* avPacket) {
	bool found = false;

	int streamIdx = avPacket->stream_index;

	AVMediaType mediaType = m_pAvFormatCtx->streams[streamIdx]->codecpar->codec_type;

	std::list<MediaSampleOutputDevice*>::iterator it;
	for (it = this->m_pOutputDevices->begin(); it != this->m_pOutputDevices->end();it++)
	{
		MediaSampleOutputDevice*pDevice = *it;
		AVPacket*encpacket = nullptr;
		found = pDevice->IsMediaTypeEncodingOutput(mediaType); // Encoding Packet
		if (found) break;
	}

	return found;

}

bool MediaSampleOutputEncoding::IsStreamEncodingOutput(int streamIdx, int& encType) {
	bool found = false;
	
	encType = EncodeTypes::EncodeNothing;

//	AVMediaType mediaType = m_pAvFormatCtx->streams[streamIdx]->codecpar->codec_type;

	std::list<MediaSampleOutputDevice*>::iterator it;
	for (it = this->m_pOutputDevices->begin(); it != this->m_pOutputDevices->end();it++)
	{
		MediaSampleOutputDevice*pDevice = *it;
		AVPacket*encpacket = nullptr;
		found = pDevice->IsStreamEncodingOutput(streamIdx, encType) || found; // Is stream-Idx encoding
	}

	return found;

}











bool MediaSampleOutputEncoding::DeleteDevice(MediaSampleOutputDevice*pdeleteDevice) {
	bool found = false;

	std::list<MediaSampleOutputDevice*>::iterator it;
	for (it = this->m_pOutputDevices->begin(); it != this->m_pOutputDevices->end();it++)
	{
		MediaSampleOutputDevice*pDev = *it;
		if (pDev == pdeleteDevice)
		{
			this->m_pOutputDevices->remove(pDev);
			delete pDev;
			found = true;
			break;
		}

	}


	return found;

}

void MediaSampleOutputEncoding::DeleteAllDevices(void) {

	while (!this->m_pOutputDevices->empty())
	{
		MediaSampleOutputDevice*pEnc = this->m_pOutputDevices->back();
		this->m_pOutputDevices->pop_back();
		delete pEnc;

	}


}

void MediaSampleOutputEncoding::Lock() {
	EnterCriticalSection(&m_CritLock);
}

void MediaSampleOutputEncoding::UnLock() {

	LeaveCriticalSection(&m_CritLock);
}

