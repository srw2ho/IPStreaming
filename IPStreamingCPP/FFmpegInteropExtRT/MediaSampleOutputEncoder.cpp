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
#include "MediaSampleOutputEncoder.h"

using namespace concurrency;
using namespace FFmpegInteropExtRT;
using namespace std;
using namespace Windows::Storage;

extern "C"
{
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}




MediaSampleEncoding::MediaSampleEncoding(AVFormatContext* inputFormaCtx) {

	InitializeCriticalSection(&m_CritLock);
	m_pAvFormatCtx = inputFormaCtx;
	m_pAvEnCodecCtx = nullptr;
	m_pAvEncFrame = nullptr;
	m_pEncSwsCtx = nullptr;
	m_pEncSwrCtx = nullptr;
	m_pAvDecFrame = nullptr;
	m_pAvCodecCtx = nullptr;
	m_frameNumber = 0;
	m_samples_count = 0;
	m_pts = 0;
	m_dts= 0;
//	m_Outputdts = 0;
	m_state = 0;
//	m_Overrunpts = 0;

//	m_pDecSwsCtx = nullptr;
//	m_psampleprovider = psampleprovider;

};

MediaSampleEncoding::~MediaSampleEncoding() {
	/*
	avcodec_free_context(&ost->enc);
	av_frame_free(&ost->frame);
	av_frame_free(&ost->tmp_frame);
	sws_freeContext(ost->sws_ctx);
	swr_free(&ost->swr_ctx);
	*/

	if (m_pAvEncFrame) {
		av_frame_free(&m_pAvEncFrame);
		/*
		if (m_pAvEncFrame->data != nullptr)
		{
			av_freep(m_pAvEncFrame->data);
		}
		av_freep(m_pAvEncFrame);
		*/
		m_pAvEncFrame = nullptr;

	}

	if (m_pAvEnCodecCtx != nullptr) {
		avcodec_close(m_pAvEnCodecCtx);

		avcodec_free_context(&m_pAvEnCodecCtx);
		m_pAvEnCodecCtx = nullptr;
	}

	if (m_pAvCodecCtx != nullptr) {
		avcodec_close(m_pAvCodecCtx);

		avcodec_free_context(&m_pAvCodecCtx);
		m_pAvCodecCtx = nullptr;
	}



	/*
	if (m_pAvDecFrame) {
		if (m_pAvDecFrame->data != nullptr)
		{
			av_freep(m_pAvDecFrame->data);
		}
		av_freep(m_pAvDecFrame);
		m_pAvDecFrame = nullptr;

	}
	*/
	if (m_pEncSwsCtx)
	{
		sws_freeContext(m_pEncSwsCtx);
		m_pEncSwsCtx = nullptr;
	}
	if (m_pEncSwrCtx) {
		swr_free(&m_pEncSwrCtx);
	}

	DeleteCriticalSection(&m_CritLock);
};



void MediaSampleEncoding::Lock() {
	EnterCriticalSection(&m_CritLock);
}

void MediaSampleEncoding::UnLock() {

	LeaveCriticalSection(&m_CritLock);
}





int MediaSampleEncoding::encode(AVCodecContext *avctx, AVPacket *pkt,  AVFrame *frame, int *got_packet)
{
	int ret;

	*got_packet = 0;
	if (frame != nullptr) {
		ret = avcodec_send_frame(avctx, frame);
		if (ret < 0)
		{
			if (ret == AVERROR(EAGAIN))
			{
				bool b = true;
			}
			else if (ret == AVERROR_EOF) {
				bool b = true;
			}
			else if (AVERROR(EINVAL)) {
				bool b = true;

			}
			else if (AVERROR(ENOMEM)) {
				bool b = true;

			}
			return ret;
		}
	}



	ret = avcodec_receive_packet(avctx, pkt);


	if (ret < 0)
	{
		if (ret == AVERROR(EAGAIN))
		{
			// The decoder doesn't have enough data to produce a frame,
			// return S_FALSE to indicate a partial frame
			ret = 0;
		}
	}
	else
	{
//		ret = 1;
		*got_packet = 1;
	}


	/*
	if (ret == 0)
	{
		*got_packet = 1;
	}
	
	else if (ret == AVERROR(EAGAIN)){
		return 0;

	}
	*/

	return ret;
}

