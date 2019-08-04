#include "..\pch.h"
#include "Connection.h"
#include "IPCamera.h"



using namespace concurrency;
using namespace FFmpegInteropExtRT;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Media::Capture;
using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;




IAsyncOperation<CameraServer^>^ CameraServer::CreateCameraServerAsync()
{
	Trace("@ creating CameraServer on port ");

	
	return create_async([]()
	{
		auto server = ref new CameraServer();
		return server;

	});
	
}



IAsyncAction ^ CameraServer::CreateHTTPClientAsync(Platform::String^ hostNameForConnect, int port)
{

	Platform::String^ servicename = port.ToString();

	auto server = this;
	
	return create_async([this,server, hostNameForConnect, servicename]()
	->void {

		Trace("@%p creating CameraServer on adress %s:%s with camera", (void*)server, hostNameForConnect, servicename);
		CancelMpegConnections();

	HostName^ hostName;
	try
	{
		hostName = ref new HostName(hostNameForConnect);
	}
	catch (InvalidArgumentException^ e)
	{

	}

	StreamSocket^ socket = ref new StreamSocket();


	auto token = this->m_pAsyncClientCancelTaskToken->get_token();
	socket->Control->KeepAlive = false;

	{
		create_task(socket->ConnectAsync(
			hostName,
			servicename,
			SocketProtectionLevel::PlainSocket),token).then([this,server, socket](task<void> previousTask)
		{
			try
			{
				previousTask.get();
				Connection* pConn = new Mep1Connection(socket); // Connection from Type MepgVideo Client
				pConn = m_connections.AddConnection(pConn);
				if (pConn != nullptr) {
					pConn->getSocketEventSource()->eventhandler += ref new FFmpegInteropExtRT::NotificationEventhandler(this, &CameraServer::OnNotifiationEventhandler);
					pConn->WriteHeader();
				}

			}
			catch (task_canceled const &) // cancel by User
			{
			//	this->Failed(this, ref new CameraServerFailedEventArgs(exception));
				delete socket;
			}

			catch (Exception^ exception )
			{

				this->Failed(this, ref new CameraServerFailedEventArgs(exception));
				delete socket;

			}
		});
	}
	});

}

void CameraServer::CloseHTTPClientAsync() {

	this->CancelMpegConnections();

}

/*
void CameraServer::CloseOpenCVClientAsync() {

	this->CancelOpenCVConnections();

}



IAsyncAction ^ CameraServer::CreateOpenCVClientAsync(Platform::String^ hostNameForConnect, int port)
{

	Platform::String^ servicename = port.ToString();

	auto server = this;

	return create_async([this, server, hostNameForConnect, servicename]()
		->void {

		Trace("@%p creating CameraServer on adress %s:%s with camera", (void*)server, hostNameForConnect, servicename);
		CancelOpenCVConnections();

		HostName^ hostName;
		try
		{
			hostName = ref new HostName(hostNameForConnect);
		}
		catch (InvalidArgumentException^ e)
		{

		}

		StreamSocket^ socket = ref new StreamSocket();


		auto token = this->m_pAsyncClientCancelTaskToken->get_token();
		socket->Control->KeepAlive = false;

		{
			create_task(socket->ConnectAsync(
				hostName,
				servicename,
				SocketProtectionLevel::PlainSocket), token).then([this, server, socket](task<void> previousTask)
			{
				try
				{
					previousTask.get();
					Connection* pConn = new OpenCVConnection(socket); // Connection from Type MepgVideo Client
					pConn = m_connections.AddConnection(pConn);
					if (pConn != nullptr) {
						pConn->getSocketEventSource()->eventhandler += ref new FFmpegInteropExtRT::NotificationEventhandler(this, &CameraServer::OnNotifiationEventhandler);
						pConn->WriteHeader();
					}

				}
				catch (task_canceled const &) // cancel by User
				{
					//	this->Failed(this, ref new CameraServerFailedEventArgs(exception));
					delete socket;
				}

				catch (Exception^ exception)
				{

					this->Failed(this, ref new CameraServerFailedEventArgs(exception));
					delete socket;

				}
			});
		}
	});

}


void CameraServer::CancelOpenCVConnections()
{
	m_connections.Lock();
	connectionlist connections;
	m_connections.getConnectionsByType(ConnectionType::OPENCVClient, connections);
	// cancel all pending Connections from getted Type
	while (!connections.empty())
	{
		Connection*pConn = connections.back();
		connections.pop_back();

		pConn->CancelAsyncSocketOperation();

	}
	m_connections.UnLock();
}
*/
void CameraServer::CancelMpegConnections()
{
	m_connections.Lock();
	connectionlist connections;
	m_connections.getConnectionsByType(ConnectionType::HttpMep1VideoClient , connections);
	// cancel all pending Connections from getted Type
	while (!connections.empty())
	{
		Connection*pConn = connections.back();
		connections.pop_back();

		pConn->CancelAsyncSocketOperation();

	}
	m_connections.UnLock();
}

