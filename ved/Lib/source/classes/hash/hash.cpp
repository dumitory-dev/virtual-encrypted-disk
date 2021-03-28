#include "hash.h"
#include "../../classes/exceptions/exception.h"
namespace ved
{

	std::string hash::get_hash(const BYTE* data, DWORD size) const 
	{
		HCRYPTPROV h_prov{};
		HCRYPTPROV h_hash{};

		if (!::CryptAcquireContext(&h_prov,
		                           nullptr,
		                           nullptr,
		                           PROV_RSA_AES,
		                           CRYPT_VERIFYCONTEXT))
		{
			throw ved::c_hash_exception(L"Error CryptAcquireContext", GetLastError());
		}

		handle uptrh_prov(reinterpret_cast<void*>(h_prov), f_destroy_context_);

		if (!::CryptCreateHash(h_prov, static_cast<unsigned>(this->type_), 0, 0, &h_hash))
		{
			throw ved::c_hash_exception(L"Error CryptCreateHash", GetLastError());
		}

		handle uptrh_hash = {reinterpret_cast<void*>(h_hash), f_destroy_hash_};


		if (!::CryptHashData(h_hash,
		                     data,
		                     size,
		                     0))
		{
			throw ved::c_hash_exception(L"Error CryptHashData", GetLastError());
		}

		DWORD dw_hash_size{};
		DWORD dw_size_dw = sizeof(DWORD);


		if (!::CryptGetHashParam(h_hash,
		                         HP_HASHSIZE,
		                         reinterpret_cast<BYTE*>(&dw_hash_size),
		                         &dw_size_dw,
		                         0))
		{
			throw ved::c_hash_exception(L"Error CryptGetHashParam", GetLastError());
		}


		std::vector<BYTE> v_data(dw_hash_size);

		if (!::CryptGetHashParam(h_hash,
		                         HP_HASHVAL,
		                         reinterpret_cast<BYTE*>(&v_data[0]),
		                         &dw_hash_size,
		                         0))
		{
			throw ved::c_hash_exception(L"Error CryptGetHashParam", GetLastError());
		}


		std::ostringstream oss{};

		for (auto iter = v_data.begin(); iter != v_data.end(); ++iter)
		{
			
			oss.fill('0');
			oss.width(2);

			oss << std::hex << static_cast<const int>(*iter);
		}
				

		return oss.str();
	}

	
}