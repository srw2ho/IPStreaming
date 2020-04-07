#pragma once
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Core;
using namespace Windows::System;
using namespace Concurrency;

extern "C"
{
#include <libavformat/avformat.h>
}

namespace FFmpegInteropExtRT
{
	typedef std::map<String^, CodecInfo^> InstalledCodecs;

	public ref class CodecReader sealed
	{
	private:
		InstalledCodecs m_SupportedVideoCodecs;
		InstalledCodecs m_SupportedAudioCodecs;
	public:
		IAsyncAction^ ReadInstalledVideoDecoderCodecsAsync();
		IAsyncAction^ ReadInstalledAudioDecoderCodecsAsync();
		bool IsVideoDecoderCodecInstalled(String^ subTypeGUID);
		bool IsAudioDecoderCodecInstalled(String^ subTypeGUID);
	internal:
		bool IsVideoCodecSupported(bool forceVideoDecode, AVCodecID  codercID);
		bool IsAudioCodecSupported(bool forceAudioDecode, AVCodecID  codercID);
	
	};

}