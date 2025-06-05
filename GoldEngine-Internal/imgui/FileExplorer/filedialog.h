
#include "../imgui.h"
#include "../Fonts/IconsFontAwesome4.h"

namespace Engine::Editor::Gui
{
	enum class explorerMode
	{
		Open,
		Save
	};

	public delegate void onFileSelected(String^ str);

	ref class fileExplorer
	{
	private:
		String^ title;
		bool* open;

		List<String^>^ files;
		String^ currentRoute;
		String^ selectedFile;
		explorerMode mode;
		String^ dialogResult;
		String^ current_filter;
		onFileSelected^ bind;

		bool registeredFont = false;

	public:
		fileExplorer(std::string title)
		{
			Singleton<fileExplorer^>::Create(this);

			this->title = gcnew String(title.c_str());
			open = false;
			files = gcnew List<String^>();
			currentRoute = Directory::GetCurrentDirectory() + "/Data";
			dialogResult = "";
			current_filter = "";
			registeredFont = false;
		}

	public:
		void setExplorerMode(explorerMode mode)
		{
			this->mode = mode;
		}

		~fileExplorer()
		{
			delete title;
			delete open;
		}

	public:
		void Open()
		{
			dialogResult = "";
			open = (bool*)true;
		}

		void Close()
		{
			open = (bool*)false;
		}

	public:
		void SetWindowName(std::string name)
		{
			this->title = gcnew String(name.c_str());
		}

	private:
		void fetchFiles(String^ route)
		{
			files->Clear();

			if (!Directory::Exists(route)) return;

			for each (String ^ s in Directory::GetFileSystemEntries(route))
			{
				if (current_filter->Length <= 0)
				{
					files->Add(s->Replace("\\", "/"));
				}
				else
				{
					if (s->Contains(current_filter))
					{
						files->Add(s->Replace("\\", "/"));
					}
				}
			}
		}

	private:
		bool isDirectory(String^ path)
		{
			if (File::Exists(path))
				return false;

			return true;
		}

	public:
		void OnCompleted(onFileSelected^ t) {
			bind = t;
			if (dialogResult != "")
			{
				t->Invoke(dialogResult);
				dialogResult = "";
				bind = nullptr;
			}
		}

	public:
		void TryComplete()
		{
			if (bind != nullptr)
				OnCompleted(bind);
		}

	public:
		String^ getDialogResult()
		{
			return dialogResult;
		}

	public:
		void resetDialogResult()
		{
			dialogResult = "";
		}

	public:
		void DrawExplorer()
		{
			if (!open)
			{
				return;
			}

			currentRoute->Replace("\\", "/");

			if (!registeredFont)
			{
				ImGuiIO& io = ImGui::GetIO();
				float baseFontSize = 13.0f;
				float iconFontSize = baseFontSize * 2.0f / 3.0f;

				static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
				ImFontConfig icons_config;
				icons_config.MergeMode = true;
				icons_config.PixelSnapH = true;
				icons_config.GlyphMinAdvanceX = iconFontSize;

				io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FA, iconFontSize, &icons_config, icons_ranges);

				registeredFont = true;
			}

			const char* name = CastStringToNative(title).c_str();

			ImGui::OpenPopup(name);
			bool isFile = !isDirectory(selectedFile);
			if (ImGui::BeginPopupModal(name, (bool*)false, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize))
			{
				char* route = new char[currentRoute->Length + (8*32)];
				strcpy(route, CastStringToNative(currentRoute).c_str());

				if (ImGui::Button("<"))
				{
					if (!currentRoute->Contains("/"))
					{
						currentRoute = currentRoute->Substring(
							0, currentRoute->LastIndexOf("\\"));
					}
					else
					{
						currentRoute = currentRoute->Substring(
							0, currentRoute->LastIndexOf("/"));
					}
				}

				ImGui::SameLine();

				ImGui::SameLine();
				ImGui::Text("Path: ");
				ImGui::SameLine();
				if (ImGui::InputText("###PATH", route, currentRoute->Length + (8 * 32), ImGuiInputTextFlags_None))
				{
					currentRoute = gcnew String(route);
				}

				ImGui::SameLine();

				if (ImGui::Button(ICON_FA_FOLDER_O))
				{
					if (current_filter != "")
						Directory::CreateDirectory(currentRoute + "/" + current_filter);
				}

				ImGui::SameLine();

				if (ImGui::Button(ICON_FA_FILE_O))
				{
					if (current_filter != "")
						File::Create(currentRoute + "/" + current_filter)->Close();
				}


				ImGui::BeginListBox("###FILES", {
					ImGui::GetWindowSize().x - 25,
					ImGui::GetWindowSize().y - 104
					});

				fetchFiles(currentRoute);

				for each (String ^ fileName in files)
				{
					if (isDirectory(fileName)) {
						if (ImGui::Selectable(std::string(ICON_FA_FOLDER_O + CastStringToNative(" " + fileName)).c_str()))
						{
							selectedFile = fileName->Replace("\\", "/");
						}
					}
					else
					{
						if (ImGui::Selectable(std::string(ICON_FA_FILE_O + CastStringToNative(" " + fileName)).c_str()))
						{
							selectedFile = fileName->Replace("\\", "/");
						}
					}
				}

				ImGui::EndListBox();

				{
					// Filter
					{
						char* data = new char[current_filter->Length + (8*32)];
						const char* newData = CastToNative(current_filter);
						strcpy(data, newData);

						ImGui::Text("Filter: ");
						ImGui::SameLine();
						if (ImGui::InputText("###FILTER", data, sizeof(data) + (8*32), ImGuiInputTextFlags_None))
						{
							current_filter = gcnew String(data);
						}

						ImGui::SameLine();

						if (ImGui::Button("Reset"))
						{
							current_filter = "";
						}

					}
				}

				// Buttons
				{
					if (mode == explorerMode::Open || !isFile)
					{
						if (ImGui::Button("Open"))
						{
							if (!isFile)
							{
								currentRoute = selectedFile;
							}
							else
							{
								dialogResult = selectedFile;
								Close();
								ImGui::CloseCurrentPopup();
							}
						}
					}
					else
					{
						if (ImGui::Button("Save"))
						{
							dialogResult = selectedFile;
							Close();
							ImGui::CloseCurrentPopup();
						}
					}



					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						Close();
						ImGui::CloseCurrentPopup();
					}
				}


			}
			ImGui::EndPopup();
		}
	};
}