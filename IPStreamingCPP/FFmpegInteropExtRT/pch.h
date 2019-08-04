//*****************************************************************************
//
//	Copyright 2015 Microsoft Corporation
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

/*#pragma once

#include <collection.h>
#include <ppltasks.h>

// Disable debug string output on non-debug build
#if !_DEBUG
#define DebugMessage(x)
#else
#define DebugMessage(x) OutputDebugString(x)
#endif
*/
#pragma once

#include <list>
#include <memory>
#include <sstream>

#include <collection.h>
#include <ppltasks.h>
#include <wrl.h>
#include <robuffer.h>

//
// Asserts
//

// Disable debug string output on non-debug build
#if !_DEBUG
#define DebugMessage(x)
#else
#define DebugMessage(x) OutputDebugString(x)
#endif

#ifndef NDEBUG
#define NT_ASSERT(expr) if (!(expr)) { __debugbreak(); }
#else
#define NT_ASSERT(expr)
#endif

//
// Tracing
//

#include "..\CameraServer\DebuggerLogger.h"
//DebuggerLogger s_logger;

#define Trace(format, ...) { \
    if(s_logger.IsEnabled(DebuggerLogLevel::Information)) { s_logger.Log(__FUNCTION__, DebuggerLogLevel::Information, format, __VA_ARGS__); } \
}
#define TraceError(format, ...) { \
    if(s_logger.IsEnabled(DebuggerLogLevel::Error)) { s_logger.Log(__FUNCTION__, DebuggerLogLevel::Error, format, __VA_ARGS__); } \
}

//
// Error handling
//

// Exception-based error handling
#define CHK(statement)  {HRESULT _hr = (statement); if (FAILED(_hr)) { throw ref new Platform::COMException(_hr); };}
#define CHKNULL(p)  {if ((p) == nullptr) { throw ref new Platform::NullReferenceException(L#p); };}
#define CHKOOM(p)  {if ((p) == nullptr) { throw ref new Platform::OutOfMemoryException(L#p); };}

//
// IBuffer data access
//

inline unsigned char* GetData(Windows::Storage::Streams::IBuffer^ buffer)
{
	unsigned char* bytes = nullptr;
	Microsoft::WRL::ComPtr<::Windows::Storage::Streams::IBufferByteAccess> bufferAccess;
	CHK(((IUnknown*)buffer)->QueryInterface(IID_PPV_ARGS(&bufferAccess)));
	CHK(bufferAccess->Buffer(&bytes));
	return bytes;
}

//convert IBuffer to byte array

inline std::vector<unsigned char> ConvertBufferToByteVector(Windows::Storage::Streams::IBuffer^ buffer) {

	auto reader = ::Windows::Storage::Streams::DataReader::FromBuffer(buffer);

	std::vector<unsigned char> data(reader->UnconsumedBufferLength);
//	Array<unsigned char>^ value = ref new Array<unsigned char>(100);

	if (!data.empty())
		reader->ReadBytes(::Platform::ArrayReference<unsigned char>(&data[0], (unsigned int) data.size()));
//	reader->ReadBytes(value);

	return data;
}




inline Platform::String^ StringFromAscIIChars(char* chars)
{
	size_t newsize = strlen(chars) + 1;
	wchar_t * wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, chars, _TRUNCATE);
	Platform::String^ str = ref new Platform::String(wcstring);
	delete[] wcstring;
	return str;
}
// mit new erzeugter char* must be deleted with delete
inline  char* StringtoAscIIChars(Platform::String^stringRT) {

	std::wstring fooW(stringRT->Begin());
	std::string fooA(fooW.begin(), fooW.end());
	size_t len = fooA.length() + sizeof(char);
	char* charStr = new char[len];
	memcpy(charStr, fooA.c_str(), len);

	return charStr;

}
/*
Platform::String ^ toSPlatformSring(const char* fmt, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	return StringFromAscIIChars(buffer);

}
*/


