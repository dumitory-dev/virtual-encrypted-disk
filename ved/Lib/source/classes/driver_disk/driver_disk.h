#pragma once

#include "../../../../general_headers/open_file_info.h"
#include "../../../../general_headers/constant.h"
#include "../define_device/define_device_manager.h"
#include "../device/device.h"
#include "../copy_sparse/copy_sparse.h"


namespace ved
{
	class driver_disk
	{
	public:
			
		static std::unique_ptr<OPEN_FILE_INFORMATION> make_file_info(
				const std::wstring& file_name,
				LARGE_INTEGER file_size,
				WCHAR drive_letter,
				const std::string& file_password,
				enum Crypt mode_crypt
			);
		
		
		driver_disk(const driver_disk& other) = delete;
		driver_disk(driver_disk&& other) noexcept = delete;
		driver_disk& operator=(const driver_disk& other) = delete;
		driver_disk& operator=(driver_disk&& other) noexcept = delete;
		driver_disk(void) = default;
		~driver_disk(void) = default;

		explicit driver_disk(const std::wstring & path_main_device)
		{
			this->main_device_ = device(path_main_device);
			this->path_main_device_ = path_main_device;
			
		}

		void connect_to_main_device(void)
		{
			this->main_device_.connect();
		}
		
		auto is_connected(void) const noexcept {

			return this->main_device_.is_connect();
		}
				
		void mount_disk(const std::unique_ptr<OPEN_FILE_INFORMATION> & open_file) const;

		void create_file_disk(const std::unique_ptr<OPEN_FILE_INFORMATION> & open_file) const;

		static void un_mount_disk(WCHAR letter);

		static void copy_image(const std::wstring & in, const std::wstring & out);
		[[nodiscard]] std::vector<OPEN_FILE_INFORMATION> get_mounted_disks(void) const;

	private:
			
		[[nodiscard]] size_t get_free_number_device(void)const;
			
		device main_device_{};
		std::wstring path_main_device_{};


	};

}

