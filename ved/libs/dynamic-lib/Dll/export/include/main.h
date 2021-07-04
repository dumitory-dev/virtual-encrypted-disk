#pragma once

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#error "not supported"
#endif

#include "functions.h"

extern "C"
{

	template DLL_API bool un_mount(wchar_t letter) noexcept;

	template DLL_API bool mount(const wchar_t* path,
		const char* password,
		wchar_t letter) noexcept;

	template DLL_API bool mount_ex(const wchar_t* path,
		unsigned long long size,
		const char* password,
		wchar_t letter,
		enum Crypt crypt_mode) noexcept;

	template DLL_API bool create_file(const wchar_t* path,
		unsigned long long size,
		const char* password,
		enum Crypt crypt_mode) noexcept;

	template DLL_API bool run_driver(unsigned long flag_startup,
		const wchar_t* path_driver,
		const wchar_t* name_service) noexcept;

	template DLL_API bool copy_image(const wchar_t* src, const wchar_t* dest) noexcept;

	template DLL_API bool get_mounted_disks(char* buffer, int* len) noexcept;

	template DLL_API const wchar_t* get_error() noexcept;
}
