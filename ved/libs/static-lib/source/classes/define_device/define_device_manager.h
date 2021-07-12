#pragma once
#include "classes/device/device.h"



namespace ved {
	/**
	* \brief A class for creating various manipulations with links in the system.
	* Here we tell the system to associate our drive letter with a kernel object.
	*/
	class define_device_manager
	{
	public:
		define_device_manager(const define_device_manager& other) = default;
		define_device_manager(define_device_manager&& other) noexcept = default;
		define_device_manager& operator=(const define_device_manager& other) =
			default;
		define_device_manager& operator=(define_device_manager&& other) noexcept =
			default;
		define_device_manager(void) = delete;
		~define_device_manager(void) = default;

		/**
		 * Constructor
		 * \param [in] letter Disk letter
		 */
		explicit define_device_manager(const WCHAR letter)
		{
			this->letter_ = define_device_manager::make_path_letter(letter);
			this->drive_name_ = define_device_manager::make_drive_name(letter);
		}

		/**
		 * The method checks if there are disks with our letter
		 * \return bool true or false
		*/
		[[nodiscard]] bool check_define(void) const;

		/**
		 * The method associates the letter of our disk with an object in the system (driver)
		 * \param [in] path_device Device object path.
		*/
		void link_device(const std::wstring& path_device);

		/**
		 * The method removes our letter from the system
		*/
		void delete_define(void) const;

		/**
		 * The method tells the system that we have added a new disk with the letter
		*/
		void report_define(void) const;

		/**
		 * The method tells the system that we have deleted our disk with the letter
		*/
		void report_delete_define(void) const;

		/**
		 * The method creates a new instance of the device class
		 */
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
