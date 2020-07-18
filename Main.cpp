#define VERSION_MAJOR 3
#define VERSION_MINOR 1
#define VERSION_PATCH 2

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>


// void LogError(const char* s, ...) {
// 	va_list vl;
// 	va_start(vl, s);
// 	vprintf(s, vl);
// 	va_end(vl);

// }


#if defined _MSC_VER
	#define ASSERT(x) if(!(x)) { __debugbreak();}
	#define ASSERT_LOG(x, ...) if(!(x)) { printf(__VA_ARGS__); __debugbreak(); }
	#include <intrin.h>
#else
	#define ASSERT(x) if(!(x)) { *((int*)0) = 0; }
#endif

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;


u8* OpenFile(const char* filename, u64* file_size) {
	FILE* fp = fopen(filename, "rb");
	if(!fp){
		*file_size = 0;
		printf("Failed To Open File\n");
		return nullptr;
	}

	fseek(fp, NULL, SEEK_END);
	u32 size = ftell(fp);
	*file_size = size;
	u8* buffer = new u8[size];
	fseek(fp, 0, SEEK_SET);
	fread(buffer, 1, size, fp);

	fclose(fp);
	return buffer;
}


u64 NextBytes(u8*& buffer, u8 numBytes){
	ASSERT(numBytes == 1 || numBytes == 2 || numBytes == 4 || numBytes == 8)

	// only works on little endian machine
	u64 value = *(u64*)buffer;
	u64 mask = 1ULL << (numBytes * 8);

	buffer += numBytes;
	u64 result = value & mask; 
	return result;	
}


int main() {

	FILE* out_file = fopen("../endianness.txt", "wb");
	int y = 0x1;
	fwrite(&y, 4, 1, out_file);
	fclose(out_file);

	u64 size;
	u8* in_file = OpenFile("../endianness.txt", &size);
	for(int i = 0; i < size; i++){
		u8 x = *(in_file + i);
	}

	return 1;


	u64 size1;
	u64 size2;
	u8* buffer1 = OpenFile("..\\example1.jpg", &size1);
	u8* buffer2 = OpenFile("..\\example3.jpg", &size2);

	ASSERT(buffer1);
	ASSERT(buffer2);

	bool imageStarted = false;

	int i = 0;
	u8* s_buffer = buffer2;
	while(s_buffer - buffer2 < size2) {
		if(*s_buffer == 0xFF){
			s_buffer++;
			u8 seg = *(s_buffer);
			s_buffer++;
			switch(seg){
				case 0xD8:
					if(!imageStarted){
						printf("Start Of Image\n");
						imageStarted = true;
					}
					break;
				case 0xC0:
					printf("Start Of Frame (baseline DCT)\n");
					break;
				case 0xC2:
					printf("Start Of Frame (progressive DCT)\n");
					break;
				case 0xC4:
					printf("Define Huffman Table(s)\n");
					break;
				case 0xDB:
					printf("Define Quantization Tables\n");
					break;
				case 0xDD:
					printf("Define Restart Interval\n");
					break;
				case 0xDA:
					printf("Start Of Scan\n");
					break;
				// case 0xDn:
				// 	printf("0xDn\n");
				// 	break;
				// case 0xEn:
				// 	printf("0xEn\n");
				// 	break;
				case 0xFE:
					printf("Comment\n");
					break;
				case 0xD9:
					printf("End Of Image\n");
					break;
				case 0xE1:{
					u32 size = *(u16*)(s_buffer) - 4;	// size minus (JPEG SOI + APPDATA1) MARKER SIZE = 4 
					s_buffer+=2;
					printf("Metadata Segment Size: %d\n", size);


					// If Exif
					ASSERT(strcmp((const char*)NextBytes(s_buffer, 4), "Exif") == 0); // next two bytes should be null so strcmp should work with s_buffer
					// skip two null bytes
					ASSERT(*(u16*)NextBytes(s_buffer, 2) == 0x0);
					
					// Tiff Header
					// Bytes 0-1: The byte order used within the file. Legal values are:“II”(4949.H)“MM” (4D4D.H).
					// u16 byte_order = *(u16*)s_buffer;
					// s_buffer += 2;
					u16 byte_order = NextBytes(s_buffer, 2);

					ASSERT(byte_order == 0x4D4D || byte_order == 0x4949);
					if(byte_order == 0x4D4D){
						// Big Endian
						u16 signature = *(u16*)NextBytes(s_buffer, 2);
						ASSERT(_byteswap_ushort(signature) == 42);

						ASSERT(_byteswap_ulong(*(u32*)NextBytes(s_buffer, 4)) == 0x80);

						u32 IFD0 = _byteswap_ulong(*(u32*)NextBytes(s_buffer, 4));	// main image

						u64 IFD1_link = _byteswap_uint64(*(u64*)NextBytes(s_buffer, 8));		// link to ifd1

						u64 data_area_IDF0 = _byteswap_uint64(NextBytes(s_buffer, 8));

						ASSERT(false);

						u32 ifd_offset = _byteswap_ulong(*(u32*)s_buffer);
						s_buffer += 4;

						u16 numDirEntries = _byteswap_ushort(*(u16*)s_buffer);
						s_buffer += 2;
						ASSERT(numDirEntries);	// must have alteast one entry
						
						for(int i = 0; i < numDirEntries; i++){
							struct IFDEntry {
								u16 tag;
								u16 fieldType;
								u32 count;
								u32 valueOffset;
							};
							IFDEntry* entry = (IFDEntry*)s_buffer;
							ASSERT(entry->valueOffset < size2);
							s_buffer += sizeof(IFDEntry);
						}
					}else {
						// Little Endian
						ASSERT(false);
					}

					// u32 second = *(u32*)s_buffer;
					// s_buffer += 4;
					break;
				}
				default:
					// ASSERT_LOG(false, "Unknown Segment: 0x%x At Index %d\n", seg, i);
					break;
			}
		}
	}


	u8* x = new u8[256];
	for(int i = 0; i < 256; i++){
		*(x+i) = i;
	}

	return 1;
}

