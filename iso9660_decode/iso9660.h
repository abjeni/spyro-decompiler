#include <stdint.h>

typedef uint16_t int16_LSB;
typedef uint16_t int16_MSB;
typedef struct{uint16_t lsb; uint16_t msb;} __attribute__((packed)) int16_LSB_MSB;
typedef int16_t sint16_LSB;
typedef int16_t sint16_MSB;
typedef struct{int16_t lsb; int16_t msb;} __attribute__((packed)) sint16_LSB_MSB;
typedef uint32_t int32_LSB;
typedef uint32_t int32_MSB;
typedef struct{uint32_t lsb; uint32_t msb;} __attribute__((packed)) int32_LSB_MSB;
typedef int32_t sint32_LSB;
typedef int32_t sint32_MSB;
typedef struct{int32_t lsb; int32_t msb;} __attribute__((packed)) sint32_LSB_MSB;

typedef char date[7];

typedef struct {
  uint16_t owner_id_group;
  uint16_t owner_id_user;
  uint16_t file_attributes;
  char signature[2];
  uint8_t file_number;
  char reserved[5];
} __attribute__((packed)) record_extra_bin;

/*
  00h 2      Owner ID Group  (whatever, usually 0000h, big endian)
  02h 2      Owner ID User   (whatever, usually 0000h, big endian)
  04h 2      File Attributes (big endian):
               0   Owner Read    (usually 1)
               1   Reserved      (0)
               2   Owner Execute (usually 1)
               3   Reserved      (0)
               4   Group Read    (usually 1)
               5   Reserved      (0)
               6   Group Execute (usually 1)
               7   Reserved      (0)
               8   World Read    (usually 1)
               9   Reserved      (0)
               10  World Execute (usually 1)
               11  IS_MODE2        (0=MODE1 or CD-DA, 1=MODE2)
               12  IS_MODE2_FORM2  (0=FORM1, 1=FORM2)
               13  IS_INTERLEAVED  (0=No, 1=Yes...?) (by file and/or channel?)
               14  IS_CDDA         (0=Data or ADPCM, 1=CD-DA Audio Track)
               15  IS_DIRECTORY    (0=File or CD-DA, 1=Directory Record)
             Commonly used Attributes are:
               0D55h=Normal Binary File (with 800h-byte sectors)
               2555h=Unknown            (wipeout .AV files) (MODE1 ??)
               4555h=CD-DA Audio Track  (wipeout .SWP files, alone .WAV file)
               3D55h=Streaming File     (ADPCM and/or MDEC or so)
               8D55h=Directory Record   (parent-, current-, or sub-directory)
  06h 2      Signature     ("XA")
  08h 1      File Number   (Must match Subheader's File Number)
  09h 5      Reserved      (00h-filled)
  */


typedef struct {
  uint8_t length;
  uint8_t extended_attribute_record_length;
  int32_LSB_MSB location_of_extent;
  int32_LSB_MSB size_of_extent;
  date recording_time;
  char flags;
  uint8_t interleave_unit_size;
  uint8_t interleave_gap_size;
  int16_LSB_MSB volume_sequence_number;
  uint8_t identifier_length;
  char identifier[223];
} __attribute__((packed)) directory_record_bin;

typedef struct {
  directory_record_bin bin;
  record_extra_bin *extra;
  int extra_len;
} directory_record;


/*
0 	1 	int8 	Length of Directory Record.
1 	1 	int8 	Extended Attribute Record length.
2 	8 	int32_LSB-MSB 	Location of extent (LBA) in both-endian format.
10 	8 	int32_LSB_MSB 	Data length (size of extent) in both-endian format.
18 	7 	see format below 	Recording date and time.
25 	1 	see below 	File flags.
26 	1 	int8 	File unit size for files recorded in interleaved mode, zero otherwise.
27 	1 	int8 	Interleave gap size for files recorded in interleaved mode, zero otherwise.
28 	4 	int16_LSB-MSB 	Volume sequence number - the volume that this extent is recorded on, in 16 bit both-endian format.
32 	1 	int8 	Length of file identifier (file name). This terminates with a ';' character followed by the file ID number in ASCII coded decimal ('1').
33 	(variable) 	strD 	File identifier.
(variable) 	1 	-- 	Padding field - zero if length of file identifier is even, otherwise, this field is not present. This means that a directory entry will always start on an even byte number.
(variable) 	(variable) 	-- 	System Use - The remaining bytes up to the maximum record size of 255 may be used for extensions of ISO 9660. The most common one is the System Use Share Protocol (SUSP) and its application, the Rock Ridge Interchange Protocol (RRIP). 
*/

