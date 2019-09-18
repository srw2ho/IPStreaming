#pragma once

#include "pch.h"
#include "DataSources.h"


using namespace IPStreamingCPP;
using namespace Platform;
using namespace Windows::Foundation;

using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace Windows::Storage;

//using namespace Windows::Storage::FileProperties;

DataSourceBase::DataSourceBase(Platform::String^ IDName)
{
	m_DataSourceIDName = IDName;
//	Windows::Storage::ApplicationDataContainer^ localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	m_DataCompositeIDName = "";
	m_DataSourcecomposite = nullptr;







}

Platform::String^ DataSourceBase::SetContainerIDName(Platform::String^ IDName)
{

	wchar_t buffer[500];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s.%s", IDName->Data(),  m_DataSourceIDName->Data());
	m_DataCompositeIDName = ref new Platform::String(buffer);;
	return m_DataCompositeIDName;

}


Platform::String^ DataSourceBase::SetSourceIDName(Platform::String^ IDName)
{

	wchar_t buffer[500];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s.%s", m_DataSourceIDName->Data(), IDName->Data());
	m_DataCompositeIDName = ref new Platform::String(buffer);;
	return m_DataCompositeIDName;

}

Platform::String^ DataSourceBase::getCompositePropertyIDName(Platform::String^ Property)
{
	wchar_t buffer[500];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s.%s", m_DataCompositeIDName->Data(), Property->Data());
	return  ref new Platform::String(buffer);;

}

Platform::String^ DataSourceBase::getSourceIDName()
{
	return m_DataSourceIDName;
}

Platform::String^ DataSourceBase::getCompositeIDName()
{
	return m_DataCompositeIDName;
}






bool DataSourceBase::writeCompositeValuetoLocalStorage()
{
	if (m_DataSourcecomposite == nullptr) return false;

	Windows::Storage::ApplicationDataContainer^ localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	bool write = localSettings->Values->Insert(m_DataCompositeIDName, m_DataSourcecomposite);


	return write;


}

Windows::Storage::ApplicationDataCompositeValue^ DataSourceBase::getCompositeValue()
{

	Windows::Storage::ApplicationDataContainer^ localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	bool keyok = localSettings->Values->HasKey(m_DataCompositeIDName);

	if (keyok) {

		m_DataSourcecomposite = (Windows::Storage::ApplicationDataCompositeValue^)localSettings->Values->Lookup(m_DataCompositeIDName);
	}
	else {
		m_DataSourcecomposite = ref new Windows::Storage::ApplicationDataCompositeValue();
	}

	return m_DataSourcecomposite;


}

Windows::Storage::ApplicationDataCompositeValue^ DataSourceBase::deleteCompositeValue()
{
//	if (m_DataSourcecomposite == nullptr) return nullptr;
	Windows::Storage::ApplicationDataContainer^ localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;


	if (localSettings->Values->HasKey(m_DataCompositeIDName)) {

		localSettings->Values->Remove(m_DataCompositeIDName);
		m_DataSourcecomposite = nullptr;
	}

	return m_DataSourcecomposite;


}

void DataSourceBase::writeSettingsToLocalStorage()
{

}

void DataSourceBase::readSettingsfromLocalStorage()
{

}

DataSourceItemBase::DataSourceItemBase(Platform::String^ IDName)
{
	m_DataSourceIDName = IDName;
}

Platform::String^ DataSourceItemBase::getCompositePropertyIDName(Platform::String^ Property, int IDx)
{
	wchar_t buffer[500];
	if (IDx == -1) {
		swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s.%s", m_DataSourceIDName->Data(), Property->Data());
	}
	else {
//		swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s.%s.%03d", m_DataSourceIDName->Data(), Property->Data(), IDx);
		swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s.%03d", Property->Data(), IDx);


	}

	

	return  ref new Platform::String(buffer);;

}


bool DataSourceItemBase::writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) {

	Platform::String^  key = getCompositePropertyIDName("m_DataSourceIDName", Idx);

	PropertyValue^ valuestring = dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_DataSourceIDName));
	bool ok = composite->Insert(key, valuestring);
		
	return ok;

}


bool DataSourceItemBase::readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^composite, int Idx) {



	Platform::String^  key = getCompositePropertyIDName("m_DataSourceIDName", Idx);

	Object^ ref = composite->Lookup(key);
	if (ref == nullptr) return false;

	m_DataSourceIDName = safe_cast<IPropertyValue^>(ref)->GetString();

	//m_DataSourceIDName = safe_cast<IPropertyValue^>(composite->Lookup("m_DataSourceIDName"))->GetString();
	return true;


}

Platform::String^ DataSourceItemBase::DataSourceIDName::get() { return this->m_DataSourceIDName; }

void DataSourceItemBase::DataSourceIDName::set(Platform::String^ value) { this->m_DataSourceIDName = value; }



ItemValue::ItemValue(Platform::String^ IDName, double Value) : DataSourceItemBase(IDName)
{

	//	m_IDName = IDName;
	m_Value = Value;


}


bool ItemValue::writeSettingsToLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::writeSettingsToLocalStorage(composite, Idx);
	bok = composite->Insert(getCompositePropertyIDName("m_Value", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(m_Value)));
	return bok;

}

