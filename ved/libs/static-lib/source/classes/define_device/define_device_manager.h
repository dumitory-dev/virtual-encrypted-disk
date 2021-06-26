#pragma once

#include "../device/device.h"
#include <Shlobj.h>

namespace ved
{

	class define_device_manager
	{
	public:
		define_device_manager(const define_device_manager& other) = default;
		define_device_manager(define_device_manager&& other) noexcept = default;
		define_device_manager& operator=(const define_device_manager& other) = default;
		define_device_manager& operator=(define_device_manager&& other) noexcept = default;
		define_device_manager(void) = delete;
		~define_device_manager(void) = default;


		explicit define_device_manager(const WCHAR letter)
		{
			this->letter_ = define_device_manager::make_path_letter(letter);
			this->drive_name_ = define_device_manager::make_drive_name(letter);
		}


		[[nodiscard]] bool check_define(void) const;
		void link_device(const std::wstring& path_device);
		void delete_define(void) const;
		void report_define(void) const;
		void report_delete_define(void) const;

		[[nodiscard]] auto make_device(void) const
		{
			return ved::device(this->letter_);
		}

	private:
		std::wstring letter_{};
		std::wstring drive_name_{};

		
		static std::wstring make_path_letter(WCHAR letter)
		{
			std::wstring path_letter = LR"(\\.\ :)";
			path_letter[4] = letter;

			return path_letter;
		}

		static std::wstring make_drive_name(WCHAR letter)
		{
			std::wstring drive_name = L" :\\";

			drive_name[0] = letter;

			return drive_name;
		}
	};



}
