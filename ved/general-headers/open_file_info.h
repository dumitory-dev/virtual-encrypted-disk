#pragma once
#include "constant.h"
#include <minwindef.h>

enum Crypt
{
	CryptPrev = 0,
	RC4,
	AES
};

typedef struct _OPEN_FILE_INFORMATION {
	LARGE_INTEGER	FileSize;
	WCHAR			DriveLetter;
	USHORT			FileNameLength;
	USHORT			PasswordLength;
	CHAR			Password[MAX_PASSWORD_SIZE];
	WCHAR			FileName[MAX_PATH];
	enum Crypt			CryptMode;
}OPEN_FILE_INFORMATION, *POPEN_FILE_INFORMATION;