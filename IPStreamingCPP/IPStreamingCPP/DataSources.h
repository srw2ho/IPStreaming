#pragma once
#ifndef DATASOURECEBASE
#define DATASOURECEBASE

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
namespace IPStreamingCPP
{

	[Windows::UI::Xaml::Data::Bindable]
	public ref class  DataSourceBase : Windows::UI::Xaml::DependencyObject
	{
	internal:
		DataSourceBase(Platform::String^ IDName);
		Platform::String^ SetSourceIDName(Platform::String^ IDName);
		Platform::String^ getSourceIDName();
		Platform::String^ getCompositePropertyIDName(Platform::String^ IDName);
		Platform::String^ SetContainerIDName(Platform::String^ IDName);
		Platform::String^ getCompositeIDName();
		property Platform::String^ DataSourceIDName
		{
			Platform::String^ get() { return m_DataSourceIDName; };
			void set(Platform::String^ value) { m_DataCompositeIDName = value; };
		}

	internal:
		virtual void writeSettingsToLocalStorage();
		virtual void readSettingsfromLocalStorage();



	protected:
	//	Windows::Storage::ApplicationDataCompositeValue^ createCompositeValue();		// create or get  composite containers from local storage
		bool writeCompositeValuetoLocalStorage();		// create or get  composite containers from local storage

		Windows::Storage::ApplicationDataCompositeValue^ getCompositeValue();		// create or get  composite containers from local storage
		Windows::Storage::ApplicationDataCompositeValue^ deleteCompositeValue();	// delete  composite containers from local storage
	private:
		Platform::String^ m_DataSourceIDName;
		Windows::Storage::ApplicationDataCompositeValue^ m_DataSourcecomposite;
		Platform::String^ m_DataCompositeIDName;
	

	};
	[Windows::UI::Xaml::Data::Bindable]
	public ref class  DataSourceItemBase : Windows::UI::Xaml::DependencyObject
	{
	internal:
		DataSourceItemBase(Platform::String^ IDName);

	//internal:
		virtual bool writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx);
		virtual bool readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx);
	
		public:
		Platform::String^ getCompositePropertyIDName(Platform::String^ IDName, int IDx);
		property Platform::String^ DataSourceIDName
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}
		
		property double Value
		{
			double get() {return 0;};
			void set(double value) {};
		}
		property bool ValueasBool
		{
			bool get() { return false; };
			void set(bool value) {};
		}

		property Platform::String^ VisibleItem
		{
			  Platform::String^ get() {return "to defined";};
		}



	internal:
		Platform::String^ m_DataSourceIDName;


	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class ItemValue sealed : public DataSourceItemBase
	{
	private:
		double m_Value;

	internal:
		ItemValue(Platform::String^ IDName, double value);
	//internal:
		virtual bool writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;
		virtual bool readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;
	//internal:

	public:
		property double Value
		{
			double get();
			void set(double value);
		}
		property bool ValueasBool
		{
			bool get();
			void set(bool value);
		}

		property Platform::String^ VisibleItem
		{
		//	Platform::String^ get() override;
			Platform::String^ get() ;
		}

		property Platform::String^ VisibleItemRecordingHours
		{
			Platform::String^ get() ;
		}

		property Platform::String^ VisibleItemDeleteOlderFiles
		{
			Platform::String^ get() ;
		}
		property Platform::String^ VisibleItemWatchRecording
		{
			Platform::String^ get();
		}

	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class ItemString sealed : public DataSourceItemBase
	{
	private:
		Platform::String^ m_strValue;
	protected:
	internal:
		ItemString(Platform::String^ IDName, Platform::String^ value);
	//internal:
		virtual bool writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;
		virtual bool readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;
	public:

		property Platform::String^ Value
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ VisibleItem
		{
			//Platform::String^ get() override;
			Platform::String^ get() ;
		}
	};

	[Windows::UI::Xaml::Data::Bindable]

	public ref class Resolution sealed  : public DataSourceItemBase
	{
	private:
		int m_height;
		int m_width;



	internal:
		virtual bool writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;
		virtual bool readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;



	public:
		Resolution(Platform::String^ IDName, int height, int width);
		property int Height
		{
			int get();
			void set(int value);
		}


		property int Width
		{
			int get();
			void set(int value);
		}
		
		property Platform::String^ VisibleItem
		{
		//	Platform::String^ get() override;
			Platform::String^ get() ;
		}
	};


	[Windows::UI::Xaml::Data::Bindable]

	public ref class Fps sealed : public DataSourceItemBase
	{
	private:
		int m_Fps;
	protected:


	internal:
	
		virtual bool writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;
		virtual bool readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;
//	internal:
	public:
		Fps(Platform::String^ IDName, int Fps);
		property int FPS
		{
			int get();
			void set(int value);
		}


		property Platform::String^ VisibleItem
		{
		//	Platform::String^ get() override;
			Platform::String^ get() ;
		}
	};

	[Windows::UI::Xaml::Data::Bindable]

	public ref class VideoInput sealed : public DataSourceItemBase
	{
	internal:


	//internal:
		virtual bool writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;
		virtual bool readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;

	public:
		VideoInput(Platform::String^ IDName);
		property double FPS
		{
			double  get() { return m_Fps; };
			void set(double  value) { m_Fps = value; };
		}
		property double Bitrate
		{
			double  get() { return m_Bitrate; };
			void set(double  value) { m_Bitrate = value; };
		}

		property double Quality
		{
			double  get() { return m_Quality; };
			void set(double  value) { m_Quality = value; };
		}
		property bool VideoEnabled
		{
			bool  get() { return m_VideoEnabled; };
			void set(bool  value) { m_VideoEnabled = value; };
		}

		property IPStreamingCPP::Resolution^ Resolution
		{
			IPStreamingCPP::Resolution^  get() { return m_Resolution; };

		}

	private:
		double m_Fps;
		double m_Bitrate;
		double m_Quality;
		IPStreamingCPP::Resolution^ m_Resolution;
		bool m_VideoEnabled;

	};

	[Windows::UI::Xaml::Data::Bindable]

	public ref class AudioInput sealed : public DataSourceItemBase
	{
	internal:


		virtual bool writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;

		virtual bool readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;

	public:
		AudioInput(Platform::String^ IDName);

		property double BitRate
		{
			double  get() { return m_BitRate; };
			void set(double  value) { m_BitRate = value; };
		}
		property double SampleRate
		{
			double  get() { return m_SampleRate; };
			void set(double  value) { m_SampleRate = value; };
		}
		property bool AudioEnabled
		{
			bool  get() { return m_AudioEnabled; };
			void set(bool  value) { m_AudioEnabled = value; };
		}



	private:
		double m_BitRate;
		double m_SampleRate;
		bool m_AudioEnabled;
	};


	[Windows::UI::Xaml::Data::Bindable]

	public ref class inputSource sealed : public DataSourceItemBase
	{
	private:


		Platform::String^ m_CameraManufactorer;
		Platform::String^ m_HardwareId;
		Platform::String^ m_SerialNumber;
		Platform::String^ m_CameraVersion;
		Platform::String^ m_CameraModel;
		Platform::String^ m_Password;
		Platform::String^ m_User;
		int m_portNr;
		Platform::String^ m_InputHostName;
		Platform::String^ m_InputsourceUri;
		Platform::String^ m_InputKey;
		bool m_bLifeStream;
		int m_ffmpegProbeSize;
		int m_ffmpegAnalyzeduration;
		double m_restartStreamingTimeinHour;
		//int m_fps
	
		VideoInput^ m_VideoInput;
		AudioInput^ m_AudioInput;
		

	internal:

//	internal:
		virtual bool writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;
		virtual bool readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;

	public:

		inputSource(Platform::String^ IDName, Platform::String^ m_InputHostName, int portNr, Platform::String^ m_User, Platform::String^ m_Password);

		void createUriPath();
		property VideoInput^ InputVideo
		{
			VideoInput^  get() { return m_VideoInput; };
		}
		property AudioInput^ InputAudio
		{
			AudioInput^  get() { return m_AudioInput; };
		}

		property Platform::String^ CameraManufactorer
		{
			Platform::String^  get() { return m_CameraManufactorer; };
			void set(Platform::String^  value) { m_CameraManufactorer = value; };
		}
		property Platform::String^ HardwareId
		{
			Platform::String^  get() { return m_HardwareId; };
			void set(Platform::String^  value) { m_HardwareId = value; };
		}
		property Platform::String^ SerialNumber
		{
			Platform::String^  get() { return m_SerialNumber; };
			void set(Platform::String^  value) { m_SerialNumber = value; };
		}

		property Platform::String^ CameraVersion
		{
			Platform::String^  get() { return m_CameraVersion; };
			void set(Platform::String^  value) { m_CameraVersion = value; };
		}
		property Platform::String^ CameraModel
		{
			Platform::String^  get() { return m_CameraModel; };
			void set(Platform::String^  value) { m_CameraModel = value; };
		}


		property Platform::String^ Password
		{
			Platform::String^  get() { return m_Password; };
			void set(Platform::String^  value) { m_Password = value; };
		}

		property Platform::String^ User
		{
			Platform::String^  get() { return m_User; };
			void set(Platform::String^  value) { m_User = value; };
		}
		property Platform::String^ HostName
		{
			Platform::String^  get() { return m_InputHostName; };
			void set(Platform::String^  value) { m_InputHostName = value; };
		}

		property double RestartStreamingTimeinHour
		{
			double  get();
			void set(double  value);
		}

		property int FFmpegAnalyzeduration
		{
			int  get();
			void set(int  value);
		}

		property int FFmpegProbeSize
		{
			int  get();
			void set(int  value);
		}

		property bool LifeStream
		{
			bool  get();
			void set(bool  lifeStream);
		}

		property Platform::String^  InputsourceUri
		{
			Platform::String^  get();
			void set(Platform::String^  value);
		}

		property Platform::String^  InputKey
		{
			Platform::String^  get();
			void set(Platform::String^  value);
		}

		property Platform::String^ VisibleStreamUri
		{
			//		Platform::String^ get() override;
			Platform::String^ get();
		}

		property Platform::String^ VisibleItem
		{
	//		Platform::String^ get() override;
			Platform::String^ get() ;
		}
	};

	public ref class outPutFormat sealed : public DataSourceItemBase
	{
	private:
		Platform::String^ m_format;

	protected:


	internal:

	//internal:
		virtual bool writeSettingsToLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;
		virtual bool readSettingsfromLocalStorage(Windows::Storage::ApplicationDataCompositeValue^ composite, int Idx) override;

	public:
		outPutFormat(Platform::String^ IDName, Platform::String^ m_format);
		property Platform::String^ Format
		{
			Platform::String^ get();
		}

		property Platform::String^ VisibleItem
		{
		//	Platform::String^ get() override;
			Platform::String^ get() ;
		}
	};

	//[Windows::UI::Xaml::Data::Bindable]

	public ref class ComboViewModelBase : public DataSourceBase
	{
	private:
		Windows::Foundation::Collections::IVector<DataSourceItemBase^>^ m_Items;
		int m_SelectedIndex;
	internal:
		ComboViewModelBase(Platform::String^ IDName);
	//internal:

		virtual void writeSettingsToLocalStorage() override ;
		virtual void readSettingsfromLocalStorage() override;

		public:

		property Windows::Foundation::Collections::IVector<DataSourceItemBase^>^ Items {
			Windows::Foundation::Collections::IVector<DataSourceItemBase^>^ Items::get() { return m_Items; };
		}
		property int SelectedIndex
		{
			int get();
			void set(int value);
		}
		void SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

	protected:

		virtual DataSourceItemBase^ createDummyItem();
		virtual void initdefaults() {};
	};

	
	public ref class ResolutionViewModel sealed :public ComboViewModelBase
	{
	private:

	//	Windows::Foundation::Collections::IVector<Resolution^>^ m_Items;

	public:
		
		ResolutionViewModel(Platform::String^ IDName);
		Resolution^ getSelectedResolution();
	protected:
		virtual void initdefaults() override;
		virtual DataSourceItemBase^ createDummyItem() override;
	};
	

	[Windows::UI::Xaml::Data::Bindable]
	public ref class FpsViewModel sealed  :public ComboViewModelBase
	{
	private:

	//	Windows::Foundation::Collections::IVector<Fps^>^ m_Items;


	public:
		FpsViewModel(Platform::String^ IDName);
		Fps^ getSelectedFps();
	
	protected:
		virtual void initdefaults() override;
		virtual DataSourceItemBase^ createDummyItem() override;
	};

	
	[Windows::UI::Xaml::Data::Bindable]
	public ref class inputSourceViewModel sealed :public ComboViewModelBase
	{
	private:
	
	public:
		inputSourceViewModel(Platform::String^ IDName);

		inputSource ^ getInputSourceBySourceUri(Platform::String^ IDName) ;
		inputSource ^ getInputSourceByInputKey(Platform::String^ IDKey); // input key
		inputSource ^ getInputSourceByHostName(Platform::String^ inputHostName);
		inputSource ^ geSelectedInputSource();
		inputSource ^ getInputSourceByIdx(unsigned int nIdy);

		bool deleteInputSource(inputSource ^ source);


	protected:
		virtual void initdefaults() override;
		virtual DataSourceItemBase^ createDummyItem() override;
	};


	[Windows::UI::Xaml::Data::Bindable]
	public ref class OutputFormatViewModel sealed :public ComboViewModelBase
	{
	private:

	public:
		OutputFormatViewModel(Platform::String^ IDName);
		outPutFormat^ getSelectedOutPutFormat();

	protected:
		virtual void initdefaults() override;
		virtual DataSourceItemBase^ createDummyItem() override;
	};


	[Windows::UI::Xaml::Data::Bindable]

	public ref class ItemValueViewModel  :public DataSourceBase
	{
	private:
		ItemValue^ m_ItemValue;
	public:
		property double Value
		{
			double get();
			void set(double value);
		}
		property bool ValueasBool
		{
			bool get();
			void set(bool value);
		}
	internal:
		ItemValueViewModel(Platform::String^ IDName);
		virtual void readSettingsfromLocalStorage() override;
		virtual void writeSettingsToLocalStorage() override;

	protected:

	};


	public ref class ItemStringViewModel :public DataSourceBase
	{
	private:
		ItemString^ m_ItemValue;


	internal:
		ItemStringViewModel(Platform::String^ IDName);
		virtual void readSettingsfromLocalStorage() override;
		virtual void writeSettingsToLocalStorage() override;

	public:

		property Platform::String^ Value
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}


	protected:

	};

	


	public ref class HourViewModel sealed :public ComboViewModelBase
	{
	private:

	public:

		HourViewModel(Platform::String^ IDName);
		ItemValue^ getSelectedHours();

	protected:
		virtual void initdefaults() override;
		virtual DataSourceItemBase^ createDummyItem() override;
	};


	public ref class  DataSources sealed
	{
	public:
		DataSources(Platform::String^ IDName);

		DataSourceBase^ AddDataSource(DataSourceBase^ Base);
		DataSourceBase^ getDataSource(Platform::String ^ itemname);
		void readSettingsfromLocalStorage();
		void writeSettingsToLocalStorage() ;

	internal:
		Platform::String^ m_DataSourcesIDName;
		Platform::Collections::Vector<DataSourceBase^>^ m_DataSources;
	//	Windows::Foundation::Collections::IVector<DataSourceBase^>^ m_DataSources;



	};
}

#endif // A_H