#pragma once
#include "..\EventBroadCaster.h"

namespace FFmpegInteropExtRT {

	enum ConnectionType
	{
		HttpMep1VideoClient = 0x1,	// VideoMPegClient -> AVPacket Data to MepgServer
		HttpMJpegVideoServer = 0x2,	// Motion JPPEG -> AVPacket to MJPEG Client (Browser)
		OPENCVClient = 0x4			// AVFrame -> OpenCV Server (AVFrame->OPENCV-Server)
	};


	class Connection {

	public:

		Connection(_In_ Windows::Networking::Sockets::StreamSocket^ socket, ConnectionType connectiontype);
		~Connection();


		virtual void NotifyMessage(_In_ Windows::Storage::Streams::IBuffer^ buffer);

		virtual void WriteHeader()=0;

		virtual void SendData(_In_ Windows::Storage::Streams::IBuffer^ buffer);


		void Close();
		Windows::Networking::Sockets::StreamSocket^ getSocket() { return m_socket; };

		ConnectionType getConnectionTyp() { return m_ConnectionType; };
		void Connection::CancelAsyncSocketOperation(void);

		SocketEventSource ^ getSocketEventSource() { return m_EventSource; };
		char* GetRemoteHostPortname();
		char* GetRemoteHostname();
		
	protected:

		SocketEventSource ^ m_EventSource;
		Windows::Networking::Sockets::StreamSocket^ m_socket;
		bool m_acceptingData;
		bool m_closed;
		ConnectionType m_ConnectionType;

		Microsoft::WRL::Wrappers::SRWLock m_lock;
		concurrency::cancellation_token_source* m_pSocketCancelTaskToken;



	protected:
		Connection(const Connection&) = delete;
		Connection& operator=(const Connection&) = delete;




	};

	class Mep1Connection : public Connection {

	private:
		char* m_psendClientHeader;
	public:

		Mep1Connection(_In_ Windows::Networking::Sockets::StreamSocket^ socket);
		~Mep1Connection();
		virtual void WriteHeader();
	private:

		void BuildClientHeader();

	};

	class MJpegConnection : public Connection {

	public:
	
		MJpegConnection(_In_ Windows::Networking::Sockets::StreamSocket^ socket);
		~MJpegConnection();
		virtual void WriteHeader();

	private:

	};
	/*
	class OpenCVConnection : public Connection {

	public:

		OpenCVConnection(_In_ Windows::Networking::Sockets::StreamSocket^ socket);
		~OpenCVConnection();
		virtual void WriteHeader();

	private:

	};
	*/
	typedef std::list<Connection*>  connectionlist;
	class connections : public connectionlist
	{
	private:
		CRITICAL_SECTION m_CritLock;
	public:
		connections(void);
		virtual ~connections();
		connectionlist* getconnnectionlist() { return (connectionlist*) this; }
	//	Connection* getConnection(Connection*pConn);
		Connection* getConnectionbySocket(Windows::Networking::Sockets::StreamSocket^ socket);
		Connection* AddConnection(Connection*pConn);
		bool DeleteConnectionBySocket(Windows::Networking::Sockets::StreamSocket^ socket);
		bool DeleteConnection(Connection*pConn);
		void DeleteAllConnections(void);
		bool getConnectionsByType(unsigned int type, connectionlist& connections );
		void UnLock();
		void Lock();
	};




//delegate void CancelEventhandler(Connection sender, Platform::String^ s);







}