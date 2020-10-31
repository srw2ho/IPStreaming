#include "pch.h"
#include <codecvt>
#include <string>
#include "AmcrestMotionDetection.h"

using namespace AmcrestMotionDetection;
using namespace Platform;


using namespace concurrency;

using namespace Windows::Foundation;

using namespace Windows::Security::Cryptography;

using namespace Windows::Storage::Streams;

using namespace Windows::UI::Xaml;

using namespace Windows::UI::Xaml::Navigation;

using namespace Windows::Web;

using namespace Windows::Web::Http;

using namespace Windows::Web::Http::Filters;

using namespace Windows::Web::Http::Headers;
using namespace Windows::Security::Credentials;


inline Platform::String^ stringToPlatformString(std::string inputString) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring intermediateForm = converter.from_bytes(inputString);
	Platform::String^ retVal = ref new Platform::String(intermediateForm.c_str());

	return retVal;
}


AmcrestMotion::AmcrestMotion()
{
	//m_httpClient = CreateHttpClient();
	m_httpClient = nullptr;
	m_cancellationTokenSource = nullptr;
	this->m_readBuffer = ref new Buffer(1500);
	m_bProcessingPackagesStarted = false;
	m_Events = ref new Platform::Collections::Vector<Platform::String^ >();
	m_Movement = false;
	m_resourceAddress = nullptr;


}
AmcrestMotion::~AmcrestMotion() {

	if (m_cancellationTokenSource != nullptr)
	{
		delete m_cancellationTokenSource;

	}

	if ((m_httpClient != nullptr)) {
		delete m_httpClient;
	}

}

void AmcrestMotion::HostName::set(Platform::String^ value) {
	m_HostName = value;

}

void AmcrestMotion::Password::set(Platform::String^ value) {
	m_Password = value;

}

void AmcrestMotion::User::set(Platform::String^ value) {
	m_User = value;

}

void AmcrestMotion::IsMoment::set(bool value) {
	m_Movement = value;
}

IAsyncAction^ AmcrestMotion::startProcessingPackagesAsync(Windows::Foundation::Collections::PropertySet^ inputconfigoptions, Windows::Foundation::Collections::PropertySet^ outputconfigoptions)
{

	auto tsk= create_async([this, inputconfigoptions, outputconfigoptions]()
		->void {
			StartMotionDetection(inputconfigoptions, outputconfigoptions);

		});

	return tsk;
}



void AmcrestMotion::StartMotionDetection(Windows::Foundation::Collections::PropertySet^ inputconfigoptions, Windows::Foundation::Collections::PropertySet^ outputconfigoptions)
{
	m_Motionoutput = outputconfigoptions;

	if (m_bProcessingPackagesStarted) return;



	bool doStart = false;
	if (inputconfigoptions->HasKey("CameraName") && inputconfigoptions->HasKey("User") && inputconfigoptions->HasKey("Password")) {
		doStart = true;
	}
	if (!doStart) return;


	Platform::Object^ hostName = inputconfigoptions->Lookup("CameraName");
	if (hostName != nullptr) {
		this->HostName = safe_cast<IPropertyValue^>(hostName)->GetString();
	}
	Platform::Object^ User = inputconfigoptions->Lookup("User");
	if (hostName != nullptr) {
		this->User = safe_cast<IPropertyValue^>(User)->GetString();
	}
	Platform::Object^ Password = inputconfigoptions->Lookup("Password");
	if (Password != nullptr) {
		this->Password = safe_cast<IPropertyValue^>(Password)->GetString();
	}

	CreateHttpClient();
	doEventsDetection();


}


Windows::Foundation::IAsyncAction^ AmcrestMotion::stopProcessingPackagesAsync() {

	auto tsks = create_async([this]()
		{
			StopMotionDetection();
		});

	return tsks;
}

void AmcrestMotion::StopMotionDetection() {

	CancelEventsDetection();
}

HttpClient^ AmcrestMotion::CreateHttpClient()