int MediaSampleEncoding::encodeandProcess(AVCodecContext *avctx, AVFrame *frame)
{
	AVPacket *pkt = av_packet_alloc();
	int ret;

	ret = avcodec_send_frame(avctx, frame);
	if (ret < 0)
		goto out;

	while (!ret) {
		ret = avcodec_receive_packet(avctx, pkt);
		if (!ret) {
			// do Processing
			//			ret = cb(priv, pkt);
		}

	}

out:
	av_packet_free(&pkt);
	if (ret == AVERROR(EAGAIN))
		return 0;
	return ret;
}

int MediaSampleEncoding::flushEncoder(AVPacket**encpacket) {

	int got_frame;
	Lock();
	AVFrame* filt_frame = nullptr;

	AVFormatContext*ifmt_ctx = m_pAvFormatCtx;

	
	int ret;

	AVPacket* enc_pkt = av_packet_alloc();

	enc_pkt->data = NULL;
	enc_pkt->size = 0;
	av_init_packet(enc_pkt);

	//ret = enc_func(m_pAvEnCodecCtx, enc_pkt, filt_frame, &got_frame);
	ret = encode(m_pAvEnCodecCtx, enc_pkt, filt_frame, &got_frame);


		//av_frame_free(&filt_frame);

	if (ret < 0)
	{
		av_packet_free(&enc_pkt);
		UnLock();
		return ret;
	}

	if (!got_frame)
	{
		av_packet_free(&enc_pkt);
		UnLock();
		return 0;
	}

	*encpacket = enc_pkt;

	// prepare packet for muxing 


		//	if (ofmt_ctx->streams[stream_index]->codec->pix_fmt != AV_PIX_FMT_YUV420P)
	rescaleEncodePacket(enc_pkt);

	UnLock();
	return 1;



}


void MediaSampleEncoding::rescaleEncodePacket(AVPacket*enc_pkt) {


	//av_packet_rescale_ts(enc_pkt, m_pAvFormatCtx->streams[m_streamIndex]->codec->time_base, m_pAvEnCodecCtx->time_base);
	//input-stream codec to encoder time-base
	m_frameNumber++;
	//enc_pkt->pts = enc_pkt->dts;
	//m_pAvCodecCtx->time_base

	//av_packet_rescale_ts(enc_pkt, m_pAvEnCodecCtx->time_base, m_pAvFormatCtx->streams[m_streamIndex]->codec->time_base);
	av_packet_rescale_ts(enc_pkt, m_pAvEnCodecCtx->time_base, m_pAvCodecCtx->time_base);
	enc_pkt->stream_index = m_streamIndex;
	m_pts = enc_pkt->pts;
	m_dts = enc_pkt->pts;
	//m_Outputdts = enc_pkt->dts;

	
		

}
AVFrame* MediaSampleEncoding::prepare_write_frame(FramePacket* Packet) 
{

	AVFrame *frame = Packet->getAVFrame();

	AVFrame*filt_frame = frame;
	AVFrame *enc_frame = getEncodeFrame();

	int ret;


	//if (frame != nullptr) {
	//	m_Inputpts = frame->pts;
	//}


	if (getMediaType() == AVMEDIA_TYPE_VIDEO)
	{
		if (enc_frame != nullptr) {
	
			if (m_pEncSwsCtx != nullptr)
			{
				int ret = sws_scale(m_pEncSwsCtx, (const uint8_t **)(frame->data), frame->linesize, 0, m_pAvCodecCtx->height, enc_frame->data, enc_frame->linesize);
				if (ret >0) {
					enc_frame->pts = frame->pts;
					enc_frame->pkt_dts = frame->pkt_dts;
//					enc_frame->pkt_pts = frame->pkt_pts;
					filt_frame = enc_frame;
				}
			}

		}
	}
	else if (getMediaType() == AVMEDIA_TYPE_AUDIO)
	{
		if (enc_frame != nullptr && m_pEncSwrCtx != nullptr) {
			/* convert samples from native format to destination codec format, using the resampler */
			/* compute destination number of samples */


			int dst_nb_samples = av_rescale_rnd(swr_get_delay(m_pEncSwrCtx, m_pAvEnCodecCtx->sample_rate) + frame->nb_samples,
				m_pAvEnCodecCtx->sample_rate, m_pAvEnCodecCtx->sample_rate, AV_ROUND_UP);

			/* convert to destination format */
			//	dst_nb_samples = enc_frame->nb_samples;

			if (dst_nb_samples > enc_frame->nb_samples) {
				dst_nb_samples = enc_frame->nb_samples;
			}

			//dst_nb_samples = enc_frame->nb_samples;

			ret = swr_convert(m_pEncSwrCtx,
				enc_frame->data, dst_nb_samples,
				(const uint8_t **)frame->data, frame->nb_samples);
			if (ret < 0) {
				Trace("#### error: Error while Audio swr_converting");
				//	fprintf(stderr, "Error while converting\n");
			}
			else {

				filt_frame = enc_frame;

				AVRational avr = { 1, m_pAvEnCodecCtx->sample_rate };

				enc_frame->pts = av_rescale_q(m_samples_count, avr, m_pAvEnCodecCtx->time_base);
				m_samples_count += dst_nb_samples;

			}
			//	frame = ost->frame;

		}

	}

	return filt_frame;


}

