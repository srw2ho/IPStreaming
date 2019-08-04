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
#include "H264AVCSampleProvider.h"

using namespace FFmpegInteropExtRT;

H264AVCSampleProvider::H264AVCSampleProvider(
	FFmpegReader^ reader,
	AVFormatContext* avFormatCtx,
	AVCodecContext* avCodecCtx, MediaSampleOutputEncoding* pOutputEncoding)
	: H264SampleProvider(reader, avFormatCtx, avCodecCtx, pOutputEncoding)
{

}

H264AVCSampleProvider::~H264AVCSampleProvider()
{
}

/*
HRESULT H264AVCSampleProvider::WriteAVPacketToStream(DataWriter^ dataWriter, AVPacket* avPacket)
{
	HRESULT hr = S_OK;
	// On first frame, write the SPS and PPS
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
		// Convert the packet to NAL format
		hr = WriteNALPacket(dataWriter, avPacket);
	}

	// We have a complete frame
	return hr;
}
*/
/*
HRESULT H264AVCSampleProvider::GetSPSAndPPSBuffer(DataWriter^ dataWriter, byte* buf, int length)
{
	HRESULT hr = S_OK;
	int spsLength = 0;
	int ppsLength = 0;

	// Get the position of the SPS
	if (m_pAvCodecCtx->extradata == nullptr && m_pAvCodecCtx->extradata_size < 8)
	{
		// The data isn't present
		hr = E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		byte* spsPos = m_pAvCodecCtx->extradata + 8;
		spsLength = spsPos[-1];

		if (m_pAvCodecCtx->extradata_size < (8 + spsLength))
		{
			// We don't have a complete SPS
			hr = E_FAIL;
		}
		else
		{
			auto vSPS = ref new Platform::Array<uint8_t>(spsPos, spsLength);

			// Write the NAL unit for the SPS
			dataWriter->WriteByte(0);
			dataWriter->WriteByte(0);
			dataWriter->WriteByte(0);
			dataWriter->WriteByte(1);

			// Write the SPS
			dataWriter->WriteBytes(vSPS);
		}
	}

	if (SUCCEEDED(hr))
	{
		if (m_pAvCodecCtx->extradata_size < (8 + spsLength + 3))
		{
			hr = E_FAIL;
		}

		if (SUCCEEDED(hr))
		{
			byte* ppsPos = m_pAvCodecCtx->extradata + 8 + spsLength + 3;
			ppsLength = ppsPos[-1];

			if (m_pAvCodecCtx->extradata_size < (8 + spsLength + 3 + ppsLength))
			{
				hr = E_FAIL;
			}
			else
			{
				auto vPPS = ref new Platform::Array<uint8_t>(ppsPos, ppsLength);

				// Write the NAL unit for the PPS
				dataWriter->WriteByte(0);
				dataWriter->WriteByte(0);
				dataWriter->WriteByte(0);
				dataWriter->WriteByte(1);

				// Write the PPS
				dataWriter->WriteBytes(vPPS);
			}
		}
	}

	return hr;
}
*/
// Write out an NAL packet converting stream offsets to start - codes
HRESULT H264AVCSampleProvider::WriteNALPacket(DataWriter^ dataWriter,AVPacket* avPacket)
{
	HRESULT hr = S_OK;
	uint32 index = 0;
	uint32 size = 0;
	uint32 packetSize = (uint32)avPacket->size;

	do
	{
		// Make sure we have enough data
		if (packetSize < (index + m_nalLenSize))
		{
			hr = E_FAIL;
			break;
		}

		// Grab the size of the blob
		size = ReadMultiByteValue(avPacket->data, index, m_nalLenSize);
		index += m_nalLenSize;

		// Write the NAL unit to the stream
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(1);

		// Stop if index and size goes beyond packet size or overflow
		if (packetSize < (index + size) || (UINT32_MAX - index) < size)
		{
			hr = E_FAIL;
			break;
		}

		// Write the rest of the packet to the stream
		auto vBuffer = Platform::ArrayReference<uint8_t>(&(avPacket->data[index]), size);
		dataWriter->WriteBytes(vBuffer);
		index += size;
	} while (index < packetSize);

	return hr;
}


HRESULT H264AVCSampleProvider::GetSPSAndPPSBuffer(DataWriter^ dataWriter, byte* buf, int length)
{
	HRESULT hr = S_OK;
	int spsLength = 0;
	int ppsLength = 0;

	// Get the position of the SPS
	if (buf == nullptr || length < 7)
	{
		// The data isn't present
		hr = E_FAIL;
	}

	/* Extradata is in hvcC format */
	int pos = 4;
	m_nalLenSize = (buf[pos++] & 0x03) + 1;

	/* Decode SPS from hvcC. */
	int cnt = buf[pos++] & 0x1f;
	for (int i = 0; i < cnt; i++) {
		int nalsize = ReadMultiByteValue(buf, pos, 2);
		pos += 2;

		if (length - pos < nalsize) {
			return E_FAIL;
		}

		// Write the NAL unit to the stream
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(1);

		auto data = Platform::ArrayReference<uint8_t>(buf + pos, nalsize);
		dataWriter->WriteBytes(data);

		pos += nalsize;
	}

	/* Decode PPS from hvcC. */
	cnt = buf[pos++];
	for (int i = 0; i < cnt; i++) {
		int nalsize = ReadMultiByteValue(buf, pos, 2);
		pos += 2;

		if (length - pos < nalsize) {
			return E_FAIL;
		}

		// Write the NAL unit to the stream
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(1);

		auto data = Platform::ArrayReference<uint8_t>(buf + pos, nalsize);
		dataWriter->WriteBytes(data);

		pos += nalsize;
	}

	return hr;
}

// Write out an H.264 packet converting stream offsets to start-codes
/*
HRESULT H264AVCSampleProvider::WriteNALPacket(DataWriter^ dataWriter, AVPacket* avPacket)
{
	HRESULT hr = S_OK;
	uint32 index = 0;
	uint32 size = 0;
	uint32 packetSize = (uint32)avPacket->size;

	do
	{
		// Make sure we have enough data
		if (packetSize < (index + 4))
		{
			hr = E_FAIL;
			break;
		}

		// Grab the size of the blob
		size = (avPacket->data[index] << 24) + (avPacket->data[index + 1] << 16) + (avPacket->data[index + 2] << 8) + avPacket->data[index + 3];

		// Write the NAL unit to the stream
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(0);
		dataWriter->WriteByte(1);
		index += 4;

		// Stop if index and size goes beyond packet size or overflow
		if (packetSize < (index + size) || (UINT32_MAX - index) < size)
		{
			hr = E_FAIL;
			break;
		}

		// Write the rest of the packet to the stream
		auto vBuffer = ref new Platform::Array<uint8_t>(&(avPacket->data[index]), size);
		dataWriter->WriteBytes(vBuffer);
		index += size;
	} while (index < packetSize);

	return hr;
}
*/

int H264AVCSampleProvider::ReadMultiByteValue(byte* buffer, int index, int numBytes)
{
	if (numBytes == 4)
	{
		return (buffer[index] << 24) + (buffer[index + 1] << 16) + (buffer[index + 2] << 8) + buffer[index + 3];
	}
	if (numBytes == 3)
	{
		return (buffer[index] << 16) + (buffer[index + 1] << 8) + buffer[index + 2];
	}
	if (numBytes == 2)
	{
		return (buffer[index] << 8) + buffer[index + 1];
	}
	if (numBytes == 1)
	{
		return (buffer[index]);
	}
	return -1;
}