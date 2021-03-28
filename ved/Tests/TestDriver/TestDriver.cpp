#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <thread>
#include "classes/driver_disk/driver_disk.h"
#include "classes/hash/hash.h"

namespace fs = std::filesystem;


void TEST_CREATE_AND_MOUNT(void)
{


	ved::driver_disk driver(MAIN_DEVICE_SYM_LINK);

	driver.connect_to_main_device();
	std::wcout << "Connection to driver completed successfully" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	const ved::hash hash(ved::hash::type_hash::MD5);
	const std::wstring path_file = L"C:\\test.img";

	if (fs::exists(path_file))
	{

		if (!fs::remove(path_file))
		{
			throw std::runtime_error("Error delete old file image!");
		}

		std::wcout << L"Old file image deleted!" << std::endl;

	}

	LARGE_INTEGER size;
	size.QuadPart = static_cast<unsigned long long>(100) * 1024 * 1024;
	const auto letter = L'J';
	const auto mode = Crypt::RC4;
	auto password = hash.get_hash({ '1','2','3' });
	password.resize(MAX_PASSWORD_SIZE);
	std::wcout << L"Password hash is ready!" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	const auto file_info = ved::driver_disk::make_file_info(path_file, size, letter, password, mode);
	driver.create_file_disk(file_info);

	std::wcout << L"Image file created!" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	file_info->CryptMode = Crypt::CryptPrev;
	file_info->FileSize.QuadPart = 0;
	driver.mount_disk(file_info);

	std::wcout << L"The disk mounted!" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	ved::driver_disk::un_mount_disk(letter);

	std::wcout << L"The disk is unmounted!" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	if (!fs::remove(path_file))
	{
		throw std::runtime_error("Error delete file image!");
	}

	std::wcout << L"File image deleted!" << std::endl;


	std::wcout << "\nTESTS SUCCESS!" << std::endl;


}

void TEST_MOUNT_ALWAYS(void)
{
	ved::driver_disk driver(MAIN_DEVICE_SYM_LINK);

	driver.connect_to_main_device();
	std::wcout << "Connection to driver completed successfully" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	const ved::hash hash(ved::hash::type_hash::MD5);
	const std::wstring path_file = L"C:\\test.img";

	if (fs::exists(path_file))
	{

		if (!fs::remove(path_file))
		{
			throw std::runtime_error("Error delete old file image!");
		}

		std::wcout << L"Old file image deleted!" << std::endl;

	}

	LARGE_INTEGER size;
	size.QuadPart = static_cast<unsigned long long>(100) * 1024 * 1024;
	const auto letter = L'J';
	const auto mode = Crypt::RC4;
	auto password = hash.get_hash({ '1','2','3' });
	password.resize(MAX_PASSWORD_SIZE);
	std::wcout << L"Password hash is ready!" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	const auto file_info = ved::driver_disk::make_file_info(path_file, size, letter, password, mode);
	//driver.create_file_disk(file_info);
	driver.mount_disk(file_info);

	std::wcout << L"Image file created!" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		
	std::wcout << L"The disk mounted!" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	ved::driver_disk::un_mount_disk(letter);

	std::wcout << L"The disk is unmounted!" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	if (!fs::remove(path_file))
	{
		throw std::runtime_error("Error delete file image!");
	}

	std::wcout << L"File image deleted!" << std::endl;
	
	std::wcout << "\nTESTS SUCCESS!" << std::endl;

	
}


int main(void)
{
	try
	{
		TEST_CREATE_AND_MOUNT();
		std::cout<<"Test two..."<<std::endl;
		TEST_MOUNT_ALWAYS();
		

	}
	catch (const ved::c_win_api_exception& error)
	{
		std::wcout << error.GetMessageW() << std::endl;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown error" << std::endl;
	}


}
