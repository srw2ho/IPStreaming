#pragma once
namespace FFmpegInteropExtRT {
	
	template <typename Event>
	class EventBroadcaster
	{
	public:
		typedef std::function<void(const Event&, int reason)> eventcallback;

		void connect(eventcallback&& connection)
		{
			eventcallbacks.push_back(std::move(eventcallback));
		}

		void signal(const Event& event)
		{
			for (const auto& eventcallback : eventcallbacks)
			{
				eventcallback(event);
			}
		}
	private:
		std::vector<eventcallback> eventcallbacks;
	};



	enum  NotificationEventReason {
		EndRequest = 0,
		CanceledbyUser,
		CanceledbyError
	} ;
	


    delegate void NotificationEventhandler(Windows::Networking::Sockets::StreamSocket^ socket, int status);

	// interface that has an event and a function to invoke the event  
	interface struct EventInterface {
	public:
		event NotificationEventhandler ^ eventhandler;
		void fire(Windows::Networking::Sockets::StreamSocket^ socket, int status);
	};


	// class that implements the interface event and function  
	ref class SocketEventSource : public EventInterface {
	public:

		virtual event NotificationEventhandler^ eventhandler;
	
		virtual void fire(Windows::Networking::Sockets::StreamSocket^ socket, int status)
		{
			eventhandler(socket, status);

		}
	};




}