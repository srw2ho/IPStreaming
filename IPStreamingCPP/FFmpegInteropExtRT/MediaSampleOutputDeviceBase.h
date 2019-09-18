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
#include <map>
#include "FFmpegTimeouthandler.h"
#include "MediaSampleOutputEncoder.h"


#include <ppl.h>
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


	inline std::vector<std::string> splitintoArray(const std::string& s, const std::string& delim)
	{
		std::vector<std::string> result;
		char*pbegin = (char*)s.c_str();
		char * next_token = nullptr;
		char* token;
		token = strtok_s(pbegin, delim.c_str(), &next_token);
		while (token != nullptr) {
			// Do something with the tok
			result.push_back(token);
			token = strtok_s(NULL, delim.c_str(), &next_token);
		}

		return result;
	}

	inline std::vector<std::wstring> splitintoArray(const std::wstring& s, const std::wstring& delim)
	{
		std::vector<std::wstring> result;
		wchar_t*pbegin = (wchar_t*)s.c_str();
		wchar_t * next_token = nullptr;
		wchar_t* token;
		token = wcstok_s(pbegin, delim.c_str(), &next_token);
		while (token != nullptr) {
			// Do something with the tok
			result.push_back(token);
			token = wcstok_s(NULL, delim.c_str(), &next_token);
		}

		return result;
	}






	typedef std::list<MediaSampleEncoding*>  MediaSampleEncodingList;


	enum EncodeTypes {
		EncodeNothing = 0x0, // nothing to do
		EncodePacket = 0x1, // encode packet
		CopyPacket = 0x2,	// copy packet
	};

	class MediaSampleOutputDevice
	{
	public:
		MediaSampleOutputDevice(Platform::String^ deviceName, AVFormatContext* inputFormaCtx, PropertySet^ configOptions);
		virtual ~MediaSampleOutputDevice();


		int getfps() { return m_fps; };
		int getheigh() {return m_height; };
		int getwidth() { return m_width; };
		int64_t getbit_rate() { return m_bit_rate; };

		virtual void SetRecordingActiv(bool activ) {m_RecordingActiv = activ;}; // recording activated

		virtual bool DeleteEncoding(MediaSampleEncoding*pdeleteEncoding);
		virtual void DeleteAllEncodings(void);
		virtual bool EncodeAndWriteFrames(FramePacket* avPacket);
		virtual bool CopyAndWriteFrames(FramePacket* avPacket);
		virtual bool IsCopyOutPutDevice();
		virtual bool IsEncodingOutPutDevice();

		virtual bool WritePackage(MediaSampleEncoding* pEncoding, AVPacket* encodePacket) {return false;};
		virtual int reopen_outputAsync() { return -1; };// reopenout file with new filename


		virtual bool IsMediaTypeEncodingOutput(AVMediaType type); // is Video or Audio-Encoding 

		virtual bool IsStreamEncodingOutput(int stream_Idx, int& encType); // is Straem-Idx encoding 
		

		Platform::String^ getDeviceName() { return m_strdeviceName; };

		virtual bool IsRecordingActiv() { return m_RecordingActiv; }; // recording activated

		virtual bool ParseConfigOptions();
		virtual AVFormatContext* getAvOutFormatCtx() { return nullptr; };

		virtual std::string& getFileName() { return std::string(""); };
	protected:

		bool CopyFrame(MediaSampleEncoding*pEnc,FramePacket* avPacket);
		bool EncodeFrame(MediaSampleEncoding*pEnc,FramePacket* avPacket);

	protected:

		MediaSampleEncodingList * m_pEncodings;
		AVFormatContext* m_pAvFormatCtx;
		Platform::String^ m_strdeviceName;
		PropertySet^ m_ConfigOptions;
		int m_fps;
		int m_height;
		int m_width;
		int64_t m_bit_rate;
		bool m_RecordingActiv;
	};

	typedef std::map<int, FFmpegInteropExtRT::MediaSampleOutputDevice*> MapConfigOptions;


}