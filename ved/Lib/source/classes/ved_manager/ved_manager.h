#pragma once
#include <Windows.h>
#include "../../../../general_headers/open_file_info.h"
#include "../../../../general_headers/constant.h"
#include "../../interface/i_ved_manager.h"
#include "../Lib/source/classes/driver_disk/driver_disk.h"
#include "../../classes/install_manager/install_manager.h"
#include "../hash/hash.h"


namespace ved
{
	
	class ved_manager:public i_ved_manager
	{
	public:
		
		ved_manager(void):driver_(MAIN_DEVICE_SYM_LINK)
		{
			
		}
		
		~ved_manager(void) = default;
                ved_manager(const ved_manager& other) = delete;
		ved_manager(ved_manager&& other) noexcept = delete;
		ved_manager& operator=(const ved_manager& other) = delete;
		ved_manager& operator=(ved_manager&& other) noexcept = delete;
		
		void mount(
			const std::wstring& path, 
			const std::string& password,
			WCHAR letter) override;
		
		void mount_ex(
			const std::wstring& path, 
			unsigned long long size, 
			const std::string& password,
			WCHAR letter,
			Crypt mode) override;
		
		void create_file(
			const std::wstring& path,
			unsigned long long size, 
			const std::string& password,
			Crypt mode) override;
		
		void un_mount(WCHAR letter) override;

		void run_driver(
		    DWORD flag_startup = SERVICE_DEMAND_START,
			const std::wstring & path_driver =  L"%SystemRoot%\\System32\\Drivers\\DriverVED.sys",
			const std::wstring & name_service =  L"VEDriver"
			) override;

		void stop_driver(const std::wstring& device_name = L"VEDriver") override;
		void copy_image(const std::wstring& in, const std::wstring& out) override;

		void un_mount_all(void) override;

		std::vector<OPEN_FILE_INFORMATION> get_mounted_disks(void) override;

	private:

		ved::hash hash_convert_{hash::type_hash::MD5};
		ved::driver_disk driver_;

		void connected(void)
		{
			if (!this->driver_.is_connected())
			{
				try
				{
					this->driver_.connect_to_main_device();
				}
				catch (...)
				{
					throw std::runtime_error("Error open driver device!");
				}
				
			}
		}

		[[nodiscard]] std::string make_md5_hash(const std::string & password) const;
	};
	
}