bool ItemValue::readSettingsfromLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::readSettingsfromLocalStorage(composite, Idx);
	if (bok) {
		Object^ ref = composite->Lookup(getCompositePropertyIDName("m_Value", Idx));
		if (ref != nullptr) {
			m_Value = safe_cast<IPropertyValue^>(ref)->GetDouble();
		}
		}
	return bok;

}

bool ItemValue::ValueasBool::get() { return (bool) this->m_Value; }
void ItemValue::ValueasBool::set(bool value) { this->m_Value = (bool) value; }

double ItemValue::Value::get() { return this->m_Value; }
void ItemValue::Value::set(double value) { this->m_Value =value; }

Platform::String^ ItemValue::VisibleItem::get()
{

	//const wchar_t *wdata = m_IDName->Data();
	wchar_t buffer[100];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%g",  m_Value);
	return ref new Platform::String(buffer);

}

Platform::String^ ItemValue::VisibleItemRecordingHours::get()
{

	//const wchar_t *wdata = m_IDName->Data();
	wchar_t buffer[100];

	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"Rec.%03d h", (int)m_Value);
	return ref new Platform::String(buffer);

}
Platform::String^ ItemValue::VisibleItemDeleteOlderFiles::get()
{

	//const wchar_t *wdata = m_IDName->Data();
	wchar_t buffer[100];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"Del.%03d h", (int) m_Value);
	return ref new Platform::String(buffer);

}
Platform::String^ ItemValue::VisibleItemWatchRecording::get()
{

	//const wchar_t *wdata = m_IDName->Data();
	wchar_t buffer[100];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"MovRec.%03d sec", (int)m_Value);
	return ref new Platform::String(buffer);

}



ItemString::ItemString(Platform::String^ IDName, Platform::String^ Value) : DataSourceItemBase(IDName)
{

	//	m_IDName = IDName;
	m_strValue = Value;


}


bool ItemString::writeSettingsToLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::writeSettingsToLocalStorage(composite, Idx);
	bok = composite->Insert(getCompositePropertyIDName("m_strValue", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_strValue)));
	return bok;

}

bool ItemString::readSettingsfromLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::readSettingsfromLocalStorage(composite, Idx);
	if (bok) {
		Object^ ref = composite->Lookup(getCompositePropertyIDName("m_strValue", Idx));
		if (ref != nullptr) {
			m_strValue = safe_cast<IPropertyValue^>(ref)->GetString();
		}
	}
	return bok;

}

Platform::String^ ItemString::Value::get() { return this->m_strValue; }
void ItemString::Value::set(Platform::String^ value) { this->m_strValue = value; }


Platform::String^ ItemString::VisibleItem::get()
{

	//const wchar_t *wdata = m_IDName->Data();
	wchar_t buffer[100];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s", m_strValue->Data());
	return ref new Platform::String(buffer);

}




Resolution::Resolution(Platform::String^ IDName, int height, int width) : DataSourceItemBase(IDName)
{

//	m_IDName = IDName;
	m_height = height;
	m_width = width;

}


bool Resolution::writeSettingsToLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::writeSettingsToLocalStorage(composite,  Idx);
	bok = composite->Insert(getCompositePropertyIDName("m_height", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(m_height)));
	bok = composite->Insert(getCompositePropertyIDName("m_width", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(m_width)));
	return bok;

}

bool Resolution::readSettingsfromLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::readSettingsfromLocalStorage(composite, Idx);
	if (bok) {
		Object^ ref = composite->Lookup(getCompositePropertyIDName("m_height", Idx));
		if (ref != nullptr) {
			m_height = safe_cast<IPropertyValue^>(ref)->GetInt32();
		}
		ref = composite->Lookup(getCompositePropertyIDName("m_width", Idx));
		if (ref != nullptr) {
			m_width = safe_cast<IPropertyValue^>(ref)->GetInt32();
		}
		/*

		m_height = safe_cast<IPropertyValue^>(composite->Lookup(getCompositePropertyIDName("m_height", Idx)))->GetInt32();
		m_width = safe_cast<IPropertyValue^>(composite->Lookup(getCompositePropertyIDName("m_width", Idx)))->GetInt32();
		*/

	}
	return bok;

}

int Resolution::Height::get() { return this->m_height; }
void Resolution::Height::set(int value) { this->m_height = value; }

int Resolution::Width::get() { return this->m_width; }
void Resolution::Width::set(int value) { this->m_width = value; }


Platform::String^ Resolution::VisibleItem::get()
{

	//const wchar_t *wdata = m_IDName->Data();
	wchar_t buffer[100];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s: ( %d * %d )", m_DataSourceIDName->Data(), m_width, m_height);
	return ref new Platform::String(buffer);

}


Fps::Fps(Platform::String^ IDName, int Fps) : DataSourceItemBase(IDName)
{

	//	m_IDName = IDName;
	m_Fps = Fps;

}


bool Fps::writeSettingsToLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::writeSettingsToLocalStorage(composite, Idx);
	bok = composite->Insert(getCompositePropertyIDName("m_Fps", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(m_Fps)));
	return bok;

}

