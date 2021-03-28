#pragma once 

#include "../exceptions/Exception.h"
#include "../wrap/Wrap.h"
namespace ved
{
	class install_manager
	{
		//using smart_handle = std::unique_ptr<void,std::function<void(void * ptr)>>;
		using handle = std::unique_ptr<ved::service::handle>;
	public:
		
		install_manager(void) = delete;
		~install_manager(void) = delete;
		install_manager(const install_manager& other) = delete;
		install_manager(install_manager&& other) noexcept = delete;
		install_manager& operator=(const install_manager& other) = delete;
		install_manager& operator=(install_manager&& other) noexcept = delete;

		
		static void install_service(
			const std::wstring& device_path,
			const std::wstring& device_name,
			DWORD startup_flag = SERVICE_DEMAND_START);
		
		static void load_service(const std::wstring& device_name);

		static void unload_service(const std::wstring& device_name);

		static void uninstall_service(const std::wstring& device_name);
		
	private:
		static handle open_manager(const DWORD access = SC_MANAGER_ALL_ACCESS);
					
		static handle open_service(
			const handle& u_handle,
			const std::wstring& device_name,
			const DWORD access = SERVICE_ALL_ACCESS);
				
	};
	
}