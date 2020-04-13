#pragma once
using namespace Platform;
//using namespace Platform::Collections;
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
	typedef std::map<String^, Windows::Media::Core::CodecInfo^> InstalledCodecs;

	public ref class CodecReader sealed
	{
	private:
		InstalledCodecs m_SupportedVideoCodecs;
		InstalledCodecs m_SupportedAudioCodecs;
		Windows::Media::Core::CodecQuery^ m_query;
	public:
		CodecReader();
		virtual ~CodecReader();
		IAsyncAction^ ReadUsedVideoDecoderCodecsAsync();
		IAsyncAction^ ReadUsedAudioDecoderCodecsAsync();

		IAsyncAction^ ReadInstalledVideoDecoderCodecsAsync();
		IAsyncAction^ ReadInstalledAudioDecoderCodecsAsync();
		bool IsVideoDecoderCodecInstalled(Platform::String^ subTypeGUID);
		bool IsAudioDecoderCodecInstalled(Platform::String^ subTypeGUID);

	internal:
		bool IsVideoCodecSupported(bool forceVideoDecode, AVCodecID  codercID);
		bool IsAudioCodecSupported(bool forceAudioDecode, AVCodecID  codercID);
		Concurrency::task<bool> GetVideoDecoderCodecAsync(Platform::String^ subTypeGUID);
		Concurrency::task<bool> GetAudioDecoderCodecAsync(Platform::String^ subTypeGUID);
	};

}