bool Fps::readSettingsfromLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::readSettingsfromLocalStorage(composite, Idx);
	if (bok) {
		Object^ ref = composite->Lookup(getCompositePropertyIDName("m_Fps", Idx));
		if (ref != nullptr) {
			m_Fps = safe_cast<IPropertyValue^>(ref)->GetInt32();
		}

	}
	return bok;

}

int Fps::FPS::get() { return this->m_Fps; }
void Fps::FPS::set(int value) { this->m_Fps = value; }

Platform::String^ Fps::VisibleItem::get()
{
	//const wchar_t *wdata = m_IDName->Data();
	wchar_t buffer[100];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s:( %d )", m_DataSourceIDName->Data(), m_Fps);
	return ref new Platform::String(buffer);

}



VideoInput::VideoInput(Platform::String^ IDName) : DataSourceItemBase(IDName)
{
	m_Fps = 0;
	m_Bitrate = 0;
	m_Quality = 0;

	m_Resolution = ref new IPStreamingCPP::Resolution(getCompositePropertyIDName(IDName, -1), 0,0);
	m_VideoEnabled = true;
}

 bool VideoInput::writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx)
{
	 bool bok = DataSourceItemBase::writeSettingsToLocalStorage(composite, Idx);

	 bok = composite->Insert(getCompositePropertyIDName("m_Fps", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(m_Fps)));
	 bok = composite->Insert(getCompositePropertyIDName("m_Bitrate", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(m_Bitrate)));
	 bok = composite->Insert(getCompositePropertyIDName("m_Quality", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(m_Quality)));
	 bok = m_Resolution->writeSettingsToLocalStorage(composite, Idx);
	 bok = composite->Insert(getCompositePropertyIDName("m_VideoEnabled", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(m_VideoEnabled)));

	 return true;
}


 bool VideoInput::readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) {
	 bool bok = DataSourceItemBase::readSettingsfromLocalStorage(composite, Idx);
	 if (bok) {
		 Object^ ref = composite->Lookup(getCompositePropertyIDName("m_Fps", Idx));
		 if (ref != nullptr) {
			 m_Fps = safe_cast<IPropertyValue^>(ref)->GetDouble();
		 }

		 ref = composite->Lookup(getCompositePropertyIDName("m_Bitrate", Idx));
		 if (ref != nullptr) {
			 m_Bitrate = safe_cast<IPropertyValue^>(ref)->GetDouble();
		 }
		 ref = composite->Lookup(getCompositePropertyIDName("m_Quality", Idx));
		 if (ref != nullptr) {
			 m_Quality = safe_cast<IPropertyValue^>(ref)->GetDouble();
		 }
		 m_Resolution->readSettingsfromLocalStorage(composite, Idx);

		 ref = composite->Lookup(getCompositePropertyIDName("m_VideoEnabled", Idx));
		 if (ref != nullptr) {
			 m_VideoEnabled = safe_cast<IPropertyValue^>(ref)->GetDouble();
		 }


	 }
	 return bok;


 };



 AudioInput::AudioInput(Platform::String^ IDName) : DataSourceItemBase(IDName)
 {
	 m_BitRate = 0;
	 m_SampleRate = 0;

	 m_AudioEnabled = true;

 }

 bool AudioInput::writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx)
 {
	 bool bok = DataSourceItemBase::writeSettingsToLocalStorage(composite, Idx);

	 bok = composite->Insert(getCompositePropertyIDName("m_BitRate", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(m_BitRate)));
	 bok = composite->Insert(getCompositePropertyIDName("m_SampleRate", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(m_SampleRate)));

	 bok = composite->Insert(getCompositePropertyIDName("m_AudioEnabled", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(m_AudioEnabled)));

	 return true;
 }


 bool AudioInput::readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) {
	 bool bok = DataSourceItemBase::readSettingsfromLocalStorage(composite, Idx);
	 if (bok) {
		 Object^ ref = composite->Lookup(getCompositePropertyIDName("m_BitRate", Idx));
		 if (ref != nullptr) {
			 m_BitRate = safe_cast<IPropertyValue^>(ref)->GetDouble();
		 }

		 ref = composite->Lookup(getCompositePropertyIDName("m_SampleRate", Idx));
		 if (ref != nullptr) {
			 m_SampleRate = safe_cast<IPropertyValue^>(ref)->GetDouble();
		 }


		 ref = composite->Lookup(getCompositePropertyIDName("m_AudioEnabled", Idx));
		 if (ref != nullptr) {
			 m_AudioEnabled = safe_cast<IPropertyValue^>(ref)->GetDouble();
		 }


	 }
	 return bok;


 };

inputSource::inputSource(Platform::String^ IDName, String^ InputHostName, int portNr,String^ User, String^ Password) : DataSourceItemBase(IDName)
{

	//	m_IDName = IDName;
	m_InputHostName = InputHostName;
	m_portNr = portNr;
	m_User = User;
	m_Password = Password;
	m_CameraManufactorer = "AMCREST";
	m_CameraVersion = "V1.0";
	m_CameraModel = "IP2M-841W-EGD-DE";
	m_SerialNumber = "...";
	m_CameraVersion = "...";
	m_HardwareId = "...";

	m_InputsourceUri = "";
	m_InputKey = "";
	m_bLifeStream = true;
	m_ffmpegProbeSize = 5000000;// 32 Min , 500000 default
	m_ffmpegAnalyzeduration = 5000000;
	m_restartStreamingTimeinHour = -1;

	this->m_VideoInput = ref new VideoInput(getCompositePropertyIDName(IDName, -1));
	this->m_AudioInput = ref new AudioInput(getCompositePropertyIDName(IDName, -1));


}



