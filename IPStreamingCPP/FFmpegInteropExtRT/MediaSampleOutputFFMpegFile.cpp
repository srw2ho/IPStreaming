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


#include "MediaSampleOutputDeviceBase.h"
#include "MediaSampleOutputCamera.h"
#include "MediaSampleOutputFFMpegFile.h"



extern "C"
{
#include <libavutil/motion_vector.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>


}

using namespace concurrency;
using namespace FFmpegInteropExtRT;
using namespace std;
using namespace Windows::Storage;
//using namespace IpCamera;

FFMpegOutputDevice::FFMpegOutputDevice(Platform::String^ deviceName, AVFormatContext* inputFormaCtx, Platform::String^ folder, int fps, int height,int width, int64_t bit_rate, PropertySet^ ffmpegOutputOptions, Platform::String^ outputformat, double deletefilesOlderFilesinHours,double RecordingInHours)
	: MediaSampleOutputDevice(deviceName,inputFormaCtx) {

	InitializeCriticalSection(&m_CritLock);
	m_pAvOutFormatCtx = nullptr;
	m_strFolder = folder;
	m_ptimeouthandler = new timeout_handler(10);
	m_bopenDevice = false;


	std::wstring valueW = folder->Data();

	m_strFolderPath = std::string(valueW.begin(), valueW.end());
	valueW = deviceName->Data();
	m_strFileName = std::string(valueW.begin(), valueW.end());
	valueW = outputformat->Data();
	m_strOutputFormat = std::string(valueW.begin(), valueW.end());
	m_strfileExtension = "";
	m_ffmpegOptions = ffmpegOutputOptions;
	m_avDict = nullptr;
	m_fps = fps;
	m_height = height;
	m_width = width;
	m_bit_rate = bit_rate;
	m_deletefilesOlderFilesinHours = deletefilesOlderFilesinHours;
	m_RecordingInHours = RecordingInHours;
//	m_bCopyInputStream = false;

}



void FFMpegOutputDevice::Lock() {
	EnterCriticalSection(&m_CritLock);
}

void FFMpegOutputDevice::UnLock() {

	LeaveCriticalSection(&m_CritLock);
}



FFMpegOutputDevice::~FFMpegOutputDevice()
{
	this->flushEncoder();
	this->writeTrailer();
	if (m_pAvOutFormatCtx != nullptr) {

		if (!(m_pAvOutFormatCtx->oformat->flags & AVFMT_NOFILE)) {
			if (m_pAvOutFormatCtx->pb != nullptr) {
				avio_closep(&m_pAvOutFormatCtx->pb);
			}

		}

		
		DeleteAllEncodings();
		m_pAvOutFormatCtx->nb_streams = 0;
		/* wird alles in DeleteEncodings gelöscht
		for (unsigned int i = 0; i < m_pAvOutFormatCtx->nb_streams; i++)
		{
			av_freep(&m_pAvOutFormatCtx->streams[i]->codec);
			av_freep(&m_pAvOutFormatCtx->streams[i]);
		}
		*/
		avformat_free_context(m_pAvOutFormatCtx);
	}

	av_dict_free(&m_avDict);

	delete m_ptimeouthandler;
	DeleteCriticalSection(&m_CritLock);
}



void FFMpegOutputDevice::deleteUnUsedEncodings()
{
	if (m_pAvOutFormatCtx == nullptr) return;

	if (this->m_pEncodings->size() > 0) {

		std::list<MediaSampleEncoding*>::iterator it;
		it = this->m_pEncodings->begin();
		while (it != this->m_pEncodings->end())
		{
			MediaSampleFFMpegEncoding*pEnc = (MediaSampleFFMpegEncoding*)*it;
			if (pEnc->getState() < 0)//error initalized
			{
	
				/*
				if (m_pAvOutFormatCtx->nb_streams > 0)
				{
					for (size_t i = 0; i < m_pAvOutFormatCtx->nb_streams; i++)
					{
						if (pEnc->getAvOutStream() == m_pAvOutFormatCtx->streams[i])
						{
							m_pAvOutFormatCtx->streams[i] = nullptr;
							m_pAvOutFormatCtx->nb_streams--;
							m_pEncodings->erase(it);
							delete pEnc;
							it = this->m_pEncodings->begin();// von vorne beginnen
							break;
						}
					}
				}
				else
				*/
				{
					for (size_t i = 0; i < m_pAvOutFormatCtx->nb_streams; i++)
					{
						if (pEnc->getAvOutStream() == m_pAvOutFormatCtx->streams[i])
						{
							m_pAvOutFormatCtx->streams[i] = nullptr;
							m_pAvOutFormatCtx->nb_streams--;
						}
					}

					m_pEncodings->erase(it);
					delete pEnc;
					it = this->m_pEncodings->begin();// von vorne beginnen
				}
				

			}
			else it++;


		}



	}


}

