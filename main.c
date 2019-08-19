#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define NCM_FILE_NAME  "1.ncm"

int filesize(FILE *stream)
{
	int curpos,length;
	curpos = ftell(stream);
	printf("curpos = %d\n", curpos);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	length -= curpos; //minus header offset
	fseek(stream, curpos, SEEK_SET);
	return length;
}

uint32_t isNcmFIle(FILE *ncmFile)
{
	uint8_t header[4] = {0};
	uint32_t br = 0;
	uint32_t dataSize = 0;
 
	br = fread(header, 1, 4, ncmFile);
	if(br != 4)
	{
		printf("ncm file error\n");
		return 0;
	}
	else
	{
		printf("%#x,%#x,%#x,%#x\n", header[0], header[1], header[2], header[3]);
	}	

	br = fread(header, 1, 4, ncmFile);
	if(br != 4)
	{
		printf("ncm file error\n");
		return 0;
	}
	else
	{
		printf("%#x,%#x,%#x,%#x\n", header[0], header[1], header[2], header[3]);
	}

	br = fread(header, 1, 2, ncmFile);
	if(br != 2)
	{
		printf("ncm file error\n");
		return 0;
	}

	dataSize = filesize(ncmFile);
	if(dataSize == 0)
	{
		printf("ncm file error\n");
		return 0;
	}
	else
	{	
		printf("dataSize = %d\n", dataSize);
		return dataSize;
	}
}

int main(int argc, char const *argv[])
{
	/* code */
	FILE *ncmFile = NULL;
	uint8_t header[4] = {0};
	uint32_t br = 0;
	uint32_t dataSize = 0;
	uint8_t *dataBuffer = NULL;
 
	ncmFile = fopen(NCM_FILE_NAME, "rb");

	if(!ncmFile)
	{
		printf("open ncm file fail\n");
		return -1;
	}

	dataSize = isNcmFIle(ncmFile);
	if(!dataSize)
	{
		printf("is not valid ncm file\n");
		return -1;
	}

	dataBuffer = malloc(dataSize);
	if (dataBuffer == NULL)
	{
		printf("RAM error\n");
		return -1;
	}

	br = fread(dataBuffer, 1, dataSize, ncmFile);
	if (br != dataSize)
	{
		printf("read file error\n");
		goto end;
	}

	for (int i = 0; i < dataSize; ++i)
	{
		dataBuffer[i] ^= 0x64;
	}

	//todo: aes decode

end:
	if(dataBuffer)
	{
		free(dataBuffer);
		dataBuffer = NULL;
	}

	return 0;
}