/*
Platform::String^  inputSource::FFmpegProbeSizeasString::get() { 
	wchar_t buffer[50];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%d", m_ffmpegProbeSize);
	return ref new String (buffer);

};
*/

double inputSource::RestartStreamingTimeinHour::get() { return m_restartStreamingTimeinHour; };
void inputSource::RestartStreamingTimeinHour::set(double value)
{
	if (value < 0.1) value = 0.1; // Min-Value
	if (value > 24*31) value = 24 * 31 ; // Max-Value
	m_restartStreamingTimeinHour = value;
};




int inputSource::FFmpegAnalyzeduration::get() { return m_ffmpegAnalyzeduration; };
void inputSource::FFmpegAnalyzeduration::set(int value)
{
	if (value < 50) value = 50; // Min-Value
	if (value > 5000000 *2) value = 5000000 *2; // Max-Value
	m_ffmpegAnalyzeduration = value;
};


int inputSource::FFmpegProbeSize::get() { return m_ffmpegProbeSize; };
void inputSource::FFmpegProbeSize::set(int value)
{ 
	if (value < 32) value = 32; // Min-Value
	if (value > 5000000*2) value = 5000000*2; // Max-Value
	m_ffmpegProbeSize = value; 
};

void inputSource::createUriPath()
{
	wchar_t buffer[200];
	if (m_CameraManufactorer == "AMCREST") {
		
		if ("AMCREST.Sub_0" == this->DataSourceIDName)
		{//  hight resolution
			swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"rtsp://%s:%s@%s:%d/cam/realmonitor?channel=1&subtype=%d", m_User->Data(), m_Password->Data(), m_InputHostName->Data(), m_portNr, 0);
			m_InputsourceUri = ref new Platform::String(buffer);
			m_InputKey = m_InputsourceUri;
		}

		if ("AMCREST.Sub_1" == this->DataSourceIDName)
		{//low resolution
			swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"rtsp://%s:%s@%s:%d/cam/realmonitor?channel=1&subtype=%d", m_User->Data(), m_Password->Data(), m_InputHostName->Data(), m_portNr, 1);
			m_InputsourceUri = ref new Platform::String(buffer);
			m_InputKey = m_InputsourceUri;
		}


	}
//	m_InputsourceUri = L"rtsp://admin:willi@%s:554/cam/realmonitor?channel=1&subtype=0";
}

bool inputSource::writeSettingsToLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::writeSettingsToLocalStorage(composite, Idx);
	bok = composite->Insert(getCompositePropertyIDName("m_InputsourceUri", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_InputsourceUri))); // with PW and user

	bok = composite->Insert(getCompositePropertyIDName("m_InputKey", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_InputKey))); // origin stream url

	bok = composite->Insert(getCompositePropertyIDName("m_InputHostName", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_InputHostName)));

	bok = composite->Insert(getCompositePropertyIDName("m_portNr", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(m_portNr)));

	bok = composite->Insert(getCompositePropertyIDName("m_User", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_User)));

	bok = composite->Insert(getCompositePropertyIDName("m_Password", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_Password)));

	bok = composite->Insert(getCompositePropertyIDName("m_CameraManufactorer", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_CameraManufactorer)));

	bok = composite->Insert(getCompositePropertyIDName("m_CameraVersion", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_CameraVersion)));

	bok = composite->Insert(getCompositePropertyIDName("m_CameraModel", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_CameraModel)));

	bok = composite->Insert(getCompositePropertyIDName("m_HardwareId", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_HardwareId)));

	bok = composite->Insert(getCompositePropertyIDName("m_SerialNumber", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_SerialNumber)));

	bok = composite->Insert(getCompositePropertyIDName("m_bLifeStream", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(m_bLifeStream)));


	bok = composite->Insert(getCompositePropertyIDName("m_ffmpegProbeSize", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(m_ffmpegProbeSize)));

	bok = composite->Insert(getCompositePropertyIDName("m_ffmpegAnalyzeduration", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(m_ffmpegAnalyzeduration)));

	bok = composite->Insert(getCompositePropertyIDName("m_restartStreamingTimeinHour", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(m_restartStreamingTimeinHour)));

	this->m_VideoInput->writeSettingsToLocalStorage(composite, Idx);
	this->m_AudioInput->writeSettingsToLocalStorage(composite, Idx);

	
	return bok;

}