int MediaSampleEncoding::encode_write_Audioframe(FramePacket* Packet, AVPacket**encpacket) {

	AVFormatContext*ifmt_ctx = m_pAvFormatCtx;
	unsigned int stream_index = this->m_streamIndex;
	*encpacket = nullptr;

	AVFrame *frame = Packet->getAVFrame();
	AVFrame*filt_frame = frame;
	AVFrame *enc_frame = getEncodeFrame();

	int ret;
	int got_frame;
	AVPacket* enc_pkt = av_packet_alloc();

	if (enc_frame != nullptr && m_pEncSwrCtx!=nullptr) {
		/* convert samples from native format to destination codec format, using the resampler */
		/* compute destination number of samples */

			
		int dst_nb_samples = av_rescale_rnd(swr_get_delay(m_pEncSwrCtx, m_pAvEnCodecCtx->sample_rate) + frame->nb_samples,
			m_pAvEnCodecCtx->sample_rate, m_pAvEnCodecCtx->sample_rate, AV_ROUND_UP);

		/* convert to destination format */
	//	dst_nb_samples = enc_frame->nb_samples;

		if (dst_nb_samples > enc_frame->nb_samples) {
			dst_nb_samples = enc_frame->nb_samples;
		}
		
		//dst_nb_samples = enc_frame->nb_samples;

		ret = swr_convert(m_pEncSwrCtx,
			enc_frame->data, dst_nb_samples,
			(const uint8_t **)frame->data, frame->nb_samples);
		if (ret < 0) {
			Trace("#### error: Error while Audio swr_converting");
		//	fprintf(stderr, "Error while converting\n");
			return -1;
		}
		//	frame = ost->frame;

		filt_frame = enc_frame;
		
		AVRational avr = { 1, m_pAvEnCodecCtx->sample_rate };

		enc_frame->pts = av_rescale_q(m_samples_count, avr, m_pAvEnCodecCtx->time_base);
		m_samples_count += dst_nb_samples;

	}



	//	av_log(NULL, AV_LOG_INFO, "Encoding frame\n");
	/* encode filtered frame */
	enc_pkt->data = NULL;
	enc_pkt->size = 0;
	av_init_packet(enc_pkt);





//	ret = avcodec_encode_audio2(m_pAvEnCodecCtx, enc_pkt, filt_frame, &got_frame);
	ret = encode(m_pAvEnCodecCtx, enc_pkt, filt_frame, &got_frame);
	//	ret = enc_func(ofmt_ctx->streams[stream_index]->codec, enc_pkt,filt_frame, got_frame);

	//av_frame_free(&filt_frame);

	if (ret < 0)
	{
		av_packet_free(&enc_pkt);

		return ret;
	}

	if (!(got_frame))
	{
		av_packet_free(&enc_pkt);
		return 0;
	}

	*encpacket = enc_pkt;



	ret = 1;
	return ret;
}

