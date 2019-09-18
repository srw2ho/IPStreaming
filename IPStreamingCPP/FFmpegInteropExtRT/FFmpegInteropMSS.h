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

#pragma once
#include <queue>
#include <mutex>
#include "FFmpegReader.h"
#include "MediaSampleProvider.h"
#include "MediaThumbnailData.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Core;

extern "C"
{
#include <libavformat/avformat.h>
}

namespace FFmpegInteropExtRT
{
	public ref class FFmpegInteropMSSInputParamArgs sealed
	{
	public:

		property int Height { int get() { return m_height; } }
		property int Width { int get() { return m_width; } }
		property double Fps { double get() { return m_fps; } }

	internal:

		FFmpegInteropMSSInputParamArgs(double fps, int height, int width)
		{
			m_fps = fps;
			m_height = height;
			m_width = width;
		}

	private:


		double m_fps;
		int m_height;
		int m_width;

	};
	
	public ref class FFmpegInteropMSS sealed
	{
	public:
		static FFmpegInteropMSS^ CreateFFmpegInteropMSS();

		Windows::Foundation::IAsyncAction ^ DestroyFFmpegAsync();
		FFmpegInteropMSS^ StartFFmpegInteropMSSFromStream(IRandomAccessStream^ stream, bool forceAudioDecode, bool forceVideoDecode, PropertySet^ ffmpegOptions);
		FFmpegInteropMSS^ StartFFmpegInteropMSSFromUri(String^ uri, bool forceAudioDecode, bool forceVideoDecode, PropertySet^ ffmpegOptions);
		static FFmpegInteropMSS^ CreateFFmpegInteropMSSFromStream(IRandomAccessStream^ stream, bool forceAudioDecode, bool forceVideoDecode, PropertySet^ ffmpegOptions);
		static FFmpegInteropMSS^ CreateFFmpegInteropMSSFromStream(IRandomAccessStream^ stream, bool forceAudioDecode, bool forceVideoDecode);
		static FFmpegInteropMSS^ CreateFFmpegInteropMSSFromUri(String^ uri, bool forceAudioDecode, bool forceVideoDecode, PropertySet^ ffmpegOptions);
		static FFmpegInteropMSS^ CreateFFmpegInteropMSSFromUri(String^ uri, bool forceAudioDecode, bool forceVideoDecode);
		MediaThumbnailData^ ExtractThumbnail();

		void PrepareHttpMJpegClientEncoding(CameraServer^ pCameraServer, Platform::String^  hostname, PropertySet^ configOptions);
		void PrepareHttpMpegClientEncoding(CameraServer^ pCameraServer, Platform::String^  hostname, PropertySet^ configOptions);
		void PrepareFFMPegOutPutEncoding(Platform::String^ fileName, PropertySet^ configOptions, PropertySet^ ffmpegOutputOptions);
	
		// Contructor
		MediaStreamSource^ GetMediaStreamSource();
		virtual ~FFmpegInteropMSS();
		///<summary>Raised when the FFmpegInteropMSS starts with streaming</summary>
		// Properties
		property AudioStreamDescriptor^ AudioDescriptor
		{
			AudioStreamDescriptor^ get()
			{
				return audioStreamDescriptor;
			};
		};
		property VideoStreamDescriptor^ VideoDescriptor
		{
			VideoStreamDescriptor^ get()
			{
				return videoStreamDescriptor;
			};
		};
		property TimeSpan Duration
		{
			TimeSpan get()
			{
				return mediaDuration;
			};
		};
		property String^ VideoCodecName
		{
			String^ get()
			{
				return videoCodecName;
			};
		};
		property String^ AudioCodecName
		{
			String^ get()
			{
				return audioCodecName;
			};
		};
		event Windows::Foundation::TypedEventHandler<Platform::Object^, FFmpegInteropMSSInputParamArgs^ >^ startStreaming;

	internal:
		int ReadPacket();
		HRESULT Seek(TimeSpan position);
	private:
		FFmpegInteropMSS();

		HRESULT CreateMediaStreamSource(IRandomAccessStream^ stream, bool forceAudioDecode, bool forceVideoDecode, PropertySet^ ffmpegOptions, MediaStreamSource^ mss);
		HRESULT CreateMediaStreamSource(String^ uri, bool forceAudioDecode, bool forceVideoDecode, PropertySet^ ffmpegOptions);
		HRESULT InitFFmpegContext(bool forceAudioDecode, bool forceVideoDecode);
		HRESULT CreateAudioStreamDescriptor(bool forceAudioDecode);
		HRESULT CreateVideoStreamDescriptor(bool forceVideoDecode);
		HRESULT ConvertCodecName(const char* codecName, String^ *outputCodecName);
		HRESULT ParseOptions(PropertySet^ ffmpegOptions);
		void OnStarting(MediaStreamSource ^sender, MediaStreamSourceStartingEventArgs ^args);
		void OnSampleRequested(MediaStreamSource ^sender, MediaStreamSourceSampleRequestedEventArgs ^args);
		void OnMediaClosed(Windows::Media::Core::MediaStreamSource ^sender, MediaStreamSourceClosedEventArgs^ args);

		MediaStreamSource^ mss;
		Windows::Foundation::EventRegistrationToken startingRequestedToken;
		Windows::Foundation::EventRegistrationToken sampleRequestedToken;
		Windows::Foundation::EventRegistrationToken MediaClosedToken;


		CameraServer^ m_pCameraServer;
		MediaSampleOutputEncoding * m_pOutPutEncoding;
		internal:
		AVDictionary* avDict;
		AVIOContext* avIOCtx;
		AVFormatContext* avFormatCtx;
		AVCodecContext* avAudioCodecCtx;
		AVCodecContext* avVideoCodecCtx;

		private:
		AudioStreamDescriptor^ audioStreamDescriptor;
		VideoStreamDescriptor^ videoStreamDescriptor;
		int audioStreamIndex;
		int videoStreamIndex;
		int thumbnailStreamIndex;
		
		bool rotateVideo;
		int rotationAngle;
		std::recursive_mutex mutexGuard;
		
		MediaSampleProvider^ audioSampleProvider;
		MediaSampleProvider^ videoSampleProvider;

		String^ videoCodecName;
		String^ audioCodecName;
		TimeSpan mediaDuration;
		IStream* fileStreamData;
		unsigned char* fileStreamBuffer;
		FFmpegReader^ m_pReader;
		timeout_handler * m_ptimeouthandler;
		bool m_isFirstSeek;
	};
}