bool FFMpegOutputDevice::ParseOptions(PropertySet^ ffmpegOptions)
{
	bool ret = true;
	av_dict_free(&m_avDict);
	// Convert FFmpeg options given in PropertySet to AVDictionary. List of options can be found in https://www.ffmpeg.org/ffmpeg-protocols.html
	if (ffmpegOptions != nullptr)
	{
		auto options = ffmpegOptions->First();

		while (options->HasCurrent)
		{
			Platform::String^ key = options->Current->Key;
			std::wstring keyW(key->Begin());
			std::string keyA(keyW.begin(), keyW.end());
			const char* keyChar = keyA.c_str();

			// Convert value from Object^ to const char*. avformat_open_input will internally convert value from const char* to the correct type
			Platform::String^ value = options->Current->Value->ToString();
			std::wstring valueW(value->Begin());
			std::string valueA(valueW.begin(), valueW.end());
			const char* valueChar = valueA.c_str();

			// Add key and value pair entry
			if (av_dict_set(&m_avDict, keyChar, valueChar, 0) < 0)
			{
				ret = false;
				break;
			}

			options->MoveNext();
		}
	}

	return ret;
}
int FFMpegOutputDevice::open_output()
{
	AVOutputFormat *fmt = nullptr;
	std::vector<std::string> extensionarray;
	std::vector<std::string> outputarray;
	/*
	

	m_bCopyInputStream = false;
	outputarray = splitintoArray(m_strOutputFormat.c_str(), ".");
	if (outputarray.size() > 1) {
		if (outputarray[0] == "cpyinput") {// copy input stream to muxer
			m_strOutputFormat = outputarray[1];

		}
		m_bCopyInputStream = true;
	}
	*/
	fmt = av_guess_format(m_strOutputFormat.c_str(), nullptr, 0);
	if (fmt == nullptr) {
		fmt = av_guess_format("mpeg", nullptr, 0);
	}
	if (fmt != nullptr) {
		extensionarray = splitintoArray(fmt->extensions, ",");
	}
	if (extensionarray.size() > 0) {
		m_strfileExtension = extensionarray[0];

	}
	m_strfullFileName = m_strFolderPath + "\\" + createTimeStampFile(m_strFileName.c_str()) + "." + m_strfileExtension;
	m_stroredFiles.clear();

	std::wstring search_path = std::wstring(m_strFolderPath.begin(), m_strFolderPath.end());

	std::vector<std::wstring> notincludefileextensions; // exclude file extensions
	notincludefileextensions.push_back(L".txt");

	get_all_files_names_within_folder(search_path, m_stroredFiles,&notincludefileextensions);
	deleteOlderFiles(nano100SecInHour, m_deletefilesOlderFilesinHours); // files older than 25 h


	int ret = open_output_file(m_strfullFileName.c_str(), fmt);
	if (this->m_pEncodings->size() > 0)  {

		if (m_pAvOutFormatCtx != nullptr)
		{
			deleteUnUsedEncodings(); // delete all not correct initalized encodings
			if (this->m_pEncodings->size() > 0)
			{
				av_dump_format(m_pAvOutFormatCtx, 0, m_strfullFileName.c_str(), 1);
				m_pAvOutFormatCtx->interrupt_callback.opaque = (void*)m_ptimeouthandler;
				m_pAvOutFormatCtx->interrupt_callback.callback = &timeout_handler::check_interrupt;
				ret = open_FFMpegoutput_file();
			}

		}



	}



	if (ret == 0) {
	//	this->startWriteOutPut();// start Output-Task

	}
	return ret;

}

int FFMpegOutputDevice::reopen_output()
{

	m_strfullFileName = m_strFolderPath + "\\" + createTimeStampFile(m_strFileName.c_str()) + "." + m_strfileExtension;
	int ret = open_FFMpegoutput_file();


	return ret;

}
/*
std::vector<std::string> FFMpegOutputDevice::splitintoArray(const std::string& s, const std::string& delim) 
{
	std::vector<string> result;
	char*pbegin = (char*) s.c_str();
	char * next_token =nullptr;
	char* token;
	token = strtok_s(pbegin, delim.c_str(),&next_token);
	while (token != nullptr) {
		// Do something with the tok
		result.push_back(token);
		token = strtok_s(NULL, delim.c_str(),&next_token);
	}

	return result;
}

*/


