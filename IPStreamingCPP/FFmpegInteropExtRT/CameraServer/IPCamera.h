#pragma once




namespace FFmpegInteropExtRT
{

	public ref class CameraServerFailedEventArgs sealed
	{
	public:

		property int ErrorCode { int get() { return m_errorCode; } }
		property Platform::String^ Message { Platform::String^ get() { return m_message; } }

	internal:

		CameraServerFailedEventArgs(_In_ Platform::Exception^ e)
			: m_errorCode(e->HResult)
			, m_message(e->Message)
		{
		}

	private:

		int m_errorCode;
		Platform::String^ m_message;
	};

	public ref class IPAddress sealed
	{
	public:

		property Windows::Networking::HostNameType Type { Windows::Networking::HostNameType get() { return m_type; } }
		property Platform::String^ Name { Platform::String^ get() { return m_name; } }

	internal:

		IPAddress(_In_ Windows::Networking::HostNameType type, _In_ Platform::String^ name)
			: m_name(name)
			, m_type(type)
		{
		}

	private:

		Windows::Networking::HostNameType m_type;
		Platform::String^ m_name;
	};



	public ref class CameraServer sealed
	{


	public:

		CameraServer();
	
		static Windows::Foundation::IAsyncOperation<CameraServer^>^ CreateCameraServerAsync();


		 Windows::Foundation::IAsyncAction ^ CreateHTTPServerAsync(int port);
		 Windows::Foundation::IAsyncAction ^  CameraServer::DestroyHTTPServerAsync(); 
		 void OpenHTTPServerAsync(); // close all connections, new connections are accepted
		 void CloseHTTPServerAsync();// close all connections, new connections are accepted
		 Windows::Foundation::IAsyncAction ^ CreateHTTPClientAsync(Platform::String^ hostNameForConnect, int port);
	//	 Windows::Foundation::IAsyncAction ^ CreateOpenCVClientAsync(Platform::String^ hostNameForConnect, int port);

		 

		 void CloseHTTPClientAsync();	//Close all connected clients
//		 void CloseOpenCVClientAsync(); //Close all OpenCV clients
	
		virtual ~CameraServer();

		///<summary>Raised when the CameraServer object becomes non-functional</summary>
		event Windows::Foundation::TypedEventHandler<Platform::Object^, CameraServerFailedEventArgs^>^ Failed;

		///<summary>IP addresses CameraServer is listening on</summary>
		///<remarks>IP addresses are either IPv4 or IPv6</remarks>
		property Windows::Foundation::Collections::IVectorView<IPAddress^>^ IPAddresses
		{
			Windows::Foundation::Collections::IVectorView<IPAddress^>^ get() { return m_ipAddresses; }
		}

		///<summary>TCP port CameraServer is listening on</summary>
		property int Port { int get() { return m_port; } }


		void DispatchHttpMpegVideoPart(_In_ Windows::Storage::Streams::IBuffer^ buffer);
		void DispatchHttpMJpegVideoPart(_In_ Windows::Storage::Streams::IBuffer^ buffer);
	//	void DispatchOpenCVVideoPart(_In_ Windows::Storage::Streams::IBuffer^ buffer);
		


		bool HasConnectionFromTypeHttpMpeg();
		bool HasConnectionFromTypeHttpMJpeg();
	//	bool HasConnectionFromTypeOpenCV();

	internal:

		bool HasConnectionFromType(unsigned int type);
		std::list<Connection*> *GetConnections() {return &m_connections;};


		void OnNotifiationEventhandler(Windows::Networking::Sockets::StreamSocket^ socket, int status);
	private:

	


		void OnConnectionReceived(
			_In_ Windows::Networking::Sockets::StreamSocketListener^ sender,
			_In_ Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^ e
		);
		/*
		void ReceiveStringLoop(
			Windows::Storage::Streams::DataReader^ reader,
			Windows::Networking::Sockets::StreamSocket^ socket);
*/
		void CancelMpegConnections();
		void CancelJMpegConnections();
//		void CancelOpenCVConnections();
		void UnLock();
		void Lock();


	//	SocketEventSource ^ m_SocketEventSource;;

		Windows::Networking::Sockets::StreamSocketListener^ m_listener;
		connections m_connections;
		Windows::Foundation::Collections::IVectorView<IPAddress^>^ m_ipAddresses;
		int m_port;

		Microsoft::WRL::Wrappers::SRWLock m_lock;
		CRITICAL_SECTION m_CritLock;

		concurrency::cancellation_token_source* m_pAsyncClientCancelTaskToken;

		Windows::Foundation::EventRegistrationToken _OnConnectionReceivedToken;
	};
}