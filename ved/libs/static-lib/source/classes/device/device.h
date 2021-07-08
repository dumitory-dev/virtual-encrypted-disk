#pragma once
#include <utility>
#include "classes/file/file.h"

namespace ved {
    /**
	 * \brief A class for interacting with the driver object.
	 * Although it is better to inherit this class from i_file, I no longer remember why it looks like this :(
	*/
	class device
	{
	public:
		device(void) = default;
		~device(void) = default;
		device(const device& other) = delete;
		device(device&& other) noexcept = default;
		device& operator=(const device& other) = delete;
		device& operator=(device&& other) noexcept = default;

		/**
		 * Constructor
		 * \param [in] path_driver - The path to the driver object.
		 * We have a master object that creates the other objects for a particular drive.
		*/
		explicit device(std::wstring path_driver)
			: path_driver_(std::move(path_driver))
		{}
		
		/**
			Method for connecting to a device. 
		*/
		void connect(void);
		
		/**
			Method for disconnecting to a device. 
		*/
		void disconnect(void) { this->h_driver_ = {}; }
		
		/**
		 * The method returns the path to the device
		 * \return wstring
		*/
		[[nodiscard]] auto get_path(void) const noexcept { return this->path_driver_; }

		/**
		 *  Setting path 
		 *  \param [in] path new path (must not be empty)
		*/
		auto set_path(const std::wstring& path)
		{
			if (path.empty()) {
				throw std::runtime_error("Path device is empty!");
			}

			this->path_driver_ = path;
		}

		/**
		 * The method sends a message to the device. See also: https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-i-o-control-codes
		 * \param [in] code Command code
		 * \param [in] data_in Data buffer for send
		 * \param [in] size_data_in Data buffer size
		 * \param [in] data_out Buffer to retrieve information
		 * \param [in] size_data_out Buffer to retrieve size
		 */
		DWORD send_ctl_code(DWORD code = 0,
			void* data_in = nullptr,
			DWORD size_data_in = 0,
			void* data_out = nullptr,
			DWORD size_data_out = 0) const;
		
		/**
		 *  Writing information to the device. 
		 *  \param [in] data  Data buffer for send
		 *  \param [in] lp LPOVERLAPPED for async
		*/
		auto write(const std::vector<BYTE>& data, LPOVERLAPPED lp = {}) const
		{
			return this->h_driver_->write(data, lp);
		}

		/**
		 *  Reading information to the device. 
		 *  \param [out] data  Buffer to retrieve information
		 *  \param [in] lp LPOVERLAPPED for async
		*/
		auto read(std::vector<BYTE>& data, LPOVERLAPPED lp = {}) const
		{
			return this->h_driver_->read(data, lp);
		}

		/**
		 *  The method checks if we are connected to the device 
		 *  \return bool
		*/
		[[nodiscard]] auto is_connect(void) const noexcept
		{
			return static_cast<bool>(this->h_driver_.get());
		}

	private:
		std::wstring path_driver_{};
		std::unique_ptr<ved::i_file> h_driver_{};
	};
}
