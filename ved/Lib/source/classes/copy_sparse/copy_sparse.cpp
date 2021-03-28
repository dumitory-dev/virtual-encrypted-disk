#include "copy_sparse.h"

namespace ved
{

	void copy_range(HANDLE h_from, HANDLE h_to, const LARGE_INTEGER i_pos, LARGE_INTEGER iSize) {
		std::vector<BYTE> buf(64 * 1024);
		DWORD dw_bytes_read, dw_bytes_written;

		auto i_pos_high = i_pos.HighPart;
		auto dr = ::SetFilePointer(h_from, i_pos.LowPart, &i_pos_high, FILE_BEGIN);

		if (dr == INVALID_SET_FILE_POINTER) {
			throw ved::sparse_exception(L"Error SetFilePointer!", ::GetLastError());
		}

		i_pos_high = i_pos.HighPart;
		dr = ::SetFilePointer(h_to, i_pos.LowPart, &i_pos_high, FILE_BEGIN);

		if (dr == INVALID_SET_FILE_POINTER) {
			throw ved::sparse_exception(L"Error SetFilePointer!", ::GetLastError());
		}

		while (iSize.QuadPart > 0) {
			if (!::ReadFile(h_from, buf.data(), static_cast<DWORD>(min(buf.size(), iSize.QuadPart)), &dw_bytes_read, nullptr))
				throw ved::sparse_exception(L"Error ReadFile!", ::GetLastError());


			if (!::WriteFile(h_to, buf.data(), dw_bytes_read, &dw_bytes_written, nullptr))
				throw ved::sparse_exception(L"Error WriteFile!", ::GetLastError());

			if (dw_bytes_written < dw_bytes_read)
				throw ved::sparse_exception(L"Error WriteFile!", ERROR_HANDLE_DISK_FULL);


			iSize.QuadPart -= dw_bytes_read;
		}
	}


