#include <memory>
#ifndef MAPPER_DELETER
#define MAPPER_DELETER
#include <Windows.h>
namespace WinapiDeleter {
	struct Mapper_Deleter {
		void operator()(BYTE* lpBaseAddress) {
			UnmapViewOfFile(lpBaseAddress);
		}
	};
}

using Mapper_Winapi_Uptr = std::unique_ptr<BYTE, WinapiDeleter::Mapper_Deleter>;


#endif // ! MAPPER_DELETER
