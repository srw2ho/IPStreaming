#include "pch.h"
#include "CodecReader.h"

//// this works, but it takes 500ms on first call, so not using it right now...
IAsyncAction^ IPStreamingCPP::CodecReader::ReadInstalledVideoDecoderCodecsAsync()
{

	return create_async([this]()->void {

		m_SupportedVideoCodecs.clear();
//

		auto query = ref new CodecQuery();
		create_task(query->FindAllAsync(CodecKind::Video, CodecCategory::Decoder, "")).then([this](IVectorView<CodecInfo^ >^ codecs) {

			for each (auto codec in codecs)
			{
				//	 std::map<String^, CodecInfo^> m_SupportedVideoCodecs;

				if (codec->IsTrusted)
				{
	
					auto types = codec->Subtypes;
					for each (auto type in types)
					{
						m_SupportedVideoCodecs[type] = codec;

					}

				}
			}
	
			});
		});

}

IAsyncAction^ IPStreamingCPP::CodecReader::ReadInstalledAudioDecoderCodecsAsync()
{

	return create_async([this]()->void {
		m_SupportedAudioCodecs.clear();
		String^ Guid = CodecSubtypes::VideoFormatH264;

		auto query = ref new CodecQuery();
		create_task(query->FindAllAsync(CodecKind::Video, CodecCategory::Decoder, "")).then([this](IVectorView<CodecInfo^ >^ codecs) {

			for each (auto codec in codecs)
			{
				//	 std::map<String^, CodecInfo^> m_SupportedVideoCodecs;

				if (codec->IsTrusted)
				{

					auto types = codec->Subtypes;
					for each (auto type in types)
					{
						m_SupportedAudioCodecs[type] = codec;

					}

				}
			}

			});
		});

}

bool IPStreamingCPP::CodecReader::IsVideoDecoderCodecInstalled(String^ subTypeGUID)
{

	InstalledCodecs::iterator ok = m_SupportedVideoCodecs.find(subTypeGUID);
	if (ok != m_SupportedVideoCodecs.end()) {
		return true;
	}
	return false;

}
bool IPStreamingCPP::CodecReader::IsAudioDecoderCodecInstalled(String^ subTypeGUID)
{

	InstalledCodecs::iterator ok = m_SupportedAudioCodecs.find(subTypeGUID);
	if (ok != m_SupportedAudioCodecs.end()) {
		return true;
	}
	return false;

}