typedef struct {
  char volume_descriptor_type;
  char standard_identifier[5];
  char volume_descriptor_version;
  char reserved1;
  char system_identifier[32];
  char volume_identifier[32];
  char reserved2[8];
  int32_LSB_MSB volume_space_size;
  char reserved3[32];
  int16_LSB_MSB volume_set_size;
  int16_LSB_MSB volume_sequence_number;
  int16_LSB_MSB logical_block_size;
  int32_LSB_MSB path_table_size;
  int32_LSB path_table_1_block_number;
  int32_LSB path_table_2_block_number;
  int32_MSB path_table_3_block_number;
  int32_MSB path_table_4_block_number;
  char root_directory_record[34];
  char volume_set_identifier[128];
  char publisher_identifier[128];
  char data_preparer_identifier[128];
  char application_identifier[128];
  char copyright_filename[37];
  char abstract_filename[37];
  char bibliographic_filename[37];
  char volume_creation_timestamp[17];
  char volume_modification_timestamp[17];
  char volume_expiration_timestamp[17];
  char volume_effective_timestamp[17];
  char file_structure_version;
  char reserved4;
  char application_use_area1[141];
  char identifying_signature[8];
  uint16_t flags;
  int32_LSB_MSB startup_directory;
  char reserved5[8];
  char application_use_area2[345];
  char reserved6[653];
} __attribute__((packed)) iso9660_header_bin;

typedef struct {
  iso9660_header_bin bin;
  directory_record root_dir_record; 
} iso9660_header;

/*

000h 1    Volume Descriptor Type        (01h=Primary Volume Descriptor)
001h 5    Standard Identifier           ("CD001")
006h 1    Volume Descriptor Version     (01h=Standard)
007h 1    Reserved                      (00h)
008h 32   System Identifier             (a-characters) ("PLAYSTATION")
028h 32   Volume Identifier             (d-characters) (max 8 chars for PSX?)
048h 8    Reserved                      (00h)
050h 8    Volume Space Size             (2x32bit, number of logical blocks)
058h 32   Reserved                      (00h)
078h 4    Volume Set Size               (2x16bit) (usually 0001h)
07Ch 4    Volume Sequence Number        (2x16bit) (usually 0001h)
080h 4    Logical Block Size in Bytes   (2x16bit) (usually 0800h) (1 sector)
084h 8    Path Table Size in Bytes      (2x32bit) (max 800h for PSX)
08Ch 4    Path Table 1 Block Number     (32bit little-endian)
090h 4    Path Table 2 Block Number     (32bit little-endian) (or 0=None)
094h 4    Path Table 3 Block Number     (32bit big-endian)
098h 4    Path Table 4 Block Number     (32bit big-endian) (or 0=None)
09Ch 34   Root Directory Record         (see next chapter)
0BEh 128  Volume Set Identifier         (d-characters) (usually empty)
13Eh 128  Publisher Identifier          (a-characters) (company name)
1BEh 128  Data Preparer Identifier      (a-characters) (empty or other)
23Eh 128  Application Identifier        (a-characters) ("PLAYSTATION")
2BEh 37   Copyright Filename            ("FILENAME.EXT;VER") (empty or text)
2E3h 37   Abstract Filename             ("FILENAME.EXT;VER") (empty)
308h 37   Bibliographic Filename        ("FILENAME.EXT;VER") (empty)
32Dh 17   Volume Creation Timestamp     ("YYYYMMDDHHMMSSFF",timezone)
33Eh 17   Volume Modification Timestamp ("0000000000000000",00h)
34Fh 17   Volume Expiration Timestamp   ("0000000000000000",00h)
360h 17   Volume Effective Timestamp    ("0000000000000000",00h)
371h 1    File Structure Version        (01h=Standard)
372h 1    Reserved for future           (00h-filled)
373h 141  Application Use Area          (00h-filled for PSX and VCD)
400h 8    CD-XA Identifying Signature   ("CD-XA001" for PSX and VCD)
408h 2    CD-XA Flags (unknown purpose) (00h-filled for PSX and VCD)
40Ah 8    CD-XA Startup Directory       (00h-filled for PSX and VCD)
412h 8    CD-XA Reserved                (00h-filled for PSX and VCD)
41Ah 345  Application Use Area          (00h-filled for PSX and VCD)
573h 653  Reserved for future           (00h-filled)
*/


int read_iso9660(char *filename);

