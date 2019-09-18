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

#include <queue>

#include "MediaSampleOutputEncoder.h"
#include "MediaSampleOutputEncoderMPegJPeg.h"
#include "MediaSampleOutputEncoderFFMpegFile.h"

#include <ppl.h>

#include "MediaSampleOutputDeviceBase.h"
#include "MediaSampleOutputCamera.h"
#include "MediaSampleOutputFFMpegFile.h"

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}


using namespace Windows::Storage::Streams;
using namespace Windows::Media::Core;

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;


namespace FFmpegInteropExtRT
{


	typedef std::list<MediaSampleOutputDevice*>  MediaSampleOutputDevices;


	class MediaSampleOutputEncoding
	{


	public:
		MediaSampleOutputEncoding(AVFormatContext* inputFormaCtx);

		virtual ~MediaSampleOutputEncoding();
		CameraOutputDevice* CreateCameraOutputDevice(Platform::String^ deviceName, PropertySet^ configOptions, CameraServer^ pCameraServer );
		FFMpegOutputDevice* CreateFFMpegOutputDevice(Platform::String^ deviceName, PropertySet^ configOptions, PropertySet^ ffmpegOutputOptions);

		virtual void WriteAVPacketToMuxer(AVPacket* avPacket); //Link to m_pOutputEncoding
		virtual void WriteAVFrameToMuxer(AVFrame* avFame, AVPacket* avPacket, int streamIdx); //Link to m_pOutputEncoding

		MediaSampleOutputDevice* getDeviceByName(Platform::String^ deviceName);
		void startEncoding();
		void stopEncoding();
		Concurrency::task<int> DostopEncoding();
		bool DeleteDevice(MediaSampleOutputDevice*pdeleteDevice);
		void DeleteAllDevices(void);
		int DecodeandProcessAVPacket(AVPacket* avPacket);// !! imprtand: must be called into FFmpegReader-Thread , AVFormatContext* inputFormaCtx not threadsafed
		bool IsMediaTypeEncodingOutput(AVPacket* avPacket); // is Video or Audio-Encoding
		bool IsStreamEncodingOutput(int streamIdx, int& encType); // is stream-Idx encoding

		bool EncodeAndWriteFrames(FramePacket* Packet);

		Concurrency::task<void>  DoEncoding();//start encoding task

	//	bool WriteEndFrames();


	protected:
//		int DecodeAudioAVPacket(AVPacket* ppacket);
//		int DecodeVideoAVPacket(AVPacket* ppacket);

		HRESULT GetFrameFromFFmpegDecoder(AVCodecContext*pAvCodecCtx, AVPacket* avPacket);
		void InitCodecContext(AVFormatContext* inputFormaCtx);
		void ProcessDecodedFrame(int streamIdx, AVFrame* pFrame);
		void ProcessPacket(int streamIdx, AVPacket* pPacket);
		void UnLock();
		void Lock();
	protected:
	
		AVFrameQueue* m_packetQueue;
		concurrency::cancellation_token_source* m_pCancelTaskToken;
		bool m_btaskStarted;
		Concurrency::task<void> m_doEncodingTsk;
		CRITICAL_SECTION m_CritLock;
		AVFormatContext* m_pAvFormatCtx;
		AVFrame* m_pAvDecFrame;
		MediaSampleOutputDevices* m_pOutputDevices;
		AVCodecContext* m_pAvVideoCtx;
		AVCodecContext* m_pAvAudioCtx;
	
	};
	

}