{
	if (m_httpClient == nullptr) {


		wchar_t buffer[500];
		//	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"http://%s/cgi-bin/eventManager.cgi?action=attach&codes=[VideoMotion,MoveDetection,MDResult]", m_HostName->Data());
		swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"http://%s/cgi-bin/eventManager.cgi?action=attach&codes=[All][&keep alive = 20]", m_HostName->Data());
		//swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"http://%s/cgi-bin/eventManager.cgi?action=attach&codes=[All]", m_HostName->Data());


		Platform::String^ res = ref new String(buffer);

		m_resourceAddress = ref new Uri(res);

		m_filter = ref new HttpBaseProtocolFilter();
		m_filter->AllowUI = true;

//		m_filter->MaxVersion = HttpVersion::Http10;

		auto pw = ref new PasswordCredential();
		pw->Resource = res;

		pw->UserName = m_User;
		pw->Password = m_Password;

			
		pw->RetrievePassword();

		m_filter->ServerCredential = pw;

		m_filter->IgnorableServerCertificateErrors->Append(Windows::Security::Cryptography::Certificates::ChainValidationResult::Expired);

		m_filter->IgnorableServerCertificateErrors->Append(Windows::Security::Cryptography::Certificates::ChainValidationResult::Untrusted);

		m_filter->IgnorableServerCertificateErrors->Append(Windows::Security::Cryptography::Certificates::ChainValidationResult::InvalidName);

		
		//filter->MaxConnectionsPerServer = 5;
		//filter->CacheControl->ReadBehavior;

		// if not cleared then request does not function one time
		m_filter->ClearAuthenticationCache();
		// if not cleared then request does not function one time
		m_httpClient = ref new HttpClient(m_filter);

		//m_httpClient->DefaultRequestHeaders->Connection->Clear();
		//m_httpClient->DefaultRequestHeaders->Connection->
	}
	else {
		m_filter->ClearAuthenticationCache();
	}

	return m_httpClient;

}

void AmcrestMotion::setstoppStreamingEvent(Platform::String^ err) {
	if (m_bStreamingStarted) {
		DisableMovementActivated(); // muaa vom gleichen Thread wie 
		m_bStreamingStarted = false;
		this->stopStreaming(this, err);
	}

}
//muss vom gleichen Thread wie die "m_MovementActiv" - Events erfolgen
void AmcrestMotion::DisableMovementActivated() {

	m_Motionoutput->Insert("m_MovementActivated", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(0)));
}
bool AmcrestMotion::doEventsDetection() {

	m_bProcessingPackagesStarted = true;
	if (m_cancellationTokenSource != nullptr)
	{
		delete m_cancellationTokenSource;

	}
	m_cancellationTokenSource = new cancellation_token_source();

	m_Movement = false;
	m_ParseState = AmcrestDetectionParseState::ParseHeader;
	m_FirstProcessEvents = true;

	HttpRequestMessage^ request = ref new HttpRequestMessage(HttpMethod::Get, m_resourceAddress);

	request->Headers->Append(ref new Platform::String(L"Connection"), ref new Platform::String(L"Keep-Alive"));
	m_ProcessingPackagesTsk = create_task(
	

		m_httpClient->SendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead),


		m_cancellationTokenSource->get_token()).then([this](HttpResponseMessage^ response)

			{
				return create_task(response->Content->ReadAsInputStreamAsync(), m_cancellationTokenSource->get_token())

					.then([=, this](IInputStream^ stream)

						{
							m_bStreamingStarted  = true;
							startStreaming(this, m_Motionoutput);
							return this->doReadData(stream);

						}, m_cancellationTokenSource->get_token()).then([=, this](task<IBuffer^ >  tskbuffer)

							{
								try
								{
									auto b = tskbuffer.get();

									wchar_t statuscodetxt[200];
									
									swprintf(statuscodetxt, sizeof(statuscodetxt), L"Amcrest Motion Streaming stopped: %s (StatusCode:%d)", stringToPlatformString(m_ResponseTextState)->Data(), response->StatusCode);

									Platform::String^ err = ref new Platform::String(statuscodetxt);

									setstoppStreamingEvent(err);
								}
					
								catch (Platform::Exception^ args)

								{
									Platform::String^ err = ref new String();
									err = args->Message;
									setstoppStreamingEvent(err);

								}
								catch (const task_canceled&)
								{
									setstoppStreamingEvent(nullptr);
								}

							});



			}).then([this](task<void> previousTask)

				{
					// This sample uses a "try" in order to support cancellation.
					// If you don't need to support cancellation, then the "try" is not needed.
					try
					{
						// Check if the task was canceled.

						previousTask.get();
					}
	
					catch (Platform::Exception^ args)
					{
						Platform::String^ err = ref new String();
						err = args->Message;
						setstoppStreamingEvent(err);
					}
					catch (const task_canceled&)
					{
						setstoppStreamingEvent(nullptr);
					}
				}
			);

			return false;
}


