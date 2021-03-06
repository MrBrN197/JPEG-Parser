
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


#include "core.h"
#include "Huffman.h"


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


u8 NextByte(u8*& buffer){
	return *buffer++;
}

u64 NextBytes(u8*& buffer, u8 numBytes){
	ASSERT(numBytes == 1 || numBytes == 2 || numBytes == 4 || numBytes == 8)

	// NOTE: reads bytes in little endian
	u64 value = *(u64*)buffer;
	u64 mask = (1ULL << (numBytes * 8)) - 1;

	buffer += numBytes;
	u64 result = value & mask; 
	return result;	
}

u64 GetBits(u8* buffer, u64 bit_pos, u8 bits) {
	ASSERT(bits > 0 && bits <= 32);
	u32 idx = (u32)(bit_pos >> 5);
	u32 offset = bit_pos & 31;
	u8* buffer_offset = (u8*)((u32*)buffer + idx);
	u8 b1 = NextByte(buffer_offset);
	u8 b2 = NextByte(buffer_offset);
	u8 b3 = NextByte(buffer_offset);
	u8 b4 = NextByte(buffer_offset);
	u32 value = b1 << 24 | b2 << 16 | b3 << 8 | b4;
	value = value >> (32 - (offset + bits));
	value &= (1ULL << bits) - 1;
	return value;
}

i16 DecodeValueCategory(i16 value, u8 bits) {
	ASSERT(bits < 16);
	ASSERT( ((value >> 15) & 1) == 0 );
	ASSERT((((1 << bits) - 1) & value) == value);

	u16 is_neg = (value >> (bits - 1)) & 1;

	if(!is_neg){
		// 16384
		ASSERT(value >= 0);
		ASSERT(value < 16384);
		// value = value & ((1 << bits) - 1);
		return (i16)(0xffff << bits) + value + 1;
		// i16 start = -(1 << bits) + 1;
		// return start + value;
	}
	return value;
}

u64 GetHuffmanValue(Table* table, u8* scan_data, u32& bit_offset) {
	while(!table->isValue){
		u8 bit_value = (u8)GetBits(scan_data, bit_offset++, 1);
		ASSERT(bit_value == 0 || bit_value == 1)
		table = (bit_value) ? table->right : table->left;
		ASSERT(table)
	}
	return table->value;
}

u32 GetByteSizeFromCountAndType(u16 size, u16 type) {
	u8 type_size;
	switch (type) {
		case 1:	// BYTE
		case 2:
			type_size = 1;
			break;
		case 3:	// SHORT
			type_size = 2;
			break;
		case 4:	// LONG
			type_size = 4; 
			break;
		case 5:	// RATIONAL
			type_size = 8;
			break;
		default:
			ASSERT_LOG(false, "Unsupported Type: %d", type);
	}
	return size * type_size;
}

