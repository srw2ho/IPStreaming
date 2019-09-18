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

#include "MediaSampleOutputEncoder.h"
#include "MediaSampleOutputEncoderMPegJPeg.h"
#include "MediaSampleOutputEncoderFFMpegFile.h"

#include <ppl.h>

#include "MediaSampleOutputDeviceBase.h"

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
using namespace Windows::System::Threading;

namespace FFmpegInteropExtRT
{

	typedef std::map<int, ThreadPoolTimer^ > MapThreadPoolTimerConfigOptions;

	class FFMpegOutputDevice : public MediaSampleOutputDevice
	{
	public:
	//	FFMpegOutputDevice(Platform::String^ deviceName, AVFormatContext* inputFormaCtx, Platform::String^ Folder, int fps, int height, int width, int64_t bit_rate, PropertySet^ ffmpegOutputOptions, Platform::String^ OutputFormat, double deletefilesOlderFilesinHours, double RecordingInHours);
		FFMpegOutputDevice(Platform::String^ deviceName, AVFormatContext* inputFormaCtx, PropertySet^ configOptions, PropertySet^ ffmpegOutputOptions);


		virtual ~FFMpegOutputDevice();


		virtual int open_output();
		virtual int reopen_outputAsync();// reopenout file with new filename

		virtual bool WritePackage(MediaSampleEncoding* pEncoding, AVPacket* encodePacket);

		virtual bool ParseConfigOptions();
		virtual void setRecordingTimeinSec(double time) { m_RecordingTimeinSec = time; }
		MapThreadPoolTimerConfigOptions& getReordingPoolTimers() { return m_RecordingPoolTimers; }

		virtual std::string& getFileName() { return m_strFileName; };

		virtual bool getMovementActiv() { return m_bMovementActiv; };
		virtual void setMovementActiv(bool Activ) { m_bMovementActiv = Activ; };

	protected:
		virtual int writeInterleaved(AVPacket* encodePacket);
		virtual int writeInterleavedAsync(AVPacket* encodePacket);
		virtual int open_output_file(const char *filename, AVOutputFormat *fmt = nullptr);
		virtual int open_FFMpegoutput_file(); // open FFMpeFile, Write, Header
		virtual int reopen_output();
		virtual int writeTrailer();
		virtual int writeHeader();
		virtual int flushEncoder();
		std::string createTimeStampFile(const char *filename);
		virtual void setpts_Overrun();
		virtual void writeNewHeader();
	//	std::vector<std::string> splitintoArray(const std::string& s, const std::string& delim);


		void UnLock();
		void Lock();
		bool get_all_files_names_within_folder(std::wstring folder, std::vector<std::wstring>& names, std::vector<std::wstring> * pextfilters =nullptr);
		size_t deleteOlderFiles(__int64 timebase, double numberOfUnits); // delete older files older than numberOfDays
		virtual void deleteUnUsedEncodings();
		bool ParseOptions();
	//	void SyncPackageTime(MediaSampleEncoding* pEncoding, AVPacket* encodePacket);
		static void OnMapChanged(Windows::Foundation::Collections::IObservableMap<Platform::String ^, Platform::Object ^> ^sender, Windows::Foundation::Collections::IMapChangedEventArgs<Platform::String ^> ^event);
		void cancelPendingTimeouts();
		static void delayRecording(FFMpegOutputDevice*pDevice,double recordingTimeinSec);
	protected:
		Platform::String^ m_strFolder;
		AVFormatContext* m_pAvOutFormatCtx;
		timeout_handler * m_ptimeouthandler;
		bool m_bopenDevice;
		bool m_bMovementActiv;
		std::string m_strFolderPath;
		std::string m_strFileName;
		std::string m_strOutputFormat;
		
		std::string m_strfileExtension;
		std::string m_strfullFileName;
		std::vector<std::wstring> m_stroredFiles;
		CRITICAL_SECTION m_CritLock;

		PropertySet^ m_ffmpegOptions;
		AVDictionary* m_avDict;

		double m_deletefilesOlderFilesinHours;
		double m_RecordingInHours;

		double m_RecordingTimeinSec;
		Windows::Foundation::EventRegistrationToken m_ConfigOptionsEvent;
		MapThreadPoolTimerConfigOptions m_RecordingPoolTimers;
	};

	class FFMpegOutputCopyDevice : public FFMpegOutputDevice
	{
	public:
		FFMpegOutputCopyDevice(Platform::String^ deviceName, AVFormatContext* inputFormaCtx, PropertySet^ configOptions, PropertySet^ ffmpegOutputOptions);

		virtual ~FFMpegOutputCopyDevice();


	protected:

		virtual int open_output_file(const char *filename, AVOutputFormat *fmt = nullptr);
		virtual int flushEncoder() { return 0; }; // there is no encoder
	protected:
	

	};


	

}