int MediaSampleEncoding::encode_write_Videoframe(FramePacket* Packet, AVPacket**encpacket) {
	
	AVFormatContext*ifmt_ctx = m_pAvFormatCtx;
	unsigned int stream_index = this->m_streamIndex;
	*encpacket = nullptr;

	AVFrame *frame = Packet->getAVFrame();

	//unsigned int stream_index = avPacket->stream_index;

	AVFrame *enc_frame = getEncodeFrame();
	AVFrame*filt_frame = frame;
	if (enc_frame != nullptr) {
		if (m_pEncSwsCtx != nullptr)
		{
			int ret = sws_scale(m_pEncSwsCtx, (const uint8_t **)(frame->data), frame->linesize, 0, m_pAvCodecCtx->height, enc_frame->data, enc_frame->linesize);
			if (ret >0) {
				enc_frame->pts = frame->pts;
				enc_frame->pkt_dts = frame->pkt_dts;
//				enc_frame->pkt_pts = frame->pkt_pts;
				filt_frame = enc_frame;
			}
		}

	}

	int ret;
	int got_frame;
	AVPacket* enc_pkt = av_packet_alloc();
	enc_pkt->data = NULL;
	enc_pkt->size = 0;
	av_init_packet(enc_pkt);

//	ret = enc_func(m_pAvEnCodecCtx, enc_pkt, filt_frame, &got_frame);
	ret = encode(m_pAvEnCodecCtx, enc_pkt, filt_frame, &got_frame);

	//av_frame_free(&filt_frame);

	if (ret < 0)
	{
		av_packet_free(&enc_pkt);

		return ret;
	}

	if (!(got_frame))
	{
		av_packet_free(&enc_pkt);
		return 0;
	}

	*encpacket = enc_pkt;

	ret = 1;
	return ret;
}





/*
int MediaSampleEncoding::EncodeAVPacket(FramePacket* Packet, AVPacket**encpacket)
{
	*encpacket = nullptr;

	if (m_pAvEnCodecCtx == nullptr)return E_FAIL;
	Lock();
	int ret = -1;

	if (getMediaType() == AVMEDIA_TYPE_VIDEO)
	{
		 ret =  encode_write_Videoframe(Packet, encpacket);
	}
	else if (getMediaType() == AVMEDIA_TYPE_AUDIO) 
	{
		ret = encode_write_Audioframe(Packet, encpacket);

	}



	if (ret == 1)
	{
		m_Inputpts = Packet->m_pAvFrame->pts;
		rescaleEncodePacket(*encpacket);
		UnLock();
		return 1;
	}
	UnLock();
	return -1;



}
*/

int MediaSampleEncoding::EncodeAVPacket(AVFrame* pFrame, AVPacket**encpacket)
{
	*encpacket = nullptr;

	if (m_pAvEnCodecCtx == nullptr)return E_FAIL;
	Lock();
	int gotframe = 0;

	AVPacket* enc_pkt = av_packet_alloc();
	enc_pkt->data = NULL;
	enc_pkt->size = 0;
	av_init_packet(enc_pkt);


	int ret = encode(m_pAvEnCodecCtx, enc_pkt, pFrame, &gotframe);
	if (ret == 0)
	{
		if (!(gotframe))
		{
			av_packet_free(&enc_pkt);
		}
		else
		{
			*encpacket = enc_pkt;
			ret = 1;
		}

	}
	else
	{
		av_packet_free(&enc_pkt);
	}



	if (ret == 1)
	{
	//	m_Inputpts = Packet->m_pAvFrame->pts;
		rescaleEncodePacket(*encpacket);

	}
	UnLock();
	return ret;



}

