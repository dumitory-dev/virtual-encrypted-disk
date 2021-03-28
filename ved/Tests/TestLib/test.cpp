#include "pch.h"
#include "classes/hash/hash.h"
#include "classes/driver_disk/driver_disk.h"

namespace hash {

	std::vector<BYTE> data{ 'm','a','m','a' };

	TEST(TestLibHash, TestMd5) {

		const std::string test_hash = "eeafbf4d9b3957b139da7b7f2e7f2d4a";
		ved::hash hash_(ved::hash::type_hash::MD5);
		EXPECT_EQ(hash_.get_hash(data), test_hash);
	}

	TEST(TestLibHash, TestSHA256) {

		const std::string test_hash = "ccb711f092ac8ef1805b5045fab7e8a6189cb97ad04565e21b5fbcfc9e542e42";
		ved::hash hash_(ved::hash::type_hash::SHA256);
		EXPECT_EQ(hash_.get_hash(data), test_hash);
	}

	TEST(TestLibHash, TestSHA1) {

		const std::string test_hash = "99df988b77e60a1718e9e6fecdaf22552047be28";
		ved::hash hash_(ved::hash::type_hash::SHA1);
		EXPECT_EQ(hash_.get_hash(data), test_hash);
	}
}


namespace input
{

	std::string password = "0000000000000000";
	std::wstring path = L"empty";
	LARGE_INTEGER size;
	Crypt mode = Crypt::RC4;
	WCHAR letter = L'C';
   
	
	TEST(TestLibInput, TestPassword) {
			
		size.QuadPart = 100;
		EXPECT_THROW(ved::driver_disk::make_file_info(path,size,letter,"1",mode),std::invalid_argument);
	}

	TEST(TestLibInput, TestPath) {
			
		size.QuadPart = 100;
		EXPECT_THROW(ved::driver_disk::make_file_info(L"",size,letter,password,mode),std::invalid_argument);
	}

	TEST(TestLibInput, TestSize) {
			
		size.QuadPart = 9;
		EXPECT_THROW(ved::driver_disk::make_file_info(path,size,letter,password,mode),std::invalid_argument);
	}

	TEST(TestLibInput, TestLetter) {
			
		size.QuadPart = 100;
		letter = '1';
		EXPECT_THROW(ved::driver_disk::make_file_info(path,size,letter,password,mode),std::invalid_argument);
	}

}