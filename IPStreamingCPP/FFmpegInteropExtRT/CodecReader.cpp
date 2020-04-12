#include "pch.h"
#include "CodecReader.h"


FFmpegInteropExtRT::CodecReader::CodecReader() {
}

FFmpegInteropExtRT::CodecReader::~CodecReader() {

};

//// this works, but it takes 500ms on first call, so not using it right now...
IAsyncAction^ FFmpegInteropExtRT::CodecReader::ReadInstalledVideoDecoderCodecsAsync()
{

	return create_async([this]()->void {



		m_SupportedVideoCodecs.clear();
		//

		auto query = ref new CodecQuery();
		create_task(query->FindAllAsync(CodecKind::Video, CodecCategory::Decoder, "")).then([this](task<IVectorView<CodecInfo^ >^> tskcodecs) {
			try
			{
				auto codecs = tskcodecs.get();

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
			}
			catch (Exception^ exception)
			{


			}

			});
		});


}

IAsyncAction^ FFmpegInteropExtRT::CodecReader::ReadInstalledAudioDecoderCodecsAsync()
{

	return create_async([this]()->void {
		m_SupportedAudioCodecs.clear();


		auto query = ref new CodecQuery();
		create_task(query->FindAllAsync(CodecKind::Video, CodecCategory::Decoder, "")).then([this](task<IVectorView<CodecInfo^ >^> tskcodecs) {
			try
			{
				auto codecs = tskcodecs.get();
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
			}
			catch (Exception^ exception)
			{


			}

			});
		});

}

bool FFmpegInteropExtRT::CodecReader::IsVideoDecoderCodecInstalled(String^ subTypeGUID)
{

	InstalledCodecs::iterator ok = m_SupportedVideoCodecs.find(subTypeGUID);
	if (ok != m_SupportedVideoCodecs.end()) {
		return true;
	}
	return false;

}
bool FFmpegInteropExtRT::CodecReader::IsAudioDecoderCodecInstalled(String^ subTypeGUID)
{

	InstalledCodecs::iterator ok = m_SupportedAudioCodecs.find(subTypeGUID);
	if (ok != m_SupportedAudioCodecs.end()) {
		return true;
	}
	return false;

}

bool FFmpegInteropExtRT::CodecReader::IsVideoCodecSupported(bool forceVideoDecode, AVCodecID  codercID)
{
	bool bret = false;
	if (forceVideoDecode) return false;

	switch (codercID) {
	case AV_CODEC_ID_H264: {
		bret = IsVideoDecoderCodecInstalled(CodecSubtypes::VideoFormatH264);
		break;
	}

	case AV_CODEC_ID_HEVC: {

		bret = IsVideoDecoderCodecInstalled(CodecSubtypes::VideoFormatHevc) || IsVideoDecoderCodecInstalled(CodecSubtypes::VideoFormatHevcES);
		break;
	}
	case AV_CODEC_ID_MPEG2VIDEO: {
		bret = IsVideoDecoderCodecInstalled(CodecSubtypes::VideoFormatMpeg2);
		break;
	}
	case AV_CODEC_ID_VP9: {
		bret = IsVideoDecoderCodecInstalled(CodecSubtypes::VideoFormatVP90);
		break;
	}
	case AV_CODEC_ID_VP8: {
		bret = IsVideoDecoderCodecInstalled(CodecSubtypes::VideoFormatVP80);
		break;




	}

	}
	return bret;

}
bool FFmpegInteropExtRT::CodecReader::IsAudioCodecSupported(bool forceAudioDecode, AVCodecID  codercID)
{
	if (forceAudioDecode) return false;
	return true;
}

