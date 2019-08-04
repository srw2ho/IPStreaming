#include "pch.h"
#include "EventBroadCaster.h"

using namespace FFmpegInterop;

namespace FFmpegInterop {

	
void EventBroadcaster::connect(eventcallback&& connection)
{
	eventcallbacks.push_back(std::move(eventcallback));
}

void EventBroadcaster::signal(const Event& event)
{
	for (const auto& eventcallback : eventcallbacks)
	{
		eventcallback(event);
	}
}

}