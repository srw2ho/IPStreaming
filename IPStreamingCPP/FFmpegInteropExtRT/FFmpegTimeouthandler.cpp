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

#include "pch.h"
#include "FFmpegTimeoutHandler.h"

using namespace FFmpegInteropExtRT;


timeout_handler::timeout_handler(LONGLONG t)
{
	m_TimeoutBreak = false;
	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency)) {

	}
	setTimeoutinMS(t);
}

timeout_handler::~timeout_handler(void)
{

}

void timeout_handler::setTimeoutBreak(bool breaktimeout) //timeout in ms
{
	m_TimeoutBreak = breaktimeout;


}


void timeout_handler::setTimeoutinMS(LONGLONG t) //timeout in ms
{
	//LARGE_INTEGER ptime;
	m_timeout = t*1000;
	QueryPerformanceCounter(&m_StartingTime);

}

int timeout_handler::is_timeout() {
	if (m_TimeoutBreak) return 1;

	LARGE_INTEGER  ElapsedMicroseconds, EndingTime;
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - m_StartingTime.QuadPart;

	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= m_Frequency.QuadPart;

	if (ElapsedMicroseconds.QuadPart > m_timeout)  {//timeout in µs? 
		return 1;
	}
	//return 0 for no timeout 
	return 0;
}

int timeout_handler::check_interrupt(void * t) {
	if (t != nullptr) {
		timeout_handler*ptimeoout = static_cast<timeout_handler *>(t);
		return ptimeoout->is_timeout();
	}
	return 0; // no timeout
//	return t && static_cast<timeout_handler *>(t)->is_timeout();
}










