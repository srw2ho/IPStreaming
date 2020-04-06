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
#include "MediaSampleProvider.h"

namespace FFmpegInteropExtRT
{
	ref class H264SampleProvider :public MediaSampleProvider
	{

	public:
		virtual ~H264SampleProvider();
		virtual void Flush() override;
	private:
		bool m_bHasSentExtradata;
	internal:
		virtual HRESULT DecodeAVPacket(DataWriter^ dataWriter, AVPacket* avPacket, int64_t& framePts, int64_t& frameDuration) override;
		virtual HRESULT WriteAVPacketToStream(DataWriter^ writer, AVPacket* avPacket) override;
		virtual HRESULT WriteNALPacket(DataWriter^ dataWriter, AVPacket* avPacket);
		virtual HRESULT GetSPSAndPPSBuffer(DataWriter^ dataWriter, byte* buf, int length);


	internal:
		H264SampleProvider(
			FFmpegReader^ reader,
			AVFormatContext* avFormatCtx,
			AVCodecContext* avCodecCtx,  MediaSampleOutputEncoding*pOutputEncoding = nullptr);

	};
}
