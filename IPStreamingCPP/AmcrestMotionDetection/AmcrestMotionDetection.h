#pragma once
using Windows::Web::Http::HttpClient;
using namespace Windows::Foundation;

namespace AmcrestMotionDetection
{
    enum AmcrestDetectionParseState {
        ParseHeader,
        ParseBody

    };

    enum AmcrestEventType {
        undefined=-1,
        VideoMotion=0,
        VideoLoss=1,
        VideoBlind=2,
        AlarmLocal=3,
        CrossLineDetection,
        CrossRegionDetection,
        LeftDetection,
        TakenAwayDetection,
        VideoAbnormalDetection,
        FaceDetection,
        AudioMutation,
        AudioAnomaly,
        VideoUnFocus,
        WanderDetection,
        RioterDetection,
        ParkingDetection,
        MoveDetection,
        StorageNotExist,
        StorageFailure,
        StorageLowSpace,
        AlarmOutput,
        MDResult,
        HeatImagingTemper,
        NewFile,
        VideoMotionInfo

    };

    enum EventState {
        undef = -1,
        Off = 0,
        On = 1,
        State = 2,
        Pulse = 3,
    };

    struct EventTypeStruc {

        AmcrestEventType m_EventType;
        EventState m_state; //0:of, 1:ON
        int m_Index;

        EventTypeStruc() {
            m_EventType = AmcrestEventType::undefined;
            m_state = EventState::undef;
            m_Index = -1;
        }
        std::wstring getStringEventType() {

            switch (m_EventType) {
            case undefined:
                return L"undefined";
                    break;
            case  VideoMotion:
                return L"VideoMotion";
                break;
            case VideoLoss:
                return L"VideoLoss";
                break;
            case VideoBlind:
                return L"VideoBlind";
                break;
            case AlarmLocal:
                return L"AlarmLocal";
                break;
            case CrossLineDetection:
                return L"CrossLineDetection";
                break;
            case CrossRegionDetection:
                return L"CrossRegionDetection";
                break;
            case LeftDetection:
                return L"LeftDetection";
                break;
            case TakenAwayDetection:
                return L"TakenAwayDetection";
                break;
            case VideoAbnormalDetection:
                return L"VideoAbnormalDetection";
                break;
            case FaceDetection:
                return L"FaceDetection";
                break;
            case AudioMutation:
                return L"AudioMutation";
                break;
            case AudioAnomaly:
                return L"AudioAnomaly";
                break;
            case VideoUnFocus:
                return L"VideoUnFocus";
                break;
            case WanderDetection:
                return L"WanderDetection";
                break;
            case RioterDetection:
                return L"RioterDetection";
                break;
            case ParkingDetection:
                return L"ParkingDetection";
                break;
            case MoveDetection:
                return L"MoveDetection";
                break;
            case StorageNotExist:
                return L"StorageNotExist";
                break;
            case StorageFailure:
                return L"StorageFailure";
                break;
            case StorageLowSpace:
                return L"StorageLowSpace";
                break;
            case AlarmOutput:
                return L"AlarmOutput";
                break;
            case MDResult:
                return L"MDResult";
                break;
            case HeatImagingTemper:
                return L"HeatImagingTemper";
                break;
            case NewFile:
                return L"NewFile";
                break;
            case VideoMotionInfo:
                return L"VideoMotionInfo";
                break;
            default:
                return L"undefined";
                break;

            }

        }
    };



    public ref class AmcrestMotion sealed
    {

  
        Windows::Web::Http::HttpClient^ m_httpClient;
        Windows::Web::Http::Filters::HttpBaseProtocolFilter^ m_filter;
        concurrency::cancellation_token_source* m_cancellationTokenSource;
        Windows::Storage::Streams::Buffer^ m_readBuffer;
        Windows::Foundation::Uri^ m_resourceAddress;
        std::string m_response;
        AmcrestDetectionParseState m_ParseState;
        int m_ResponseState;
        std::string m_ResponseTextState;
        std::string m_Boundary;
        std::vector<EventTypeStruc> m_RecognizedEvents;
        Platform::String^ m_HostName;
        Platform::String^ m_Password;
        Platform::String^ m_User;
        Windows::Foundation::Collections::PropertySet^ m_Motionoutput;

        Windows::Foundation::EventRegistrationToken m_startStreamingEventRegister;
        Windows::Foundation::EventRegistrationToken m_stopStreamingEventRegister;

        bool m_bStreamingStarted;
        bool m_bProcessingPackagesStarted;
        concurrency::task <void> m_ProcessingPackagesTsk;
        bool m_FirstProcessEvents;
        Platform::Collections::Vector<Platform::String^ >^ m_Events;// = ref new Platform::Array<Platform::String^>(m_RecognizedEvents.size());

        bool m_Movement;
    public:
        AmcrestMotion();
        virtual ~AmcrestMotion();

        event Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^  >^ ChangeMovement;
        event Windows::Foundation::TypedEventHandler<Platform::Object^, Windows::Foundation::Collections::PropertySet^  >^ startStreaming;
        event Windows::Foundation::TypedEventHandler<Platform::Object^, Platform::String^>^ stopStreaming;


        Windows::Foundation::IAsyncAction^ stopProcessingPackagesAsync();
        Windows::Foundation::IAsyncAction^ startProcessingPackagesAsync(Windows::Foundation::Collections::PropertySet^ inputconfigoptions, Windows::Foundation::Collections::PropertySet^ motionoutputconfigoptions);
        
        property bool IsMoment {
            bool get() { return m_Movement; };
            void set(bool value);
        }

        property Windows::Foundation::Collections::IObservableVector<Platform::String^ >^ Events {
            Windows::Foundation::Collections::IObservableVector<Platform::String^ >^ get() { return m_Events; };
        }

        property Platform::String^ HostName {
            Platform::String^ get() { return m_HostName; };
            void set(Platform::String^ value);
        }
        property Platform::String^ Password {
            Platform::String^ get() { return m_Password; };
            void set(Platform::String^ value);
        }
        property Platform::String^ User {
            Platform::String^ get() { return m_User; };
            void set(Platform::String^ value);
        }

    internal:

        bool doEventsDetection();
        Windows::Web::Http::HttpClient^ CreateHttpClient();
        void CancelEventsDetection();

    private:
        concurrency::task<Windows::Storage::Streams::IBuffer^> doReadData(Windows::Storage::Streams::IInputStream^ stream);
        bool doParseResponse();
        bool ParseHeader();
        bool ParseBody();
        bool doProcessEvents();
        std::vector<std::string> splitintoArray(const std::string& s, const std::string& delim);
        void removechar(std::string& str, char removechar);

        void StartMotionDetection(Windows::Foundation::Collections::PropertySet^ inputconfigoptions, Windows::Foundation::Collections::PropertySet^ outputconfigoptions);
        
        void StopMotionDetection();
        void setstoppStreamingEvent(Platform::String^ err);
        void DisableMovementActivated();
    };
}