AVMediaType MediaSampleEncoding::getMediaType()
{
	if (m_pAvEnCodecCtx == nullptr) return AVMEDIA_TYPE_UNKNOWN;
	return m_pAvEnCodecCtx->codec_type;

}
int MediaSampleEncoding::AddEncoding(AVMediaType codeType, AVCodecID codec_id, int fps, int height, int width, int64_t bit_rate)
{
	AVCodecParameters *dec_ctx;
	AVFormatContext *ifmt_ctx = this->m_pAvFormatCtx;
	AVStream *in_stream;
	int ret = -1;

	for (size_t streamIdx = 0; streamIdx < ifmt_ctx->nb_streams; streamIdx++) {
		in_stream = ifmt_ctx->streams[streamIdx];
		dec_ctx = in_stream->codecpar;

		if (dec_ctx->codec_type == codeType) {
			ret = AddEncoding((int)streamIdx,codec_id, fps, height, width, bit_rate);
			break;
		}

	}

	return ret;

}

int MediaSampleEncoding::AddEncoding(int streamIdx, AVCodecID codec_id, int fps, int height, int width, int64_t bit_rate)
{
	m_streamIndex = streamIdx;
	m_codec_id = codec_id;

	AVCodec *codec;
	AVCodec* instreamcodec;
	AVCodecContext *enc_ctx = NULL;

	//AVCodecContext *dec_ctx;
	AVCodecParameters *dec_ctx;
	AVStream *in_stream;

	AVFormatContext *ifmt_ctx = this->m_pAvFormatCtx;



	//m_pAvCodecCtx = ifmt_ctx->streams[streamIdx]->codec;

	in_stream = ifmt_ctx->streams[streamIdx];
	dec_ctx = in_stream->codecpar;
	instreamcodec = nullptr;

	int audioStreamIndex = av_find_best_stream(ifmt_ctx, dec_ctx->codec_type, streamIdx, -1, &instreamcodec, 0);

	//instreamcodec = avcodec_find_encoder(in_stream->codecpar->codec_id);
	if (instreamcodec == nullptr)
	{
		return -1;
	}

	m_pAvCodecCtx = avcodec_alloc_context3(instreamcodec);
	if (avcodec_parameters_to_context(m_pAvCodecCtx, in_stream->codecpar) < 0)
	{
		return -1;
	}

	if (avcodec_open2(m_pAvCodecCtx, instreamcodec, NULL) < 0)
	{
		return -1;
	}



	dec_ctx = in_stream->codecpar;
	if (dec_ctx->codec_id == codec_id)
	{// es braucht nix encoded zu werden

	}

	codec = avcodec_find_encoder(codec_id);
	if (!codec) {
		return -1;

	}
	enc_ctx = avcodec_alloc_context3(codec);
	if (!enc_ctx) {
		return -1;

	}
	m_pAvEnCodecCtx = enc_ctx;

	if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {

		/* In this example, we transcode to same properties (picture size,
		* sample rate etc.). These properties can be changed for output
		* streams easily using filters */
		enc_ctx->height = dec_ctx->height;
		enc_ctx->width = dec_ctx->width;

		enc_ctx->height = height;
		enc_ctx->width = width;

		enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
		/* take first format from list of supported formats */
		enc_ctx->pix_fmt = codec->pix_fmts[0];
		/* video time_base can be set to whatever is handy and supported by encoder */
		m_pAvCodecCtx->time_base = av_inv_q(in_stream->r_frame_rate);
		if (fps == -1) {

	
			double value = av_q2d(in_stream->r_frame_rate);
			//mpegvideo 1 : framerate 24<= x <=60
			if (enc_ctx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
				if (value > 60)
				{
					AVRational r = { 60,1 };
					m_pAvCodecCtx->time_base = av_inv_q(r);
				}
				else if (value < 24)
				{
					AVRational r = { 24,1 };
					m_pAvCodecCtx->time_base = av_inv_q(r);
				}
			}

	
			enc_ctx->time_base = m_pAvCodecCtx->time_base;
		}
		else
		{
	
			//mpegvideo 1 : framerate 24<= x <=60
			if (enc_ctx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
				if (fps > 60) fps = 60;
				if (fps < 24) fps = 24;
			}
			enc_ctx->time_base.den = fps;
			enc_ctx->time_base.num = 1;
		}


		enc_ctx->gop_size = 12; /* emit one intra frame every twelve frames at most */
		enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

	//	bit_rate = 800000;
		enc_ctx->bit_rate = bit_rate;
		//	enc_ctx->bit_rate = 800000;
		enc_ctx->codec_id = codec_id;

		if (enc_ctx->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
			/* just for testing, we also add B frames */
			enc_ctx->max_b_frames = 2;
		}
		if (enc_ctx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
			/* Needed to avoid using macroblocks in which some coeffs overflow.
			* This does not happen with normal video, it just happens here as
			* the motion of the chroma plane does not match the luma plane. */
			enc_ctx->mb_decision = 2;
			enc_ctx->max_b_frames = 1;
		}

		if (enc_ctx->codec_id == AV_CODEC_ID_MJPEG) {
			enc_ctx->pix_fmt = AV_PIX_FMT_YUVJ420P;
//				AV_PIX_FMT_YUVJ422P
		}
		

		if (codec_id == AV_CODEC_ID_H264) {
				av_opt_set(enc_ctx->priv_data, "preset", "slow", 0);
		}


	}
	else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
		enc_ctx->sample_rate = dec_ctx->sample_rate;
		enc_ctx->channel_layout = dec_ctx->channel_layout;
		enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
		/* take first format from list of supported formats */
		//enc_ctx->sample_fmt = encoder->sample_fmts[0];
		enc_ctx->sample_fmt = codec->sample_fmts ?
			codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;

		enc_ctx->time_base.den = 1;
		enc_ctx->time_base.num = enc_ctx->sample_rate;
	}
	int ret = avcodec_open2(enc_ctx, codec, NULL);
	if (ret < 0) {
		return -1;

	}
	/*
	ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
	if (ret < 0)
	{
		return -1;
	}
	*/

	return PrepareEncodingContext();

}





