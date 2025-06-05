#pragma once

using namespace System;
using namespace System::IO;

namespace Engine::Assets::IO
{
	public ref class FileManager
	{
	private:
		static String^ fileHeader = "GOLD ";
		static short int fileVersion = 100;

	private:
		static void concat(System::Collections::Generic::List<String^>^% array1, System::Collections::Generic::List<String^>^ array2)
		{
			for each (String ^ v2 in array2)
			{
				array1->Add(v2);
			}
		}

	private:
		static System::Collections::Generic::List<String^>^ getDescendants(String^ inPath)
		{
			System::Collections::Generic::List<String^>^ listedData = gcnew System::Collections::Generic::List<String^>();

			if (Directory::Exists(inPath))
			{
				for each (String^ childNode in Directory::GetFileSystemEntries(inPath))
				{
					if (Directory::Exists(childNode))
					{
						concat(listedData, getDescendants(childNode));
					}
					else
					{
						listedData->Add(childNode);
					}
				}
			}

			return listedData;
		}

	private:
		static array<String^>^ hasWildcards(String^ inFile)
		{
			if (inFile->Contains(R"(*)"))
			{
				String^ routeAccessPath = inFile->Substring(0, inFile->Length - 1);

				return getDescendants(routeAccessPath)->ToArray();
			}

			return nullptr;
		}

	public:
		static void WriteToCustomFile(String^ fileName, String^ password, array<String^>^ inFile)
		{
			auto file = File::Open(fileName, FileMode::OpenOrCreate);

			auto stream = gcnew BinaryWriter(file);
			stream->Flush();
			stream->Write(fileHeader);
			stream->Write(fileVersion);

			auto deflateStream = gcnew Compression::DeflateStream(file, Compression::CompressionMode::Compress);
			stream = gcnew BinaryWriter(deflateStream);

			int assetCount = inFile->Length;

			for each (String^ str in inFile)
			{
				auto wildcards = hasWildcards("Data/" + str);

				if (wildcards != nullptr)
				{
					assetCount += (wildcards->Length)-1;
				}
			}

			stream->Write(assetCount); // assets in file
			for (int x = 0; x < inFile->Length; x++)
			{
				auto fileName = inFile[x];
				auto wildcards = hasWildcards("Data/" + fileName);

				if (wildcards != nullptr)
				{
					for each(String^ file in wildcards)
					{
						file = file->Substring(0, 5);
						stream->Write(file); // write model name
						auto contents = File::ReadAllBytes(file);
						stream->Write(contents->Length);
						stream->Write(contents);
					}
				}
				else
				{
					stream->Write(fileName); // write model name
					auto contents = File::ReadAllBytes("Data/" + inFile[x]);
					stream->Write(contents->Length);
					stream->Write(contents);
				}
			}

			stream->Close();
			deflateStream->Close();
			file->Close();
		}

		static void ReadCustomFileFormat(String^ fileName, String^ password)
		{
			auto file = File::Open(fileName, FileMode::OpenOrCreate);

			auto stream = gcnew BinaryReader(file);

			String^ header = stream->ReadString();

			if (fileHeader->Equals(header))
			{
				short int version = stream->ReadInt16();

				if (fileVersion == version)
				{
					auto deflateStream = gcnew Compression::DeflateStream(file, Compression::CompressionMode::Decompress);
					stream = gcnew BinaryReader(deflateStream);

					if(!Directory::Exists("Data/unpacked/"))
						Directory::CreateDirectory("Data/unpacked/");

					auto dirInfo = gcnew DirectoryInfo("Data/unpacked/");

					/*
					auto dirSecurity = dirInfo->GetAccessControl();

					auto everyone = gcnew System::Security::Principal::SecurityIdentifier(System::Security::Principal::WellKnownSidType::WorldSid, nullptr);
					dirSecurity->AddAccessRule(gcnew System::Security::AccessControl::FileSystemAccessRule(everyone, System::Security::AccessControl::FileSystemRights::FullControl, System::Security::AccessControl::AccessControlType::Deny));
					*/
					//WinAPI::SetAttribute("Data/unpacked/", 1);
					int assets = stream->ReadInt32();

					for (int x = 0; x < assets; x++)
					{
						String^ fN = stream->ReadString();
						unsigned long length = stream->ReadInt32();
						auto fC = stream->ReadBytes(length);
						Directory::CreateDirectory(Path::GetDirectoryName("Data/unpacked/" + fN));
						auto fS = File::Open("Data/unpacked/" + fN, FileMode::OpenOrCreate);
						auto bW = gcnew BinaryWriter(fS);

						bW->Write(
							fC
						);
						bW->Flush();

						bW->Close();
					}
				}
			}


			stream->Close();
		}

		static void CleanupUnpackedAssets()
		{
			if (Directory::Exists("Data/unpacked/"))
				Directory::Delete("Data/unpacked/");
		}

		static void UnpackAsset(String^ fileName, String^ password, String^ resourceName)
		{
			auto file = File::Open(fileName, FileMode::OpenOrCreate);

			auto stream = gcnew BinaryReader(file);

			String^ header = stream->ReadString();

			if (fileHeader->Equals(header))
			{
				short int version = stream->ReadInt16();

				if (fileVersion == version)
				{
					auto deflateStream = gcnew Compression::DeflateStream(file, Compression::CompressionMode::Decompress);
					stream = gcnew BinaryReader(deflateStream);

					if (!Directory::Exists("Data/unpacked/"))
						Directory::CreateDirectory("Data/unpacked/");

					auto dirInfo = gcnew DirectoryInfo("Data/unpacked/");

					int assets = stream->ReadInt32();

					for (int x = 0; x < assets; x++)
					{
						String^ fN = stream->ReadString();
						String^ resName = fN->Substring(0, fN->IndexOf("."));

						if (resName->Equals(resourceName))
						{
							unsigned long length = stream->ReadInt32();
							auto fC = stream->ReadBytes(length);
							Directory::CreateDirectory(Path::GetDirectoryName("Data/unpacked/" + fN));
							auto fS = File::Open("Data/unpacked/" + fN, FileMode::OpenOrCreate);
							auto bW = gcnew BinaryWriter(fS);

							bW->Write(
								fC
							);
							bW->Flush();

							bW->Close();
						}
						else
						{
							unsigned long length = stream->ReadInt32();
							auto fC = stream->ReadBytes(length);
						}
					}
				}
			}


			stream->Close();
		}
	};


}