bool inputSource::readSettingsfromLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::readSettingsfromLocalStorage(composite, Idx);
	if (bok) {
		Object^ ref = composite->Lookup(getCompositePropertyIDName("m_InputsourceUri", Idx));
		if (ref != nullptr) {
			m_InputsourceUri = safe_cast<IPropertyValue^>(ref)->GetString();
		}

		ref = composite->Lookup(getCompositePropertyIDName("m_InputKey", Idx));
		if (ref != nullptr) {
			m_InputKey = safe_cast<IPropertyValue^>(ref)->GetString();
		}


		ref = composite->Lookup(getCompositePropertyIDName("m_InputHostName", Idx));
		if (ref != nullptr) {
			m_InputHostName = safe_cast<IPropertyValue^>(ref)->GetString();
		}

		ref = composite->Lookup(getCompositePropertyIDName("m_portNr", Idx));
		if (ref != nullptr) {
			m_portNr = safe_cast<IPropertyValue^>(ref)->GetInt32();
		}

		ref = composite->Lookup(getCompositePropertyIDName("m_User", Idx));
		if (ref != nullptr) {
			m_User = safe_cast<IPropertyValue^>(ref)->GetString();
		}
		ref = composite->Lookup(getCompositePropertyIDName("m_Password", Idx));
		if (ref != nullptr) {
			m_Password = safe_cast<IPropertyValue^>(ref)->GetString();
		}
		ref = composite->Lookup(getCompositePropertyIDName("m_CameraManufactorer", Idx));
		if (ref != nullptr) {
			m_CameraManufactorer = safe_cast<IPropertyValue^>(ref)->GetString();
		}
		ref = composite->Lookup(getCompositePropertyIDName("m_CameraVersion", Idx));
		if (ref != nullptr) {
			m_CameraVersion = safe_cast<IPropertyValue^>(ref)->GetString();
		}
		ref = composite->Lookup(getCompositePropertyIDName("m_CameraModel", Idx));
		if (ref != nullptr) {
			m_CameraModel = safe_cast<IPropertyValue^>(ref)->GetString();
		}
		
		ref = composite->Lookup(getCompositePropertyIDName("m_HardwareId", Idx));
		if (ref != nullptr) {
			m_HardwareId = safe_cast<IPropertyValue^>(ref)->GetString();
		}

		ref = composite->Lookup(getCompositePropertyIDName("m_SerialNumber", Idx));
		if (ref != nullptr) {
			m_SerialNumber = safe_cast<IPropertyValue^>(ref)->GetString();
		}


		ref = composite->Lookup(getCompositePropertyIDName("m_bLifeStream", Idx));
		if (ref != nullptr) {
			m_bLifeStream = safe_cast<IPropertyValue^>(ref)->GetBoolean();
		}
		ref = composite->Lookup(getCompositePropertyIDName("m_ffmpegProbeSize", Idx));
		if (ref != nullptr) {
			m_ffmpegProbeSize = safe_cast<IPropertyValue^>(ref)->GetInt32();
		}

		ref = composite->Lookup(getCompositePropertyIDName("m_ffmpegAnalyzeduration", Idx));
		if (ref != nullptr) {
			m_ffmpegAnalyzeduration = safe_cast<IPropertyValue^>(ref)->GetInt32();
		}

		ref = composite->Lookup(getCompositePropertyIDName("m_restartStreamingTimeinHour", Idx));
		if (ref != nullptr) {
			m_restartStreamingTimeinHour = safe_cast<IPropertyValue^>(ref)->GetDouble();
		}


		this->m_VideoInput->readSettingsfromLocalStorage(composite, Idx);
		this->m_AudioInput->readSettingsfromLocalStorage(composite, Idx);
		createUriPath();
	}

	return bok;

}

bool inputSource::LifeStream::get() { return m_bLifeStream; };

void inputSource::LifeStream::set (bool  life) {  m_bLifeStream = life; };


Platform::String^ inputSource::InputsourceUri::get() {
	return this->m_InputsourceUri; 
}
void inputSource::InputsourceUri::set(Platform::String^ value) { this->m_InputsourceUri = value; }

Platform::String^ inputSource::InputKey::get() {
	return this->m_InputKey;
}
void inputSource::InputKey::set(Platform::String^ value) { this->m_InputKey = value; }

Platform::String^ inputSource::VisibleItem::get()
{
	//const wchar_t *wdata = m_IDName->Data();
	wchar_t buffer[200];
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s:( %s )", m_DataSourceIDName->Data(), m_InputHostName->Data());
//	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s", m_InputHostName->Data());

	return ref new Platform::String(buffer);


	return m_InputsourceUri;

}
Platform::String^ inputSource::VisibleStreamUri::get()
{
	//const wchar_t *wdata = m_IDName->Data();Height
	wchar_t buffer[200]; 
	
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s.Res(%d x %d)@Fps(%f)", m_InputHostName->Data(), this->InputVideo->Resolution->Width, this->InputVideo->Resolution->Height, this->InputVideo->FPS);
	//	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s", m_InputHostName->Data());

	return ref new Platform::String(buffer);

}





outPutFormat::outPutFormat(Platform::String^ IDName, String^ format) : DataSourceItemBase(IDName)
{

	//	m_IDName = IDName;
	m_format = format;
}





bool outPutFormat::writeSettingsToLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::writeSettingsToLocalStorage(composite, Idx);
	bok = composite->Insert(getCompositePropertyIDName("m_format", Idx), dynamic_cast<PropertyValue^>(PropertyValue::CreateString(m_format)));


	return bok;

}

bool outPutFormat::readSettingsfromLocalStorage(ApplicationDataCompositeValue^ composite, int Idx)
{
	bool bok = DataSourceItemBase::readSettingsfromLocalStorage(composite, Idx);
	if (bok) {
		Object^ ref = composite->Lookup(getCompositePropertyIDName("m_format", Idx));
		if (ref != nullptr) {
			m_format = safe_cast<IPropertyValue^>(ref)->GetString();
		}

	}

	return bok;

}


