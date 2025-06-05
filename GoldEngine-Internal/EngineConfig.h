using namespace System;
using namespace Newtonsoft::Json;

namespace Engine::Config
{
	private ref class Resolution
	{
	public:
		int x, y, w, h;

	public:
		[[JsonConstructorAttribute]]
		Resolution(int x, int y, int w, int h)
		{
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
		}
	};

	private ref class EngineSecrets
	{
	public:
		String^ encryptionPassword;

	private:
		static EngineSecrets^ self;

	public:
		initonly static EngineSecrets^ default = gcnew EngineSecrets(ENCRYPTION_PASSWORD);

	public:
		EngineSecrets(String^ password)
		{
			self = this;
			this->encryptionPassword = password;
		}

	private:
		static long long convertToInt(String^ data)
		{
			long long value = 0;

			for each (auto c in data->ToCharArray())
			{
				value += (short)c;
			}

			return value;
		}

	public:
		std::string getPassword()
		{
			return CastStringToNative(encryptionPassword);
		}

		static EngineSecrets^ singleton()
		{
			return self;
		}

		void ExportSecrets(System::String^ fN)
		{
			File::WriteAllText(fN, Convert::ToBase64String(
				Encoding::UTF8->GetBytes
				(
					CypherLib::EncryptFileContents(encryptionPassword, convertToInt(File::ReadAllText("./Data/Keys/map.iv")))
				)
			)
			);
		}

		static EngineSecrets^ ImportSecrets(System::String^ fN)
		{
			if (File::Exists(fN))
			{
				String^ encodedData = File::ReadAllText(fN);
				String^ decodedData = Encoding::UTF8->GetString(Convert::FromBase64String(encodedData));

				String^ password = CypherLib::DecryptFileContents(decodedData, convertToInt(File::ReadAllText("./Data/Keys/map.iv")));

				self = gcnew EngineSecrets(password);
				return self;
			}

			return EngineSecrets::default;
		}

	};

	private ref class WinFlags
	{
	public:
		bool VSync;
		bool Fullscreen;
		bool Resizable;
		bool Undecorated;
		bool Hidden;
		bool Minimized;
		bool Maximized;
		bool Unfocused;
		bool Topmost;
		bool AlwaysRun;
		bool Transparent;
		bool HighDPI;
		bool MousePassthrough;
		bool BorderlessWindowed;
		bool MSAA_X4;
		bool Interlaced;

	public:
		WinFlags()
		{
			this->VSync = false;
			this->Fullscreen = false;
			this->Resizable = false;
			this->Undecorated = false;
			this->Hidden = false;
			this->Minimized = false;
			this->Maximized = false;
			this->Unfocused = false;
			this->Topmost = false;
			this->AlwaysRun = false;
			this->Transparent = false;
			this->HighDPI = false;
			this->MousePassthrough = false;
			this->BorderlessWindowed = false;
			this->MSAA_X4 = false;
			this->Interlaced = false;
		}

	public:
		unsigned int toWindowFlags()
		{
			unsigned int retFlag = 0x0;

			{
				(VSync == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_VSYNC_HINT : retFlag = retFlag;
				(Fullscreen == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_FULLSCREEN_MODE : retFlag = retFlag;
				(Resizable == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_RESIZABLE : retFlag = retFlag;
				(Undecorated == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_UNDECORATED : retFlag = retFlag;
				(Hidden == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_HIDDEN : retFlag = retFlag;
				(Minimized == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_MINIMIZED : retFlag = retFlag;
				(Maximized == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_MAXIMIZED : retFlag = retFlag;
				(Unfocused == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_UNFOCUSED : retFlag = retFlag;
				(Topmost == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_TOPMOST : retFlag = retFlag;
				(AlwaysRun == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_ALWAYS_RUN : retFlag = retFlag;
				(Transparent == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_TRANSPARENT : retFlag = retFlag;
				(HighDPI == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_HIGHDPI : retFlag = retFlag;
				(MousePassthrough == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_WINDOW_MOUSE_PASSTHROUGH : retFlag = retFlag;
				(BorderlessWindowed == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_BORDERLESS_WINDOWED_MODE : retFlag = retFlag;
				(MSAA_X4 == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_MSAA_4X_HINT : retFlag = retFlag;
				(Interlaced == true) ? retFlag |= RAYLIB::ConfigFlags::FLAG_INTERLACED_HINT : retFlag = retFlag;
			}

			return retFlag;
		}
	};

	private ref class EngineConfiguration
	{
	public:
		String^ windowName;
		String^ logPath;
		Resolution^ resolution;
		unsigned int windowFlags;
		WinFlags^ _windowFlags;

	private:
		static EngineConfiguration^ self;

	public:
		initonly static EngineConfiguration^ defaultConfiguration = gcnew EngineConfiguration("Gold Engine Window", gcnew Resolution(0, 0, 1280, 720), "GoldEngine/", 0, gcnew WinFlags());

	public:
		[[JsonConstructorAttribute]]
		EngineConfiguration(String^ windowName, Resolution^ resolution, String^ logPath, unsigned int flags, WinFlags^ _flags)
		{
			self = this;
			this->resolution = resolution;
			this->windowName = windowName;
			this->logPath = logPath;
			this->windowFlags = flags;
			this->_windowFlags = _flags;
		}

		EngineConfiguration()
		{
			self = this;
			this->logPath = defaultConfiguration->logPath;
			this->windowName = defaultConfiguration->windowName;
			this->resolution = defaultConfiguration->resolution;
			this->windowFlags = defaultConfiguration->windowFlags;
			this->_windowFlags = defaultConfiguration->_windowFlags;
		}

		std::string getWindowName()
		{
			return CastStringToNative(windowName);
		}

	public:
		void ExportConfig(System::String^ fN)
		{
			File::WriteAllText(fN, Convert::ToBase64String(
				Encoding::UTF8->GetBytes(
					CypherLib::EncryptFileContents(
						JsonConvert::SerializeObject(this), passwd
					)
				)
			));
		}

	public:
		static EngineConfiguration^ singleton()
		{
			return self;
		}

	private:
		static void setSingleton(EngineConfiguration^ inst)
		{
			self = inst;
		}

	public:
		static EngineConfiguration^ ImportConfig(System::String^ fN)
		{
			if (File::Exists(fN))
			{
				String^ encodedData = File::ReadAllText(fN);

				EngineConfiguration^ configuration = JsonConvert::DeserializeObject<EngineConfiguration^>(
					CypherLib::DecryptFileContents(
						Encoding::UTF8->GetString(
							Convert::FromBase64String(encodedData)
						),
						passwd)
				);

				setSingleton(configuration);

				return configuration;
			}

			return EngineConfiguration::defaultConfiguration;
		}
	};
}