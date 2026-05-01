#pragma once
#include <string>

using namespace System;
using namespace System::Text;
using namespace System::Security::Cryptography;

namespace Engine::Encryption
{
	public ref class CypherLib
	{
	private:
		static RSACryptoServiceProvider^ serviceProvider;

	public:
		static String^ Encrypt(System::String^ content, String^ password, int rounds)
		{
			unsigned int passwordBytes = GetPasswordBytes(password);

			for (int x = 0; x < rounds; x++)
			{
				content = EncryptString(content, passwordBytes);
			}

			return content;
		}

		static String^ Decrypt(System::String^ content, String^ password, int rounds)
		{
			unsigned int passwordBytes = GetPasswordBytes(password);

			for (int x = 0; x < rounds; x++)
			{
				content = DecryptString(content, passwordBytes);
			}

			return content;
		}

		static unsigned int GetPasswordBytes(System::String^ passwd)
		{
			unsigned int retn = 0;

			for each(char c in passwd->ToCharArray())
			{
				retn += (unsigned int)c;
			}

			return retn;
		}

		static unsigned int GetPasswordBytes(const char* passwd)
		{
			unsigned int retn = 0;

			std::string dest(passwd);

			for (int i = 0; i < dest.size(); i++)
			{
				retn += (unsigned int)dest[i];
			}

			return retn;
		}

		static System::String^ EncryptString(System::String^ fileContents, unsigned int password)
		{
			String^ encryptedContents = "";

			for each(char s in fileContents->ToCharArray())
			{
				int cId = (int)s;
				int byteResult = cId ^ password;
				encryptedContents += "/" + byteResult.ToString();
			}

			return encryptedContents->Substring(1, encryptedContents->Length - 1);
		}

		static System::String^ DecryptString(System::String^ fileContents, unsigned int password)
		{
			System::String^ decryptedContents = "";
			
			auto decStr = fileContents->Split('/');

			for each(String ^ s in decStr)
			{
				decryptedContents += (Char)(System::Int32::Parse(s) ^ password);
			}

			return decryptedContents;
		}

		/*
		static String^ EncryptString(String^ message, String^ passwd)
		{
			RijndaelManaged^ cryptMethod = gcnew RijndaelManaged();
			cryptMethod->BlockSize = 256;
			cryptMethod->KeySize = 128;
			cryptMethod->Mode = CipherMode::ECB;
			cryptMethod->Padding = PaddingMode::ISO10126;
			cryptMethod->Key = Encoding::UTF8->GetBytes(passwd);
			ICryptoTransform^ cryptoBro = cryptMethod->CreateEncryptor();

			auto plainText = Encoding::UTF8->GetBytes(message);

			return Convert::ToBase64String(cryptoBro->TransformFinalBlock(plainText, 0, plainText->Length));

			return "";
		}

		static String^ DecryptString(String^ message, String^ passwd)
		{
			RijndaelManaged^ cryptMethod = gcnew RijndaelManaged();
			cryptMethod->BlockSize = 256;
			cryptMethod->KeySize = 128;
			cryptMethod->Mode = CipherMode::ECB;
			cryptMethod->Padding = PaddingMode::ISO10126;
			cryptMethod->Key = Encoding::UTF8->GetBytes(passwd);
			ICryptoTransform^ cryptoBro = cryptMethod->CreateDecryptor();

			auto plainText = Convert::FromBase64CharArray(message->ToCharArray(), 0, message->Length);

			return Encoding::UTF8->GetString(cryptoBro->TransformFinalBlock(plainText, 0, plainText->Length));

			return "";
		}
		*/
	};
}