Platform::String^ outPutFormat::VisibleItem::get()
{
	//const wchar_t *wdata = m_IDName->Data();
	wchar_t buffer[200];
//	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s:( %s )", m_DataSourceIDName->Data(), m_format->Data());
	swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"%s", m_format->Data());
	return ref new Platform::String(buffer);

}

Platform::String^ outPutFormat::Format::get()
{
	return m_format;
	//return ref new Platform::String(m_format);
}



ComboViewModelBase::ComboViewModelBase(Platform::String^ IDName) : DataSourceBase(IDName)
{
	this->m_Items = ref new Platform::Collections::Vector<DataSourceItemBase^>();
	m_SelectedIndex = -1;

}

DataSourceItemBase^ ComboViewModelBase::createDummyItem()
{

	return nullptr;
}

void ComboViewModelBase::readSettingsfromLocalStorage()
{
	if (Items == nullptr)return;
	SelectedIndex = -1;

	this->Items->Clear();

	ApplicationDataCompositeValue^ composite = getCompositeValue();
	int i = 0;
	/*
	this->initdefaults();
	while (i < Items->Size)
	{
		DataSourceItemBase^ res = Items->GetAt((unsigned int)i );
		bool found = res->readSettingsfromLocalStorage(composite, i); //ID vom Container wird gesetzt
		i++;
	}
	*/

	while (true)
	{
		DataSourceItemBase^ res = createDummyItem();
		if (res == nullptr) break;

		bool found = res->readSettingsfromLocalStorage(composite, i); //ID vom Container wird gesetzt
		if (found)
		{
			this->Items->Append(res);
		}
		else break;

		i++;
	}


	Object^ ref = composite->Lookup("SelectedIndex");
	if (ref != nullptr) {
		SelectedIndex = safe_cast<IPropertyValue^>(ref)->GetInt32();
	}
	else SelectedIndex = 0;

	if (this->Items->Size == 0) {
		this->initdefaults();
		this->SelectedIndex = 0;

	};

}

void ComboViewModelBase::writeSettingsToLocalStorage()
{
	if (Items == nullptr)return;
	//roamingSettings->Values->Remove(settingName);

	this->deleteCompositeValue();
//return;
	ApplicationDataCompositeValue^ composite = getCompositeValue();

	for (unsigned int i = 0; i < Items->Size;i++) {
		DataSourceItemBase^ base = Items->GetAt(i);
		base->writeSettingsToLocalStorage(composite, i); //ID vom Container wird gesetzt
	}


	bool bok = composite->Insert("SelectedIndex", dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(this->SelectedIndex)));


	this->writeCompositeValuetoLocalStorage();
}

void ComboViewModelBase::SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	Windows::UI::Xaml::Controls::ComboBox^ inputsourceurl = safe_cast<ComboBox^>(sender);
	if (inputsourceurl->SelectedIndex == -1) return;
	
	auto selected = inputsourceurl->Items->GetAt(inputsourceurl->SelectedIndex);
	this->SelectedIndex = inputsourceurl->SelectedIndex;



}

int ComboViewModelBase::SelectedIndex::get() {
	return this->m_SelectedIndex;
}
void ComboViewModelBase::SelectedIndex::set(int value) {
	this->m_SelectedIndex = value;
}




ResolutionViewModel::ResolutionViewModel(Platform::String^ IDName) : ComboViewModelBase(IDName)
{
		
}
Resolution^ ResolutionViewModel::getSelectedResolution()
{
	if (this->SelectedIndex == -1)
	{
		return nullptr;
	}
	return(Resolution^) Items->GetAt((unsigned int)SelectedIndex);

}

void ResolutionViewModel::initdefaults()
{


	SelectedIndex = -1;
	Resolution^ resolution = ref new Resolution("VGA", 480, 640);
	this->Items->Append(resolution);
	resolution = ref new Resolution("SVGA", 600, 800);
	this->Items->Append(resolution);
	resolution = ref new Resolution("720p", 720, 1280);

	this->Items->Append(resolution);
	resolution = ref new Resolution("1080p", 1080, 1920);
	this->Items->Append(resolution);

}





DataSourceItemBase^ ResolutionViewModel::createDummyItem()
{
	Resolution^ res = ref new Resolution("", 0, 0);
	return res;
}







FpsViewModel::FpsViewModel(Platform::String^ IDName) : ComboViewModelBase(IDName)
{
//	this->m_Items = ref new Platform::Collections::Vector<Fps^>();
}

void FpsViewModel::initdefaults()
{

	wchar_t buffer[500];
	SelectedIndex = -1;
	Fps^ fps = ref new Fps("fps.input", -1);
	this->Items->Append(fps);
	for (int i = 4; i <= 30; i=i+2)
	{
		swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"fps.%2d", i);
		Fps^ fps = ref new Fps(ref new String(buffer), i);
		this->Items->Append(fps);
	}



}


Fps^ FpsViewModel::getSelectedFps()
{
	if (this->SelectedIndex == -1)
	{
		return nullptr;
	}
	return(Fps^)Items->GetAt((unsigned int)SelectedIndex);

}