task<IBuffer^> AmcrestMotion::doReadData(IInputStream^ stream)

{

	// Do an asynchronous read.

	return create_task(

		stream->ReadAsync(m_readBuffer, m_readBuffer->Capacity, InputStreamOptions::Partial),

		m_cancellationTokenSource->get_token()).then([=, this](task<IBuffer^> readTask)
			{
				IBuffer^ buffer = readTask.get();
				m_response.clear();
				auto  dataReader = Windows::Storage::Streams::DataReader::FromBuffer(buffer);
				while (dataReader->UnconsumedBufferLength > 0) {
					auto bytes = ref new Array<byte>(buffer->Length);
					dataReader->ReadBytes(bytes);
					m_response.append(bytes->begin(), bytes->end());
				}
				if (m_response.size() > 0) {
					this->doParseResponse();
				}
				return buffer->Length ? doReadData(stream) : readTask;
			});

}
void AmcrestMotion::CancelEventsDetection()

{
	if (!m_bProcessingPackagesStarted) return;

	m_bProcessingPackagesStarted = false;

	if (m_cancellationTokenSource != nullptr) {
		m_cancellationTokenSource->cancel();
	}


	// waiting for end tasks
	m_ProcessingPackagesTsk.wait();

	m_Events->Clear();
	this->m_Movement = false;
	ChangeMovement(this, m_Motionoutput);



	if ((m_httpClient != nullptr)) {

	//	delete m_httpClient;
	
	}



}

std::vector<std::string> AmcrestMotion::splitintoArray(const std::string& s, const std::string& delim)
{
	std::vector<std::string> result;
	char* pbegin = (char*)s.c_str();
	char* next_token = nullptr;
	char* token;
	token = strtok_s(pbegin, delim.c_str(), &next_token);
	while (token != nullptr) {
		// Do something with the tok
		result.push_back(token);
		token = strtok_s(NULL, delim.c_str(), &next_token);
	}

	return result;
}

void AmcrestMotion::removechar(std::string& str, char removechar) { // removing char from
	str.erase(std::remove(str.begin(), str.end(), removechar),
		str.end());
}

// Parse Response-Header
//HTTP Code : 200 OK\r\n
//Cache - Control: no - cache\r\n
//Pragma : no - cache\r\n
//Expires : Thu, 01 Dec 2099 16 : 00 : 00 GMT\r\n
//Connection : close\r\n
//Content-Type:multipart/x-mixed-replace; boundary=myboundary\r\n\r\n

bool AmcrestMotion::ParseHeader() {
	m_Boundary.clear();
	m_ResponseTextState.clear();
	m_ResponseState = -1;

	std::size_t found = m_response.find("HTTP");
	if (found != std::string::npos) {
		std::string substring = m_response.substr(found, found+100);// use all posible chars
		std::vector<std::string> array = splitintoArray(substring, ":");
		if (array.size() > 0) {
			//  removechar(array[1], (char)' '); // remove blanks form string

			std::size_t found200OK = array[0].find("200 OK");
			if (found200OK != std::string::npos) {
				m_ResponseState = 200;
			}
		}

	}
	found = m_response.find("Error");
	if (found != std::string::npos) {
		std::string substring = m_response.substr(found, found + 150);// use all posible chars
		m_ResponseTextState = substring;
		m_ResponseState = 401;

		////	Error ErrorID = 0, Detail = Invalid Authority!
		//found = m_response.find("ErrorID");
		//if (found != std::string::npos) {
		//	std::string substring = m_response.substr(found, found + 100);// use all posible chars
		//	removechar(substring, (char)' '); // remove blanks form string
		//	removechar(substring, (char)'\r'); // remove \r form string
		//	removechar(substring, (char)'\n'); // remove \n form string
		//	std::vector<std::string> array = splitintoArray(substring, "=");
		//	if (array.size() > 0) {
		//		m_ResponseState = atoi(array[1].c_str());
		//	}

		//}
		//found = m_response.find("Detail");
		//if (found != std::string::npos) {
		//	std::string substring = m_response.substr(found, found + 100);// use all posible chars
		//	removechar(substring, (char)'\r'); // remove \r form string
		//	removechar(substring, (char)'\n'); // remove \n form string
		//	std::vector<std::string> array = splitintoArray(substring, "=");
		//	if (array.size() > 0) {
		//		m_ResponseTextState = array[1];
		//	}
		//}





	
	}



	found = m_response.find("boundary", found + 1);
	if (found != std::string::npos) {
		std::string substring = m_response.substr(found, found + 100);// use all posible chars
		std::vector<std::string> array = splitintoArray(substring, "=");
		if (array.size() > 1) {
			removechar(array[1], (char)' '); // remove blanks form string
			removechar(array[1], (char)'\r'); // remove \r form string
			removechar(array[1], (char)'\n'); // remove \n form string
			m_Boundary = array[1];



		}
	}

	m_ParseState = AmcrestDetectionParseState::ParseBody;

	return true;

}
//Body:
//--myboundary \r\n
//Content-Type: text / plain\r\n
//Content-Length: 39\r\n
//Code = VideoMotion; action = Start; index = 0\r\n\r\n
//-- myboundary \r\n
//Content - Type: text / plain\r\n
//Content - Length: 38\r\n
//Code = VideoBlind; action = Start; index = 0\r\n\r\n
//-- myboundary \r\n
//Content - Type: text / plain\r\n
//Content - Length: 38\r\n
//Code = AlarmLocal; action = Start; index = 0\r\n\r\n
//-- myboundary \r\n
//Content - Type: text / plain\r\n
//Content - Length: 38\r\n
//Code = MDResult; action = Pulse; index = 0; data = 61708863, 61708863…\r\n\r\n