int FFMpegOutputDevice::open_output_file(const char *filename, AVOutputFormat *fmt)
{
	HRESULT hr = S_OK;

	AVStream *out_stream;
	AVStream *in_stream;
	
	AVCodecParameters *dec_ctx;
	AVCodecContext	*enc_ctx;
	AVCodec *inputencoder;
	AVCodec *encoder;
	int ret;
	unsigned int i;
	AVCodecID codec_id = AV_CODEC_ID_NONE;
	AVCodecID videocodec_id = AV_CODEC_ID_NONE;
	AVFormatContext *ifmt_ctx = m_pAvFormatCtx;

	

	AVCodecContext* pAvInputCodecCtx;

	if (m_ffmpegOptions != nullptr) {
		ParseOptions(m_ffmpegOptions);
	}
	


	if (fmt != nullptr) {
		videocodec_id = fmt->video_codec;
		if (videocodec_id == AV_CODEC_ID_H264) {

		}
	}


	m_pAvOutFormatCtx = nullptr;
	
	ret = avformat_alloc_output_context2(&m_pAvOutFormatCtx, fmt, nullptr, filename);


	if (!m_pAvOutFormatCtx) {
		TraceError("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&m_pAvOutFormatCtx, NULL, "mpeg", filename);

	}
	if (!m_pAvOutFormatCtx) {

		return AVERROR_UNKNOWN;
	}


	fmt = m_pAvOutFormatCtx->oformat;

	//dec_ctx

	for (i = 0; i < ifmt_ctx->nb_streams; i++) {

		in_stream = ifmt_ctx->streams[i];
		dec_ctx = in_stream->codecpar;

		MediaSampleFFMpegEncoding * encoding = nullptr;
		
		encoding = new MediaSampleFFMpegEncoding(this->m_pAvFormatCtx, this->m_RecordingInHours);
		encoding->setState(-1);//not completed
		this->m_pEncodings->push_back(encoding);
		
		inputencoder = nullptr;
		int audioStreamIndex = av_find_best_stream(ifmt_ctx, dec_ctx->codec_type, i, -1, &inputencoder, 0);


	//	inputencoder = avcodec_find_encoder(in_stream->codecpar->codec_id);
		if (inputencoder == nullptr)
		{
			return -1;
		}

		pAvInputCodecCtx = avcodec_alloc_context3(inputencoder);
		encoding->setCodecContext(pAvInputCodecCtx);

		if (avcodec_parameters_to_context(pAvInputCodecCtx, in_stream->codecpar) < 0)
		{
			return -1;
		}

	
		if (avcodec_open2(pAvInputCodecCtx, inputencoder, nullptr) < 0)
//		if (avcodec_open2(pAvInputCodecCtx, inputencoder, &this->m_avDict) < 0)
		{
			return -1;
		}


		//setCodecContext();
		//	out_stream = avformat_new_stream(m_pAvOutFormatCtx, encoder);
		AVCodecID codec_id = dec_ctx->codec_id;
		if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
			codec_id = fmt->video_codec;
		}
		if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			codec_id = fmt->audio_codec;
		}
		encoder = avcodec_find_encoder(codec_id);

		if (!encoder) {
			if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
				codec_id = fmt->video_codec;
				encoder = avcodec_find_encoder(codec_id);

			}
			if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
				codec_id = fmt->audio_codec;
				encoder = avcodec_find_encoder(codec_id);
				codec_id = fmt->audio_codec;
			}
			if (!encoder) {
				TraceError("#### Error: Necessary encoder not found");
			//	av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
				return AVERROR_INVALIDDATA;
			}
		}

	

		out_stream = avformat_new_stream(m_pAvOutFormatCtx, nullptr);
		encoding->SetAvOutStream(out_stream);
		//out_stream = avformat_new_stream(m_pAvOutFormatCtx, encoder);

		enc_ctx = avcodec_alloc_context3(encoder);
		if (!enc_ctx) {
			return -1;

		}

	//	out_stream->codec = enc_ctx;
		encoding->setEncCodecContext(enc_ctx);



		out_stream->id = m_pAvOutFormatCtx->nb_streams - 1;
		if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
			|| dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			/* in this example, we choose transcoding to same codec */


			/* In this example, we transcode to same properties (picture size,
			* sample rate etc.). These properties can be changed for output
			* streams easily using filters */
			if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {

				enc_ctx->codec_id = codec_id;
				enc_ctx->height = 480;
				enc_ctx->width = 640;

				//1280 × 0 720
				// SVGA 800x600
				// VGA 640x480

				if ( (m_height == -1) && (m_width == -1) ) {//  take from input
					enc_ctx->height = dec_ctx->height;
					enc_ctx->width = dec_ctx->width;
				}
				else
				{
					if (dec_ctx->height > m_height) enc_ctx->height = m_height;
					else if (dec_ctx->height > 0) enc_ctx->height = dec_ctx->height;

					if (dec_ctx->width > m_width) enc_ctx->width = m_width;
					else if (dec_ctx->width > 0 )  enc_ctx->width = dec_ctx->width;
				}


				enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
				/* take first format from list of supported formats */

				if (encoder->pix_fmts)
					enc_ctx->pix_fmt = encoder->pix_fmts[0];
			//	else
		//			enc_ctx->pix_fmt = dec_ctx->pix_fmt;


				/* video time_base can be set to whatever is handy and supported by encoder */
				/*
				if (m_fps == -1) {
					if ((av_q2d(dec_ctx->time_base)*dec_ctx->ticks_per_frame > av_q2d(in_stream->time_base)) && (av_q2d(in_stream->time_base) < 1.0 / 1000))
					{
						enc_ctx->time_base = dec_ctx->time_base;
						enc_ctx->time_base.num *= dec_ctx->ticks_per_frame;
					}
					else
					{
						enc_ctx->time_base = dec_ctx->time_base;
					}

				}
				else
				{
					enc_ctx->time_base.num = 1;
					if (m_fps<=30)	enc_ctx->time_base.den = m_fps;
					else enc_ctx->time_base.den = 30;
				}
				*/
	

				if (m_fps == -1) {
					//pAvInputCodecCtx->time_base.den = in_stream->r_frame_rate.num;
					//pAvInputCodecCtx->time_base.num = in_stream->r_frame_rate.den;
					pAvInputCodecCtx->time_base = av_inv_q(in_stream->r_frame_rate);

					/*
					if ((av_q2d(pAvInputCodecCtx->time_base)*pAvInputCodecCtx->ticks_per_frame > av_q2d(in_stream->time_base)) && (av_q2d(in_stream->time_base) < 1.0 / 1000))
					{
						enc_ctx->time_base = pAvInputCodecCtx->time_base;
						enc_ctx->time_base.num *= pAvInputCodecCtx->ticks_per_frame;
					}
					else
					{
						enc_ctx->time_base = pAvInputCodecCtx->time_base;
					}
					*/
					enc_ctx->time_base = pAvInputCodecCtx->time_base;
				}
				else
				{
					pAvInputCodecCtx->time_base = av_inv_q(in_stream->r_frame_rate);
					enc_ctx->time_base.num = 1;
					if (m_fps <= 30)	enc_ctx->time_base.den = m_fps;
					else enc_ctx->time_base.den = 30;
				}


				enc_ctx->bit_rate = this->m_bit_rate;
				

				if (enc_ctx->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
		
					enc_ctx->max_b_frames = 2;
				}
				if (enc_ctx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {

					enc_ctx->mb_decision = 2;
					enc_ctx->max_b_frames = 1;
				}

				if (enc_ctx->codec_id == AV_CODEC_ID_MJPEG) {
					enc_ctx->pix_fmt = AV_PIX_FMT_YUVJ420P;
					//		or		AV_PIX_FMT_YUVJ422P
				}


				if (enc_ctx->codec_id == AV_CODEC_ID_H264) {
					av_opt_set(enc_ctx->priv_data, "preset", "slow", 0);
					//		av_dict_set(&opts, "b", "2.5M", 0);
				}


		//		out_stream->time_base = enc_ctx->time_base;

	
	//			avcodec_parameters_to_context(pAvInputCodecCtx, out_stream->codecpar);


			}
			else {
				// Audio
				
				enc_ctx->bit_rate = 64000;

				/* check that the encoder supports s16 pcm input */
				enc_ctx->sample_fmt = AV_SAMPLE_FMT_S16;
		
				if (!FFmpegUty::check_sample_fmt(encoder, enc_ctx->sample_fmt)) {
				//	fprintf(stderr, "Encoder does not support sample format %s",av_get_sample_fmt_name(enc_ctx->sample_fmt));
					enc_ctx->sample_fmt = encoder->sample_fmts[0];

				//	return -1;
				}
				
				/* select other audio parameters supported by the encoder */
				enc_ctx->sample_rate = FFmpegUty::select_sample_rate(encoder);
				enc_ctx->channel_layout = FFmpegUty::select_channel_layout(encoder);
				enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
				enc_ctx->time_base.num = 1;
				enc_ctx->time_base.den = enc_ctx->sample_rate;

				out_stream->time_base.den = enc_ctx->sample_rate;
				out_stream->time_base.num = 1;
		//		enc_ctx->time_base = (AVRational) { 1, enc_ctx->sample_rate };


			//	out_stream->time_base = in_stream->time_base;
			//	out_stream->disposition = in_stream->disposition;
			//	out_stream->time_base.num = in_stream->time_base.num;
			//	out_stream->time_base.den = in_stream->time_base.den;
			}

			if (m_pAvOutFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
				enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
			/* Third parameter can be used to pass settings to encoder */
			int retcodec = avcodec_open2(enc_ctx, encoder, &this->m_avDict);
			if (retcodec < 0) {
				if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) { TraceError("#### error: Cannot open video encoder for stream #%u", i); }
				else { TraceError("#### error: Cannot open audio encoder for stream #%u", i); }
				//	av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
				return retcodec;


			}
			retcodec = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
			if (retcodec < 0)
			{
				return -1;
			}

			out_stream->time_base = enc_ctx->time_base;
		}
		else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
			//av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
			//return AVERROR_INVALIDDATA;
			return -1;;
		}
		else {
			
		}


		int ret = encoding->SetEncoding(m_pAvOutFormatCtx, i);
		encoding->setState(ret);


	}



	return 0;
}

