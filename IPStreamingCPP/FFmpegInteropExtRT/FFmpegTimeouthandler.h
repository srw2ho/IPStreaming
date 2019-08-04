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

#pragma once



//#include "MediaSampleProvider.h"

namespace FFmpegInteropExtRT
{

	class timeout_handler {
	public:
		timeout_handler(LONGLONG t);
		virtual ~timeout_handler(void);
		void setTimeoutinMS(LONGLONG t);
		int is_timeout(void);
		static int timeout_handler::check_interrupt(void * t);

		void setTimeoutBreak(bool breaktimeout); //timeout break- Timeout

	protected:

		LARGE_INTEGER m_StartingTime;
		LARGE_INTEGER m_Frequency;
		LONGLONG m_timeout;
		bool m_TimeoutBreak;
	};


}