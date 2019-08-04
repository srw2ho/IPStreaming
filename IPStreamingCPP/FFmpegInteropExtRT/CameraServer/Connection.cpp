#include "..\pch.h"
#include <ppl.h>
#include "Connection.h"




using namespace concurrency;
using namespace Platform;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace FFmpegInteropExtRT;



 // Pre-canned response sent to HTTP clients
const char s_responseHeader[] =
    "HTTP/1.1 200 OK\r\n"
    "Cache-Control: no-cache\r\n"
    "Pragma: no-cache\r\n"
    "Expires: Thu, 01 Dec 1994 16:00:00 GMT\r\n"
    "Connection: close\r\n"
    "Content-Type: multipart/x-mixed-replace; boundary=CameraServerBoundary\r\n"
    "\r\n"
    "--CameraServerBoundary\r\n";

const char s_sendClientHeader[] =
"POST /stream/640/480/ HTTP/1.1\r\n"
"Host: %s\r\n"
//"Host: localhost:3000\r\n"
"Accept: '*/*'\r\n"
"Transfer-encoding:chunked\r\n"
"Cache-Control:no-cache\r\n"
//"icy - metadata: 1\r\n"
"Connection: keep-alive\r\n"
//"Cache-Control: no-cache\r\n"
//"Pragma: no-cache\r\n"
//"Expires: Thu, 01 Dec 1994 16:00:00 GMT\r\n"
"\r\n";

const char s_OPENCVClientHeader[] = "Connect from";

Connection::Connection(_In_ StreamSocket^ socket, ConnectionType connectiontype)
	: m_acceptingData(false)
	, m_closed(false)
	, m_socket(socket)

{
	NT_ASSERT(socket != nullptr);

	m_pSocketCancelTaskToken = new concurrency::cancellation_token_source();

	m_ConnectionType = connectiontype;

	m_EventSource = ref new SocketEventSource;






}



char* Connection::GetRemoteHostPortname()
{
	HostName^ remotehostName = m_socket->Information->RemoteHostName;
	String^ remoteport = m_socket->Information->RemotePort;
	String^  strhostname = remotehostName->ToString();
	String^ newhostname = strhostname + ":" + remoteport;
	return StringtoAscIIChars(newhostname);

}

char* Connection::GetRemoteHostname()
{
	HostName^ remotehostName = m_socket->Information->RemoteHostName;

	String^  strhostname = remotehostName->ToString();

	return StringtoAscIIChars(strhostname);

}




void Connection::CancelAsyncSocketOperation(void){

	m_pSocketCancelTaskToken->cancel();
	Close();

}







Connection::~Connection()
{
	this->Close();
	

	if (m_socket != nullptr) {
		delete m_socket; // calls IClosable::Close()
		m_socket = nullptr;
	}
	delete m_pSocketCancelTaskToken;
	

}

void Connection::Close()
{
    auto lock = m_lock.LockExclusive();
	
    m_closed = true;
    m_acceptingData = false;
	if (m_socket != nullptr) {
	//	delete m_socket; // calls IClosable::Close()


	}
}



void Connection::SendData(_In_ IBuffer^ buffer)
{
	IOutputStream^ stream;

	//Trace("@%p SendData", (void*)this);
	auto lock = m_lock.LockExclusive();

	if (m_closed || !m_acceptingData)
	{
		Trace("@%p dropping HTTP part @%p: closed %i, acceptingData %i", (void*)this, (void*)buffer, m_closed, m_acceptingData);
		return;
	}

	stream = m_socket->OutputStream;

	auto token = m_pSocketCancelTaskToken->get_token();
	auto WriteAsync = stream->WriteAsync(buffer);


	create_task(WriteAsync).then([this,stream,buffer](unsigned int sendbytes)  {

		if (sendbytes > 0) {
		//	Trace("@%p completed sending HTTP part @%p", (void*)this, (void*)buffer);

			auto lock = m_lock.LockExclusive();
			if (!m_closed)
			{
				m_acceptingData = true;
			}
			return stream->FlushAsync();
		}
		else
		{ // when nothing is sended -> cancel
			//this->CancelAsyncSocketOperation();
			cancel_current_task();
		}

	}, token).then([](task<bool> previos) // multiple threads are queueing, before a Connection Close of Remote are recognized
	{
		bool bflushed = false;
		try
		{
			bflushed = previos.get();
		}

		catch (task_canceled const &) // cancel by User
		{
			return;
		}
		catch (Exception^ exception) {
			return;
		}

		catch (...)
		{
			return;
		}
	});



	
}


void Connection::NotifyMessage(_In_ IBuffer^ buffer)
{

	SendData(buffer);

}


connections::connections() {
	InitializeCriticalSection(&m_CritLock);

	}

connections::~connections() {

	DeleteAllConnections();
	DeleteCriticalSection(&m_CritLock);
}