void CameraServer::CancelJMpegConnections()
{
	m_connections.Lock();
	connectionlist connections;
	m_connections.getConnectionsByType(ConnectionType::HttpMJpegVideoServer, connections);
	// cancel all pending Connections from getted Type
	while (!connections.empty())
	{
		Connection*pConn = connections.back();
		connections.pop_back();

		pConn->CancelAsyncSocketOperation();

	}
	m_connections.UnLock();
}

IAsyncAction ^ CameraServer::CreateHTTPServerAsync(int port)
{


	auto server = this;
	Trace("@%p creating CameraServer on port %i with camera", (void*)server, port);

	

	return create_async([server, port]()
	{

		server->m_listener->Control->KeepAlive = false;
		return create_task(server->m_listener->BindServiceNameAsync(port == 0 ? L"" : port.ToString()))

		.then([server] {

			server->m_port = _wtoi(server->m_listener->Information->LocalPort->Data());
			if (server->m_port == 0)
			{
				throw ref new InvalidArgumentException(L"Failed to convert TCP port");
			}
			Trace("@%p bound socket listener to port %i", (void*)server, server->m_port);

			auto ipAddresses = ref new Vector<IPAddress^>();
			for (HostName^ host : NetworkInformation::GetHostNames())
			{
				if ((host->Type == HostNameType::Ipv4) || (host->Type == HostNameType::Ipv6))
				{
					Trace("@%p network IP %S %S", (void*)server, host->Type.ToString()->Data(), host->CanonicalName->Data());
					ipAddresses->Append(ref new IPAddress(host->Type, host->CanonicalName));
				}
			}
			server->m_ipAddresses = ipAddresses->GetView();


		});


	});
	

}


void CameraServer::OpenHTTPServerAsync()
{
	this->CancelJMpegConnections();
	
	

	if (_OnConnectionReceivedToken.Value == 0 )
	{
		_OnConnectionReceivedToken, m_listener->ConnectionReceived +=
			ref new TypedEventHandler<StreamSocketListener^, StreamSocketListenerConnectionReceivedEventArgs^>(
				this,
				&CameraServer::OnConnectionReceived
				);
		_OnConnectionReceivedToken.Value++;
	}
}

void CameraServer::CloseHTTPServerAsync()
{
	this->CancelJMpegConnections();
	if (_OnConnectionReceivedToken.Value > 0) {

		m_listener->ConnectionReceived -= _OnConnectionReceivedToken; //keine neue Connections zulassen
		_OnConnectionReceivedToken.Value--;
	}


}

CameraServer::CameraServer()
	: m_listener(ref new StreamSocketListener())
	, m_port(-1)
{

	
	

	InitializeCriticalSection(&m_CritLock);
	

	m_pAsyncClientCancelTaskToken = new concurrency::cancellation_token_source;
//	m_SocketEventSource = ref new SocketEventSource;


}





void CameraServer::Lock()
{
	EnterCriticalSection(&m_CritLock);
}
void CameraServer::UnLock()
{
	LeaveCriticalSection(&m_CritLock);
}