bool AmcrestMotion::doProcessEvents() {


	m_Events->Clear();
	bool doMovement = false;
	if (m_RecognizedEvents.size() > 0) {
		for (size_t i = 0; i < m_RecognizedEvents.size(); i++) {
			EventTypeStruc& _event = m_RecognizedEvents.at(i);

			if (_event.m_EventType == AmcrestEventType::VideoMotionInfo) continue;
			if (_event.m_EventType == AmcrestEventType::NewFile) continue;
			if (_event.m_EventType == AmcrestEventType::VideoMotion) {
				doMovement = true;
				m_Movement = _event.m_state == EventState::On;

				if (m_Motionoutput->HasKey("m_MovementActiv")) {
					Platform::Object^ isActivvalue = m_Motionoutput->Lookup("m_MovementActiv");
					if (isActivvalue != nullptr) {
	
						bool isActiv = safe_cast<IPropertyValue^>(isActivvalue)->GetBoolean();
						if ((m_Movement != isActiv || m_FirstProcessEvents)) {// only in case of changes

							m_Motionoutput->Insert("m_MovementActiv", dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(m_Movement)));
						}
					}
				}
				continue;
			}

			Platform::Array<Platform::String^ >^ filleddata = ref new Platform::Array<Platform::String^>(m_RecognizedEvents.size());
			wchar_t buffer[100];
			swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s=%d, Index = %d", _event.getStringEventType().c_str(), _event.m_state, _event.m_Index);
			m_Events->Append(ref new String(buffer));
		}
		if (doMovement || (m_Events->Size > 0)) {
			ChangeMovement(this, m_Motionoutput);
		}

		m_FirstProcessEvents = false;
	}

	return (m_RecognizedEvents.size() > 0);

}
bool AmcrestMotion::ParseBody() {

	//	bool doMotionTrigger = false;
	m_RecognizedEvents.clear();

	std::size_t found = m_response.find(m_Boundary);
	while (found != std::string::npos)
	{
		//	doMotionTrigger = false;
		std::size_t foundCode = m_response.find("Code", found + 1);
		if (foundCode != std::string::npos) {

			std::string substring = m_response.substr(foundCode, foundCode + 100);// use all posible chars
			removechar(substring, (char)' '); // remove blanks form string
			removechar(substring, (char)'\r'); // remove \r form string
			removechar(substring, (char)'\n'); // remove \n form string

			std::vector<std::string> resparray = splitintoArray(substring, ";");


			EventTypeStruc _event;
			_event.m_EventType = AmcrestEventType::undefined;

			if (resparray.size() > 2) {
				std::vector<std::string> valarray = splitintoArray(resparray[0], "=");
				if (valarray.size() > 1) {

					if (valarray[0].find("Code") == 0) {
						if (valarray[1].find("VideoMotion") == 0) {
							_event.m_EventType = AmcrestEventType::VideoMotion;
						}
						if (valarray[1].find("VideoLoss") == 0) {
							_event.m_EventType = AmcrestEventType::VideoLoss;
						}
						if (valarray[1].find("VideoBlind") == 0) {
							_event.m_EventType = AmcrestEventType::VideoBlind;
						}
						if (valarray[1].find("AlarmLocal") == 0) {
							_event.m_EventType = AmcrestEventType::AlarmLocal;
						}
						if (valarray[1].find("CrossLineDetection") == 0) {
							_event.m_EventType = AmcrestEventType::CrossLineDetection;
						}
						if (valarray[1].find("CrossRegionDetection") == 0) {
							_event.m_EventType = AmcrestEventType::CrossRegionDetection;
						}
						if (valarray[1].find("LeftDetection") == 0) {
							_event.m_EventType = AmcrestEventType::LeftDetection;
						}
						if (valarray[1].find("TakenAwayDetection") == 0) {
							_event.m_EventType = AmcrestEventType::TakenAwayDetection;
						}
						if (valarray[1].find("VideoAbnormalDetection") == 0) {
							_event.m_EventType = AmcrestEventType::VideoAbnormalDetection;
						}
						if (valarray[1].find("FaceDetection") == 0) {
							_event.m_EventType = AmcrestEventType::FaceDetection;
						}
						if (valarray[1].find("AudioMutation") == 0) {
							_event.m_EventType = AmcrestEventType::AudioMutation;
						}
						if (valarray[1].find("AudioAnomaly") == 0) {
							_event.m_EventType = AmcrestEventType::AudioAnomaly;
						}
						if (valarray[1].find("VideoUnFocus") == 0) {
							_event.m_EventType = AmcrestEventType::VideoUnFocus;
						}

						if (valarray[1].find("WanderDetection") == 0) {
							_event.m_EventType = AmcrestEventType::WanderDetection;
						}

						if (valarray[1].find("RioterDetection") == 0) {
							_event.m_EventType = AmcrestEventType::RioterDetection;
						}

						if (valarray[1].find("ParkingDetection") == 0) {
							_event.m_EventType = AmcrestEventType::ParkingDetection;
						}
						if (valarray[1].find("StorageNotExist") == 0) {
							_event.m_EventType = AmcrestEventType::StorageNotExist;
						}

						if (valarray[1].find("StorageFailure") == 0) {
							_event.m_EventType = AmcrestEventType::StorageFailure;
						}

						if (valarray[1].find("StorageLowSpace") == 0) {
							_event.m_EventType = AmcrestEventType::StorageLowSpace;
						}
						if (valarray[1].find("AlarmOutput") == 0) {
							_event.m_EventType = AmcrestEventType::AlarmOutput;
						}
						if (valarray[1].find("MDResult") == 0) {
							_event.m_EventType = AmcrestEventType::MDResult;
						}
						if (valarray[1].find("HeatImagingTemper") == 0) {
							_event.m_EventType = AmcrestEventType::HeatImagingTemper;
						}
						if (valarray[1].find("NewFile") == 0) {
							_event.m_EventType = AmcrestEventType::NewFile;
						}
						if (valarray[1].find("VideoMotionInfo") == 0) {
							_event.m_EventType = AmcrestEventType::VideoMotionInfo;
						}

					}

				}

				if (_event.m_EventType != AmcrestEventType::undefined) {
					std::vector<std::string> valarray = splitintoArray(resparray[1], "=");
					if (valarray[0].find("action") == 0) {
						if (valarray[1].find("Start") == 0) {
							_event.m_state = EventState::On;
						}
						else
							if (valarray[1].find("Stop") == 0) {
								_event.m_state = EventState::Off;
							}
							else if (valarray[1].find("State") == 0) {
								_event.m_state = EventState::State;
							}
							else if (valarray[1].find("Pulse") == 0) {
								_event.m_state = EventState::Pulse;
							}
							else _event.m_state = EventState::undef;
					}

					valarray = splitintoArray(resparray[2], "=");
					if (valarray[0].find("index") == 0) {
						int ind = atoi(valarray[1].c_str());
						_event.m_Index = ind;
					}
					m_RecognizedEvents.push_back(_event);

				}

			}
		}
		found = m_response.find(m_Boundary, found + 10);

	}
	return (m_RecognizedEvents.size() > 0);
}

bool AmcrestMotion::doParseResponse() {

	bool doCancel = false;
	while (!doCancel) {
		switch (m_ParseState) {
		case AmcrestDetectionParseState::ParseHeader:
			ParseHeader();
			break;
		case AmcrestDetectionParseState::ParseBody:
			if (ParseBody()) {
				doProcessEvents();
			}
			doCancel = true;
			break;
		}
	}

	return true;
}