Connection* connections::getConnectionbySocket(Windows::Networking::Sockets::StreamSocket^ socket)
{
	Connection* pRetConn = nullptr;
	Lock();
	std::list<Connection*>::iterator it;
	for (it = this->begin(); it != this->end();it++)
	{
		Connection*pVglConn = *it;
		if (pVglConn->getSocket() == socket) {
			pRetConn = pVglConn;
			break;
		}

	}
	UnLock();
	return pRetConn;

}


Mep1Connection::Mep1Connection(_In_ Windows::Networking::Sockets::StreamSocket^ socket) :Connection(socket, ConnectionType::HttpMep1VideoClient)
{
	m_psendClientHeader = nullptr;
	//if (m_ConnectionType == ConnectionType::HttpMep1VideoClient) //Client Connection-Type
	{
		BuildClientHeader();

	}
}

void Mep1Connection::BuildClientHeader() {


	char* phostname = GetRemoteHostPortname();
	size_t lenclientheader = sizeof(s_sendClientHeader) + strlen(phostname) + 100;

	m_psendClientHeader = new char[lenclientheader];
	sprintf_s(&m_psendClientHeader[0], lenclientheader, s_sendClientHeader, phostname);

	delete[] phostname;

}

Mep1Connection::~Mep1Connection() {
	if (m_psendClientHeader != nullptr) {
		delete[] m_psendClientHeader;
		m_psendClientHeader = nullptr;
	}
}


void Mep1Connection::WriteHeader()

{
	StreamSocket^ socket = m_socket;
	NT_ASSERT(socket != nullptr);
	auto token = m_pSocketCancelTaskToken->get_token();

	create_task([this, socket]() {
		size_t len = strlen(m_psendClientHeader);
		auto responseHeaderBuffer = ref new Buffer(len);
		(void)memcpy(GetData(responseHeaderBuffer), m_psendClientHeader, responseHeaderBuffer->Capacity);


		responseHeaderBuffer->Length = responseHeaderBuffer->Capacity;


		Trace("@%p sending HTTP Client header to socket @%p: %iB", (void*)this, (void*)m_socket, responseHeaderBuffer->Length);
		return socket->OutputStream->WriteAsync(responseHeaderBuffer);


	}, token).then([this, socket](unsigned int sendvalues)
	{
		return socket->OutputStream->FlushAsync();

	}).then([this, socket](bool ret)
	{
		auto requestBuffer = ref new Buffer(4096);
		Trace("@%p received HTTP request from socket @%p", (void*)this, (void*)m_socket);

		if (!m_closed)
		{
			m_acceptingData = true;
		}
		return socket->InputStream->ReadAsync(requestBuffer, requestBuffer->Capacity, InputStreamOptions::Partial);

	}).then([this, socket](IBuffer^ buf)
	{

		//	unsigned char *data = GetData(buf);


	}).then([this](task<void> t)
	{
		try
		{
			t.get();
		}

		catch (task_canceled const &) // cancel by User
		{
			this->getSocketEventSource()->fire(this->getSocket(), NotificationEventReason::CanceledbyUser);
			return;
		}

		catch (Exception^ exception) {
			this->getSocketEventSource()->fire(this->getSocket(), NotificationEventReason::CanceledbyError);
			return;
		}
		catch (...)
		{
			this->getSocketEventSource()->fire(this->getSocket(), NotificationEventReason::CanceledbyError);
			return;
		}
		this->getSocketEventSource()->fire(this->getSocket(), NotificationEventReason::EndRequest);
		return;
	});

	return;

}

MJpegConnection::MJpegConnection(_In_ Windows::Networking::Sockets::StreamSocket^ socket):Connection(socket, ConnectionType::HttpMJpegVideoServer)
{

}

MJpegConnection::~MJpegConnection() {

}

void MJpegConnection::WriteHeader()

{
	StreamSocket^ socket = m_socket;
	NT_ASSERT(socket != nullptr);

	auto token = m_pSocketCancelTaskToken->get_token();

	task_from_result().then([this, socket]()
	{
		auto requestBuffer = ref new Buffer(4096);
		return socket->InputStream->ReadAsync(requestBuffer, requestBuffer->Capacity, InputStreamOptions::Partial);
	}, token).then([this, socket](IBuffer^)
	{
		Trace("@%p received HTTP request from socket @%p", (void*)this, (void*)m_socket);

		auto responseHeaderBuffer = ref new Buffer(sizeof(s_responseHeader) - 1);
		(void)memcpy(GetData(responseHeaderBuffer), s_responseHeader, responseHeaderBuffer->Capacity);
		responseHeaderBuffer->Length = responseHeaderBuffer->Capacity;

		Trace("@%p sending HTTP response header to socket @%p: %iB", (void*)this, (void*)m_socket, responseHeaderBuffer->Length);

		return socket->OutputStream->WriteAsync(responseHeaderBuffer);
	}).then([this, socket](unsigned int)
	{
		return socket->OutputStream->FlushAsync();
	}).then([this, socket](bool)
	{
		auto lock = m_lock.LockExclusive();
		if (!m_closed)
		{
			m_acceptingData = true;
		}
		auto requestBuffer = ref new Buffer(4096);
		return socket->InputStream->ReadAsync(requestBuffer, requestBuffer->Capacity, InputStreamOptions::Partial);
	}).then([this](task<IBuffer ^> t)
	{
		IBuffer^ buf;
		try
		{
			buf = t.get();
			if (buf == nullptr) {
				cancel_current_task();
			}
			else
				if (buf->Length == 0) {
					cancel_current_task();
				}
		}

		catch (task_canceled const &) // cancel by User
		{
			this->getSocketEventSource()->fire(this->getSocket(), NotificationEventReason::CanceledbyUser);
			return;
		}

		catch (Exception^ exception) {
			this->getSocketEventSource()->fire(this->getSocket(), NotificationEventReason::CanceledbyError);
			return;
		}

		catch (...)
		{
			this->getSocketEventSource()->fire(this->getSocket(), NotificationEventReason::CanceledbyError);
			return;
		}

	});
}