CameraServer::~CameraServer()
{
	auto lock = m_lock.LockExclusive();


	m_connections.DeleteAllConnections();

	
	delete m_listener; // calls IClosable::Close()
	m_listener = nullptr;

	DeleteCriticalSection(&m_CritLock);
	delete m_pAsyncClientCancelTaskToken;

}



void CameraServer::OnNotifiationEventhandler(Windows::Networking::Sockets::StreamSocket^ socket, int status)
{
	m_connections.DeleteConnectionBySocket(socket);

}

void CameraServer::OnConnectionReceived(_In_ StreamSocketListener^ sender, _In_ StreamSocketListenerConnectionReceivedEventArgs^ e)
{
	auto lock = m_lock.LockExclusive();


	Trace("@%p new connection received: socket @%p", (void*)this, (void*)e->Socket);
	Connection* pConn = new MJpegConnection(e->Socket);

	pConn = m_connections.AddConnection(pConn);
	if (pConn != nullptr) {
		pConn->getSocketEventSource()->eventhandler += ref new FFmpegInteropExtRT::NotificationEventhandler(this, &CameraServer::OnNotifiationEventhandler);
		pConn->WriteHeader();
	}
	
}



bool CameraServer::HasConnectionFromTypeHttpMpeg() {

	return HasConnectionFromType(ConnectionType::HttpMep1VideoClient);
}
bool CameraServer::HasConnectionFromTypeHttpMJpeg() {

	return HasConnectionFromType(ConnectionType::HttpMJpegVideoServer);
}

/*
bool CameraServer::HasConnectionFromTypeOpenCV() {

	return HasConnectionFromType(ConnectionType::OPENCVClient);
}
*/



bool CameraServer::HasConnectionFromType(unsigned int type)
{
	//auto lock = m_lock.LockExclusive();
	m_connections.Lock();
	std::list<Connection*>::iterator it;
	for (it = m_connections.begin(); it != m_connections.end();it++)
	{
		Connection*pConn = *it;
		if ((pConn->getConnectionTyp() & type) == type)
		{
			m_connections.UnLock();
			return true;
		}
	}
	m_connections.UnLock();
	return false;
}



void CameraServer::DispatchHttpMpegVideoPart(_In_ IBuffer^ buffer)
{
//	Trace("@%p dispatching HTTP-Mpeg part @%p", (void*)this, (void*)buffer);

	auto lock = m_lock.LockExclusive();

	m_connections.Lock();
	connectionlist connections;
	m_connections.getConnectionsByType(ConnectionType::HttpMep1VideoClient, connections);
	std::list<Connection*>::iterator it;
	for (it = connections.begin(); it != connections.end();it++)
	{
		Connection*pConn = *it;
		pConn->NotifyMessage(buffer);
	}
	m_connections.UnLock();

}
/*
void CameraServer::DispatchOpenCVVideoPart(_In_ IBuffer^ buffer)
{
	//	Trace("@%p dispatching HTTP-Mpeg part @%p", (void*)this, (void*)buffer);

	auto lock = m_lock.LockExclusive();

	m_connections.Lock();
	connectionlist connections;
	m_connections.getConnectionsByType(ConnectionType::OPENCVClient, connections);
	std::list<Connection*>::iterator it;
	for (it = connections.begin(); it != connections.end(); it++)
	{
		Connection*pConn = *it;
		pConn->NotifyMessage(buffer);
	}
	m_connections.UnLock();

}
*/


void CameraServer::DispatchHttpMJpegVideoPart(_In_ IBuffer^ buffer)
{
//	Trace("@%p dispatching HTTP-MJpeg part @%p", (void*)this, (void*)buffer);
	auto lock = m_lock.LockExclusive();

	m_connections.Lock();
	connectionlist connections;
	m_connections.getConnectionsByType(ConnectionType::HttpMJpegVideoServer, connections);
	std::list<Connection*>::iterator it;
	for (it = connections.begin(); it != connections.end();it++)
	{
		Connection*pConn = *it;
		pConn->NotifyMessage(buffer);
	}
	m_connections.UnLock();

}

