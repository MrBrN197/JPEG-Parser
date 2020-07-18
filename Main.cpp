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
	u64 mask = (1ULL << (numBytes * 8)) - 1;

	buffer += numBytes;
	u64 result = value & mask; 
	return result;	
}


int main() {


	u64 size1;
	u64 size2;
	u8* buffer1 = OpenFile("..\\example1.jpg", &size1);
	u8* buffer2 = OpenFile("..\\example3.jpg", &size2);

	ASSERT(buffer1);
	ASSERT(buffer2);

	bool imageStarted = false;

	int i = 0;
	u8* s_buffer = buffer2;

	u16 SOI = NextBytes(s_buffer, 2);
	ASSERT(SOI == 0xD8FF)	// SOI Start Of Image

	while(s_buffer - buffer2 < size2) {
		ASSERT(NextBytes(s_buffer, 1) == 0xFF)
		u16 segment = NextBytes(s_buffer, 1);
		switch(segment){
			//case 0xD8:
			//	if(!imageStarted){
			//		printf("Start Of Image\n");
			//		imageStarted = true;
			//	}
			//	break;
			//case 0xC0:
			//	printf("Start Of Frame (baseline DCT)\n");
			//	break;
			//case 0xC2:
			//	printf("Start Of Frame (progressive DCT)\n");
			//	break;
			//case 0xC4:
			//	printf("Define Huffman Table(s)\n");
			//	break;
			//case 0xDB:
			//	printf("Define Quantization Tables\n");
			//	break;
			//case 0xDD:
			//	printf("Define Restart Interval\n");
			//	break;
			//case 0xDA:
			//	printf("Start Of Scan\n");
			//	break;
			//// case 0xDn:
			//// 	printf("0xDn\n");
			//// 	break;
			//// case 0xEn:
			//// 	printf("0xEn\n");
			//// 	break;
			//case 0xFE:
			//	printf("Comment\n");
			//	break;
			//case 0xD9:
			//	printf("End Of Image\n");
			//	break;
			case 0xE1:{
				// NOTE: Exif does not use APPn segments other than APP1, APP2 and COM segments. However, some unknown APPn may still exist on the file structure and Exif readers should be designed to skip over them.
				u16 length = _byteswap_ushort(NextBytes(s_buffer, 2));
				length -= 4;
				printf("Metadata Segment Size: %d\n", length);

				// If Exif
				ASSERT(strcmp((const char*)s_buffer, "Exif") == 0); // next two bytes should be null so strcmp should work with s_buffer
				s_buffer += 4;
				// skip two null bytes
				ASSERT(NextBytes(s_buffer, 2) == 0x0);
					
				// Tiff Image File Header
				struct TIFHEAD {
					u16 ByteOrderIdentifier; /* Byte-order Identifier */
					u16 Version; /* TIFF version number (always 2Ah) */
					u32 IFDOffset; /* Offset of the first Image File Directory*/
				};
				TIFHEAD* tiff_header = (TIFHEAD*)s_buffer;
				s_buffer += sizeof(TIFHEAD);
				// Bytes 0-1: The byte order used within the file. Legal values are:“II”(4949.H)“MM” (4D4D.H).
				// u8* tiffStart = s_buffer; 
				// NOTE: currently only supporting big endian storage
				ASSERT(tiff_header->ByteOrderIdentifier == 0x4D4D);	// TODO: support little endian format
				ASSERT(tiff_header->Version == 0x2A00);	// signature is 0x2A, in little endian machine read 0x2A00 or swap bytes
				// NOTE: 0th IFD offset migh this might not always be 0x08
				ASSERT(tiff_header->IFDOffset == 0x08000000); // in little endian swap bytes or read 0x80000000
				// u8* ifd = tiffStart + 8;

				// Reading an Image File Directory 
				struct TIFTAG {
					u16 TagId; /* The tag identifier */
					u16 DataType; /* The scalar type of the data items */
					u32 DataCount; /* The number of items in the tag data */
					u32 DataOffset; /* The byte offset to the data items */
				};
				struct TIFIFD {
					u16 NumDirEntries; /* Number of Tags in IFD */
					TIFTAG* TagList; /* Array of Tags (NumDirEntries number of them) */
					u32 NextIFDOffset; /* Offset to next IFD */
				};
				TIFIFD ifd = {};
				ifd.NumDirEntries = _byteswap_ushort(NextBytes(s_buffer, 2));
				ifd.TagList = (TIFTAG*)s_buffer;
				s_buffer += (sizeof(TIFTAG) * ifd.NumDirEntries);	// skip TIFTAGS
				ifd.NextIFDOffset = _byteswap_ulong(NextBytes(s_buffer, 4));
				ASSERT(false);

#define SWAP_STRUCT_ENTRY(entry, mode) entry = _byteswap_##mode(entry); 

				for(int i = 0; i < ifd.NumDirEntries; i++){
					TIFTAG* entry = (ifd.TagList + i);
					SWAP_STRUCT_ENTRY(entry->TagId, ushort);
					SWAP_STRUCT_ENTRY(entry->DataType, ushort);
					SWAP_STRUCT_ENTRY(entry->DataCount, ulong);
					SWAP_STRUCT_ENTRY(entry->DataOffset, ulong);

					ASSERT(entry->DataType > 0 && entry->DataType <= 5)		// NOTE: with TIFF 6.0 can go upto 12
					ASSERT(entry->DataOffset < size2);
				}

				// Next IFD
				u32 nextIFD = _byteswap_ulong(NextBytes(s_buffer, 4));

				ASSERT(false);

				u32 IFD0 = _byteswap_ulong(NextBytes(s_buffer, 4));	// main image

				u64 IFD1_link = _byteswap_ulong(NextBytes(s_buffer, 8));		// link to ifd1

				u64 data_area_IDF0 = _byteswap_ulong(NextBytes(s_buffer, 8));

				ASSERT(false);

				u32 ifd_offset = _byteswap_ulong(NextBytes(s_buffer, 4));
				s_buffer += 4;

				u16 numDirEntries = _byteswap_ulong(NextBytes(s_buffer, 2));
				s_buffer += 2;
				ASSERT(numDirEntries);	// must have alteast one entry


				// u32 second = *(u32*)s_buffer;
				// s_buffer += 4;
				break;
			}
			default:
				ASSERT(false);
				// ASSERT_LOG(false, "Unknown Segment: 0x%x At Index %d\n", seg, i);
				break;
		}
	}


	u8* x = new u8[256];
	for(int i = 0; i < 256; i++){
		*(x+i) = i;
	}

	return 1;
}

