#pragma once
#ifdef DRIVER_PROJECT
#include <ntifs.h>
#endif

#include <Bcrypt.h>
#define BLOCK_SIZE 16

typedef __int32 uint32_t ;

typedef unsigned char BYTE;

void Rc4Crypt(PCHAR key,size_t len,PCHAR data,size_t data_len);

NTSTATUS DecryptEncrypt(UCHAR * puDataPassword, size_t uSizePassword, UCHAR * puData, size_t uSizeData, BOOLEAN bIsEncrypt);