int FFMpegOutputDevice::open_FFMpegoutput_file() {

	int ret = 0;

	if (!(m_pAvOutFormatCtx->oformat->flags & AVFMT_NOFILE)) {
		if (m_pAvOutFormatCtx->pb != nullptr) {
			//	  avio_flush(m_pAvOutFormatCtx->pb);
			writeTrailer();
			avio_closep(&m_pAvOutFormatCtx->pb); // close 
		}

		ret = avio_open(&m_pAvOutFormatCtx->pb, this->m_strfullFileName.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) {
			TraceError("Could not open output file '%s'", this->m_strfullFileName.c_str());
			return ret;
		}
		std::wstring storedfile = wstring(this->m_strfullFileName.begin(), this->m_strfullFileName.end());
		m_stroredFiles.push_back(storedfile);
	}

	m_bopenDevice = false;

	return writeHeader();
}



size_t FFMpegOutputDevice::deleteOlderFiles(__int64 timebase,double numberOfUnits)
{
	bool ret = false;
	SYSTEMTIME st;
	FILETIME ft;
	LARGE_INTEGER minetime, filetime;

	//DateTimeToSystemTime
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	minetime.LowPart = ft.dwLowDateTime;
	minetime.HighPart = ft.dwHighDateTime;
	size_t deletedfiles = 0;

	WIN32_FILE_ATTRIBUTE_DATA fd;

	size_t Idx = 0;
	while (Idx  < m_stroredFiles.size())	{
		//	for (it = m_stroredFiles.begin(); it != m_stroredFiles.end();it++) {
		wstring file = m_stroredFiles[Idx];
		BOOL fileatr = GetFileAttributesEx(file.c_str(), GetFileExInfoStandard, &fd);
	//	DWORD err = GetLastError();
		if (fileatr) {
			// Time for last write

			filetime.LowPart = fd.ftLastWriteTime.dwLowDateTime;
			filetime.HighPart = fd.ftLastWriteTime.dwHighDateTime;

			
			//if (minetime.QuadPart > filetime.QuadPart + nano100SecInHour *numberOfDays) {
			if (minetime.QuadPart  > filetime.QuadPart+ timebase *numberOfUnits) {
				// delete file
				::DeleteFile(file.c_str());
				m_stroredFiles.erase(m_stroredFiles.begin()+ Idx);
				deletedfiles++;
				continue;
			}
		}
		Idx++;
	}

	return 	(deletedfiles); // number of files deleted;

}




