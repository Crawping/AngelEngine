#pragma once

#include"SubSystemBase.h"
#include<memory>

namespace AngelCore
{
	namespace AngelSubSystem
	{
		class FileSystemManager : public ISubSystem
		{
		private:

			struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };
			typedef public std::unique_ptr<void, handle_closer> ScopedHandle;
			static HANDLE safe_handle(HANDLE h) { return (h == INVALID_HANDLE_VALUE) ? 0 : h; }

		public:
			virtual bool StartUp(...) override;
			virtual bool ShutDown() override;

			static std::wstring GetContentDirectory()
			{
				TCHAR Buffer[MAX_PATH];
				
				GetCurrentDirectory(MAX_PATH, Buffer);
				std::wstring data = Buffer;

				return std::wstring(data.begin(),data.end());
			}

			static int IsFileExists(const std::wstring& filePath)
			{
				


				auto path = filePath;
				auto hr = _waccess(path.c_str(), 0);
				if (hr != -1)
				{
					//File exists, now check for write permission
					hr = _waccess(path.c_str(), 2);
					if (hr == -1)
					{
						AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("FAILED FIND SHADER!");
						return 2;
					}
					return 1;
				}
				return -1;
			}

			static HRESULT ReadBinaryFile(_In_z_ std::wstring fileName, _Inout_ std::unique_ptr<uint8_t[]>& data,
				_Out_ size_t* dataSize, _Out_ int* fileState)
			{
				//Based on DirectXTK

				/*auto path = fileName.c_str();
				*fileState = IsFileExists(path);
				if (*fileState != 1)
				{
					return S_FALSE;
				}*/

				// Open the file.
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
				ScopedHandle hFile(safe_handle(CreateFile2(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
#else
				ScopedHandle hFile(safe_handle(CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
#endif

				if (!hFile)	return HRESULT_FROM_WIN32(GetLastError());

				// Get the file size.
				LARGE_INTEGER fileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
				FILE_STANDARD_INFO fileInfo;

				if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
				{
					return HRESULT_FROM_WIN32(GetLastError());
				}

				fileSize = fileInfo.EndOfFile;
#else
				GetFileSizeEx(hFile.get(), &fileSize);
#endif

				// File is too big for 32-bit allocation, so reject read.
				if (fileSize.HighPart > 0)
					return E_FAIL;

				// Create enough space for the file data.
				data.reset(new uint8_t[fileSize.LowPart]);

				if (!data)
				{
					return E_OUTOFMEMORY;
				}
				// Read the data in.
				DWORD bytesRead = 0;

				if (!ReadFile(hFile.get(), data.get(), fileSize.LowPart, &bytesRead, nullptr))
				{
					return HRESULT_FROM_WIN32(GetLastError());
				}

				if (bytesRead < fileSize.LowPart)
				{
					return E_FAIL;
				}

				*dataSize = bytesRead;

				return S_OK;
			}

		private:

			bool InitLogFile();
		};
	}
}
