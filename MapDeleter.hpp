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

	struct HANDLE_Deleter {
		void operator()(HANDLE hObject) {
			CloseHandle(hObject);
		}
	};
}

using Mapper_Winapi_Uptr = std::unique_ptr<BYTE, WinapiDeleter::Mapper_Deleter>;

using HANDLE_Winapi_Uptr = std::unique_ptr<std::remove_pointer<HANDLE>::type, WinapiDeleter::HANDLE_Deleter>;


#endif // ! MAPPER_DELETER
