#pragma once

#include "classes/copy_sparse/copy_sparse.h"
#include "classes/device/device.h"
#include "open_file_info.h"

namespace ved {
	/**
	 * \brief A class for interacting with the disk driver.
	 * It encapsulates the work with the kernel objects  
	 * 
	*/
	class driver_disk
	{
	public:
		/**
			The method creates OPEN_FILE_INFORMATION structure, which contains information about the disk image we are opening.
			Then, this object is passed to the driver
			\param [in] file_name Disk image file
			\param [in] file_size Disk image size
			\param [in] drive_letter Disk image letter
			\param [in] file_password Disk image password
			\param [in] mode_crypt Disk image crypt mode (AES or GCM)
		*/
		static std::unique_ptr<OPEN_FILE_INFORMATION> make_file_info(
			const std::wstring& file_name,
			LARGE_INTEGER file_size,
			WCHAR drive_letter,
			const std::string& file_password,
			enum Crypt mode_crypt);

		driver_disk(const driver_disk& other) = delete;
		driver_disk(driver_disk&& other) noexcept = delete;
		driver_disk& operator=(const driver_disk& other) = delete;
		driver_disk& operator=(driver_disk&& other) noexcept = delete;
		driver_disk(void) = default;
		~driver_disk(void) = default;

		/**
			The constructor takes the path to the main kernel object  
			\param [in] path_main_device Path to main kernel object 
		*/
		explicit driver_disk(const std::wstring& path_main_device)
		{
			this->main_device_ = device(path_main_device);
			this->path_main_device_ = path_main_device;
		}

		/**
			Trying to open the device...
		*/
		void connect_to_main_device(void) { this->main_device_.connect(); }

		/**
			The method checks if we are connected to the main kernel object (driver)
		*/
		[[nodiscard]] auto is_connected(void) const noexcept
		{
			return this->main_device_.is_connect();
		}

		/**
			The method mounts the disk to the system
			\param[in] open_file OPEN_FILE_INFORMATION structure, which contains information about the disk image we are opening.
		*/
		void mount_disk(
			const std::unique_ptr<OPEN_FILE_INFORMATION>& open_file) const;

		/**
			The method creates a disk image in the file system 
			\param[in] open_file OPEN_FILE_INFORMATION structure, which contains information about the disk image we are opening.
		*/
		void create_file_disk(
			const std::unique_ptr<OPEN_FILE_INFORMATION>& open_file) const;

		/**
			Method of unmounting a disk from the system 
			\param[in] letter Disk letter
		*/
		static void un_mount_disk(WCHAR letter);

		/**
			The method copies the sparse file 
			\param[in] in copy from where
			\param[in] out copy to where 
		*/
		static void copy_image(const std::wstring& in, const std::wstring& out);

		/**
			The method returns disks that we have mounted to the system 
			\return std::vector<OPEN_FILE_INFORMATION>
		*/
		[[nodiscard]] std::vector<OPEN_FILE_INFORMATION> get_mounted_disks(
			void) const;

	private:
		[[nodiscard]] size_t get_free_number_device(void) const;

		device main_device_{};
		std::wstring path_main_device_{};
	};

}
