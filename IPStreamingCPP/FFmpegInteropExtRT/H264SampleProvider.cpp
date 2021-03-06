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
#include "H264SampleProvider.h"

using namespace FFmpegInteropExtRT;

H264SampleProvider::H264SampleProvider(
	FFmpegReader^ reader,
	AVFormatContext* avFormatCtx,
	AVCodecContext* avCodecCtx, MediaSampleOutputEncoding* pOutputEncoding)
	: MediaSampleProvider(reader, avFormatCtx, avCodecCtx, pOutputEncoding)
{
	m_bHasSentExtradata = false;
}

H264SampleProvider::~H264SampleProvider()
{
}
void H264SampleProvider::Flush()
{		
	MediaSampleProvider::Flush();
	m_bHasSentExtradata = false;
}


HRESULT H264SampleProvider::DecodeAVPacket(DataWriter^ dataWriter, AVPacket* avPacket, int64_t& framePts, int64_t& frameDuration)
{
	HRESULT hr = S_OK;
	// srw2ho, 05.04.2020: for reducing IP-streaming latency we start with a key-Frame
	// ohther frames are skipped
	if (!m_bHasSentExtradata) {

		if (avPacket->flags & AV_PKT_FLAG_KEY)
		{
			hr = S_OK;
			m_bHasSentExtradata = true;
		}
		else {
			hr = S_FALSE;
		}

	}

	if (hr==S_OK)
	{
		hr = MediaSampleProvider::DecodeAVPacket(dataWriter, avPacket, framePts,  frameDuration);
	}

	// For the simple case of compressed samples, each packet is a sample

	return hr;
}

HRESULT H264SampleProvider::WriteAVPacketToStream(DataWriter^ dataWriter, AVPacket* avPacket)
{
	HRESULT hr = S_OK;

	if (!m_bHasSentExtradata)
	{
		dataWriter = ref new DataWriter();
		hr = GetSPSAndPPSBuffer(dataWriter, m_pAvCodecCtx->extradata, m_pAvCodecCtx->extradata_size);
		m_bHasSentExtradata = true;
	}

	// On a KeyFrame, write the SPS and PPS
	if (avPacket->flags & AV_PKT_FLAG_KEY)
	{
		hr = GetSPSAndPPSBuffer(dataWriter, m_pAvCodecCtx->extradata, m_pAvCodecCtx->extradata_size);
	}

	if (SUCCEEDED(hr))
	{
		hr = WriteNALPacket(dataWriter, avPacket);

	//	hr = MediaSampleProvider::WriteAVPacketToStream(dataWriter, avPacket);
	}

	// We have a complete frame
	return hr;
}


// Write out an NAL packet converting stream offsets to start - codes
HRESULT H264SampleProvider::WriteNALPacket(DataWriter^ dataWriter, AVPacket* avPacket)
{
	// Call base class method that simply write the packet to stream as is
	HRESULT hr = S_OK;
	hr = MediaSampleProvider::WriteAVPacketToStream(dataWriter, avPacket);;
	return hr;


}

HRESULT H264SampleProvider::GetSPSAndPPSBuffer(DataWriter^ dataWriter, byte* buf, int length)
{
	HRESULT hr = S_OK;
	/*
	if (m_pAvCodecCtx->extradata == nullptr && m_pAvCodecCtx->extradata_size < 8)
	{
		// The data isn't present
		hr = E_FAIL;
	}
	*/
	if (buf == nullptr || length < 8)
	{
		// The data isn't present
		hr = E_FAIL;
	}
	else
	{
		// Write both SPS and PPS sequence as is from extradata
		auto vSPSPPS = ref new Platform::Array<uint8_t>(m_pAvCodecCtx->extradata, m_pAvCodecCtx->extradata_size);
		dataWriter->WriteBytes(vSPSPPS);
	}

	return hr;
}