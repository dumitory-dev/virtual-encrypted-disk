#pragma once
#include "../../global_headers/pch.h"
#include "../../../general_headers/constant.h"
#include "../../../general_headers/open_file_info.h"
namespace  ved
{

	class i_ved_manager
	{
	
	public:
		i_ved_manager(const i_ved_manager& other) = delete;
		i_ved_manager(i_ved_manager&& other) noexcept = delete;
		i_ved_manager& operator=(const i_ved_manager& other) = delete;
		i_ved_manager& operator=(i_ved_manager&& other) noexcept = delete;
		virtual ~i_ved_manager(void) = default;
		i_ved_manager(void) = default;
	
		
		virtual void mount(
			const std::wstring & path,
			const std::string & password,
			WCHAR letter) = 0;
		
		virtual void mount_ex(
			const std::wstring & path,
			unsigned long long size,
			const std::string & password, 
			WCHAR letter, 
			Crypt mode) = 0;
		
		virtual void create_file(
			const std::wstring & path,
			unsigned long long size, 
			const std::string & password,
			Crypt mode) = 0;
		
		virtual void un_mount(WCHAR letter) = 0;

		virtual void run_driver(
			DWORD flag_startup =SERVICE_DEMAND_START ,
			const std::wstring & path_driver =  L"%SystemRoot%\\System32\\Drivers\\DriverVED.sys",
			const std::wstring & name_service = L"VEDriver"
			) = 0;


		virtual std::vector<OPEN_FILE_INFORMATION> get_mounted_disks(void) = 0;

	    virtual void stop_driver(const std::wstring & device_name = L"VEDriver") = 0;

		virtual void copy_image(const std::wstring & in, const std::wstring & out) = 0;

		virtual void un_mount_all(void) = 0;

		template<typename T>
		static std::unique_ptr<i_ved_manager> create(void)
		{
			std::unique_ptr<i_ved_manager> u_ptr_return(new T());
					   
			return u_ptr_return;
			
		}

		
	};
	
}

