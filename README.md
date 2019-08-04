# IPStreamin (UWP-Project divided in two packages IPStreamingCPP and OnVifUniversal)
Extended FFmpegInterop in conjunction with OnVif capable IP cameras for recording / remuxing ffmpeg input streams

Created with Visual Studio 2017

Usable for Raperry Pi: with streaming rate (640x480)@10fps, no audio, only 1 camera active (tested with  Pi 3 B and standard Touch Display 800x480), rtsp-input of IP camera (AMCREST 1080P) are remuxed to output file in local APP storage folder. Streaming settings are configured, that each hour a new file are created, older files than 72 hours are deleted automatically.

# Package IPStreamingCPP (C++ UWP)

FFmpeg straming based on FFmpegInterop : https://github.com/Microsoft/FFmpegInterop (my sincerely thank to the programmers, a wonderful project)
with extensions as follow:
- rtsp-IP-streaming with timeout behavior
- rtsp-IP-streaming parameter for minimal time lag between camera and screen, please using adjustment "Force Video"
- rtsp-IP-streaming: adjustment "Force Video" should be used, and Video or Audio can be switch on/off
- Muxing or remuxing ffmpeg input stream to output file:
  - recorded files are stored in local APP storage folder
  - in output format (.mp4, ...) with variable parameters (size, fps)
  - remuxing input to output file in .mp4 format, parameters comes from input stream
  - older recorded files can be deleted: after choosen time older files are deleted to prevent storage overrun
  - recorded input can be splitted in multiple files: each hour (adjustable) a new file is created
  - automatic restarting of input stream to prevent time lag between camera input and shown screen display. Experience had shown, that in case of more than 12 hour continuous streaming, a time lag between camera output and screen display can occurs. To prevent that a restart of streaming each 12 hour is meaningful.
  
- Muxing ffmpeg input stream to video format for using in connected browser clients:
  - mpegvideo format i sended to node.js listener which can streaming incoming steam over WebSocket to connected clients browser
  - start MJPEG server for supporting connected browser clients with streaming data in MJPEG format
  
 - using OnVifUniversal-Library as reference for reading out streaming url used as rtsp-ffmpeg input
 - using OnVifUniversal-Library for adding / editing OnVif IP-cameras
 
 - more than One OnVif-camera are supported, before you can use streaming you have to add Onvif Camera with (IP adress, User,     Password). User and Password for camera access are often (admin/admin). Please attend, camera can only used in local network.
 - all Streaming adjustements of each camera are store in local storage for reloading after new start

# Package OnVifUniversal (.Net UWP)
- Library with interface to IP cameras in local network, which are supports OnVif interface (e.g. AMCREST 1080P)
- reading out of camera profile (video or audio): streaming url is used for ffmpeg streaming input
- setting camera profile parameter: video.FrameRate, video.Resolution, video.Bitrate, audio.Bitrate, audio.SampleRate 
- PTZ controlling
- important camera adjustments are store in local storage for reproducing after progra restart

# Package OnVifDiscoveryApp (.Net Win32 Desktop -App)
- Console App for Discovery Onvif capable device in local network
- Datagram Server listen to a Socket (30000) for processing any request from Discovery-Client (e.g. IPStreamingCPP)
- After Client request founded OnVif Cients are returned  (information item like IP Adress, http-Web-Adress, DNS-Name) to requester. So returned information  can be further used for reading out streaming url of Onvif Devives or for PTZ controlling.
- direct accessing to local Onvif Devices in UWP-App was not possible. "System.ServiceModel.Discovery" is not available in UWP, therefore function was be outsourced in own win32 .Net App implementation. First, OnVifDiscoveryApp.exe must be started before any OnVif discovery request from IPStreamingCPP can be responsed.
 
# Actual Problem in Release Version of IPStreamingCpp (C++ UWP)
I use OnVifUniversal .Net Library as reference in IPStreamingCPP (C++ UWP). There are a problem regarding .Net native building. 
In Release Version, I am not able to connect OnVif camera, rejected with exception (ServiceModel::CommunicationException, System::Net::Http::HttpRequestException).
If I try Connection with Debug-Version instead there are no problems, connection is established.
I think .Net Online Compiler ICL load at time the right moduls/library.
If I use OnVifUniversal .Net Library as reference in own .Net Release (UWP-App), no connections problems. 
.Net natvive Building must be differenct between C++ and .Net App.

While building the project in IPStreamingCPP (C++ UWP) Release the native compiler throws an warning. This warning comes only when I use OnVifUniversal in C++ UWP, using OnVifUniversal in .NET App no warning while building.
Warning while .Net native Compilation:
2>CSC : warning : Ungültiger Suchpfad 'lib\um\x64' in 'LIB-Umgebungsvariable' -- 'Das System kann den angegebenen Pfad nicht finden. '.
2>Processing application code
2> Resources.System.Linq.Expressions.rd.xml(35): warning : ILTransform_0027: Method 'CreateLambda' within 'System.Linq.Expressions.Expression' could not be found.
2> Resources.System.Linq.Expressions.rd.xml(91): warning : ILTransform_0027: Method 'ParameterIsAssignable' within 'System.Linq.Expressions.Expression' could not be found.

Up to now I have no idea what is the reason for that. Does anyone have a idea or could help?

Workaround (no solution) for Native building:	IPStreamingCPP.vcxproj: setting 'UseDotNetNativeToolchain>false</UseDotNetNativeToolchain' in Release building.
The benefit is, that in release version ffmpeg-streaming and viewing on display is much more faster as in debug version.

Short progam explanation in FFmpegInteropExt.pdf.

Type errors are to apologize.
