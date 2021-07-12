#pragma once
#include "classes/exceptions/exception.h"
#include "classes/wrap/wrap.h"

namespace ved {
	/**
	 * \brief The class is responsible for installing the driver on the system
	 * Note that at the moment the driver is not signed and without a windows test mode it is impossible to load it on the system.
	 * Except for a couple of hack, but i don't use them (though why?)
	 *
	 * If UAC is enabled:
	 *
	 * Also, driver registration should be in administrator mode, and disk management in normal user mode.
	 * This is due to the fact that the disk is visible to the user of the specific session from which he is logged in.
	 */
	class install_manager
	{
		using handle = std::unique_ptr<ved::service::handle>;

	public:
		install_manager(void) = delete;
		~install_manager(void) = delete;
		install_manager(const install_manager& other) = delete;
		install_manager(install_manager&& other) noexcept = delete;
		install_manager& operator=(const install_manager& other) = delete;
		install_manager& operator=(install_manager&& other) noexcept = delete;

		/**
			Registers driver on the system
			\param[in] startup_flag The driver launch flag. There can be several variants:
			SERVICE_DEMAND_START - A service started by the service control manager when a process calls the StartService function.
			SERVICE_AUTO_START - A service started automatically by the service control manager during system startup.
			\param[in] device_path Path to the driver file. 
			\param[in] device_name Driver service name
		*/
		static void install_service(const std::wstring& device_path,
			const std::wstring& device_name,
			DWORD startup_flag = SERVICE_DEMAND_START);

		/**
			Run the driver on the system
			\param[in] device_name Driver service name
		*/
		static void load_service(const std::wstring& device_name);

		/**
			Stop the driver on the system
			\param[in] device_name Driver service name
		*/
		static void unload_service(const std::wstring& device_name);

		/**
			Unregister the driver on the system
			\param[in] device_name Driver service name
		*/
		static void uninstall_service(const std::wstring& device_name);

	private:
		static handle open_manager(const DWORD access = SC_MANAGER_ALL_ACCESS);

		static handle open_service(const handle& u_handle,
			const std::wstring& device_name,
			const DWORD access = SERVICE_ALL_ACCESS);
	};

}