DataSourceItemBase^ FpsViewModel::createDummyItem()
{
	Fps^ res = ref new Fps("dummy",0);
	return res;
}



inputSourceViewModel::inputSourceViewModel(Platform::String^ IDName) : ComboViewModelBase(IDName)
{
	//	this->m_Items = ref new Platform::Collections::Vector<Fps^>();
}

inputSource ^ inputSourceViewModel::getInputSourceBySourceUri(Platform::String^ inputIDName)
{

	for (unsigned int i = 0; i < Items->Size;i++) {
		inputSource^ base = (inputSource ^) Items->GetAt(i);
		if (base->InputsourceUri == inputIDName)
		{
			return base;
		}
	}
	return nullptr;

}
inputSource ^ inputSourceViewModel::getInputSourceByInputKey(Platform::String^ inputIDName)
{

	for (unsigned int i = 0; i < Items->Size; i++) {
		inputSource^ base = (inputSource ^)Items->GetAt(i);
		if (base->InputKey == inputIDName)
		{
			return base;
		}
	}
	return nullptr;

}


bool inputSourceViewModel::deleteInputSource(inputSource ^ source)
{

	for (unsigned int i = 0; i < Items->Size; i++) {
		inputSource^ base = (inputSource ^)Items->GetAt(i);
		if (base == source)
		{
			Items->RemoveAt(i);
			return true;
		}
	}
	return false;

}

inputSource ^ inputSourceViewModel::getInputSourceByHostName(Platform::String^ inputHostName)
{

	for (unsigned int i = 0; i < Items->Size; i++) {
		inputSource^ base = (inputSource ^)Items->GetAt(i);
		if (base->HostName == inputHostName)
		{
			return base;
		}
	}
	return nullptr;

}


inputSource ^ inputSourceViewModel::getInputSourceByIdx(unsigned int nIdx)
{
	if (nIdx <this->Items->Size)
	{
		return(inputSource^)Items->GetAt((unsigned int)nIdx);
	}
	return nullptr;


}




inputSource ^ inputSourceViewModel::geSelectedInputSource()
{

	if (this->SelectedIndex == -1)
	{
		return nullptr;
	}
	return(inputSource^)Items->GetAt((unsigned int)SelectedIndex);

}





void inputSourceViewModel::initdefaults()
{

	SelectedIndex = -1;


	inputSource^ inpSource = ref new inputSource("AMCREST.Sub_0", "AMC000UY_89WJ0B", 554,"admin", "willi");
	inpSource->createUriPath();
	this->Items->Append(inpSource);

	
	inpSource = ref new inputSource("AMCREST.Sub_1", "AMC000UY_89WJ0B", 554, "admin", "willi");
	inpSource->createUriPath();
	this->Items->Append(inpSource);
	
	//only for testing
	inpSource = ref new inputSource("AMCREST.Sub_0", "192.168.1.230", 554, "admin", "willi");
	inpSource->createUriPath();
	this->Items->Append(inpSource);

	inpSource = ref new inputSource("AMCREST.Sub_1", "192.168.1.230", 554, "admin", "willi");
	inpSource->createUriPath();
	this->Items->Append(inpSource);

	
}


OutputFormatViewModel::OutputFormatViewModel(Platform::String^ IDName) : ComboViewModelBase(IDName)
{

	//	this->m_Items = ref new Platform::Collections::Vector<Resolution^>();

}
outPutFormat^ OutputFormatViewModel::getSelectedOutPutFormat()
{

	if (this->SelectedIndex == -1)
	{
		return nullptr;
	}
	return(outPutFormat^)Items->GetAt((unsigned int)SelectedIndex);

}

void OutputFormatViewModel::initdefaults()
{
	SelectedIndex = -1;
	outPutFormat^ format;
	// = ref new outPutFormat("file_cpyinput.mp4", "cpyinput.mp4");
	//this->Items->Append(format);

	format = ref new outPutFormat("file_mp4", "mp4");
	this->Items->Append(format);
	format = ref new outPutFormat("file_avi", "avi");
	this->Items->Append(format);

	format = ref new outPutFormat("file_mpeg", "mpeg");
	this->Items->Append(format);

	format = ref new outPutFormat("file_flv", "flv");
	this->Items->Append(format);

	//format = ref new outPutFormat("file_Ogg", "ogg");
	//this->Items->Append(format);

//	format = ref new outPutFormat("file_cpyinput.mjpeg", "cpyinput.mjpeg");
//	this->Items->Append(format);

}






DataSourceItemBase^ OutputFormatViewModel::createDummyItem()
{
	outPutFormat^ format = ref new outPutFormat("file_mp4", "mp4");
	return format;
}

DataSourceItemBase^ inputSourceViewModel::createDummyItem()
{
	inputSource^ inpSource = ref new inputSource("AMCREST_Sub_1", "AMC000UY_89WJ0B", 554, "admin", "willi");
	return inpSource;
}



ItemValueViewModel::ItemValueViewModel(Platform::String^ IDName) : DataSourceBase(IDName)
{

	m_ItemValue = ref new ItemValue(IDName, 0);
	//	temValue^ ref = ref new ItemValue("toggleSwitchVideoDecode", 0);

}