bool FFMpegOutputDevice::get_all_files_names_within_folder(std::wstring folder, std::vector<std::wstring> & names, std::vector<std::wstring> * pextfilters)
{

	std::wstring search_path =folder + L"//*.*";
	WIN32_FIND_DATA fd;
	wstring fullfilename;
	wchar_t* ext = new wchar_t[_MAX_EXT];
	//wchar_t ext[_MAX_EXT];
	HANDLE hFind = ::FindFirstFileEx(search_path.c_str(), FindExInfoStandard, &fd, FindExSearchNameMatch, NULL, 0);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				bool insert = true;
				
				if (pextfilters != nullptr) {
				
					_wsplitpath_s(fd.cFileName, nullptr,0, nullptr,0, nullptr,0, ext, _MAX_EXT);
					for (size_t i = 0; i < pextfilters->size();i++) 
					{
						if (_wcsicmp(pextfilters->at(i).c_str() ,ext) == 0 )
						{
							insert = false;
						}
					}
				}
				
			
				if (insert) 
				{
				  fullfilename = folder + L"//" + fd.cFileName;
				  names.push_back(fullfilename); 
				}
			}
			else
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
				if (wstring(fd.cFileName) == L".") continue;
				if (wstring(fd.cFileName) == L"..") continue;
				folder = folder + L"//"+ fd.cFileName;
				get_all_files_names_within_folder(folder, names, pextfilters);
			}

		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	delete[] ext;
	return names.size() > 0;
}


std::string FFMpegOutputDevice::createTimeStampFile(const char *filename)
{
	std::string ret = "";
	time_t rawtime;
	struct tm  timeinfo;
	char buffer[100];
	char filebuffer[MAX_PATH];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, sizeof(buffer), "%d_%m_%Y_%H_%M_%S",  &timeinfo);

	sprintf_s(filebuffer, sizeof(filebuffer), "%s_%s", buffer, filename);
	
	ret = std::string (filebuffer);
	return ret;



}

