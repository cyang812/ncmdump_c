#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"


#define MAKEWORD(b3, b2, b1, b0) (((uint32_t)((uint8_t)(b3) << 24)) | ((uint8_t)(b2) << 16) | ((uint8_t)(b1) << 8) | ((uint8_t)(b0)))

#define NCM_FILE_NAME  "1.ncm"

//for debug
int dumpToFile(const char *name, const uint8_t *data, uint32_t dataSize)
{
	FILE *file = NULL;
	file = fopen(name, "wb");
	if (!file)
	{
		printf("can't create %s file\n", name);
		return -1;
	}

	fwrite(data, 1, dataSize, file);

	fclose(file);

	return 1;
}

uint32_t isNcmFIle(FILE *ncmFile)
{
	uint8_t header[4] = {0};
	uint32_t br = 0;
	uint32_t keyLength = 0;
 
	br = fread(header, 1, 4, ncmFile);
	if(br != 4 && (MAKEWORD(header[0], header[1], header[2], header[3]) != 0x4354454E))
	{
		printf("ncm file error\n");
		printf("%#x\n", MAKEWORD(header[0], header[1], header[2], header[3]));
		return 0;
	}

	br = fread(header, 1, 4, ncmFile);
	if(br != 4 && (MAKEWORD(header[0], header[1], header[2], header[3]) != 0x4644414D))
	{
		printf("ncm file error\n");
		printf("%#x\n", MAKEWORD(header[0], header[1], header[2], header[3]));
		return 0;
	}

	br = fread(header, 1, 2, ncmFile);
	if(br != 2)
	{
		printf("ncm file error\n");
		return 0;
	}

	br = fread(header, 1, 4, ncmFile);
	if(br != 4)
	{
		printf("ncm file error\n");
		return 0;
	}
	else
	{
		keyLength = MAKEWORD(header[3], header[2], header[1], header[0]);//little-endian
		printf("keyLength = %d\n", keyLength); 
	}

	return keyLength;
}

int main(int argc, char const *argv[])
{
	/* code */
	FILE *ncmFile = NULL;
	uint8_t header[4] = {0};
	uint32_t br = 0;
	uint32_t keyLength = 0;
	uint8_t rawDataBuffer[128] = {0};
	uint8_t decDataBuffer[128] = {0};
 
	ncmFile = fopen(NCM_FILE_NAME, "rb");

	if(!ncmFile)
	{
		printf("open ncm file fail\n");
		goto end;
	}

	keyLength = isNcmFIle(ncmFile);

	br = fread(rawDataBuffer, 1, keyLength, ncmFile);
	if (br != keyLength)
	{
		printf("read file error\n");
		goto end;
	}

	// dumpToFile("rawData", rawDataBuffer, 128);

	for (int i = 0; i < keyLength; ++i)
	{
		rawDataBuffer[i] ^= 0x64;
	}

	//todo: aes128 decode

	// dumpToFile("rawData", rawDataBuffer, 128);
	struct AES_ctx ctx;
	uint8_t key[16] = {0x68, 0x7A, 0x48, 0x52, 0x41, 0x6D, 0x73, 0x6F, 0x35, 0x6B, 0x49, 0x6E, 0x62, 0x61, 0x78, 0x57};
    AES_init_ctx(&ctx, key);

    for (int i = 0; i < (keyLength >> 4); ++i)
    {
	    AES_ECB_decrypt(&ctx, (uint8_t *)&rawDataBuffer[i*16]);	
    }

	memcpy(decDataBuffer, rawDataBuffer, keyLength);
    // dumpToFile("decData", decDataBuffer, 128);

    //todo: buildKeyBox



end:
	fclose(ncmFile);

	return 0;
}