void ItemValueViewModel::readSettingsfromLocalStorage()
{
	
	ApplicationDataCompositeValue^ composite = getCompositeValue();
	m_ItemValue->readSettingsfromLocalStorage(composite, 0); //ID vom Container wird gesetzt

}

void ItemValueViewModel::writeSettingsToLocalStorage()
{

	this->deleteCompositeValue();
	ApplicationDataCompositeValue^ composite = getCompositeValue();
	m_ItemValue->writeSettingsToLocalStorage(composite, 0); //ID vom Container wird gesetzt
	this->writeCompositeValuetoLocalStorage();
}


double ItemValueViewModel::Value::get() 
{ 
	return m_ItemValue->Value;
};

void ItemValueViewModel::Value::set(double value) 
{
	m_ItemValue->Value = value;
};

bool ItemValueViewModel::ValueasBool::get()
{
	return (bool) m_ItemValue->Value;
};

void ItemValueViewModel::ValueasBool::set(bool value)
{
	m_ItemValue->Value = (bool)value;
};



ItemStringViewModel::ItemStringViewModel(Platform::String^ IDName) : DataSourceBase(IDName)
{

	m_ItemValue = ref new ItemString(IDName, "");

}

void ItemStringViewModel::writeSettingsToLocalStorage()
{

	this->deleteCompositeValue();
	ApplicationDataCompositeValue^ composite = getCompositeValue();
	m_ItemValue->writeSettingsToLocalStorage(composite, 0); //ID vom Container wird gesetzt
	this->writeCompositeValuetoLocalStorage();
}



void ItemStringViewModel::readSettingsfromLocalStorage()
{

	ApplicationDataCompositeValue^ composite = getCompositeValue();
	m_ItemValue->readSettingsfromLocalStorage(composite, 0); //ID vom Container wird gesetzt

}



Platform::String^ ItemStringViewModel::Value::get()
{
	return m_ItemValue->Value;
};

void ItemStringViewModel::Value::set(Platform::String^ value)
{
	m_ItemValue->Value = value;
};






HourViewModel::HourViewModel(Platform::String^ IDName) : ComboViewModelBase(IDName)
{

}

ItemValue^ HourViewModel::getSelectedHours()
{

	if (this->SelectedIndex == -1)
	{
		return nullptr;
	}
	return(ItemValue^)Items->GetAt((unsigned int)SelectedIndex);

}

void HourViewModel::initdefaults()
{
	SelectedIndex = -1;
	wchar_t buffer[500];
	if (this->DataSourceIDName == "_ffmpegoutDeleteOlderFiles") {
		for (int i = 24; i <= 7 * 24; i = i + 24)
		{
			swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"DelOlderFiles.%3d", i);
			ItemValue^ item = ref new ItemValue(ref new String(buffer), i);
			this->Items->Append(item);
		}
	}
	else if (this->DataSourceIDName == "_ffmpegoutRecordingHours") 
	{
			for (int i = 1 ; i <= 24; i = i + 1)
			{
				swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"RecordHours.%3d", i);
				ItemValue^ item = ref new ItemValue(ref new String(buffer), i);
				this->Items->Append(item);
			}


	}
	else if (this->DataSourceIDName == "_MovementRecordingTimeSecs")
	{
		for (int i = 0; i <= 200; i = i + 10)
		{
			swprintf(&buffer[0], sizeof(buffer) / sizeof(buffer[0]), L"RecSecs.%3d", i);
			ItemValue^ item = ref new ItemValue(ref new String(buffer), i);
			this->Items->Append(item);
		}


	}



}

DataSourceItemBase^ HourViewModel::createDummyItem()
{
	ItemValue^ res = ref new ItemValue("", 0);
	return res;
}




DataSources::DataSources(Platform::String^ IDName) 
{
	m_DataSourcesIDName = IDName;

	m_DataSources = ref new Platform::Collections::Vector<DataSourceBase^>();


}

DataSourceBase^ DataSources::AddDataSource(DataSourceBase^ Baseitem)
{
	this->m_DataSources->Append(Baseitem);
	return Baseitem;
}


DataSourceBase^ DataSources::getDataSource(Platform::String^ itemname)
{
	for (unsigned int i = 0; i < m_DataSources->Size; i++) {
		DataSourceBase^ base = m_DataSources->GetAt(i);
		if (base->DataSourceIDName == itemname) {
			return base;
		}

	}

	return nullptr;
}



void DataSources::writeSettingsToLocalStorage()
{
	unsigned int s = m_DataSources->Size;

	for (unsigned int i = 0; i < m_DataSources->Size;i++) {
		DataSourceBase^ base = m_DataSources->GetAt(i);
		base->SetContainerIDName(m_DataSourcesIDName); //ID vom Container wird gesetzt
		base->writeSettingsToLocalStorage();
	}

}

void DataSources::readSettingsfromLocalStorage()
{
	unsigned int s = m_DataSources->Size;

	for (unsigned int i = 0; i < m_DataSources->Size;i++) {
		DataSourceBase^ base = m_DataSources->GetAt(i);
		base->SetContainerIDName(m_DataSourcesIDName); //ID vom Container wird gesetzt
		base->readSettingsfromLocalStorage();
	}

}