void FFMpegOutputDevice::setpts_Overrun() {
	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end();it++)
	{
		MediaSampleEncoding*pEnc = *it;

		pEnc->Setpts_Overrun();
	}
}


int FFMpegOutputDevice::reopen_outputAsync() {
	//flushEncoder();
//	setpts_Overrun();// set new Over-Run times 
	create_task([this]()
	{
		Lock();
	//	flushEncoder();
		int ret = reopen_output();
		if (ret < 0)
		{
			//	m_bopenDevice = false;
		}
	//	Sleep(500);


		UnLock();
	//	deleteOlderFiles(nano100SecInHour, deletefilesOlderFiles); // files older than 24 h
		return (ret);

	}).then([this](int ret) {

		deleteOlderFiles(nano100SecInHour, this->m_deletefilesOlderFilesinHours); // files older than 24 h
		//		av_packet_free(&pClonePaket);

	});


	return 0;


}

int FFMpegOutputDevice::writeInterleavedAsync(AVPacket* encodePacket) {

//	return 0;

	AVPacket*pCloneencodePacket = av_packet_clone(encodePacket);

	auto pClonedPacket = make_shared<AVPacket*>(pCloneencodePacket);

	create_task([this, pClonedPacket]()
	{
		Lock();
		m_ptimeouthandler->setTimeoutinMS(2000);
		int ret;
		if (m_pAvOutFormatCtx->nb_streams == 1) // only one stream write direct is performanter
		{
			ret = av_write_frame(m_pAvOutFormatCtx, *pClonedPacket);
		}
		else
		{
			ret = av_interleaved_write_frame(m_pAvOutFormatCtx, *pClonedPacket);
		}

		if (ret < 0)
		{
			//	m_bopenDevice = false;
		}

		av_packet_free(&(*pClonedPacket));
		UnLock();
		return (ret);
	
	}).then([this](int ret) {

//		av_packet_free(&pClonePaket);
	
	});


	return 0;


}


int FFMpegOutputDevice::writeInterleaved(AVPacket* encodePacket) {
	
	m_ptimeouthandler->setTimeoutinMS(2000);
	int ret = av_interleaved_write_frame(m_pAvOutFormatCtx, encodePacket);
	if (ret < 0)
	{
		//	m_bopenDevice = false;
	}
	return (ret);

}



int FFMpegOutputDevice::flushEncoder()
{
	int ret = 0;

	if (m_pEncodings == nullptr) return false;
	std::list<MediaSampleEncoding*>::iterator it;
	for (it = this->m_pEncodings->begin(); it != this->m_pEncodings->end();it++)
	{
		MediaSampleEncoding*pEnc = *it;
//		if (!(this->m_pAvOutFormatCtx->streams[pEnc->getStreamIdx()]->codec->codec->capabilities & AV_CODEC_CAP_DELAY)) {
//			continue;
//		}
		while (1)
		{
			AVPacket*encpacket = nullptr;
			int ret = pEnc->flushEncoder(&encpacket);
			if (encpacket != nullptr) {
				writeInterleavedAsync(encpacket);
				av_packet_free(&encpacket);// free encoded packet
			}
			else
			{

				break;
			}
		}
		

	}

	return ret;
	/*

	while (1) {
		if (!(m_pAvOutFormatCtx->streams[stream_index]->codec->codec->capabilities &
			AV_CODEC_CAP_DELAY))
			return 0;

		av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
		ret = encode_write_frame(NULL, stream_index, &got_frame);
		if (ret < 0)
			break;
		if (!got_frame)
			return 0;
	}
	return ret;
	*/




}


int FFMpegOutputDevice::writeTrailer()
{
	if (m_pAvOutFormatCtx == nullptr) return -1;
	if (!m_bopenDevice) return -1;
	//	AVFormatContext* ofmt_ctx = m_pAvOutFormatCtx;
	m_ptimeouthandler->setTimeoutinMS(2000);
	int ret = av_write_trailer(m_pAvOutFormatCtx);
	return (ret);

}

int FFMpegOutputDevice::writeHeader()
{
	if (m_pAvOutFormatCtx == nullptr) return -1;
	AVFormatContext* ofmt_ctx = m_pAvOutFormatCtx;
	/* init muxer, write output file header */
	m_ptimeouthandler->setTimeoutinMS(2000);
	int ret = avformat_write_header(m_pAvOutFormatCtx, NULL);
	
	if (ret < 0) {
	//av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
	//return ret;
		return ret;;
	}
	m_bopenDevice = true;
	return (ret);

}

