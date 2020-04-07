#pragma once
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Core;
using namespace Windows::System;
using namespace Concurrency;

namespace IPStreamingCPP
{
	typedef std::map<String^, CodecInfo^> InstalledCodecs;

	ref class CodecReader
	{
	private:
		InstalledCodecs m_SupportedVideoCodecs;
		InstalledCodecs m_SupportedAudioCodecs;
	public:
		IAsyncAction^ ReadInstalledVideoDecoderCodecsAsync();
		IAsyncAction^ ReadInstalledAudioDecoderCodecsAsync();
		bool IsVideoDecoderCodecInstalled(String^ subTypeGUID);
		bool IsAudioDecoderCodecInstalled(String^ subTypeGUID);

	
	};

}