/*

OpenCVConnection::OpenCVConnection(_In_ Windows::Networking::Sockets::StreamSocket^ socket) :Connection(socket, ConnectionType::OPENCVClient)
{

}

OpenCVConnection::~OpenCVConnection() {

}

void OpenCVConnection::WriteHeader()
{
	StreamSocket^ socket = m_socket;

	auto token = m_pSocketCancelTaskToken->get_token();

	DataWriter^ dataWriter = ref new DataWriter(socket->OutputStream);
	dataWriter->WriteString("Connect");
	// Send the contents of the writer to the backing stream.
	create_task(dataWriter->StoreAsync(),token).then([this, dataWriter](unsigned int bytesStored)
	{
		// For the in-memory stream implementation we are using, the FlushAsync() call 
		// is superfluous, but other types of streams may require it.
		return dataWriter->FlushAsync();
	}).then([this, socket](bool flushOp)
	{
		auto lock = m_lock.LockExclusive();
		if (!m_closed)
		{
			m_acceptingData = true;
		}
		auto requestBuffer = ref new Buffer(4096);
		return socket->InputStream->ReadAsync(requestBuffer, requestBuffer->Capacity, InputStreamOptions::Partial);

	}).then([this](task<IBuffer ^> t)
	{
		IBuffer^ buf;
		try
		{
			buf = t.get();
			if (buf == nullptr) {
				cancel_current_task();
			}
			else
				if (buf->Length == 0) {
					cancel_current_task();
				}
		}

		catch (task_canceled const &) // cancel by User
		{
			this->getSocketEventSource()->fire(this->getSocket(), NotificationEventReason::CanceledbyUser);
			return;
		}

		catch (Exception^ exception) {
			this->getSocketEventSource()->fire(this->getSocket(), NotificationEventReason::CanceledbyError);
			return;
		}

		catch (...)
		{
			this->getSocketEventSource()->fire(this->getSocket(), NotificationEventReason::CanceledbyError);
			return;
		}
	});
}
*/

Connection* connections::AddConnection(Connection*pConn)
{
	Lock();
	Connection* pRetConn = pConn;
	this->push_back(pRetConn);
	UnLock();
	return pRetConn;

}

bool connections::DeleteConnectionBySocket(Windows::Networking::Sockets::StreamSocket^ socket) {
	bool found = false;
	Lock();
	std::list<Connection*>::iterator it;
	for (it = this->begin(); it != this->end();it++)
	{
		Connection*pConn = *it;
		if (pConn->getSocket() == socket)
		{
			this->remove(pConn);
			delete pConn;
			found = true;
			break;
		}

	}

	UnLock();
	return found;

}



bool connections::DeleteConnection(Connection*pdeleteConn) {
	bool found = false;
	Lock();
	std::list<Connection*>::iterator it;
	for (it = this->begin(); it != this->end();it++)
	{
		Connection*pConn = *it;
		if (pConn == pdeleteConn)
		{
			this->remove(pConn);
			delete pConn;
			found = true;
			break;
		}

	}

	UnLock();
	return found;

}

void connections::DeleteAllConnections(void) {
	Lock();
	while (!this->empty())
	{
		Connection*pConn = this->back();
		this->pop_back();
		pConn->Close();

		delete pConn;

	}
	UnLock();

}

bool connections::getConnectionsByType(unsigned int  type, connectionlist& connections) {

	bool found = false;
	Lock();
	std::list<Connection*>::iterator it;
	for (it = this->begin(); it != this->end();it++)
	{
		Connection*pConn = *it;
		if ((pConn->getConnectionTyp() & type) == type)
		{
			connections.push_back(pConn);
		}

	}

	UnLock();
	return (connections.size() > 0);


}

void connections::UnLock() {

	LeaveCriticalSection(&m_CritLock);
}

void connections::Lock() {
	EnterCriticalSection(&m_CritLock);
	}
