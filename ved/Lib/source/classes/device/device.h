#pragma once
#include "../file/file.h"
#include "../exceptions/Exception.h"

namespace ved
{
	class device
	{
	public:

		device(void) = default;

		~device(void) = default;

		device(const device& other) = delete;

		device(device&& other) noexcept = default;

		device& operator=(const device& other) = delete;

		device& operator=(device&& other) noexcept = default;


		explicit device(const std::wstring& path_driver) :path_driver_(path_driver)
		{

		}

		void connect(void);

		void disconnect(void)
		{
			this->h_driver_ = {};
		}

		auto get_path(void) const noexcept
		{
			return this->path_driver_;
		}

		auto set_path(const std::wstring& path)
		{
			if (path.empty())
			{
				throw std::runtime_error("Path device is empty!");
			}

			this->path_driver_ = path;
		}

		DWORD send_ctl_code(
			DWORD code = 0,
			void* data_in = nullptr,
			DWORD size_data_in = 0,
			void* data_out = nullptr,
			DWORD size_data_out = 0) const;
		

		auto write(const std::vector<BYTE>& data, LPOVERLAPPED lp = {}) const
		{
			return this->h_driver_->write(data, lp);
		}

		auto read(std::vector<BYTE>& data, LPOVERLAPPED lp = {}) const
		{
			return this->h_driver_->read(data, lp);
		}

		auto is_connect(void) const noexcept
		{
			return static_cast<bool>(this->h_driver_.get());
		}


	private:
		std::wstring path_driver_{};
		std::unique_ptr<ved::i_file> h_driver_{};
		

	};
}
