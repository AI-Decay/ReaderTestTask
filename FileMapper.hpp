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
	std::vector<BYTE> data;
	Mapper_Winapi_Uptr memory = nullptr;

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

			CloseHandle(file);
		}

		data.resize(bufferSize);
	};

	const std::vector<BYTE>& ReadFile(size_t pos) {

		//int32_t x = static_cast<UINT32>((pos & 0xFFFFFFFF00000000LL) >> 32);
		//int32_t y = static_cast<UINT32>(pos & 0xFFFFFFFFLL);

		memory = Mapper_Winapi_Uptr{ static_cast<BYTE*>(MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, bufferSize)) };
		std::memcpy(&data[0], memory.get(), bufferSize);
		return data;
	}


	~FileMapper() {
		CloseHandle(mapping);
	}

};


#endif // ! FILE_MAPPER_HPP
