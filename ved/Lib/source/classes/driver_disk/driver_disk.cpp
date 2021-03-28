#include "driver_disk.h"
namespace ved
{
	std::unique_ptr<OPEN_FILE_INFORMATION> driver_disk::make_file_info(
		const std::wstring& file_name, LARGE_INTEGER file_size, WCHAR drive_letter, const std::string& file_password, enum Crypt mode_crypt)
	{
		if (file_name.empty()
			||
			(file_size.QuadPart != 0 && file_size.QuadPart < 10)
			||
			file_password.length() != 16
			||
			(!iswalpha(drive_letter) && drive_letter != ' ')
			)
		{
			throw std::invalid_argument("Failed create_file_info! Invalid arguments!");
		}
		
		std::unique_ptr<OPEN_FILE_INFORMATION> uptr_file_info = std::make_unique<OPEN_FILE_INFORMATION>();
					

		wcscpy_s(uptr_file_info->FileName, L"\\??\\");
		wcscat_s(uptr_file_info->FileName, file_name.c_str());

		uptr_file_info->FileNameLength = static_cast<USHORT>(wcslen(uptr_file_info->FileName));
		uptr_file_info->FileSize.QuadPart = file_size.QuadPart;
		uptr_file_info->DriveLetter = drive_letter;

		std::copy(file_password.cbegin(), file_password.cend(), uptr_file_info->Password);
		uptr_file_info->PasswordLength = static_cast<USHORT>(file_password.length());
		uptr_file_info->CryptMode = mode_crypt;
		

		return uptr_file_info;
	}

	void driver_disk::mount_disk(const std::unique_ptr<OPEN_FILE_INFORMATION> & open_file) const
	{


		ved::define_device_manager define_manager(open_file->DriveLetter);
		
		if (define_manager.check_define())
		{
			throw std::runtime_error("Error check_define! Letter is exist!");
		}
		
		
		const auto number_device = this->get_free_number_device();
		const auto path_new_device = DEVICE_NAME_PREFIX + std::to_wstring(number_device);
				
		
		define_manager.link_device(path_new_device);
		
		try
		{
			auto define_device = define_manager.make_device();
			define_device.connect();
			define_manager.report_define();
			define_device.send_ctl_code(
				IOCTL_FILE_DISK_OPEN_FILE,
				open_file.get(),
				sizeof(_OPEN_FILE_INFORMATION) + open_file->FileNameLength * sizeof(WCHAR) + 7);
		
						
		}
		catch (...)
		{
			std::wcout<<"Error mount disk!"<<std::endl;
			define_manager.delete_define();
			define_manager.report_delete_define();
			
			throw;
		}

	
	}

	void driver_disk::create_file_disk(const std::unique_ptr<OPEN_FILE_INFORMATION> & open_file) const
	{
		
		auto ret = this->main_device_.send_ctl_code(
			IOCTL_FILE_DISK_OPEN_FILE,
			open_file.get(),
			sizeof(_OPEN_FILE_INFORMATION) + open_file->FileNameLength * sizeof(WCHAR) + 7);


		if (ret != 0)
		{
			 throw ved::c_win_api_exception(L"Error create file disk!", ret);
		}
		
	}

	
	size_t driver_disk::get_free_number_device(void)const
	{
		size_t number_device = this->main_device_.send_ctl_code(IOCTL_GET_FREE_DEVICE);
		
		if (!number_device)
		{
			number_device = this->main_device_.send_ctl_code(IOCTL_FILE_ADD_DEVICE);
		}

		return number_device;
	}
	

	void driver_disk::un_mount_disk(const WCHAR letter)
	{
			
		const ved::define_device_manager define_manager(letter);
		auto device = define_manager.make_device();
		device.connect();
		device.send_ctl_code(IOCTL_FILE_DISK_CLOSE_FILE);
		device.send_ctl_code(FSCTL_DISMOUNT_VOLUME);
		define_manager.delete_define();
		define_manager.report_delete_define();
			
	}

	void driver_disk::copy_image(const std::wstring & in, const std::wstring& out)
	{
		ved::copy_sparse::copy(in,out);
	}

	std::vector<OPEN_FILE_INFORMATION> driver_disk::get_mounted_disks(void) const
	{

		const size_t max_count_disk = 25;
		std::vector<OPEN_FILE_INFORMATION> data(max_count_disk,OPEN_FILE_INFORMATION());
		const auto ret = this->main_device_.send_ctl_code(IOCTL_GET_MOUNT_DEVICES,nullptr,0,&data[0],max_count_disk * sizeof(OPEN_FILE_INFORMATION));
		data.resize(ret);

		return data;
	}
}