#pragma warning( suppress : 4715)
const char* GetTagNameFromId(u16 tagId) {
	switch (tagId) {
		case 254:
			return "NewSubfileType";
			break;
		case 255:
			return "SubfileType";
			break;
		case 256:
			return "ImageWidth";
			break;
		case 257:
			return "ImageLength";
			break;
		case 258:
			return "BitsPerSample";
			break;
		case 259:
			return "Compression";
			break;
		case 262:
			return "PhotometricInterpretation";
			break;
		case 263:
			return "Threshholding";
			break;
		case 264:
			return "CellWidth";
			break;
		case 265:
			return "CellLength";
			break;
		case 266:
			return "FillOrder";
			break;
		case 270:
			return "ImageDescription";
			break;
		case 271:
			return "Make";
			break;
		case 272:
			return "Model";
			break;
		case 273:
			return "StripOffsets";
			break;
		case 274:
			return "Orientation";
			break;
		case 277:
			return "SamplesPerPixel";
			break;
		case 278:
			return "RowsPerStrip";
			break;
		case 279:
			return "StripByteCounts";
			break;
		case 280:
			return "MinSampleValue";
			break;
		case 281:
			return "MaxSampleValue";
			break;
		case 282:
			return "XResolution";
			break;
		case 283:
			return "YResolution";
			break;
		case 284:
			return "PlanarConfiguration";
			break;
		case 288:
			return "FreeOffsets";
			break;
		case 289:
			return "FreeByteCounts";
			break;
		case 290:
			return "GrayResponseUnit";
			break;
		case 291:
			return "GrayResponseCurve";
			break;
		case 296:
			return "ResolutionUnit";
			break;
		case 305:
			return "Software";
			break;
		case 306:
			return "DateTime";
			break;
		case 315:
			return "Artist";
			break;
		case 316:
			return "HostComputer";
			break;
		case 320:
			return "ColorMap";
			break;
		case 338:
			return "ExtraSamples";
			break;
		case 33432:
			return "Copyright";
			break;

// Extension Tags
		case 269:
			return "DocumentName";
			break;
		case 285:
			return "PageName";
			break;
		case 286:
			return "XPosition";
			break;
		case 287:
			return "YPosition";
			break;
		case 292:
			return "T4Options";
			break;
		case 293:
			return "T6Options";
			break;
		case 297:
			return "PageNumber";
			break;
		case 301:
			return "TransferFunction";
			break;
		case 317:
			return "Predictor";
			break;
		case 318:
			return "WhitePoint";
			break;
		case 319:
			return "PrimaryChromaticities";
			break;
		case 321:
			return "HalftoneHints";
			break;
		case 322:
			return "TileWidth";
			break;
		case 323:
			return "TileLength";
			break;
		case 324:
			return "TileOffsets";
			break;
		case 325:
			return "TileByteCounts";
			break;
		case 326:
			return "BadFaxLines";
			break;
		case 327:
			return "CleanFaxData";
			break;
		case 328:
			return "ConsecutiveBadFaxLines";
			break;
		case 330:
			return "SubIFDs";
			break;
		case 332:
			return "InkSet";
			break;
		case 333:
			return "InkNames";
			break;
		case 334:
			return "NumberOfInks";
			break;
		case 336:
			return "DotRange";
			break;
		case 337:
			return "TargetPrinter";
			break;
		case 339:
			return "SampleFormat";
			break;
		case 340:
			return "SMinSampleValue";
			break;
		case 341:
			return "SMaxSampleValue";
			break;
		case 342:
			return "TransferRange";
			break;
		case 343:
			return "ClipPath";
			break;
		case 344:
			return "XClipPathUnits";
			break;
		case 345:
			return "YClipPathUnits";
			break;
		case 346:
			return "Indexed";
			break;
		case 347:
			return "JPEGTables";
			break;
		case 351:
			return "OPIProxy";
			break;
		case 400:
			return "GlobalParametersIFD";
			break;
		case 401:
			return "ProfileType";
			break;
		case 402:
			return "FaxProfile";
			break;
		case 403:
			return "CodingMethods";
			break;
		case 404:
			return "VersionYear";
			break;
		case 405:
			return "ModeNumber";
			break;
		case 433:
			return "Decode";
			break;
		case 434:
			return "DefaultImageColor";
			break;
		case 512:
			return "JPEGProc";
			break;
		case 513:
			return "JPEGInterchangeFormat";
			break;
		case 514:
			return "JPEGInterchangeFormatLength";
			break;
		case 515:
			return "JPEGRestartInterval";
			break;
		case 517:
			return "JPEGLosslessPredictors";
			break;
		case 518:
			return "JPEGPointTransforms";
			break;
		case 519:
			return "JPEGQTables";
			break;
		case 520:
			return "JPEGDCTables";
			break;
		case 521:
			return "JPEGACTables";
			break;
		case 529:
			return "YCbCrCoefficients";
			break;
		case 530:
			return "YCbCrSubSampling";
			break;
		case 531:
			return "YCbCrPositioning";
			break;
		case 532:
			return "ReferenceBlackWhite";
			break;
		case 559:
			return "StripRowCounts";
			break;
		case 700:
			return "XMP";
			break;
		case 32781:
			return "ImageID";
			break;
		case 34732:
			return "ImageLayer";
			break;

// Private Tags
		case 32932:
			return "Wang Annotation";
			break;
		case 33445:
			return "MD FileTag";
			break;
		case 33446:
			return "MD ScalePixel";
			break;
		case 33447:
			return "MD ColorTable";
			break;
		case 33448:
			return "MD LabName";
			break;
		case 33449:
			return "MD SampleInfo";
			break;
		case 33450:
			return "MD PrepDate";
			break;
		case 33451:
			return "MD PrepTime";
			break;
		case 33452:
			return "MD FileUnits";
			break;
		case 33550:
			return "ModelPixelScaleTag";
			break;
		case 33723:
			return "IPTC";
			break;
		case 33918:
			return "INGR Packet Data Tag";
			break;
		case 33919:
			return "INGR Flag Registers";
			break;
		case 33920:
			return "IrasB Transformation Matrix";
			break;
		case 33922:
			return "ModelTiepointTag";
			break;
		case 34264:
			return "ModelTransformationTag";
			break;
		case 34377:
			return "Photoshop";
			break;
		case 34665:
			return "Exif IFD";
			break;
		case 34675:
			return "ICC Profile";
			break;
		case 34735:
			return "GeoKeyDirectoryTag";
			break;
		case 34736:
			return "GeoDoubleParamsTag";
			break;
		case 34737:
			return "GeoAsciiParamsTag";
			break;
		case 34853:
			return "GPS IFD";
			break;
		case 34908:
			return "HylaFAX FaxRecvParams";
			break;
		case 34909:
			return "HylaFAX FaxSubAddress";
			break;
		case 34910:
			return "HylaFAX FaxRecvTime";
			break;
		case 37724:
			return "ImageSourceData";
			break;
		case 40965:
			return "Interoperability IFD";
			break;
		case 42112:
			return "GDAL_METADATA";
			break;
		case 42113:
			return "GDAL_NODATA";
			break;
		case 50215:
			return "Oce Scanjob Description";
			break;
		case 50216:
			return "Oce Application Selector";
			break;
		case 50217:
			return "Oce Identification Number";
			break;
		case 50218:
			return "Oce ImageLogic Characteristics";
			break;
		case 50706:
			return "DNGVersion";
			break;
		case 50707:
			return "DNGBackwardVersion";
			break;
		case 50708:
			return "UniqueCameraModel";
			break;
		case 50709:
			return "LocalizedCameraModel";
			break;
		case 50710:
			return "CFAPlaneColor";
			break;
		case 50711:
			return "CFALayout";
			break;
		case 50712:
			return "LinearizationTable";
			break;
		case 50713:
			return "BlackLevelRepeatDim";
			break;
		case 50714:
			return "BlackLevel";
			break;
		case 50715:
			return "BlackLevelDeltaH";
			break;
		case 50716:
			return "BlackLevelDeltaV";
			break;
		case 50717:
			return "WhiteLevel";
			break;
		case 50718:
			return "DefaultScale";
			break;
		case 50719:
			return "DefaultCropOrigin";
			break;
		case 50720:
			return "DefaultCropSize";
			break;
		case 50721:
			return "ColorMatrix1";
			break;
		case 50722:
			return "ColorMatrix2";
			break;
		case 50723:
			return "CameraCalibration1";
			break;
		case 50724:
			return "CameraCalibration2";
			break;
		case 50725:
			return "ReductionMatrix1";
			break;
		case 50726:
			return "ReductionMatrix2";
			break;
		case 50727:
			return "AnalogBalance";
			break;
		case 50728:
			return "AsShotNeutral";
			break;
		case 50729:
			return "AsShotWhiteXY";
			break;
		case 50730:
			return "BaselineExposure";
			break;
		case 50731:
			return "BaselineNoise";
			break;
		case 50732:
			return "BaselineSharpness";
			break;
		case 50733:
			return "BayerGreenSplit";
			break;
		case 50734:
			return "LinearResponseLimit";
			break;
		case 50735:
			return "CameraSerialNumber";
			break;
		case 50736:
			return "LensInfo";
			break;
		case 50737:
			return "ChromaBlurRadius";
			break;
		case 50738:
			return "AntiAliasStrength";
			break;
		case 50740:
			return "DNGPrivateData";
			break;
		case 50741:
			return "MakerNoteSafety";
			break;
		case 50778:
			return "CalibrationIlluminant1";
			break;
		case 50779:
			return "CalibrationIlluminant2";
			break;
		case 50780:
			return "BestQualityScale";
			break;
		case 50784:
			return "Alias Layer Metadata";
			break;
		case 50908:
			return "TIFF_RSID";
			break;
		case 50909:
			return "GEO_METADATA";
			break;
		default:
			ASSERT_LOG(false, "Unknonw Tag %4d", tagId);
			break;
	}
}


