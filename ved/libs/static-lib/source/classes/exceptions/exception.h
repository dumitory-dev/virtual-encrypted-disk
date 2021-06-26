#pragma once
#include "../../../global_headers/pch.h"
#include "winternl.h"
namespace ved
{

	class c_exception
	{

	public:

		c_exception(const c_exception&) = default;
		c_exception(c_exception&&) = default;
		c_exception& operator = (const c_exception&) = default;
		c_exception& operator = (c_exception&&) = default;

	protected:

		std::wstring m_wsMessage{};
		using ULONG = unsigned long;
		mutable ULONG m_ulCode{};

	public:

		explicit c_exception(const std::wstring& ws_message) : c_exception(ws_message, {}) { }

		explicit c_exception(const ULONG ul_code) : c_exception({}, ul_code) { }

		explicit c_exception(const std::wstring& ws_message,
			ULONG ulCode) : m_wsMessage(ws_message), m_ulCode(ulCode) { }

		virtual ~c_exception(void) = default;

		virtual std::wstring GetMessage(void) const
		{

			return this->m_wsMessage;

		}

		auto get_code(void) const noexcept
		{

			return this->m_ulCode;

		}

	};

	

	class c_win_api_exception : public c_exception
	{

	public:

		using c_exception::c_exception;

		std::wstring GetMessage(void) const override
		{
						
			if (!c_exception::m_ulCode)
				return c_exception::m_wsMessage;

			LPVOID lp_msg_buf{};

			if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				{},
				c_exception::m_ulCode,
				MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
				reinterpret_cast<LPWSTR>(&lp_msg_buf),
				{},
				{}))
				return c_exception::m_wsMessage;

			std::wostringstream wos{};
			wos << c_exception::m_wsMessage << std::endl;
			wos << L"Error code " << c_exception::m_ulCode << std::endl;
			wos << reinterpret_cast<LPCWSTR>(lp_msg_buf);

			::LocalFree(lp_msg_buf);

			return wos.str();

		}

	};


	class c_command_line_argument_exception final : public c_win_api_exception
	{

	public:

		using c_win_api_exception::c_win_api_exception;

	};

	class c_hash_exception final : public c_win_api_exception
	{

	public:

		using c_win_api_exception::c_win_api_exception;

	};

	
	class handle_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;

	};


	class map_file_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;

	};


	class i_file_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;

	};


	class sparse_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;

	};

	class file_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;

	};
		
	class service_exception : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;

	};


	class driver_exception : public c_win_api_exception
	{
	public:
		using c_win_api_exception::c_win_api_exception;


		std::wstring GetMessage(void) const override
		{
						
			if (!c_exception::m_ulCode)
				return c_exception::m_wsMessage;

		
			LPVOID lp_msg_buf{};

			if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				{},
				c_exception::m_ulCode,
				MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
				reinterpret_cast<LPWSTR>(&lp_msg_buf),
				{},
				{}))
			{
					
					c_exception::m_ulCode = driver_exception::convert_nt_status_to_win32_error(c_exception::m_ulCode);

					if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                               {},
                               c_exception::m_ulCode,
                               MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                               reinterpret_cast<LPWSTR>(&lp_msg_buf),
                               {},
                               {}))
					{
						return c_exception::m_wsMessage;
					}

			}
				

			std::wostringstream wos{};
			wos << c_exception::m_wsMessage << std::endl;
			wos << L"Error code " << c_exception::m_ulCode << std::endl;
			wos << reinterpret_cast<LPCWSTR>(lp_msg_buf);

			::LocalFree(lp_msg_buf);

			return wos.str();

		}
		
		static DWORD convert_nt_status_to_win32_error(NTSTATUS nt_status)
		{
#pragma warning(push)

#pragma warning(disable:4005)
				return RtlNtStatusToDosError(nt_status);
#pragma warning(pop)
		
		}
	};

}