//*****************************************************************************
//
//	Copyright 2015 Microsoft Corporation
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
#include "FFmpegReader.h"

using namespace FFmpegInteropExtRT;

FFmpegReader::FFmpegReader(AVFormatContext* avFormatCtx, timeout_handler*timeouthandler)
	: m_pAvFormatCtx(avFormatCtx)
	, m_audioStreamIndex(AVERROR_STREAM_NOT_FOUND)
	, m_videoStreamIndex(AVERROR_STREAM_NOT_FOUND)
{
	m_ptimeouthandler = timeouthandler;
//	m_pOutputEncoding = pOutputEncoding;
	m_ErrConter = 0;
}

FFmpegReader::~FFmpegReader()
{
}

// Read the next packet from the stream and push it into the appropriate
// sample provider
int FFmpegReader::ReadPacket()
{
	int ret;
	AVPacket avPacket;
	av_init_packet(&avPacket);
	avPacket.data = NULL;
	avPacket.size = 0;
	if (m_ptimeouthandler!=nullptr)	m_ptimeouthandler->setTimeoutinMS(10000);// 10000 ms Timeout
	ret = av_read_frame(m_pAvFormatCtx, &avPacket);
	if (ret < 0)
	{
		char buf[AV_ERROR_MAX_STRING_SIZE];
		av_make_error_string(buf, sizeof(buf), ret);
		av_log(NULL, AV_LOG_ERROR, "#### error %d = av_read_frame(), error = %s", ret, (char*)buf);
	//	Trace("#### error %d = av_read_frame(), error = %s", ret, (char*)buf);

		if (ret == (EAGAIN)) {
			ret = 0;
		}
		else if (ret == (AVERROR_EOF)) {
			if (m_ErrConter < 6)
			{
				ret = 0;
			}
			else
			{
				bool b = true;
			}
		}
		else if (ret == (AVERROR_EXIT)) {
			bool b = true;
		}
		m_ErrConter++;
		return ret;
	}
	else if (m_ErrConter > 0) {
		m_ErrConter = 0;
	}

	// Push the packet to the appropriate
	if (avPacket.stream_index == m_audioStreamIndex && m_audioSampleProvider != nullptr)
	{
		m_audioSampleProvider->QueuePacket(avPacket);
	}
	else if (avPacket.stream_index == m_videoStreamIndex && m_videoSampleProvider != nullptr)
	{
		m_videoSampleProvider->QueuePacket(avPacket);
	}
	else
	{
		DebugMessage(L"Ignoring unused stream\n");
		av_packet_unref(&avPacket);
	}

	return ret;
}

void FFmpegReader::SetAudioStream(int audioStreamIndex, MediaSampleProvider^ audioSampleProvider)
{
	m_audioStreamIndex = audioStreamIndex;
	m_audioSampleProvider = audioSampleProvider;
	if (audioSampleProvider != nullptr)
	{
		audioSampleProvider->SetCurrentStreamIndex(m_audioStreamIndex);
	}
}

void FFmpegReader::SetVideoStream(int videoStreamIndex, MediaSampleProvider^ videoSampleProvider)
{
	m_videoStreamIndex = videoStreamIndex;
	m_videoSampleProvider = videoSampleProvider;
	if (videoSampleProvider != nullptr)
	{
		videoSampleProvider->SetCurrentStreamIndex(m_videoStreamIndex);
	}
}

