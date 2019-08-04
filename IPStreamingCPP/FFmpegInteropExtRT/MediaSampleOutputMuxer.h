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
#include "..\CameraServer\Connection.h"
#include "..\CameraServer\IPCamera.h"

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

	typedef std::list<MediaSampleEncoding*>  MediaSampleEncodingList;

	enum EncodeTypes {
		EncodeNothing = 0x0, // nothing to do
		EncodePacket = 0x1, // encode packet
		CopyPacket = 0x2,	// copy packet
	};

	class MediaSampleOutputDevice
	{
	public:
		MediaSampleOutputDevice(Platform::String^ deviceName, AVFormatContext* inputFormaCtx);
		virtual ~MediaSampleOutputDevice();

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


//		virtual bool WriteEndFrames();
	protected:

		bool CopyFrame(MediaSampleEncoding*pEnc,FramePacket* avPacket);
		bool EncodeFrame(MediaSampleEncoding*pEnc,FramePacket* avPacket);
	protected:

		MediaSampleEncodingList * m_pEncodings;
		AVFormatContext* m_pAvFormatCtx;
		Platform::String^ m_strdeviceName;
	};

	class CameraOutputDevice: public MediaSampleOutputDevice
	{
	public:
		CameraOutputDevice(Platform::String^ deviceName,AVFormatContext* inputFormaCtx, CameraServer^ m_pCameraServer);
		virtual ~CameraOutputDevice();

		MediaSampleEncoding* AddMpegEncoding(int fps, int height, int width, int64_t bit_rate);
		MediaSampleEncoding* AddMJpegEncoding(int fps, int height, int width,int64_t bit_rate);
		virtual bool WritePackage(MediaSampleEncoding* pEncoding, AVPacket* encodePacket);
		virtual bool IsEncodingOutPutDevice();

	protected:

		IBuffer^ CreateHttMJpegChunksFromAvPacket(AVPacket* avPacket);
		IBuffer^ CreateHttpMpegChunksfromAvPacket(AVPacket* avPacket);
	protected:

		CameraServer^ m_pCameraServer;
	};


	class FFMpegOutputDevice : public MediaSampleOutputDevice
	{
	public:
		FFMpegOutputDevice(Platform::String^ deviceName,AVFormatContext* inputFormaCtx, Platform::String^ Folder, int fps, int height, int width, int64_t bit_rate, PropertySet^ ffmpegOutputOptions, Platform::String^ OutputFormat, double deletefilesOlderFilesinHours, double RecordingInHours);
		virtual ~FFMpegOutputDevice();


		virtual int open_output();
		virtual int reopen_outputAsync();// reopenout file with new filename

		virtual bool WritePackage(MediaSampleEncoding* pEncoding, AVPacket* encodePacket);
	//	virtual bool isTimeForNewFileCreation();
		
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
	//	std::vector<std::string> splitintoArray(const std::string& s, const std::string& delim);


		void UnLock();
		void Lock();
		bool get_all_files_names_within_folder(std::wstring folder, std::vector<std::wstring>& names, std::vector<std::wstring> * pextfilters =nullptr);
		size_t deleteOlderFiles(__int64 timebase, double numberOfUnits); // delete older files older than numberOfDays
		virtual void deleteUnUsedEncodings();
		bool ParseOptions(PropertySet^ ffmpegOptions);

	protected:
		Platform::String^ m_strFolder;
		AVFormatContext* m_pAvOutFormatCtx;
		timeout_handler * m_ptimeouthandler;
		bool m_bopenDevice;
	//	bool m_bCopyInputStream;
		std::string m_strFolderPath;
		std::string m_strFileName;
		std::string m_strOutputFormat;
		
		std::string m_strfileExtension;
		std::string m_strfullFileName;
		std::vector<std::wstring> m_stroredFiles;
		CRITICAL_SECTION m_CritLock;

		PropertySet^ m_ffmpegOptions;
		AVDictionary* m_avDict;
		int m_fps;
		int m_height;
		int m_width;

		int64_t m_bit_rate;
		double m_deletefilesOlderFilesinHours;
		double m_RecordingInHours;

	};

	class FFMpegOutputCopyDevice : public FFMpegOutputDevice
	{
	public:
		FFMpegOutputCopyDevice(Platform::String^ deviceName, AVFormatContext* inputFormaCtx, Platform::String^ Folder, int fps, int height, int width, int64_t bit_rate, PropertySet^ ffmpegOutputOptions, Platform::String^ OutputFormat, double deletefilesOlderFilesinHours, double RecordingInHours);
		virtual ~FFMpegOutputCopyDevice();


	protected:

		virtual int open_output_file(const char *filename, AVOutputFormat *fmt = nullptr);
		virtual int flushEncoder() { return 0; }; // there is no encoder
	protected:
	

	};

	typedef std::list<MediaSampleOutputDevice*>  MediaSampleOutputDevices;

	



	class MediaSampleOutputEncoding
	{


	public:
		MediaSampleOutputEncoding(AVFormatContext* inputFormaCtx);

		virtual ~MediaSampleOutputEncoding();
		CameraOutputDevice* CreateCameraOutputDevice(Platform::String^ deviceName,CameraServer^ pCameraServer);
		FFMpegOutputDevice* CreateFFMpegOutputDevice(Platform::String^ deviceName, Platform::String^ folder, int fps, int height, int width, int64_t bit_rate, PropertySet^ ffmpegOutputOptions, Platform::String^ outputformat, double deletefilesOlderFilesinHours, double videoTimeinHours);

		virtual void WriteAVPacketToMuxer(AVPacket* avPacket); //Link to m_pOutputEncoding
		virtual void WriteAVFrameToMuxer(AVFrame* avFame, AVPacket* avPacket, int streamIdx); //Link to m_pOutputEncoding

		MediaSampleOutputDevice* getDeviceByName(Platform::String^ deviceName);
		void startEncoding();
		void stopEncoding();
		Concurrency::task<int> DostopEncoding();
		bool DeleteDevice(MediaSampleOutputDevice*pdeleteDevice);
		void DeleteAllDevices(void);
		int DecodeandProcessAVPacket(AVPacket* avPacket);// !! imprtand: must be called into FFmpegReader-Thread , AVFormatContext* inputFormaCtx not threadsafed
		bool IsMediaTypeEncodingOutput(AVPacket* avPacket); // is Video or Audio-Encoding
		bool IsStreamEncodingOutput(int streamIdx, int& encType); // is stream-Idx encoding

		bool EncodeAndWriteFrames(FramePacket* Packet);

		Concurrency::task<void>  DoEncoding();//start encoding task

	//	bool WriteEndFrames();


	protected:
//		int DecodeAudioAVPacket(AVPacket* ppacket);
//		int DecodeVideoAVPacket(AVPacket* ppacket);

		HRESULT GetFrameFromFFmpegDecoder(AVCodecContext*pAvCodecCtx, AVPacket* avPacket);
		void InitCodecContext(AVFormatContext* inputFormaCtx);
		void ProcessDecodedFrame(int streamIdx, AVFrame* pFrame);
		void ProcessPacket(int streamIdx, AVPacket* pPacket);
		void UnLock();
		void Lock();
	protected:
	
		AVFrameQueue* m_packetQueue;
		concurrency::cancellation_token_source* m_pCancelTaskToken;
		bool m_btaskStarted;
		Concurrency::task<void> m_doEncodingTsk;
		CRITICAL_SECTION m_CritLock;
		AVFormatContext* m_pAvFormatCtx;
		AVFrame* m_pAvDecFrame;
		MediaSampleOutputDevices* m_pOutputDevices;
		AVCodecContext* m_pAvVideoCtx;
		AVCodecContext* m_pAvAudioCtx;
	
	};
	

}