	void copy_sparse::copy(const std::wstring& in, const std::wstring& out)
	{
		
		BY_HANDLE_FILE_INFORMATION bhfi;

		std::wstring sz_path(MAX_PATH, '\0');
		std::wstring sz_drive(_MAX_DRIVE, '\0');
		std::wstring sz_dir(_MAX_DIR, '\0');
		std::wstring sz_file_name(_MAX_FNAME, '\0');
		std::wstring sz_ext(_MAX_FNAME, '\0');



		PWCHAR pszName;
		std::wstring sz_vol_name(MAX_PATH, '\0');

		DWORD dw_sn, dw_max_len, dw_vol_flags;
		auto b_sparse_copy = TRUE;
		DWORD dr;
		
		if (!::GetFullPathName(in.c_str(), MAX_PATH, sz_path.data(), &pszName))
			throw ved::sparse_exception(L"Error GetFullPathName!", ::GetLastError());


		_wsplitpath(
			sz_path.c_str(),
			sz_drive.data(),
			sz_dir.data(),
			sz_file_name.data(),
			sz_ext.data());

		
		if (!sz_drive.empty()) {
		
			sz_drive.pop_back(); //delete '\0'
			sz_drive+=L"\\";
		
			if (!::GetVolumeInformation(
				sz_drive.c_str(),
				sz_vol_name.data(),
				MAX_PATH,
				&dw_sn,
				&dw_max_len,
				&dw_vol_flags,
				sz_file_name.data(),
				_MAX_FNAME))
				throw ved::sparse_exception(L"Error GetVolumeInformation!", ::GetLastError());

			b_sparse_copy = dw_vol_flags & FILE_SUPPORTS_SPARSE_FILES;
		}
		

		if (b_sparse_copy) {		// No need to check if source drive does not support
			if (!::GetFullPathName(out.c_str(), MAX_PATH, sz_path.data(), &pszName))
				throw  ved::sparse_exception(L"Error GetFullPathName!", ::GetLastError());


			_wsplitpath(
				sz_path.c_str(),
				sz_drive.data(),
				sz_dir.data(),
				sz_file_name.data(),
				sz_ext.data());

			sz_drive.pop_back(); //delete '\0'
			sz_drive+=L"\\";
			
			if (!::GetVolumeInformation(
				sz_drive.c_str(),
				sz_vol_name.data(),
				MAX_PATH,
				&dw_sn,
				&dw_max_len,
				&dw_vol_flags,
				sz_file_name.data(),
				MAX_PATH))
				throw ved::sparse_exception(L"Error GetVolumeInformation!", ::GetLastError());

			b_sparse_copy = dw_vol_flags & FILE_SUPPORTS_SPARSE_FILES;
		}

		ved::file::settings settings;
		settings.ws_file_name = in.c_str();
		settings.dw_desired_access = GENERIC_READ;
		settings.dw_flags_and_attributes = FILE_FLAG_SEQUENTIAL_SCAN;
		settings.h_template_file = nullptr;


		auto h_in_file = ved::file::create(settings);

		if (!::GetFileInformationByHandle(h_in_file.operator*(), &bhfi))
			throw ved::sparse_exception(L"Error GetFileInformationByHandle!", ::GetLastError());


		b_sparse_copy = b_sparse_copy && (bhfi.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE);	// Check if the source file is sparse


		ved::file::settings settings_write;
		settings.ws_file_name = out.c_str();
		settings.dw_desired_access = GENERIC_WRITE;
		settings.dw_creation_disposition = CREATE_ALWAYS;
		settings.dw_flags_and_attributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN;
		settings.h_template_file = nullptr;

		auto h_out_file = ved::file::create(settings);

		if (b_sparse_copy) {
			// Perform sparse-aware copy

			// Get file size
			LARGE_INTEGER i_file_size;
			i_file_size.LowPart = ::GetFileSize(h_in_file.operator*(), reinterpret_cast<LPDWORD>(&i_file_size.HighPart));
			if (i_file_size.LowPart == static_cast<DWORD>(-1)) {
				throw ved::sparse_exception(L"Error GetFileSize!", ::GetLastError());
			}

			DWORD dw_temp;
			if (!::DeviceIoControl(h_out_file.operator*(), FSCTL_SET_SPARSE, nullptr, 0, nullptr, 0, &dw_temp, nullptr))
				throw ved::sparse_exception(L"Error DeviceIoControl!(FSCTL_SET_SPARSE)", ::GetLastError());

			// Main loop querying and copying allocated area ranges
			FILE_ALLOCATED_RANGE_BUFFER queryrange;
			std::vector<FILE_ALLOCATED_RANGE_BUFFER>ranges(1024);

			DWORD nbytes, n, i;
			BOOL br;

			queryrange.FileOffset.QuadPart = 0;
			queryrange.Length = i_file_size;

			do {	// This loop executes more than once only if the file has more than 1024 allocated areas
				br = ::DeviceIoControl(h_in_file.operator*(), FSCTL_QUERY_ALLOCATED_RANGES, &queryrange, sizeof(queryrange), ranges.data(), ranges.size(), &nbytes, nullptr);

				if (!br) {
					dr = ::GetLastError();
					if (dr != ERROR_MORE_DATA)
						throw ved::sparse_exception(L"Error DeviceIoControl!(FSCTL_QUERY_ALLOCATED_RANGES)", ::GetLastError());
				}

				n = nbytes / sizeof(FILE_ALLOCATED_RANGE_BUFFER);		// Calculate the number of records returned
				for (i = 0; i < n; i++)		// Main loop
					copy_range(h_in_file.operator*(), h_out_file.operator*(), ranges[i].FileOffset, ranges[i].Length);

				// Set starting address and size for the next query
				if (!br && n > 0) {
					queryrange.FileOffset.QuadPart = ranges[n - 1].FileOffset.QuadPart + ranges[n - 1].Length.QuadPart;
					queryrange.Length.QuadPart = i_file_size.QuadPart - queryrange.FileOffset.QuadPart;
				}
			} while (!br);	// Continue loop if ERROR_MORE_DATA

			// Set end of file (required because there can be a sparse area in the end)
			dr = ::SetFilePointer(h_out_file.operator*(), i_file_size.LowPart, &i_file_size.HighPart, FILE_BEGIN);

			if (dr == INVALID_SET_FILE_POINTER) {
				throw ved::sparse_exception(L"Error SetFilePointer!", ::GetLastError());
			}

			if (!::SetEndOfFile(h_out_file.operator*()))
				throw ved::sparse_exception(L"Error SetEndOfFile!", ::GetLastError());
		}

		else {
			// Not a sparse file, or sparse copy is impossible
			std::vector<BYTE> buf(64 * 1024);
			//BYTE buf[64 * 1024];
			DWORD dw_bytes_read, dw_bytes_written;

			do {
				dw_bytes_read = h_in_file->read(buf);
				buf.resize(dw_bytes_read);
				if (dw_bytes_read) {
					dw_bytes_written = h_out_file->write(buf);

					if (dw_bytes_written < dw_bytes_read)
						throw ved::sparse_exception(L"Error ERROR_HANDLE_DISK_FULL!", ERROR_HANDLE_DISK_FULL);
				}
			} while (!buf.empty());
		}

		if (!::SetFileTime(h_out_file.operator*(), &bhfi.ftCreationTime, &bhfi.ftLastAccessTime, &bhfi.ftLastWriteTime))
			throw ved::sparse_exception(L"Error SetFileTime!", ::GetLastError());

		if (!::SetFileAttributes(out.c_str(), bhfi.dwFileAttributes & ALLOWED_ATTRIBUTES))
			throw ved::sparse_exception(L"Error SetFileAttributes!", ::GetLastError());


	}
}