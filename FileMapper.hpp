#ifndef FILE_MAPPER_HPP
#define FILE_MAPPER_HPP
#include <string>
#include <vector>
#include <memory>
#include <AclAPI.h>
#include "MapDeleter.hpp"

class FileMapper
{
private:
	std::wstring path;
	HANDLE mapping;
	const size_t bufferSize = 0;
	Mapper_Winapi_Uptr memory = nullptr;
	unsigned int errorInfo = 0;
	SYSTEM_INFO SysInfo;
	DWORD SysGran;

public:

	FileMapper(std::wstring _path, size_t _bufferSize)
		:path(_path), bufferSize(_bufferSize) {
		
		mapping = OpenFileMapping(GENERIC_READ | FILE_MAP_READ, FALSE, path.c_str());
		if (mapping == NULL)
		{
			HANDLE file = CreateFile(path.c_str(),
				GENERIC_READ | FILE_MAP_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_READONLY,
				NULL);

			mapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, 0);

			if (mapping == NULL)
			    errorInfo = GetLastError();

			CloseHandle(file);
		}

		GetSystemInfo(&SysInfo);
		SysGran = SysInfo.dwAllocationGranularity;
	};

	bool ReadFile(size_t pos, std::vector<BYTE>& buffer) {

	size_t MapViewStart = (pos / SysGran) * SysGran;
	ULARGE_INTEGER ulOffset;
	ulOffset.QuadPart = MapViewStart;

	memory = Mapper_Winapi_Uptr{ static_cast<BYTE*>(MapViewOfFile(mapping, FILE_MAP_READ, ulOffset.HighPart, ulOffset.LowPart, bufferSize)) };
	if (memory == nullptr) {
	    errorInfo = GetLastError();
	    return false;
	  }
	else
	   std::memcpy(&buffer[0], memory.get(), bufferSize);

	  return true;
	}

	const unsigned int GetInfoError() { return errorInfo; }

	~FileMapper() {
		CloseHandle(mapping);
	}

};


#endif // ! FILE_MAPPER_HPP
