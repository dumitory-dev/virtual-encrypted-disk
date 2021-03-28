#include "ved_manager.h"
namespace ved
{
	void ved_manager::mount(
		const std::wstring& path,
		const std::string& password,
		WCHAR letter)
	{
		if (!std::filesystem::exists(path))
		{
			throw std::invalid_argument("File is not exist!");
		}
		
	    this->connected();
		const auto hash_password = this->make_md5_hash(password);
		LARGE_INTEGER size;
		size.QuadPart = 0;
		const auto disk_info = ved::driver_disk::make_file_info(path,size,letter,hash_password,Crypt::CryptPrev);
		this->driver_.mount_disk(disk_info);
	}

	void ved_manager::mount_ex(
		const std::wstring& path,
		const unsigned long long size, 
		const std::string& password,
		const WCHAR letter,
		const Crypt mode)
	{
		if (std::filesystem::exists(path))
		{
			throw std::invalid_argument("File is now exist!");
		}
		
		this->connected();
		const auto hash_password = this->make_md5_hash(password);
		LARGE_INTEGER size_file;
		size_file.QuadPart = size;
		const auto disk_info = ved::driver_disk::make_file_info(path,size_file,letter,hash_password,mode);
		this->driver_.mount_disk(disk_info);
		
	}
	

	void ved_manager::create_file(
		const std::wstring& path,
		const unsigned long long size,
		const std::string& password,                      
		const Crypt mode)
	{

		if (std::filesystem::exists(path))
		{
			throw std::invalid_argument("File is now exist!");
		}
		
		this->connected();
		const auto hash_password = this->make_md5_hash(password);
		LARGE_INTEGER size_file;
		size_file.QuadPart = size;
		const auto disk_info = ved::driver_disk::make_file_info(path,size_file,L'C',hash_password,mode);
		this->driver_.create_file_disk(disk_info);
		
		
	}
		
	void ved_manager::un_mount(const WCHAR letter)
	{
		this->connected();
		ved::driver_disk::un_mount_disk(letter);
		
	}

	void ved_manager::run_driver(DWORD flag_startup, const std::wstring& path_driver, const std::wstring& name_service)
	{
		
		ved::install_manager::install_service(path_driver,name_service,flag_startup);
		ved::install_manager::load_service(name_service);
						
	}

	void ved_manager::stop_driver(const std::wstring& device_name)
	{


		this->un_mount_all();
		
		ved::install_manager::unload_service(device_name);
		ved::install_manager::uninstall_service(device_name);
		
	}

	void ved_manager::copy_image(const std::wstring& in, const std::wstring& out)
	{
		this->driver_.copy_image(in,out);
	}

	void ved_manager::un_mount_all(void)
	{
	    auto disks = this->get_mounted_disks();

		for (auto & element : disks)
		{
			this->un_mount(element.DriveLetter);
		}
				
	}

	std::vector<OPEN_FILE_INFORMATION> ved_manager::get_mounted_disks(void)
	{
		this->connected();
		return this->driver_.get_mounted_disks();
				
	}

	std::string ved_manager::make_md5_hash(const std::string& password) const
	{
		auto hash = this->hash_convert_.get_hash({password.cbegin(),password.cend()});
		hash.resize(MAX_PASSWORD_SIZE);
		return hash;
		
	}
}