bool FFMpegOutputDevice::WritePackage(MediaSampleEncoding* pEncoding, AVPacket* encodePacket) {

	if (m_pAvOutFormatCtx == nullptr) return false;
	if (!m_bopenDevice) return false;
	//writeInterleaved(encodePacket);
	writeInterleavedAsync(encodePacket);
	//this->m_packetQueue->PushPacket(encodePacket);




	return true;



}



FFMpegOutputCopyDevice::FFMpegOutputCopyDevice(Platform::String^ deviceName, AVFormatContext* inputFormaCtx, Platform::String^ folder, int fps, int height, int width, int64_t bit_rate, PropertySet^ ffmpegOutputOptions, Platform::String^ outputformat, double deletefilesOlderFilesinHours, double RecordingInHours)
	: FFMpegOutputDevice(deviceName, inputFormaCtx,  folder,  fps,  height,  width, bit_rate, ffmpegOutputOptions, outputformat,  deletefilesOlderFilesinHours,  RecordingInHours) {



}


FFMpegOutputCopyDevice::~FFMpegOutputCopyDevice(){


}



int FFMpegOutputCopyDevice::open_output_file(const char *filename, AVOutputFormat *fmt)
{
	HRESULT hr = S_OK;

	AVStream *out_stream;
	AVStream *in_stream;

	AVCodecParameters *dec_ctx;
	AVCodecContext	*enc_ctx;
	AVCodec *inputencoder;
//	AVCodec *encoder;
	int ret;
	unsigned int i;
	AVCodecID codec_id = AV_CODEC_ID_NONE;
	AVCodecID videocodec_id = AV_CODEC_ID_NONE;
	AVFormatContext *ifmt_ctx = m_pAvFormatCtx;

	AVCodecContext* pAvInputCodecCtx;


	if (fmt != nullptr) {
		videocodec_id = fmt->video_codec;
		if (videocodec_id == AV_CODEC_ID_H264) {

		}
	}


	m_pAvOutFormatCtx = nullptr;

	ret = avformat_alloc_output_context2(&m_pAvOutFormatCtx, fmt, nullptr, filename);


	if (!m_pAvOutFormatCtx) {
		TraceError("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&m_pAvOutFormatCtx, NULL, "mpeg", filename);

	}
	if (!m_pAvOutFormatCtx) {

		return AVERROR_UNKNOWN;
	}


	fmt = m_pAvOutFormatCtx->oformat;

	//dec_ctx

	for (i = 0; i < ifmt_ctx->nb_streams; i++) {

		in_stream = ifmt_ctx->streams[i];
		dec_ctx = in_stream->codecpar;

		MediaSampleFFMpegEncoding * encoding = nullptr;
	
		encoding = new MediaSampleFFMpegCopy(this->m_pAvFormatCtx, this->m_RecordingInHours);
		this->m_pEncodings->push_back(encoding);
		encoding->setState(-1);//not completed

		inputencoder = nullptr;
		int audioStreamIndex = av_find_best_stream(ifmt_ctx, dec_ctx->codec_type, i, -1, &inputencoder, 0);


		//	inputencoder = avcodec_find_encoder(in_stream->codecpar->codec_id);
		if (inputencoder == nullptr)
		{
			return -1;
		}

		pAvInputCodecCtx = avcodec_alloc_context3(inputencoder);

		encoding->setCodecContext(pAvInputCodecCtx);

		if (avcodec_parameters_to_context(pAvInputCodecCtx, in_stream->codecpar) < 0)
		{
			return -1;
		}

		if (avcodec_open2(pAvInputCodecCtx, inputencoder, NULL) < 0)
		{
			return -1;
		}


		//setCodecContext();
		//	out_stream = avformat_new_stream(m_pAvOutFormatCtx, encoder);
		AVCodecID codec_id = dec_ctx->codec_id;

		out_stream = avformat_new_stream(m_pAvOutFormatCtx, nullptr);
		encoding->SetAvOutStream(out_stream);
		//out_stream = avformat_new_stream(m_pAvOutFormatCtx, encoder);

		enc_ctx = avcodec_alloc_context3(nullptr);
		if (!enc_ctx) {
			return -1;

		}



		enc_ctx->codec_type = dec_ctx->codec_type;
		//	out_stream->codec = enc_ctx;
		encoding->setEncCodecContext(enc_ctx);



		out_stream->id = m_pAvOutFormatCtx->nb_streams - 1;
		if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
			|| dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			/* in this example, we choose transcoding to same codec */


			/* In this example, we transcode to same properties (picture size,
			* sample rate etc.). These properties can be changed for output
			* streams easily using filters */
			if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {

				enc_ctx->codec_id = codec_id;
				m_pAvOutFormatCtx->video_codec_id = codec_id;
				enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
				/* take first format from list of supported formats */

		//		AVPixelFormat outPixelFormat = MediaSampleEncoding::getVideoAVPixelFormat(pAvInputCodecCtx->pix_fmt);
		//		AVPixelFormat outPixelFormat = pAvInputCodecCtx->pix_fmt;

				AVPixelFormat outPixelFormat = FFmpegUty::getVideoAVPixelforDeprecatedFormat(pAvInputCodecCtx->pix_fmt);


				enc_ctx->pix_fmt = outPixelFormat;
		//		enc_ctx->pix_fmt = pAvInputCodecCtx->pix_fmt;
				enc_ctx->height = dec_ctx->height;
				enc_ctx->width = dec_ctx->width;

				pAvInputCodecCtx->time_base = av_inv_q(in_stream->r_frame_rate);
				enc_ctx->time_base = pAvInputCodecCtx->time_base;

				enc_ctx->bit_rate = m_bit_rate;

				/*
				video_file_stream->avg_frame_rate = rtsp_vidstream->avg_frame_rate;
				video_file_stream->r_frame_rate = rtsp_vidstream->r_frame_rate;
				video_file_stream->time_base = rtsp_vidstream->time_base;
				video_file_encoder_context->time_base = rtsp_vidstream_codec_context->time_base;
				*/
				//out_stream->avg_frame_rate = in_stream->avg_frame_rate;
				//out_stream->r_frame_rate = in_stream->r_frame_rate;

				if (enc_ctx->codec_id == AV_CODEC_ID_MPEG2VIDEO) {

					enc_ctx->max_b_frames = 2;
				}
				if (enc_ctx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {

					enc_ctx->mb_decision = 2;
					enc_ctx->max_b_frames = 1;
				}

				if (enc_ctx->codec_id == AV_CODEC_ID_MJPEG) {
					enc_ctx->pix_fmt = AV_PIX_FMT_YUVJ420P;
					//		or		AV_PIX_FMT_YUVJ422P
				}


				if (enc_ctx->codec_id == AV_CODEC_ID_H264) {
					av_opt_set(enc_ctx->priv_data, "preset", "slow", 0);
					//		av_dict_set(&opts, "b", "2.5M", 0);
				}

			}
			else {
				// Audio
				m_pAvOutFormatCtx->audio_codec_id = codec_id;
				enc_ctx->bit_rate = 64000;
				enc_ctx->bit_rate = pAvInputCodecCtx->bit_rate;

				/* check that the encoder supports s16 pcm input */
				enc_ctx->sample_fmt = pAvInputCodecCtx->sample_fmt;
				enc_ctx->sample_rate = pAvInputCodecCtx->sample_rate;

				enc_ctx->channels = pAvInputCodecCtx->channels;
				enc_ctx->time_base.num = 1;
				enc_ctx->time_base.den = pAvInputCodecCtx->sample_rate;
				enc_ctx->channel_layout = pAvInputCodecCtx->channel_layout;

			//	enc_ctx->sample_fmt = dec_ctx->sample_fmt;
			

				//if (!MediaSampleEncoding::check_sample_fmt(encoder, enc_ctx->sample_fmt)) 
				{
					//	fprintf(stderr, "Encoder does not support sample format %s",av_get_sample_fmt_name(enc_ctx->sample_fmt));
	//				enc_ctx->sample_fmt = encoder->sample_fmts[0];

					//	return -1;
				}

				/* select other audio parameters supported by the encoder */
			//	enc_ctx->sample_rate = MediaSampleEncoding::select_sample_rate(encoder);
		//		enc_ctx->channel_layout = MediaSampleEncoding::select_channel_layout(encoder);
		//		enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
		//		enc_ctx->time_base.num = 1;
		//		enc_ctx->time_base.den = enc_ctx->sample_rate;

				out_stream->time_base.den = enc_ctx->sample_rate;
				out_stream->time_base.num = 1;
				//		enc_ctx->time_base = (AVRational) { 1, enc_ctx->sample_rate };

			}

			if (m_pAvOutFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
				enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
			/* Third parameter can be used to pass settings to encoder */
		//	int retcodec = avcodec_open2(enc_ctx, encoder, NULL);
		
			int retcodec = avcodec_parameters_from_context(out_stream->codecpar, pAvInputCodecCtx);

		//	ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
			if (retcodec < 0)
			{
				return -1;
			}

			out_stream->time_base = enc_ctx->time_base;
		//	enc_ctx->pix_fmt = pAvInputCodecCtx->pix_fmt;
		}
		else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
			//av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
			//return AVERROR_INVALIDDATA;
			return -1;;
		}
		else {

		}


		int ret = encoding->SetEncoding(m_pAvOutFormatCtx, i);
		encoding->setState(ret);


	}



	return 0;
}




