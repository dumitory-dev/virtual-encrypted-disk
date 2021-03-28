
#include "device.h"
namespace ved
{
	void device::connect()
	{
		ved::file::settings settings{};

		settings.dw_creation_disposition = OPEN_EXISTING;
		settings.dw_desired_access = GENERIC_READ | GENERIC_WRITE;
		settings.dw_flags_and_attributes = FILE_FLAG_NO_BUFFERING;
		settings.dw_share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		settings.ws_file_name = this->path_driver_;

		h_driver_ = ved::file::create(settings);
	}

	DWORD device::send_ctl_code(
		const DWORD code, 
		void* data_in, 
		const DWORD size_data_in,
		void* data_out,
	    const DWORD size_data_out) const
	{

		
		DWORD ret = 0;

		if (!::DeviceIoControl(
			h_driver_->operator void*(),
			code,
			data_in,
			size_data_in,
			data_out,
			size_data_out,
			&ret,
			nullptr
		))
		{

		
			throw ved::driver_exception(L"Error DeviceIoControl!", GetLastError());
		}
		
		return ret;
	}
}

