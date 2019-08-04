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


//#include "FFmpegInteropMSS.h"
//#include "MediaSampleOutputEncoder.h"

#include "MediaSampleOutputDeviceBase.h"


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





MediaSampleOutputDevice::MediaSampleOutputDevice(Platform::String^ deviceName, AVFormatContext* inputFormaCtx) {

	m_pAvFormatCtx = inputFormaCtx;
	m_pEncodings = new MediaSampleEncodingList();
	this->m_strdeviceName = deviceName;
}

MediaSampleOutputDevice::~MediaSampleOutputDevice() {

	DeleteAllEncodings();
	delete m_pEncodings;
	m_pEncodings = nullptr;

}

bool MediaSampleOutputDevice::DeleteEncoding(MediaSampleEncoding*pdeleteEncoding) {
	bool found = false;

	if (m_pEncodings == nullptr) return false;
	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end();it++)
	{
		MediaSampleEncoding*pEnc = *it;
		if (pEnc == pdeleteEncoding)
		{
		//	this->m_pEncodings->remove(pEnc);
			this->m_pEncodings->erase(it);
			delete pEnc;
			found = true;
			break;
		}

	}


	return found;

}
void MediaSampleOutputDevice::DeleteAllEncodings(void) {
	if (m_pEncodings == nullptr) return;

	while (!this->m_pEncodings->empty())
	{
		MediaSampleEncoding*pEnc = this->m_pEncodings->back();
		this->m_pEncodings->pop_back();
		delete pEnc;

	}


}

bool MediaSampleOutputDevice::IsMediaTypeEncodingOutput(AVMediaType type) // is Video or Audio-Encoding  
{
	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end();it++)
	{
		MediaSampleEncoding*pEnc = *it;
		if (pEnc->getMediaType() == type) return true;
	}

	return false;

}




bool MediaSampleOutputDevice::IsStreamEncodingOutput(int stream_Idx, int& encType) // is Straem-Idx encoding 
{
	bool ret = false;
	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end();it++)
	{
		MediaSampleEncoding*pEnc = *it;
		if (pEnc->getStreamIdx() == stream_Idx) {
			if (pEnc->IsInputStreamCopy()) encType = encType | EncodeTypes::CopyPacket;
			else encType = encType | EncodeTypes::EncodePacket;
			ret = true;
		}
	}

	return ret;

}

/*

bool MediaSampleOutputDevice::IsStreamCopyingOutput(int stream_Idx) // is Straem-Idx encoding 
{
	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end();it++)
	{
		MediaSampleEncoding*pEnc = *it;
		if (pEnc->getStreamIdx() == stream_Idx) {
			return pEnc->IsInputStreamCopy(); // should Input Stram be copied
											   //		return true;
		}
	}

	return false;

}
*/

bool MediaSampleOutputDevice::IsCopyOutPutDevice() 
{
	bool found = false;


	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end();it++)
	{
		MediaSampleEncoding*pEnc = *it;
		if (pEnc->IsInputStreamCopy()) {//copy frame
			return true;
		}
	}

	return false;

}

bool MediaSampleOutputDevice::IsEncodingOutPutDevice()
{
	bool found = false;


	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end();it++)
	{
		MediaSampleEncoding*pEnc = *it;
		if (!pEnc->IsInputStreamCopy()) {//encoding
			return true;
		}
	}

	return false;

}



bool MediaSampleOutputDevice::CopyAndWriteFrames(FramePacket* avPacket) {
	bool found = false;

//	bool bTimeforNewFile = (this->m_pEncodings->size() > 0);
	bool bTimeforNewFile = false;

	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end();it++)
	{
		AVPacket*encpacket = nullptr;
		MediaSampleEncoding*pEnc = *it;
		bTimeforNewFile = pEnc->IsTimeForNewFile() || bTimeforNewFile; // Only Video-File looking
		if (avPacket->getstream_Index() != pEnc->getStreamIdx()) {
			continue;
		}
		this->CopyFrame(pEnc, avPacket);
	}

	if (bTimeforNewFile) {
		reopen_outputAsync();
	}

	return found;

}

bool MediaSampleOutputDevice::EncodeAndWriteFrames(FramePacket* avPacket) {
	bool found = false;

	//bool bTimeforNewFile  = (this->m_pEncodings->size() > 0);
	bool bTimeforNewFile = false;

	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end();it++)
	{

		AVPacket*encpacket = nullptr;
		MediaSampleEncoding*pEnc = *it;
		bTimeforNewFile = pEnc->IsTimeForNewFile() || bTimeforNewFile; // Only Video-File looking
		if (avPacket->getstream_Index() != pEnc->getStreamIdx()) {
			continue;
		}
		this->EncodeFrame(pEnc, avPacket);

	}
	
	if (bTimeforNewFile) {
		reopen_outputAsync();
	}
	
	return found;

}

bool MediaSampleOutputDevice::CopyFrame(MediaSampleEncoding*pEnc, FramePacket* avPacket) {
	AVPacket*encpacket = nullptr;
	AVFrame*pinutFrame = pEnc->prepare_write_frame(avPacket);
	encpacket = avPacket->getAVPacket();
	bool found = this->WritePackage(pEnc, encpacket); // Write Packet direct to muxer
	return found;

}
bool MediaSampleOutputDevice::EncodeFrame(MediaSampleEncoding*pEnc,FramePacket* avPacket) {
	bool found = false;
	AVPacket*encpacket = nullptr;
	AVFrame*pinutFrame = pEnc->prepare_write_frame(avPacket);
	while (1)
	{
		int ret = 0;
	
		ret = pEnc->EncodeAVPacket(pinutFrame, &encpacket); // Encoding Packet
		pinutFrame = nullptr; // pinutFrame nur einmal zum encoder senden
		if (encpacket != nullptr) {
			found = this->WritePackage(pEnc, encpacket); // Write Packet
			av_packet_free(&encpacket);// free encoded packet
		}
		else break;
		if (ret < 0) break;
		//	Trace("#####EncodeAVPacket-Error()");

	}
	return found;
}
