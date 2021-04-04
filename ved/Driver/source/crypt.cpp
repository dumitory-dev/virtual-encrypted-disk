#include "../headers/crypt.h"
#include "../headers/constants.h"


void Rc4Crypt(PCHAR key,size_t len,PCHAR data,size_t data_len)
{
	char S[256];
    unsigned int i,j;

	if(!len) len=strlen((char*)key);
	for(i=0; i<256; i++) S[i]=(CHAR)i;
	for(i=j=0; i<256; i++) {
		j=(j+S[i]+key[i%len])&255;
		const char t=S[i];
		S[i]=S[j];
		S[j]=t;
	}
	i=j=0;


	size_t n;
	for(n=0; n<data_len; n++) {

		i=(i+1)&255; j=(j+S[i])&255;

		const char t=S[i];
		S[i]=S[j];
		S[j]=t;
					
		data[n]^=(int)S[(S[i]+S[j])&255];;
		
	}
	
	
}


NTSTATUS DecryptEncrypt(UCHAR * puDataPassword, size_t uSizePassword, UCHAR * puData, size_t uSizeData, BOOLEAN bIsEncrypt)
{
	
	PUCHAR puOrigNonce = NULL;
	BCRYPT_KEY_HANDLE keyHandle = NULL;
	PUCHAR puAuthTag = NULL;

	NTSTATUS status = STATUS_SUCCESS;
	unsigned long bytesDone = 0;
	BCRYPT_ALG_HANDLE algHandle = 0;
	status = BCryptOpenAlgorithmProvider(&algHandle, BCRYPT_AES_ALGORITHM, 0, 0);
	
	if (!BCRYPT_SUCCESS(status))
	{
		goto Clean;
	}
	
	status = BCryptSetProperty(algHandle, BCRYPT_CHAINING_MODE, (BYTE*)BCRYPT_CHAIN_MODE_GCM, sizeof(BCRYPT_CHAIN_MODE_GCM), 0);
	if (!BCRYPT_SUCCESS(status))
	{
		goto Clean;
	}

	BCRYPT_AUTH_TAG_LENGTHS_STRUCT authTagLengths;
	status = BCryptGetProperty(algHandle, BCRYPT_AUTH_TAG_LENGTH, (BYTE*)& authTagLengths, sizeof(authTagLengths), &bytesDone, 0);
	if (!BCRYPT_SUCCESS(status))
	{
		goto Clean;
	}

	unsigned long blockLength = 0;
	status = BCryptGetProperty(algHandle, BCRYPT_BLOCK_LENGTH, (BYTE*)& blockLength, sizeof(blockLength), &bytesDone, 0);
	if (!BCRYPT_SUCCESS(status))
	{
		goto Clean;
	}
	
	status = BCryptGenerateSymmetricKey(algHandle, &keyHandle, 0, 0, &puDataPassword[0], uSizePassword, 0);
	if (!BCRYPT_SUCCESS(status))
	{
		goto Clean;
	}

	const size_t GCM_NONCE_SIZE = 12;

	puOrigNonce = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, GCM_NONCE_SIZE, 'ppc_');
	
	if (puOrigNonce == NULL)
	{
		status = STATUS_ALLOCATE_BUCKET;
		goto Clean;
	}

	
	for (ULONG i = 0; i < GCM_NONCE_SIZE; ++i)
	{
		puOrigNonce[i] = (CHAR)i;
		puOrigNonce[i] ^= puDataPassword[i];
	}
	
	// Encrypt data as a whole

	puAuthTag = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, authTagLengths.dwMinLength, 'ppc_');
	if (puAuthTag == NULL)
	{
		status = STATUS_ALLOCATE_BUCKET;
		
		goto Clean;
	}

	const unsigned long partSize = uSizeData;

	BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO authInfo;
	BCRYPT_INIT_AUTH_MODE_INFO(authInfo);
	authInfo.pbNonce = &puOrigNonce[0];
	authInfo.cbNonce = GCM_NONCE_SIZE;
	authInfo.pbTag = &puAuthTag[0];
	authInfo.cbTag = authTagLengths.dwMinLength;
	
	if (bIsEncrypt)
	{
		status = BCryptEncrypt
		(
			keyHandle,
			&puData[0], partSize,
			&authInfo,
			NULL, 0,
			&puData[0], partSize,
			&bytesDone, 0
		);
				
	}
	else
	{

		PUCHAR puMacContext = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, authTagLengths.dwMaxLength, 'ppc_');

		if (puMacContext == NULL)
		{
			status = STATUS_ALLOCATE_BUCKET;
			goto Clean;
		}
				
		PUCHAR puContextIV = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, blockLength, 'ppc_');

		if (puContextIV == NULL)
		{
			ExFreePool(puMacContext);
			status = STATUS_ALLOCATE_BUCKET;
			goto Clean;
			
		}

		authInfo.cbMacContext = authTagLengths.dwMaxLength;
		authInfo.pbMacContext = puMacContext;
		authInfo.dwFlags = BCRYPT_AUTH_MODE_CHAIN_CALLS_FLAG;

#pragma warning(push)

#pragma warning(disable:28193)
		status = BCryptDecrypt
		(
			keyHandle,
			&puData[0], partSize,
			&authInfo,
			&puContextIV[0], blockLength,
			&puData[0], partSize,
			&bytesDone, 0
		);

#pragma warning(pop)
		

		ExFreePool(puMacContext);
		ExFreePool(puContextIV);

	}
	
	if (bytesDone != partSize)
	{
		status = STATUS_CRYPTO_SYSTEM_INVALID;
	}

Clean:
	
	if (puOrigNonce != NULL)
	{
		ExFreePool(puOrigNonce);
	}

	if (puAuthTag != NULL)
	{
		ExFreePool(puAuthTag);
	}
	
	if (keyHandle != NULL)
	{
		BCryptDestroyKey(keyHandle);
	}
	
	if (algHandle != NULL)
	{
		BCryptCloseAlgorithmProvider(algHandle, 0);
	}
	
	return status;
}


