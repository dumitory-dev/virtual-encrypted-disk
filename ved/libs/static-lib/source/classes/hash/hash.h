#pragma once
#include "../exceptions/Exception.h"

namespace ved
{
	class hash
	{
		
	public:

		hash(void) = delete;
		hash(const hash& other) = delete;
		hash(hash&& other) = delete;
		hash & operator=(const hash& other) = delete;
		hash & operator=(hash&& other) = delete;
		
		enum class type_hash { MD5 = (ALG_CLASS_HASH | ALG_TYPE_ANY | 3), SHA1 = (ALG_CLASS_HASH | ALG_TYPE_ANY | 4), SHA256 = (ALG_CLASS_HASH | ALG_TYPE_ANY | 12) };

		[[nodiscard]] std::string get_hash(const std::vector<byte>& buf) const 
		{
			if (buf.empty())
				throw ved::c_hash_exception(L"Data bytes is empty!(GetHash)");

			return this->get_hash_from_byte(buf.data(), static_cast<DWORD>(buf.size()));
		}
			

		explicit hash(const type_hash & type) :type_(type)
		{

		}

	
		
	private:
		type_hash type_{};
		
		using handle = std::unique_ptr<void, std::function<void(void *ptr)>>;

		const std::function<void(void *)> f_destroy_hash_ = [](void * p)
		{
			if (p)
				::CryptDestroyHash(reinterpret_cast<HCRYPTPROV>(p));

		};

		const std::function<void(void *)> f_destroy_context_ = [](void * p)
		{
			if (p)
				::CryptReleaseContext(reinterpret_cast<HCRYPTPROV>(p), 0);

		};

		std::string get_hash(const BYTE* data, DWORD size) const;
			
		std::string get_hash_from_byte(const BYTE * data, DWORD size) const
		{
			return this->get_hash(data, size);
		}
	};

}
