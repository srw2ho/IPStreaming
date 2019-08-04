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


#include "MediaSampleOutputCamera.h"



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
//using namespace IpCamera;

CameraOutputDevice::CameraOutputDevice(Platform::String^ deviceName,AVFormatContext* inputFormaCtx, CameraServer^ pCameraServer)
	: MediaSampleOutputDevice(deviceName,inputFormaCtx) {

	m_pCameraServer = pCameraServer;
}

CameraOutputDevice::~CameraOutputDevice() 
{
	

}

bool CameraOutputDevice::IsEncodingOutPutDevice()
{
	bool found = false;
	if (this->m_pCameraServer == nullptr) return false;

	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end(); it++)
	{
		MediaSampleEncoding*pEnc = *it;

		if (pEnc->getCodecId() == AV_CODEC_ID_MJPEG) {
			if (this->m_pCameraServer->HasConnectionFromTypeHttpMJpeg()) { // check for Active Connections
				return true;
			}
		}
		else if (pEnc->getCodecId() == AV_CODEC_ID_MPEG1VIDEO) {
			if (this->m_pCameraServer->HasConnectionFromTypeHttpMpeg()) { // check for Active Connections
				return true;
			}
		}

	}

	return false;

}

IBuffer^ CameraOutputDevice::CreateHttpMpegChunksfromAvPacket(AVPacket* avPacket)
{
	ostringstream partHeader;
	char chunkbuf[100];

	sprintf_s(chunkbuf, "%05X\r\n", avPacket->size);
	//	sprintf_s(chunkbuf, "%X\r\n", 50);


	//partHeader << buffer->Length << "\r\n";

	const char partFooter[] = "\r\n";

	partHeader << chunkbuf;

	//const char partFooter[] = "\r\n\r\n";

	auto part = ref new Buffer((unsigned int)(partHeader.str().length() + avPacket->size + sizeof(partFooter) - 1));
	part->Length = part->Capacity;

	unsigned char *data = GetData(part);

	(void)memcpy(data, partHeader.str().c_str(), partHeader.str().length());
	(void)memcpy(data + partHeader.str().length(), avPacket->data, avPacket->size);
	(void)memcpy(data + partHeader.str().length() + avPacket->size, partFooter, sizeof(partFooter) - 1);

	//(void)memcpy(data, GetData(buffer), buffer->Length);
	//Trace("@ created HTTP part @%p of length %iB", (void*)part, part->Length);

	return part;
}
IBuffer^ CameraOutputDevice::CreateHttMJpegChunksFromAvPacket(AVPacket* avPacket)
{
	ostringstream partHeader;
	partHeader << "Content-Type: image/jpeg\r\nContent-length: " << avPacket->size << "\r\n\r\n";

	const char partFooter[] = "\r\n--CameraServerBoundary\r\n";

	auto part = ref new Buffer((unsigned int)(partHeader.str().length() + avPacket->size + sizeof(partFooter) - 1));
	part->Length = part->Capacity;

	unsigned char *data = GetData(part);

	(void)memcpy(data, partHeader.str().c_str(), partHeader.str().length());
	(void)memcpy(data + partHeader.str().length(), (avPacket->data), avPacket->size);
	(void)memcpy(data + partHeader.str().length() + avPacket->size, partFooter, sizeof(partFooter) - 1);

	//(void)memcpy(data, GetData(buffer), buffer->Length);
//	Trace("@ created HTTP part @%p of length %iB", (void*)part, part->Length);

	return part;
}
/*
IBuffer^ CameraOutputDevice::CreateOpenCVChunksFromAvFrame(AVFrame* avFrame)
{
	auto pix_format = AVPixelFormat(avFrame->format);
	int framebufferSize = av_image_get_buffer_size(pix_format, avFrame->width, avFrame->height,1);



	DataWriter^ dataWriter = ref new DataWriter();
	int fullBufferSize = framebufferSize + 4 * sizeof(int);
	// TransferLen + Pixel-Format + Width + Hight 
	
	Platform::Array<uint8_t>^ chunkArray = ref new Platform::Array<uint8_t>(fullBufferSize);
	
	int* len = (int*) &chunkArray[0];

	int* pixelformat = (int*)&chunkArray[2];
	int* width = (int*)&chunkArray[4];
	int* height = (int*)&chunkArray[6];

	*pixelformat = pix_format;
	*width = avFrame->width;

	*height = avFrame->height;



	unsigned int writeLen = 0;
	dataWriter->WriteInt16(avFrame->format);		//Pixel-Format
	dataWriter->WriteInt16(avFrame->width);			// Width
	dataWriter->WriteInt16(avFrame->height);		// Hight

	writeLen = sizeof(short) * 3;
	int i = 0;
	while (avFrame->linesize[i] != 0)
	{
		dataWriter->WriteInt16(avFrame->linesize[i]);		// line-Size-Len
		writeLen += sizeof(short);
		Platform::Array<uint8_t>^ YBuffer = ref new Platform::Array<uint8_t>(avFrame->data[i], avFrame->linesize[i]);
		memcpy(&chunkArray[10], &YBuffer[0], YBuffer->Length);

		dataWriter->WriteBytes(YBuffer);		// line-Size-Len

	}
	
	dataWriter->WriteBytes(chunkArray);		// line-Size-Len

	return dataWriter->DetachBuffer();
}
*/



bool CameraOutputDevice::WritePackage(MediaSampleEncoding* pEncoding, AVPacket* encodePacket) {

	//	Trace("@ WritePackage");
//	Trace("@%p", (void*)this);

	if (this->m_pCameraServer != nullptr) {
		if (pEncoding->getCodecId() == AV_CODEC_ID_MJPEG) {
			if (this->m_pCameraServer->HasConnectionFromTypeHttpMJpeg()) 
			{ // check for Active Connections
				IBuffer ^ Senddata = this->CreateHttMJpegChunksFromAvPacket(encodePacket);
				this->m_pCameraServer->DispatchHttpMJpegVideoPart(Senddata); // Send data to all clients
			}

		}
		else	if (pEncoding->getCodecId() == AV_CODEC_ID_MPEG1VIDEO) {
			if (this->m_pCameraServer->HasConnectionFromTypeHttpMpeg()) 
			{ // check for Active Connections
				IBuffer ^ Senddata = this->CreateHttpMpegChunksfromAvPacket(encodePacket);
				this->m_pCameraServer->DispatchHttpMpegVideoPart(Senddata); // Send data to all clients}
			}
		}
	}

	return false;

}

