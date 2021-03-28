#include  "define_device_manager.h"

namespace  ved
{
	void define_device_manager::link_device(const std::wstring& path_device)
	{
		if (!::DefineDosDevice(
			DDD_RAW_TARGET_PATH,
			&this->letter_[4],
			path_device.c_str()))
		{
			throw ved::c_win_api_exception(L"Error DefineDosDevice!(add)", ::GetLastError());
		}
	}

	void define_device_manager::delete_define(void) const
	{
		if (!::DefineDosDevice(
			DDD_REMOVE_DEFINITION,
			&this->letter_[4],
			nullptr
		))
		{
			throw ved::c_win_api_exception(L"Error DefineDosDevice(delete)!", ::GetLastError());
		}
	}

	void define_device_manager::report_define(void) const
	{
		::SHChangeNotify(SHCNE_DRIVEADD, SHCNF_PATH, this->drive_name_.c_str(), nullptr);
	}

	void define_device_manager::report_delete_define(void) const
	{
		::SHChangeNotify(SHCNE_DRIVEREMOVED, SHCNF_PATH, this->drive_name_.c_str(), nullptr);
	}

	bool define_device_manager::check_define(void) const
	{
		try
		{
			ved::file::settings settings{};

			settings.dw_creation_disposition = OPEN_EXISTING;
			settings.dw_desired_access = GENERIC_READ | GENERIC_WRITE;
			settings.dw_flags_and_attributes = FILE_FLAG_NO_BUFFERING;
			settings.dw_share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
			settings.ws_file_name = this->letter_;
		
			auto tmp = ved::file::create(settings);
		
			return true;
		}
		catch (const ved::c_win_api_exception & error)
		{
			if (error.get_code() != 0x02)
			{
				return true;
			}
			return false;
		}
	}

	
}
