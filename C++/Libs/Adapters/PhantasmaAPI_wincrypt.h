#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Include crypto API adaptors immediately after including PhantasmaAPI.h"
#endif 
//------------------------------------------------------------------------------
// This header supplies the Phantasma API with memory protection features  
//  provided by the wincrypt library.
//------------------------------------------------------------------------------

#include <wincrypt.h>
#pragma comment(lib, "crypt32.lib")

namespace phantasma { 

	//Make sure that allocations used here have been aligned to PHANTASMA_SECURE_ALLOC_ALIGNMENT!
	inline bool Phantasma_EncryptMemory(void* memory, int length)
	{
		int padded = ((length + CRYPTPROTECTMEMORY_BLOCK_SIZE - 1) / CRYPTPROTECTMEMORY_BLOCK_SIZE)*CRYPTPROTECTMEMORY_BLOCK_SIZE;
		return FALSE != CryptProtectMemory(memory, (DWORD)padded, CRYPTPROTECTMEMORY_SAME_PROCESS);
	}
	inline bool Phantasma_DecryptMemory(void* memory, int length)
	{
		int padded = ((length + CRYPTPROTECTMEMORY_BLOCK_SIZE - 1) / CRYPTPROTECTMEMORY_BLOCK_SIZE)*CRYPTPROTECTMEMORY_BLOCK_SIZE;
		return FALSE != CryptUnprotectMemory(memory, (DWORD)padded, CRYPTPROTECTMEMORY_SAME_PROCESS);
	}
	
	inline bool Phantasma_EncryptSessionData(phantasma::ByteArray& result, const phantasma::Byte* message, int messageLength, const phantasma::Byte* password=0, int passwordLength=0)
	{
		if( !message || messageLength <= 0 )
			return false;
		DATA_BLOB input{ (DWORD)messageLength, (BYTE*)message };
		DATA_BLOB entropy{ (DWORD)passwordLength, (BYTE*)password };
		DATA_BLOB output;
		BOOL ok = CryptProtectData(&input, 0, password && passwordLength > 0 ? &entropy : 0, 0, 0, 0, &output);
		if( ok )
		{
			result.resize(output.cbData);
			if( output.cbData )
				PHANTASMA_COPY(output.pbData, output.pbData+output.cbData, (BYTE*)&result.front());
			LocalFree(output.pbData);
			return true;
		}
		return false;
	}

	template<class SecureByteArray>
	bool Phantasma_DecryptSessionData(SecureByteArray& result, const phantasma::Byte* message, int messageLength, const phantasma::Byte* password=0, int passwordLength=0)
	{
		if( !message || messageLength <= 0 )
			return false;
		DATA_BLOB input{ (DWORD)messageLength, (BYTE*)message };
		DATA_BLOB entropy{ (DWORD)passwordLength, (BYTE*)password };
		DATA_BLOB output;
		BOOL ok = CryptUnprotectData(&input, 0, password && passwordLength > 0 ? &entropy : 0, 0, 0, 0, &output);
		if( ok )
		{
			result = SecureByteArray((int)output.cbData, (phantasma::Byte*)output.pbData);
			LocalFree(output.pbData);
			return true;
		}
		return false;
	}

#define PHANTASMA_SECURE_ALLOC_ALIGNMENT     CRYPTPROTECTMEMORY_BLOCK_SIZE
#define PHANTASMA_SECURE_ENCRYPT_MEMORY(p,s) phantasma::Phantasma_EncryptMemory(p,s)
#define PHANTASMA_SECURE_DECRYPT_MEMORY(p,s) phantasma::Phantasma_DecryptMemory(p,s)

#define PHANTASMA_ENCRYPT_SESSION_DATA(r,m,ml,p,pl)	phantasma::Phantasma_EncryptSessionData(r,m,ml,p,pl)
#define PHANTASMA_DECRYPT_SESSION_DATA(r,m,ml,p,pl)	phantasma::Phantasma_DecryptSessionData(r,m,ml,p,pl)

#ifndef PHANTASMA_WIPEMEM
# define PHANTASMA_WIPEMEM(buffer, size)     SecureZeroMemory(buffer, size)
#endif
}
