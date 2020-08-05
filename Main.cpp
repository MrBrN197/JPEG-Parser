
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>


#include "core.h"


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

u64 GetBits(u8* buffer, u64 bit_pos, u8 bits) {
	ASSERT(bits > 0 && bits <= 32);
	u32 idx = bit_pos >> 5;
	u32 offset = bit_pos & 31;
	// u64 value = *((u64*)buffer + idx);
	u8 b1 = NextBytes(buffer, 1);
	u8 b2 = NextBytes(buffer, 1);
	u8 b3 = NextBytes(buffer, 1);
	u8 b4 = NextBytes(buffer, 1);
	u32 value = b1 << 24 | b2 << 16 | b3 << 8 | b4;
	value = value >> (32 - (offset + bits));
	value &= (1ULL << bits) - 1;
	return value;
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


#define HUFFMAN_NUM_TABLES 4
#define QT_NUM_TABLES 2


#include "Huffman.h"

int main() {


	u64 file_size;
	u8* buffer = OpenFile("..\\assets\\example7.jpg", &file_size);

	ASSERT(buffer);

	u8* s_buffer = buffer;

	u16 SOI = NextBytes(s_buffer, 2);
	ASSERT(SOI == 0xD8FF)	// SOI Start Of Image

	u16 components;
	u32 imageWidth;
	u32 imageHeight;
	u8 huffmanTableCount = 0;
	bool progressive = false;

	Table* huffman_tables[HUFFMAN_NUM_TABLES];
	u8* data = new u8[QT_NUM_TABLES * 64];
	u8* qt_tables[QT_NUM_TABLES];	// chrominance and luminance quantization tables;
	memset(data, NULL, 64 * QT_NUM_TABLES);		// TODO: REMOVE
	for(int qt = 0; qt < QT_NUM_TABLES; qt++){
		qt_tables[qt] = data + (64 * qt);
	}

	while(s_buffer - buffer < file_size) {
		ASSERT(NextBytes(s_buffer, 1) == 0xFF)
		u16 segment = NextBytes(s_buffer, 1);
		ASSERT(segment != 0x00 && segment != 0xFF)
		switch(segment){
			//case 0xD8:
			//	printf("Start Of Image\n");
			//	break;
			case 0xC0:
			case 0xC2: {
				// Start Of Frame
				printf("\n\n------------------- Start Of Frame -------------------\n");
				if(segment == 0xC2)
					progressive = true;
				u16 length = (NextBytes(s_buffer, 1) << 8) | NextBytes(s_buffer, 1);
				printf("Legnth %d\n", length);
				ASSERT(NextBytes(s_buffer, 1) == 8)	// bit-depth samples
				u16 height = (NextBytes(s_buffer, 1) << 8) | NextBytes(s_buffer, 1);
				u16 width = (NextBytes(s_buffer, 1) << 8) | NextBytes(s_buffer, 1);
				ASSERT(width > 0 && height > 0)
				printf("Width: %d Height: %d \n", width, height);
				components = NextBytes(s_buffer, 1);	// Set number of components
				ASSERT(components == 3)		// NOTE: 3 components for now
				// ASSERT(components == 1 || components == 3);	// Grayscale or YCbCr
				switch (components)
				{
				case 1:
					printf("Grayscale\n");
					break;
				case 3:
					printf("YUV\n");
					break;
				default:
					break;
				}
				// NOTE: read components. I think?
				for(int i = 0; i < components; i++){
					u8 id = NextBytes(s_buffer, 1);
					u8 sample_factor = NextBytes(s_buffer, 1);
					u8 v_sample_factor = sample_factor & ((1 << 4) - 1);	// lower 4 bits = vertical
					u8 h_sample_factor = sample_factor >> 4;				// higher 4 bits = horizantal
					ASSERT(v_sample_factor >= 1 && v_sample_factor <= 4)
					ASSERT(h_sample_factor >= 1 && h_sample_factor <= 4)
					ASSERT((v_sample_factor * h_sample_factor) <= 10)
					u8 qt_table_number = NextBytes(s_buffer, 1);
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
				break;

			}
			case 0xC4:{
				// NOTE: Skip
				printf("\n\n------------------- Define Huffman Table(s) Count: %d -------------------\n", huffmanTableCount+1);
				u8 b1 = NextBytes(s_buffer, 1);
				u8 b2 = NextBytes(s_buffer, 1);
				u16 length = (b1 << 8) | b2;
				length -= 2; // skip length bytes
				const u8* data_start = s_buffer;
				ASSERT((buffer + file_size) - s_buffer > length);	// length should be within file
				u8 huffman_info = NextBytes(s_buffer, 1);
				//u8 huff_type = (huffman_info << 4) == 1;
				// ASSERT(huff_type == 0 || huff_type == 1)
				u8 huff_dst_id = huffman_info & 0x0F;				// NOTE: 0 for Y 1 for Cb/Cr
				bool is_ac =  (1 << 4) & huffman_info;				// NOTE: 0 == for dc huffman tables, 1 == ac		
				ASSERT(huff_dst_id == 0 || huff_dst_id == 1)

				// BITS
				u8 i_count;
				u32 code_counts[16]; 
				memset(code_counts, 0, 16 * sizeof(u32));
				for(int i = 0; i < 16; i++){
					i_count = NextBytes(s_buffer, 1);
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
						u8 value = NextBytes(s_buffer, 1);
						*(huffman_bytes + byte_idx++) = value;
						// printf(" %d ", value);
					}
					// printf(" ]\n");
				}
				
				huffman_tables[huff_dst_id] = CreateHuffmanTable(code_counts, huffman_bytes);
				ASSERT(s_buffer - data_start == length);
				break;
			}
			case 0xDB:{
				printf("\n\n------------------- Define Quantization Tables -------------------\n");
				u8 b1 = NextBytes(s_buffer, 1);
				u8 b2 = NextBytes(s_buffer, 1);
				u16 length = (b1 << 8) | b2;
				length -= 2; // skip length bytes
				ASSERT((buffer + file_size) - s_buffer > length);	// length should be within file
				ASSERT(length % 65 == 0);
				for(int i = 0; i < (length / 65); i++){
					ASSERT(length >= 65);
					u8 qt_info = NextBytes(s_buffer, 1);
					u8 qt_num = qt_info & 0x0F;
					ASSERT(qt_num >= 0 && qt_num <= 3);
					u8 qt_prec = qt_info >> 4;
					ASSERT(qt_prec == 0);		// NOTE: for now 0 = 8-bit, can also be 1 = 16 bit
					ASSERT(i < QT_NUM_TABLES);
					u8* qt_table_data = qt_tables[i];
					u32 numBytes = 64 * (qt_prec+1);
					for(int j = 0; j < 64; j++){
						u8 value = NextBytes(s_buffer, 1);
						*qt_table_data++ = value;
					}
				}

				for( int qt = 0; qt < 1; qt++){		// TODO: extract all tables
					printf("QT ========== %d\n", qt);
					u8* current_table = qt_tables[qt];
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
				printf("\n\n------------------- Define Restart Interval -------------------\n");
				u8 b1 = NextBytes(s_buffer, 1);
				u8 b2 = NextBytes(s_buffer, 1);
				u16 length = (b1 << 8) | b2;
				ASSERT(length == 4);	// Should be fixed size of 4 bytes
				u16 interval = (NextBytes(s_buffer, 1) << 8) | NextBytes(s_buffer, 1);	// interval value
				break;
			}
			case 0xDA:{
				printf("\n\n------------------- Start Of Scan -------------------\n");
				u16 length = (NextBytes(s_buffer, 1) << 8) | NextBytes(s_buffer, 1);
				u16 numScanComponents = NextBytes(s_buffer, 1);
				ASSERT(numScanComponents == components)		// NOTE: ??
				ASSERT(length == 6+2*components);	// NOTE: must equal 6+2*components
				// ASSERT(numScanComponents >= 1 && numScanComponents <= 4);
				ASSERT(numScanComponents == 3)		// TODO: can be >= 1 and <= 4

				u32 comp_tables[3][2] = {};
				for(int i = 0; i < numScanComponents; i++){
					u8 id = NextBytes(s_buffer, 1);
					ASSERT(id == i + 1);	// NOTE: ??

					u8 huffman_table = NextBytes(s_buffer, 1);	// DC and AC Huffman table
					u8 ac = huffman_table & 0x0F;
					u8 dc = huffman_table >> 4;
					ASSERT(ac < HUFFMAN_NUM_TABLES && dc < HUFFMAN_NUM_TABLES);
					comp_tables[i][0] = ac;
					comp_tables[i][1] = dc;
					printf("Component ID: %d | DC: %d AC: %d\n", id, dc , ac);
				}
				s_buffer += 3;	// Skip 3 Unused Bytes
				// TODO: Scan Data non-progressive image
				constexpr u32 MAX_SCAN_DATA = MegaBytes(12);
				u8* scan_data = new u8[MAX_SCAN_DATA];
				u32 scan_count = 0;
				while(true){
					ASSERT((s_buffer - buffer) < file_size)
					u8 byte = NextBytes(s_buffer, 1);
					if(byte == 0xFF){
						if (*s_buffer == 0xD9){
							s_buffer -= 1;
							break;
							ASSERT(false);
						}else {
							ASSERT(*s_buffer == 0x0)
							s_buffer++;		//  Skip stuff byte
						}
					}
					ASSERT(scan_count < MAX_SCAN_DATA);
					*(scan_data + scan_count++) = byte;
				}

				// Decode Scan Data
				u8* decoded_data = new u8[MAX_SCAN_DATA];
				memset(decoded_data, NULL, MAX_SCAN_DATA * sizeof(u8));

				u32 bit = 0;
				while(true) {
					Table* root = huffman_tables[comp_tables[0][1]];	// TODO: use correct table
					while(!root->isValue){
						u8 bit_value = GetBits(scan_data, bit++, 1);
						ASSERT(bit_value == 0 || bit_value == 1)
						root = (bit_value) ? root->right : root->left;
						ASSERT(root)
					}
					// get value category
					u32 val_category = root->value;
					ASSERT(val_category < 16);
					// convert bit representation to value
					u16 value = GetBits(scan_data, bit, val_category);
					bit += val_category;
					u16 is_neg = (value >> (val_category - 1)) & 1;
					if(!is_neg){
						value = (i16)(0xffff << val_category) + value + 1;
					}
					printf("Value: %d\n", value);
					ASSERT(false)
				}
				ASSERT(false)
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
				u8 b1 = NextBytes(s_buffer, 1);
				u8 b2 = NextBytes(s_buffer, 1);
				u16 length = (b1 << 8) | b2;
				length -= 2; // skip length bytes
				ASSERT((buffer + file_size) - s_buffer > length);	// length should be within file
				s_buffer += length;
				break;
			}
			case 0xE0:{
				// APP0 segment used for JFIF standard
				// NOTE: Skip
				u8 b1 = NextBytes(s_buffer, 1);
				u8 b2 = NextBytes(s_buffer, 1);
				u16 length = (b1 << 8) | b2;
				length -= 2; // skip length bytes
				ASSERT((buffer + file_size) - s_buffer > length);	// length should be within file
				s_buffer += length;
				break;
			}
			case 0xE1:{
				// NOTE: Exif does not use APPn segments other than APP1, APP2 and COM segments. However, some unknown APPn may still exist on the file structure and Exif readers should be designed to skip over them.
				u8 b1 = NextBytes(s_buffer, 1);
				u8 b2 = NextBytes(s_buffer, 1);
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
				ASSERT(NextBytes(s_buffer, 2) == 0x0);
					
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
					ifd.NumDirEntries = _byteswap_ushort(NextBytes(s_buffer, 2));
					ifd.TagList = (TIFTAG*)s_buffer;
					s_buffer += (sizeof(TIFTAG) * ifd.NumDirEntries);	// skip TIFTAGS
					ifd.NextIFDOffset = _byteswap_ulong(NextBytes(s_buffer, 4));

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
						printf("Tag Found: %2d == %s\n", entry->TagId, tag_name);

						if(entry->TagId == 256) {
							ASSERT(entry->DataType == 3 || entry->DataType == 4);
							if(entry->DataType == 3){
								// NOTE: if DataType == SHORT; because we already swapped the 4 byte DataOffset using STRUCT_ENTRY_SWAP
								// we must unswap it take only the first two bytes and swap those.
								// but this is essentially the same as just taking the last two bytes of DataOffset that was already swapped 
								imageWidth = *(u16*)(data + 2);
							}else{
								imageWidth = *(u32*)data;
							}
							printf("Image Width:  %4d\n", imageWidth);
						}else if(entry->TagId == 257) {
							ASSERT(entry->DataType == 3 || entry->DataType == 4);
							if(entry->DataType == 3){
								imageHeight = *(u16*)(data + 2);
							}else{
								imageHeight = *(u32*)data;
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

	return 0;
}



