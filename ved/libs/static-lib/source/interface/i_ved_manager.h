#pragma once
#include <Windows.h>

#include "open_file_info.h"
#include "pch.h"

namespace ved {

	/**
	 \brief Main interface for classes that interact with the driver
	*/
	class i_ved_manager
	{

	public:
		i_ved_manager(const i_ved_manager& other) = delete;
		i_ved_manager(i_ved_manager&& other) noexcept = delete;
		i_ved_manager& operator=(const i_ved_manager& other) = delete;
		i_ved_manager& operator=(i_ved_manager&& other) noexcept = delete;
		virtual ~i_ved_manager(void) = default;
		i_ved_manager(void) = default;

		/**
			Mounting the disk to the system.  
			\param[in] path Path to disk image (file)
			\param[in] password Disk image password
			\param[in] letter Disk letter
		*/
		virtual void mount(const std::wstring& path,
			const std::string& password,
			WCHAR letter) = 0;

		/**
			The method creates a disk image and immediately mounts it to the system.
			\param[in] path Path to disk image (file)
			\param[in] size Disk image size
			\param[in] password Disk image password
			\param[in] letter Disk letter
			\param[in] mode Disk image crypt mode (AES or RC4)
			
			\warning The size of the disk can be specified in kilobytes or megabytes,
			the class that implements this interface must determine this itself.
		*/
		virtual void mount_ex(const std::wstring& path,
			unsigned long long size,
			const std::string& password,
			WCHAR letter,
			Crypt mode) = 0;

		/**
			The method creates a disk image
			\param[in] path Path to disk image (file)
			\param[in] size Disk image size
			\param[in] password Disk image password
			\param[in] mode Disk image crypt mode (AES or RC4)
			
			\warning The size of the disk can be specified in kilobytes or megabytes,
			the class that implements this interface must determine this itself.
		*/
		virtual void create_file(const std::wstring& path,
			unsigned long long size,
			const std::string& password,
			Crypt mode) = 0;

		/**
			Unmount the disk 
			\param[in] letter Disk letter
		*/
		virtual void un_mount(WCHAR letter) = 0;

		/**
			Registers and runs the driver on the system
			\param[in] flag_startup The driver launch flag. There can be several variants:
			SERVICE_DEMAND_START - A service started by the service control manager when a process calls the StartService function.
			SERVICE_AUTO_START - A service started automatically by the service control manager during system startup.
			\param[in] path_driver Path to the driver file. We assume it will be in System32\\Drivers\\
			\param[in] name_service Driver service name
		*/
		virtual void run_driver(DWORD flag_startup = SERVICE_DEMAND_START,
			const std::wstring& path_driver =
			L"%SystemRoot%\\System32\\Drivers\\DriverVED.sys",
			const std::wstring& name_service = L"VEDriver") = 0;
		
		/**
			Returns the list of disks that are currently mounted to the system (our disks)
			\return std::vector<OPEN_FILE_INFORMATION>
		*/
		virtual std::vector<OPEN_FILE_INFORMATION> get_mounted_disks(void) = 0;

		/**
			Stop driver
			\param[in] device_name Driver service name
		*/
		virtual void stop_driver(const std::wstring& device_name = L"VEDriver") = 0;

		/**
			All files we create can be "sparse". They have to be copied in a special way.
			See also: https://en.wikipedia.org/wiki/Sparse_file
			\param[in] in Where to copy from
			\param[in] out Where to copy 
		*/
		virtual void copy_image(const std::wstring& in, const std::wstring& out) = 0;

		/**
			Unmount all disks at once
		*/
		virtual void un_mount_all(void) = 0;

		/**
			Factory method 
		*/
		template<typename T>
		static std::unique_ptr<i_ved_manager> create(void)
		{
			std::unique_ptr<i_ved_manager> u_ptr_return = std::make_unique<T>();

			return u_ptr_return;
		}
	};

}