int MediaSampleEncoding::PrepareEncodingContext()
{
	int ret = 0;
	if (this->getMediaType() == AVMEDIA_TYPE_VIDEO)
	{
		bool createEncSwsCtx;
		if (m_pAvCodecCtx->width == 0 || m_pAvCodecCtx->height == 0) {

			return -1;
		}

	
		createEncSwsCtx = (m_pAvCodecCtx->width != m_pAvEnCodecCtx->width) || (m_pAvCodecCtx->height != m_pAvEnCodecCtx->height) || (m_pAvCodecCtx->pix_fmt != m_pAvEnCodecCtx->pix_fmt);



		if (createEncSwsCtx) {
		//	AVPixelFormat outputPixel = MediaSampleEncoding::getVideoAVPixelFormat(m_pAvEnCodecCtx->pix_fmt);
			m_pAvEncFrame = av_frame_alloc();
			if (!m_pAvEncFrame)
			{
				return -1;
			}

			m_pAvEncFrame->width = m_pAvEnCodecCtx->width;
			m_pAvEncFrame->height = m_pAvEnCodecCtx->height;
			m_pAvEncFrame->format = m_pAvEnCodecCtx->pix_fmt;
			ret = av_image_alloc(m_pAvEncFrame->data, m_pAvEncFrame->linesize, m_pAvEncFrame->width, m_pAvEncFrame->height, m_pAvEnCodecCtx->pix_fmt, 1);
	
		

			AVPixelFormat newFormat = FFmpegUty::getVideoAVPixelforDeprecatedFormat(m_pAvCodecCtx->pix_fmt);


			m_pEncSwsCtx = sws_getContext(m_pAvCodecCtx->width, m_pAvCodecCtx->height, newFormat, m_pAvEnCodecCtx->width, m_pAvEnCodecCtx->height, m_pAvEnCodecCtx->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);


	//		m_pEncSwsCtx = sws_getContext(m_pAvCodecCtx->width, m_pAvCodecCtx->height, m_pAvCodecCtx->pix_fmt, m_pAvEnCodecCtx->width, m_pAvEnCodecCtx->height, m_pAvEnCodecCtx->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);

			//	m_pEncSwsCtx = sws_getContext(m_pAvCodecCtx->width, m_pAvCodecCtx->height, m_pAvCodecCtx->pix_fmt, m_pAvEnCodecCtx->width, m_pAvEnCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

			if (m_pEncSwsCtx == nullptr)
			{
				return -1;
			}



		}

	

	}
	else
	{
		bool createEncSwrCtx = (m_pAvEnCodecCtx->sample_fmt != m_pAvCodecCtx->sample_fmt) || (m_pAvEnCodecCtx->sample_rate != m_pAvCodecCtx->sample_rate) || (m_pAvEnCodecCtx->channel_layout != m_pAvCodecCtx->channel_layout);
		if (createEncSwrCtx) {
			m_pAvEncFrame = av_frame_alloc();

			if (!m_pAvEncFrame)
			{
				return -1;
			}


			int nb_samples;

			if ((m_pAvEnCodecCtx->codec != nullptr) && m_pAvEnCodecCtx->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
				nb_samples = 10000;
			else
				nb_samples = m_pAvEnCodecCtx->frame_size;

			m_pAvEncFrame->format = m_pAvEnCodecCtx->sample_fmt;
			m_pAvEncFrame->channel_layout = m_pAvEnCodecCtx->channel_layout;
			m_pAvEncFrame->sample_rate = m_pAvEnCodecCtx->sample_rate;
			m_pAvEncFrame->nb_samples = nb_samples;

			if (nb_samples) {

				//	unsigned int aBufferSize = av_samples_alloc(m_pAvEncFrame->data, m_pAvEncFrame->linesize, m_pAvEnCodecCtx->channels, nb_samples, AV_SAMPLE_FMT_S16, 0);


				ret = av_frame_get_buffer(m_pAvEncFrame, 0);
				if (ret < 0) {
					Trace("##### Error allocating an audio buffer");
					return -1;;
				}

			}

			m_pEncSwrCtx = swr_alloc();
			if (m_pEncSwrCtx == nullptr) {
				return -1;
			}
			/* set options */
			m_pEncSwrCtx = swr_alloc_set_opts(
				NULL,
				m_pAvEnCodecCtx->channel_layout,
				m_pAvEnCodecCtx->sample_fmt, //AV_SAMPLE_FMT_S16,
				m_pAvEnCodecCtx->sample_rate,
				m_pAvCodecCtx->channel_layout,
				m_pAvCodecCtx->sample_fmt,
				m_pAvCodecCtx->sample_rate,
				0,
				NULL);
			/*

			av_opt_set_int(m_pEncSwrCtx, "in_channel_count", m_pAvCodecCtx->channels, 0);
			av_opt_set_int(m_pEncSwrCtx, "in_sample_rate", m_pAvCodecCtx->sample_rate, 0);
			av_opt_set_sample_fmt(m_pEncSwrCtx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
			av_opt_set_int(m_pEncSwrCtx, "out_channel_count", m_pAvCodecCtx->channels, 0);
			av_opt_set_int(m_pEncSwrCtx, "out_sample_rate", m_pAvCodecCtx->sample_rate, 0);
			av_opt_set_sample_fmt(m_pEncSwrCtx, "out_sample_fmt", m_pAvCodecCtx->sample_fmt, 0);
			*/
			//	ret = avcodec_parameters_from_context(this->m_ocodecpar, m_pAvCodecCtx);
			//	if (ret < 0)
			//	{
			//	return -1;
			//	}
			/* initialize the resampling context */
			ret = swr_init(m_pEncSwrCtx);
			if (ret < 0)
			{

				return -1;
			}



		}

	
	

	}


	if (ret < 0)
	{
		return ret;
	}

	return ret;

}