#define QT_NUM_TABLES 2

bool OpenJPEGFile(const char* file);

#define MAX_FILES 7
// attempt to open all files listed in file_path
int main() {

	FILE* fp = fopen("../files.txt", "r");
	if(!fp) {
		printf("Failed To Open File \"files.txt\"\n");
	}

	u32 total_files = 0;
	char files[MAX_FILES][80]; 
	bool files_opened[MAX_FILES];
	while (fgets(files[total_files], 80, fp) && total_files < MAX_FILES) {
		total_files++;
	}

	for(u32 i = 0; i < total_files; i++) {

		u8 index = (u8)strlen(files[i]);
		if(files[i][index-1] == '\n'){
			files[i][index-1] = '\0';
		}

		printf("Opening File: \"%s\"\n", files[i]);
		files_opened[i] = OpenJPEGFile(files[i]);
	}

	for(u32 i = 0; i < total_files; i++){
		char x = files_opened[i] ? 'X' : ' ';
		printf("%-80s [%c]\n", files[i], x);
	}

	getchar();

	return 0;
}


bool OpenJPEGFile(const char* file) {

	u64 file_size;
	u8* buffer = OpenFile(file, &file_size);
	if(!buffer) {
		printf("Couldn't find \"%s\"\n", file);
		return false;
	}

	u8* s_buffer = buffer;

	u16 SOI = (u16)NextBytes(s_buffer, 2);
	ASSERT(SOI == 0xD8FF)	// SOI Start Of Image

	bool components[255] = {false};
	u8 comp_qt_table[255] = {0};
	u8 numComponents = 0; 
	u32 imageWidth = 0;
	u32 imageHeight = 0;
	u8 huffmanTableCount = 0;
	bool progressive = false;

	Table* huffman_tables[2][4] = {{nullptr, nullptr}, {nullptr, nullptr}};
	u8 data[QT_NUM_TABLES * 64];
	u8* qt_tables[QT_NUM_TABLES];	// chrominance and luminance quantization tables;
	memset(data, NULL, 64 * QT_NUM_TABLES);		// TODO: REMOVE
	for(int qt = 0; qt < QT_NUM_TABLES; qt++){
		qt_tables[qt] = data + (64 * qt);
	}

	while((u64)(s_buffer - buffer) < file_size) {
		ASSERT(NextByte(s_buffer) == 0xFF)
		u8 segment = NextByte(s_buffer);
		ASSERT(segment != 0x00 && segment != 0xFF)
		switch(segment){
			//case 0xD8:
			//	printf("Start Of Image\n");
			//	break;
			case 0xC0:
			case 0xC2: {
				// Start Of Frame
				printf("\n\n------------------- Start Of Frame -------------------\n");
				if(segment == 0xC2) {
					progressive = true;
					printf("Doesn't Support Progressive JPEG\n");
					return false;	// NO progressive support
				}
				u16 length = (NextByte(s_buffer) << 8) | NextByte(s_buffer);
				printf("Legnth %d\n", length);
				ASSERT(NextByte(s_buffer) == 8)	// bit-depth samples
				imageHeight = (NextByte(s_buffer) << 8) | NextByte(s_buffer);
				imageWidth = (NextByte(s_buffer) << 8) | NextByte(s_buffer);
				ASSERT(imageWidth > 0 && imageHeight > 0)
				printf("Width: %d Height: %d \n", imageWidth, imageHeight);
				numComponents = NextByte(s_buffer);	// Get number of components
				ASSERT(numComponents == 3)		// TODO: 3 components for now
				// ASSERT(numComponents == 1 || numComponents == 3);	// Grayscale or YCbCr
				if (numComponents == 1) {
					printf("No Grayscale Support\n");
					return false;
				}

				u32 sample_sum = 0;
				for(int i = 0; i < numComponents; i++){
					u8 id = NextByte(s_buffer);
					components[id] = true;
					u8 sample_factor = NextByte(s_buffer);
					u8 v_sample_factor = sample_factor & ((1 << 4) - 1);	// lower 4 bits = vertical
					u8 h_sample_factor = sample_factor >> 4;				// higher 4 bits = horizantal
					sample_sum += h_sample_factor * v_sample_factor;
					if(v_sample_factor != 1 || h_sample_factor != 1) {
						printf("No Sub Sampling Support\n");
						return false;
					}
					ASSERT(v_sample_factor >= 1 && v_sample_factor <= 4)
					ASSERT(h_sample_factor >= 1 && h_sample_factor <= 4)
					ASSERT((v_sample_factor * h_sample_factor) <= 10)
					u8 qt_table_number = NextByte(s_buffer);
					comp_qt_table[id]= qt_table_number;
					ASSERT(qt_table_number >= 0 && qt_table_number <= 3)
					switch (id)
					{
					case 1:
						printf("Y  QT: %d samples: v%d:h%d\n", qt_table_number, h_sample_factor, v_sample_factor);
						break;
					case 2:
						printf("Cb QT: %d samples: v%d:h%d\n", qt_table_number, h_sample_factor, v_sample_factor);
						break;
					case 3:
						printf("Cr QT: %d samples: v%d:h%d\n", qt_table_number, h_sample_factor, v_sample_factor);
						break;
					default:
						ASSERT(false);
					}

				}
				if (numComponents > 1) {
					ASSERT(sample_sum <= 10)
				}
				break;

			}
			case 0xC4:{
				// NOTE: Skip
				printf("\n\n------------------- Define Huffman Table(s) Count: %d -------------------\n", ++huffmanTableCount);
				u8 b1 = NextByte(s_buffer);
				u8 b2 = NextByte(s_buffer);
				u16 length = (b1 << 8) | b2;
				length -= 2; // skip length bytes
				const u8* data_start = s_buffer;
				ASSERT((buffer + file_size) - s_buffer > length);	// length should be within file
				while ( (s_buffer - data_start) < length) {

					u8 huffman_info = NextByte(s_buffer);
					u8 huff_dst_id = huffman_info & 0x0F;				// NOTE: 0 for Y 1 for Cb/Cr
					u8 is_ac =  (huffman_info >> 4) & 1;				// NOTE: 0 == for dc huffman tables, 1 == ac		
					printf("Huffman Table IDX: %hhu,  is AC: %d\n", huff_dst_id, is_ac);	
					ASSERT(huff_dst_id < 4)

					// BITS
					u8 i_count;
					u32 code_counts[16]; 
					memset(code_counts, 0, 16 * sizeof(u32));
					for(int i = 0; i < 16; i++){
						i_count = NextByte(s_buffer);
						code_counts[i] = i_count;
						// printf("%2d Bits, Count = %d\n", i + 1, i_count);
					}

					// Get Total Bytes For Huffman Table
					u32 total_bytes = 0;
					for(int i = 0; i < 16; i++){
						total_bytes += code_counts[i];
					}
					ASSERT(total_bytes <= 256);

					u8* huffman_bytes = new u8[total_bytes];
					u32 byte_idx = 0;
					for(int i = 0; i < 16; i++){
						u8 count = code_counts[i];
						// printf("%-2d Bits = [ ", i+1);
						for(int j = 0; j < count; j++){
							u8 value = NextByte(s_buffer);
							*(huffman_bytes + byte_idx++) = value;
							// printf(" %d ", value);
						}
						// printf(" ]\n");
					}
					
					huffman_tables[is_ac][huff_dst_id] = CreateHuffmanTable(code_counts, huffman_bytes);
				}
				ASSERT(s_buffer - data_start == length);
				break;
			}
			case 0xDB:{
				printf("\n\n------------------- Define Quantization Tables -------------------\n");
				u8 b1 = NextByte(s_buffer);
				u8 b2 = NextByte(s_buffer);
				u16 length = (b1 << 8) | b2;
				length -= 2; // skip length bytes
				ASSERT((buffer + file_size) - s_buffer > length);	// length should be within file
				ASSERT(length % 65 == 0);
				u8 qt_num;
				for(int i = 0; i < (length / 65); i++){
					ASSERT(length >= 65);
					u8 qt_info = NextByte(s_buffer);
					qt_num = qt_info & 0x0F;
					ASSERT(qt_num >= 0 && qt_num <= 3);
					u8 qt_prec = qt_info >> 4;
					ASSERT(qt_prec == 0);		// NOTE: for now 0 = 8-bit, can also be 1 = 16 bit
					ASSERT(qt_num < QT_NUM_TABLES);
					u8* qt_table_data = qt_tables[qt_num];
					u32 numBytes = 64 * (qt_prec+1);
					for(u32 j = 0; j < numBytes; j++){
						u8 value = NextByte(s_buffer);
						ASSERT(value != 0)
						*qt_table_data++ = value;
					}

					printf("QT ========== %d\n", qt_num);
					u8* current_table = qt_tables[qt_num];
					for(int x = 0; x < 8; x++){
						printf("[");
						for(int y = 0; y < 8; y++){
							u8 value = *current_table++;
							printf(" %3d ", value);
						}
						printf("]\n");
					}
				}

				break;

			}
			case 0xDD:{
				// NOTE: Skip
				printf("No Restart Interval Support\n");
				return false;
				printf("\n\n------------------- Define Restart Interval -------------------\n");
				u8 b1 = NextByte(s_buffer);
				u8 b2 = NextByte(s_buffer);
				u16 length = (b1 << 8) | b2;
				ASSERT(length == 4);	// Should be fixed size of 4 bytes
				u16 interval = (NextByte(s_buffer) << 8) | NextByte(s_buffer);	// interval value
				break;
			}
			case 0xDA:{
				printf("\n\n------------------- Start Of Scan -------------------\n");
				u16 length = (NextByte(s_buffer) << 8) | NextByte(s_buffer);
				u16 numScanComponents = NextByte(s_buffer);
				ASSERT(numScanComponents >= 1 && numScanComponents <= 4);
				ASSERT(numScanComponents == numComponents)		// NOTE: ??
				ASSERT(length == 6+2*numScanComponents);	// NOTE: must equal 6+2*components
				ASSERT(numScanComponents == 3)		// TODO: can be >= 1 and <= 4
				u32 comp_tables[4][2] = {0xFF};		// NOTE: initialize to invalid value
				u32 comp_order[4] = {};
				bool comp_order_set[4] = {false};
				ASSERT(numScanComponents == 3)
				const char* componentNames[4] = {"", "Y", "Cb", "Cr"};		// NOTE: Probably not always correct
				for(int i = 0; i < numScanComponents; i++){
					u8 id = NextByte(s_buffer);
					ASSERT(components[id])
					comp_order[i] = id;
					comp_order_set[id] = true;

					u8 huffman_table = NextByte(s_buffer);	// DC and AC Huffman table
					u8 ac = huffman_table & 0x0F;
					u8 dc = huffman_table >> 4;
					ASSERT(ac < 4 && dc < 4);	// NOTE 0-3 for extended DCT
					comp_tables[id][0] = dc;
					comp_tables[id][1] = ac;
					ASSERT(id < 4)
					// printf("%s\n", componentNames[id]);
					printf("Component %d ID: [%2s] | DC: %d AC: %d\n", id, componentNames[id], dc, ac);
				}
				s_buffer += 3;	// TODO: Start of spectral selection, End of spectral selection, point transform
				// TODO: Scan Data non-progressive image
				constexpr u32 MAX_SCAN_DATA = MegaBytes(12);
				u8* scan_data = new u8[MAX_SCAN_DATA];
				memset(scan_data, 0, sizeof(u8) * MAX_SCAN_DATA);
				u32 scan_count = 0;
				while(true){
					ASSERT(((u64)s_buffer - (u64)buffer) < file_size)
					u8 byte = NextByte(s_buffer);
					if(byte == 0xFF){
						if (*s_buffer == 0x0) {
							s_buffer++;		//  Skip stuff byte
						} else {
							s_buffer -= 1;
							break;
						}
					}
					ASSERT(scan_count < MAX_SCAN_DATA);
					*(scan_data + scan_count++) = byte;
				}

				// Decode Scan Data
				u8* decoded_data = new u8[MAX_SCAN_DATA];
				memset(decoded_data, NULL, MAX_SCAN_DATA * sizeof(u8));

				u32 bit = 0;
				u32 numMcuBlocks = ((imageWidth + 7u)/ 8u) * ((imageHeight + 7u) / 8u);
				i16 prevDCValues[4] = {0};
				u8* dc_only_image = new u8[numMcuBlocks];
				memset(dc_only_image, 0, sizeof(u8) * numMcuBlocks);
				u32 dc_only_idx = 0;
				for(u32 mcu = 0; mcu < numMcuBlocks; mcu++){
					printf("--------------- MCU: [ %08d] ------------------\n", mcu);
					
					i16 block[4][64] = {};
					memset(block[0], 0, sizeof(i16) * 64);
					memset(block[1], 0, sizeof(i16) * 64);
					memset(block[2], 0, sizeof(i16) * 64);
					memset(block[3], 0, sizeof(i16) * 64);
					for(u8 c = 0; c < numScanComponents; c++) {
						u8 comp_id = comp_order[c]; 
						ASSERT(comp_order_set[comp_id])
						u8 dc_table = comp_tables[comp_id][0];
						u8 ac_table = comp_tables[comp_id][1];

						ASSERT(numScanComponents == 3)
						printf("COMPONENT: %s\n", componentNames[comp_id]);
						{
							Table* root = huffman_tables[0][dc_table];		// DC table
							ASSERT( bit < scan_count * 8)
							u64 vc = GetHuffmanValue(root, scan_data, bit);
							ASSERT(vc < 16);
							if (vc != 0) {
								// convert bit representation to value
								i16 value = (i16)GetBits(scan_data, bit, (u8)vc);
								bit += (u8)vc;
								ASSERT( bit < scan_count * 8)
 								value = DecodeValueCategory(value, (u8)vc);		// NOTE: delta-encoded value
								ASSERT(comp_id <= 3)
								prevDCValues[comp_id] += value;
								// printf("DC Coff: %hd\n", prevDCValues[comp_id]);
							} else {
								// printf("DC Coff: EOB\n");
							}
							block[comp_id][0] = prevDCValues[comp_id] * qt_tables[comp_qt_table[comp_id]][0];;
						}

						int ac;
						for(ac = 0; ac < 63; ac++){
							// Read AC Values
							Table* root = huffman_tables[1][ac_table];	// AC table
							ASSERT( bit < scan_count * 8)
							u64 ac_info = GetHuffmanValue(root, scan_data, bit);
							ASSERT(ac_info <= 0xff);

							u8 zrl = (ac_info >> 4) & 0x0F;
							u8 vc = ac_info & 0x0F; 

							if( vc == 0) {
								if ( zrl == 0x0) {
									// EOB
									// printf("AC[%2d] == EOB\n", ac);
									break;
								} else if (zrl == 0x0F) {
									// printf("AC[%2d ... %2d] == 0\n", ac, ac+zrl+1);
									ASSERT((ac + zrl) <= 63)
									ac += zrl;	// +16
								} else {
									ASSERT(false)
								}
							} else  {
								ASSERT( bit < scan_count * 8)
								i16 value = (i16)GetBits(scan_data, bit, vc);
								bit += vc;
								value = DecodeValueCategory(value, vc);
								if(zrl > 0){
									// printf("AC[%2d ... %2d] == 0\n", ac, ac+zrl);
								}
								// printf("AC[%2d] == %hd\n", ac, value);
								ASSERT((ac + zrl) <= 63)
								block[comp_id][ac + 1] = value * qt_tables[comp_qt_table[comp_id]][ac + 1];
								ac += zrl;
							}
						}
						ASSERT(ac <= 63)
						for(int y = 0; y < 8; y++) {
							printf("[");
							for(int x = 0; x < 8; x++) {
								i16 value = block[comp_id][(y * 8) + x];
								printf(" %4hd,", value);
							}
							printf(" ]\n");
						}

						// IDCT
						u8 zig_zag[64] = {		// returns index in zig_zag order
							0, 1, 5, 6, 14, 15, 27, 28,
							2, 4, 7, 13, 16, 26, 29, 42, 
							3, 8, 12, 17, 25, 30, 41, 43,
							9, 11, 18, 24, 31, 40, 44, 53,
							10, 19, 23, 32, 39, 45, 52, 54,
							20, 22, 33, 38, 46, 51, 55, 60,
							21, 34, 37, 47, 50, 56, 59, 61,
							35, 36, 48, 49, 57, 58, 62, 63
						};

						if(comp_id == 1) {		// Luminance Only
							float x = 0; float y = 0;	// compute only top-left pixel value
							float inv = 1.f / sqrtf(2.0f); 
							float s_yx = 0;								
							constexpr float PI = 3.141592653589793f;
							for(int v = 0; v < 8; v++) {
								for(int u = 0; u < 8; u++) { 
									float cv = (v == 0) ? 1 / sqrtf(2.f) : 1.0f;
									float cu = (u == 0) ? 1 / sqrtf(2.f) : 1.0f;
									float cx = cosf((2 * x + 1.f) * u * PI / 16.f );
									float cy = cosf((2 * y + 1.f) * v * PI / 16.f );
									int z_idx = v * 8 + u;
									float s_vu = block[comp_id][zig_zag[z_idx]];
									s_yx += cu * cv * s_vu * cx * cy;
								}
							}
							s_yx *= 0.25;

							// Level Shift
							s_yx += 128;
							printf("Pixel Value: %6f\n", s_yx);
							ASSERT(s_yx >= 0 && s_yx < 255)		// NOTE: ??

							*(dc_only_image + dc_only_idx++) = (u8)fmaxf(0.f, fminf(255.f, s_yx));
						}
						// printf("Finished at %d\n", ac);
					}
					printf("\n\n");
				}

				u32 width_blocks =  ((imageWidth + 7u)/ 8u);
				u32 height_blocks =  ((imageHeight + 7u)/ 8u);
				ASSERT((width_blocks * height_blocks) == dc_only_idx)

				// TODO: save dc_only_image to other format
				printf("Width Blocks %d Height Blocks: %d\n", width_blocks, height_blocks);
				// ASSERT(false)
				break;
			}
			//// case 0xDn:
			//// 	printf("0xDn\n");
			//// 	break;
			//// case 0xEn:
			//// 	printf("0xEn\n");
			//// 	break;
			//case 0xFE:
			//	printf("Comment\n");
			//	break;
			case 0xD9:
				printf("End Of Image\n");
				ASSERT((s_buffer - buffer) == file_size);
				break;
			case 0xE2:
			case 0xE3:
			case 0xE4:
			case 0xE5:
			case 0xE6:
			case 0xE7:
			case 0xE8:
			case 0xE9:
			case 0xEA:
			case 0xEB:
			case 0xEC:
			case 0xED:
			case 0xEE:
			case 0xEF: {
				// NOTE: Skip
				u8 b1 = NextByte(s_buffer);
				u8 b2 = NextByte(s_buffer);
				u16 length = (b1 << 8) | b2;
				length -= 2; // skip length bytes
				ASSERT((buffer + file_size) - s_buffer > length);	// length should be within file
				s_buffer += length;
				break;
			}
			case 0xE0:{
				// APP0 segment used for JFIF standard
				// NOTE: Skip
				u8 b1 = NextByte(s_buffer);
				u8 b2 = NextByte(s_buffer);
				u16 length = (b1 << 8) | b2;
				length -= 2; // skip length bytes
				ASSERT((buffer + file_size) - s_buffer > length);	// length should be within file
				s_buffer += length;
				break;
			}
			case 0xE1:{
				// NOTE: Exif does not use APPn segments other than APP1, APP2 and COM segments. However, some unknown APPn may still exist on the file structure and Exif readers should be designed to skip over them.
				u8 b1 = NextByte(s_buffer);
				u8 b2 = NextByte(s_buffer);
				u16 length = (b1 << 8) | b2;
				length -= 2; // skip length bytes
				ASSERT((buffer + file_size) - s_buffer > length);	// length should be within file
				s_buffer += length;
				break;		// skip APP1 for now
				printf("Metadata Segment Size: %d\n", length);

				// If Exif
				ASSERT(strcmp((const char*)s_buffer, "Exif") == 0); // next two bytes should be null so strcmp should work with s_buffer
				s_buffer += 4;
				// skip two null bytes
				ASSERT((u16)NextBytes(s_buffer, 2) == 0x0);
					
				// Tiff Image File Header
				struct TIFHEAD {
					u16 ByteOrderIdentifier; /* Byte-order Identifier */
					u16 Version; /* TIFF version number (always 2Ah) */
					u32 IFDOffset; /* Offset of the first Image File Directory*/
				};
				TIFHEAD* tiff_header = (TIFHEAD*)s_buffer;
				u8* tiff_header_location = s_buffer;
				s_buffer += sizeof(TIFHEAD);
				// Bytes 0-1: The byte order used within the file. Legal values are:“II”(4949.H)“MM” (4D4D.H).
				// u8* tiffStart = s_buffer; 
				// NOTE: currently only supporting big endian storage
				ASSERT(tiff_header->ByteOrderIdentifier == 0x4D4D);	// TODO: support little endian format
				ASSERT(tiff_header->Version == 0x2A00);	// signature is 0x2A, in little endian machine read 0x2A00 or swap bytes
				// TODO: 0th IFD offset migh this might not always be 0x08
				ASSERT(tiff_header->IFDOffset == 0x08000000); // in little endian swap bytes or read 0x80000000
				// u8* ifd = tiffHeaderStart + 8;

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
				int ifd_number = 0;
				while(true){
					printf("\n==============  IFD %2d  ===============\n", ifd_number++);
					ifd.NumDirEntries = _byteswap_ushort((u16)NextBytes(s_buffer, 2));
					ifd.TagList = (TIFTAG*)s_buffer;
					s_buffer += (sizeof(TIFTAG) * ifd.NumDirEntries);	// skip TIFTAGS
					ifd.NextIFDOffset = _byteswap_ulong((u8)NextBytes(s_buffer, 4));

#define SWAP_STRUCT_ENTRY(entry, mode) entry = _byteswap_##mode(entry); 

					for(int i = 0; i < ifd.NumDirEntries; i++){
						TIFTAG* entry = (ifd.TagList + i);
						SWAP_STRUCT_ENTRY(entry->TagId, ushort);
						SWAP_STRUCT_ENTRY(entry->DataType, ushort);
						SWAP_STRUCT_ENTRY(entry->DataCount, ulong);
						SWAP_STRUCT_ENTRY(entry->DataOffset, ulong);

						ASSERT(entry->DataType > 0 && entry->DataType <= 5)		// NOTE: with TIFF 6.0 can go upto 12

						u32 data_size = GetByteSizeFromCountAndType(entry->DataCount, entry->DataType);

						u8* data = (tiff_header_location + entry->DataOffset);
						// NOTE: if data_size <= 4 bytes data maybe in this field, but this may not always be the case handle this case
						if(data_size <= 4) {
							data = (u8*)&(entry->DataOffset);
						}else{
							ASSERT(entry->DataOffset < file_size);
						}

						// Print Tag
						const char* tag_name = GetTagNameFromId(entry->TagId);
						printf("Tag Found: %2d == \"%s\"\n", entry->TagId, tag_name);

						if(entry->TagId == 256) {
							ASSERT(entry->DataType == 3 || entry->DataType == 4);
							u16 width;
							if(entry->DataType == 3){
								// NOTE: if DataType == SHORT; because we already swapped the 4 byte DataOffset using STRUCT_ENTRY_SWAP
								// we must unswap it take only the first two bytes and swap those.
								// but this is essentially the same as just taking the last two bytes of DataOffset that was already swapped 
								width = *(u16*)(data + 2);
							}else{
								width = *(u32*)data;
							}
							printf("Image Width:  %4d\n", imageWidth);
						}else if(entry->TagId == 257) {
							ASSERT(entry->DataType == 3 || entry->DataType == 4);
							u16 height;
							if(entry->DataType == 3){
								height = *(u16*)(data + 2);
							}else{
								height = *(u32*)data;
							}
							printf("Image Height:  %4d\n", imageHeight);
						}
					}
					ASSERT(ifd.NextIFDOffset >= 0);
					if(!ifd.NextIFDOffset)
						break;
					s_buffer = tiff_header_location + ifd.NextIFDOffset;
				}
				break;
			}
			default:
				ASSERT(false);
				// ASSERT_LOG(false, "Unknown Segment: 0x%x\n", seg);
				break;
		}
	}

	return true;
}



