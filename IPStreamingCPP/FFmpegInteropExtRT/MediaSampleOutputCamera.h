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

#include "..\CameraServer\Connection.h"
#include "..\CameraServer\IPCamera.h"

#include <ppl.h>

#include "MediaSampleOutputDeviceBase.h"
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



	class CameraOutputDevice: public MediaSampleOutputDevice
	{
	public:
		CameraOutputDevice(Platform::String^ deviceName,AVFormatContext* inputFormaCtx, CameraServer^ m_pCameraServer);
		virtual ~CameraOutputDevice();

		MediaSampleEncoding* AddMpegEncoding(int fps, int height, int width, int64_t bit_rate);
		MediaSampleEncoding* AddMJpegEncoding(int fps, int height, int width,int64_t bit_rate);
		virtual bool WritePackage(MediaSampleEncoding* pEncoding, AVPacket* encodePacket);
		virtual bool IsEncodingOutPutDevice();

	protected:

		IBuffer^ CreateHttMJpegChunksFromAvPacket(AVPacket* avPacket);
		IBuffer^ CreateHttpMpegChunksfromAvPacket(AVPacket* avPacket);
	protected:

		CameraServer^ m_pCameraServer;
	};



	

}