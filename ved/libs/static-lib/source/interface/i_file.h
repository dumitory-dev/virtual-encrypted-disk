#pragma once
#include "classes/wrap/wrap.h"
#include "classes/exceptions/exception.h"

namespace ved {
	
	/**
	 \brief Interface for classes that use IO operations.
	*/
	class i_file
	{

		using handle = std::unique_ptr<ved::handle>;

	public:
		i_file(const i_file&) = delete;
		i_file(i_file&&) = delete;
		i_file& operator=(const i_file&) = delete;
		i_file& operator=(i_file&&) = delete;
		
		/**
			Destructor
		*/
		virtual ~i_file(void) = default;

		/**
			Pure virtual method for connecting to a device. 
			\param[in] LPOVERLAPPED Contains information used in asynchronous (or overlapped) input and output (I/O).
		*/
		virtual i_file& connect(LPOVERLAPPED = nullptr) const = 0;

		/**
			Pure virtual method for disconnecting the device.  
			\param[in] LPOVERLAPPED Contains information used in asynchronous (or overlapped) input and output (I/O).
		*/
		virtual i_file& disconnect(void) const = 0;

		/**
			A method for writing data to the device. 
			\param[in] vec_buf data
			\param[in] LPOVERLAPPED Contains information used in asynchronous (or overlapped) input and output (I/O).
		*/
		virtual DWORD write(const std::vector<BYTE>& vec_buf,
			LPOVERLAPPED lp_overlapped = {}) const
		{

			DWORD dw_return = {};

			if (!WriteFile(*this->h_,
				vec_buf.data(),
				static_cast<DWORD>(vec_buf.size()),
				&dw_return,
				lp_overlapped)) {
				if (const auto dw_err_code = ::GetLastError(); dw_err_code != ERROR_IO_PENDING)
					throw i_file_exception(L"Error WriteFile", dw_err_code);
			}

			return dw_return;
		}

		/**
			A method for reading data to the device. 
			\param[out] vec_buf data
			\param[in] LPOVERLAPPED Contains information used in asynchronous (or overlapped) input and output (I/O).
			\return Number of reads
		*/
		virtual DWORD read(std::vector<BYTE>& vec_buf,
			LPOVERLAPPED lp_overlapped = {}) const
		{

			DWORD dw_return = {};

			if (!ReadFile(*this->h_,
				vec_buf.data(),
				static_cast<DWORD>(vec_buf.size()),
				&dw_return,
				lp_overlapped)) {
				const auto dw_err_code = ::GetLastError();
				if (dw_err_code != ERROR_IO_PENDING)
					throw i_file_exception(L"Error ReadFile", dw_err_code);
			}

			return dw_return;
		}

		/**
			Return native handle
			\return native handle
		*/
		operator HANDLE(void) const noexcept { return *this->h_; }

		/**
			Return device size
			\return LARGE_INTEGER
		*/
		[[nodiscard]] virtual LARGE_INTEGER get_size(void) const = 0;

		virtual bool operator!(void) const noexcept { return !*this->h_; }

	protected:
		
		template<typename T, typename Param>
		static std::unique_ptr<i_file> create(const Param& param)
		{
			std::unique_ptr<i_file> u_ptr_return = std::make_unique<T>(param);
			u_ptr_return->create();
			return u_ptr_return;
		}

		handle h_{};

		i_file(void)
			: h_(std::make_unique<ved::handle>())
		{}

		virtual i_file& create(void) = 0;
	};

}