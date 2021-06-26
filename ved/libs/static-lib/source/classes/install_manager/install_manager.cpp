
#include "install_manager.h"
namespace ved
{
	install_manager::handle install_manager::open_manager(const DWORD access)
	{
		auto h = ved::service::handle::create(
			::OpenSCManager(
				{},
				{},
				access));

		if (!*h)
		{
			throw ved::service_exception(L"OpenSCManager error!", ::GetLastError());
		}

		return h;
	}

	void install_manager::install_service(const std::wstring& device_path, const std::wstring& device_name, DWORD startup_flag)
	{
		const auto h = ved::service::handle::create(::CreateService(
			*install_manager::open_manager(),
			device_name.c_str(),
			device_name.c_str(),
			SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,
			startup_flag,
			SERVICE_ERROR_NORMAL,
			device_path.c_str(),
			{},
			{},
			{},
			{},
			{}
		));


		if (!*h)
		{
			const auto error_code = ::GetLastError();
			if (error_code != ERROR_SERVICE_EXISTS)
			{
				throw ved::service_exception(L"Error CreateService!", ::GetLastError());
			}
		}
	}

	install_manager::handle install_manager::open_service(const handle& u_handle,
	                                                            const std::wstring& device_name, const DWORD access)
	{
		auto h = ved::service::handle::create(::OpenService(
			*u_handle,
			device_name.c_str(),
			access
		));

		if (!*h)
		{
			throw ved::service_exception(L"Error OpenService!", ::GetLastError());
		}

		return h;
	}

	void install_manager::load_service(const std::wstring& device_name)
	{
		
		const auto h = install_manager::open_service(
			install_manager::open_manager(),
			device_name.c_str(),
			SERVICE_START
		);
		
		if (!::StartService(
			*h,
			{},
			{}
		))
		{
			const auto error = ::GetLastError();
			if (error != ERROR_SERVICE_ALREADY_RUNNING)
			{
				throw ved::service_exception(L"Error StartService!", GetLastError());
			}
		}
	}

	void install_manager::unload_service(const std::wstring& device_name)
	{
		const auto h = install_manager::open_service(
			install_manager::open_manager(),
			device_name.c_str(),
			SERVICE_STOP
		);

		SERVICE_STATUS status{};

		if (!::ControlService(
			*h,
			SERVICE_CONTROL_STOP,
			&status
		))
		{
			throw ved::service_exception(L"Error ControlService!", ::GetLastError());
		}
	}

	void install_manager::uninstall_service(const std::wstring& device_name)
	{
		const auto h = install_manager::open_service(
			install_manager::open_manager(),
			device_name,
			SERVICE_ALL_ACCESS
		);

		if (!::DeleteService(*h))
		{
			throw ved::service_exception(L"Error DeleteService!", ::GetLastError());
		}
	}
}
