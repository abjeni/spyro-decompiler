#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "iso9660.h"

//#define CREATE_FILESYSTEM
#define OUTPUT_INFO

void directory_record_extra(directory_record *dir_rec)
{
  int num = dir_rec->bin.identifier_length;

  //padding to keep the lenght even
  if ((num&1) == 0) num++;

  //the rest of the length is extra
  dir_rec->extra_len = dir_rec->bin.length-33-num;

  if (dir_rec->extra_len == 14)
    dir_rec->extra = (record_extra_bin*)&dir_rec->bin.identifier[num];
  else
    dir_rec->extra = NULL;
}

int seek_sector(FILE *file, int sector_num)
{
  return fseek(file, 2352*sector_num, SEEK_SET);
}

int read_sector_header(FILE *file)
{
  return fseek(file, 24, SEEK_CUR);
}

void print_record_extra_bin(record_extra_bin *extra, int depth)
{
  if (extra == NULL)
  {
    printf("NULL\n");
    return;
  }

  assert(depth > 0);
  char tabs[depth+1];
  for (int i = 0; i < depth; i++) tabs[i] = '\t';
  tabs[depth] = 0;
  printf("(record_extra_bin*){\n");

  printf("%sowner id group: %d\n", tabs, extra->owner_id_group);
  printf("%sowner id user: %d\n", tabs, extra->owner_id_user);
  printf("%sfile attribute: %16b %4x\n", tabs, extra->file_attributes, extra->file_attributes);
  printf("%ssignature: %.2s\n", tabs, extra->signature);
  printf("%ssignature: %d\n", tabs, extra->file_number);

  tabs[depth-1] = 0;
  printf("%s}\n", tabs);
}

void print_directory_record_bin(directory_record_bin dir_rec, int depth)
{
  assert(depth > 0);
  char tabs[depth+1];
  for (int i = 0; i < depth; i++) tabs[i] = '\t';
  tabs[depth] = 0;
  printf("directory_record_bin{\n");

  printf("%slength: %d\n", tabs, dir_rec.length);
  printf("%sextended_attribute_record_length: %d\n", tabs, dir_rec.extended_attribute_record_length);
  printf("%slocation_of_extent: [%d - %d] %d\n", tabs, dir_rec.location_of_extent.lsb, dir_rec.location_of_extent.lsb+dir_rec.size_of_extent.lsb/2048, dir_rec.size_of_extent.lsb/2048);
  printf("%ssize_of_extent: %d %d\n", tabs, dir_rec.size_of_extent.lsb, dir_rec.size_of_extent.lsb/2048);
  printf("%srecording_time: wip\n", tabs);
  printf("%sflags: %X\n", tabs, dir_rec.flags);
  printf("%sinterleave_unit_size: %d\n", tabs, dir_rec.interleave_unit_size);
  printf("%sinterleave_gap_size: %d\n", tabs, dir_rec.interleave_gap_size);
  printf("%svolume_sequence_number: %d\n", tabs, dir_rec.volume_sequence_number.lsb);
  printf("%sidentifier_length: %d\n", tabs, dir_rec.identifier_length);
  printf("%sidentifier: %.*s\n", tabs, dir_rec.identifier_length, dir_rec.identifier);

  tabs[depth-1] = 0;
  printf("%s}\n", tabs);
}

void print_directory_record(directory_record dir_rec, int depth)
{
  assert(depth > 0);
  char tabs[depth+1];
  for (int i = 0; i < depth; i++) tabs[i] = '\t';
  tabs[depth] = 0;
  printf("directory_record{\n");

  printf("%sbin: ", tabs);
  print_directory_record_bin(dir_rec.bin, depth+1);
  printf("%sextra_len: %d\n", tabs, dir_rec.extra_len);

  printf("%sextra: ",tabs);
  print_record_extra_bin(dir_rec.extra, depth+1);

  tabs[depth-1] = 0;
  printf("%s}\n", tabs);
}

void print_iso9660_header_bin(iso9660_header_bin header, int depth)
{
  assert(depth > 0);
  char tabs[depth+1];
  for (int i = 0; i < depth; i++) tabs[i] = '\t';
  tabs[depth] = 0;
  printf("iso9660_header{\n");
  printf("%svolume_descriptor_type: %d\n", tabs, header.volume_descriptor_type);
  printf("%sstandard_identifier: %.5s\n", tabs, header.standard_identifier);
  printf("%svolume_descriptor_version: %d\n", tabs, header.volume_descriptor_version);
  printf("%ssystem_identifier: %.32s\n", tabs, header.system_identifier);
  printf("%svolume_identifier: %.32s\n", tabs, header.volume_identifier);
  printf("%svolume_space_size: %d\n", tabs, header.volume_space_size.lsb);
  printf("%svolume_set_size: %d\n", tabs, header.volume_set_size.lsb);
  printf("%svolume_sequence_number: %d\n", tabs, header.volume_sequence_number.lsb);
  printf("%slogical_block_size: %d\n", tabs, header.logical_block_size.lsb);
  printf("%spath_table_size: %d\n", tabs, header.path_table_size.lsb);
  printf("%spath_table_1_block_number: %d\n", tabs, header.path_table_1_block_number);
  printf("%spath_table_2_block_number: %d\n", tabs, header.path_table_2_block_number);
  printf("%spath_table_3_block_number: %d\n", tabs, header.path_table_3_block_number);
  printf("%spath_table_4_block_number: %d\n", tabs, header.path_table_4_block_number);
  printf("%sroot_directory_record: wip\n", tabs);
  printf("%svolume_set_identifier: %.128s\n", tabs, header.volume_set_identifier);
  printf("%spublisher_identifier: %.128s\n", tabs, header.publisher_identifier);
  printf("%sdata_preparer_identifier: %.128s\n", tabs, header.data_preparer_identifier);
  printf("%sapplication_identifier: %.128s\n", tabs, header.application_identifier);
  printf("%scopyright_filename: %.37s\n", tabs, header.copyright_filename);
  printf("%sabstract_filename: %.37s\n", tabs, header.abstract_filename);
  printf("%sbibliographic_filename: %.37s\n", tabs, header.bibliographic_filename);
  printf("%svolume_creation_timestamp: %.16s %d\n", tabs, header.volume_creation_timestamp, header.volume_creation_timestamp[16]);
  printf("%svolume_modification_timestamp: %.16s %d\n", tabs, header.volume_modification_timestamp, header.volume_modification_timestamp[16]);
  printf("%svolume_expiration_timestamp: %.16s %d\n", tabs, header.volume_expiration_timestamp, header.volume_expiration_timestamp[16]);
  printf("%svolume_effective_timestamp: %.16s %d\n", tabs, header.volume_effective_timestamp, header.volume_effective_timestamp[16]);
  printf("%sfile_structure_version: %d\n", tabs, header.file_structure_version);
  printf("%sidentifying_signature: %.8s\n", tabs, header.identifying_signature);
  printf("%sflags: %4.4X\n", tabs, header.flags);
  printf("%sstartup_directory: %d\n", tabs, header.startup_directory.lsb);
  tabs[depth-1] = 0;
  printf("%s}\n", tabs);
}

void print_iso9660_header(iso9660_header header, int depth)
{
  assert(depth > 0);
  char tabs[depth+1];
  for (int i = 0; i < depth; i++) tabs[i] = '\t';
  tabs[depth] = 0;

  printf("iso9660_header{\n");
  printf("%sbin: ", tabs);
  print_iso9660_header_bin(header.bin, depth+1);

  printf("%sroot_dir_record: ", tabs);
  print_directory_record(header.root_dir_record, depth+1);

  tabs[depth-1] = 0;
  printf("%s}\n", tabs);
}

iso9660_header read_iso9660_header(FILE *file)
{
  iso9660_header header = {};

  size_t things_read = fread(&header.bin, sizeof(header.bin), 1, file);
  if (things_read < 1) return header;

  memcpy(&header.root_dir_record.bin, header.bin.root_directory_record, 34);

  directory_record_extra(&header.root_dir_record);

  print_iso9660_header(header, 1);

  return header;
}

void write_to_file(FILE *bin_file, FILE *dir_file, directory_record dir_rec)
{
  int sector = dir_rec.bin.location_of_extent.lsb;
  int size = dir_rec.bin.size_of_extent.lsb;

  //printf("size: %d bytes\n", size);

  seek_sector(bin_file, sector);
  read_sector_header(bin_file);

  while (size > 0)
  {
    char bin[2048];
    int read_size = 2048;
    if (read_size > size) read_size = size;

    int bytes_read = fread(bin, 1, read_size, bin_file);
    fwrite(bin, 1, bytes_read, dir_file);

    if (bytes_read < read_size) break;

    sector++;
    seek_sector(bin_file, sector);
    read_sector_header(bin_file);


    size -= 2048;
  }

}

int read_directory_record(FILE *file, directory_record dir_rec)
{
  seek_sector(file, dir_rec.bin.location_of_extent.lsb);
  read_sector_header(file);

  int size = dir_rec.bin.size_of_extent.lsb;

  assert(size <= 2048);

  char buf[size];
  char *ptr = buf;

  size_t things_read = fread(ptr, size, 1, file);
  if (things_read < 1) return 1;

  while (1)
  {
    directory_record_bin *dir_rec_bin = (directory_record_bin *)ptr;
    int size2 = dir_rec_bin->length;

    if (size2 == 0) break;

    directory_record dir_rec2 = {};
    memcpy(&dir_rec2.bin, ptr, size2);
    directory_record_extra(&dir_rec2);
    print_directory_record(dir_rec2, 1);

    if (dir_rec2.bin.identifier_length > 1)
    {
      char file_name[256];
      memcpy(file_name, dir_rec2.bin.identifier, dir_rec2.bin.identifier_length);
      file_name[dir_rec2.bin.identifier_length] = 0;

      if (dir_rec2.bin.flags == 2)
      {
#ifdef CREATE_FILESYSTEM
        int err = mkdir(file_name, 0777);
        if (err) {
          return err;
        }

        err = chdir(file_name);
        if (err) {
          return err;
        }
#endif /* CREATE_FILESYSTEM */

        read_directory_record(file, dir_rec2);

#ifdef CREATE_FILESYSTEM
        chdir("..");
#endif /* CREATE_FILESYSTEM */
      }
#ifdef CREATE_FILESYSTEM
      else
      {
        FILE *file2 = fopen(file_name, "w");

        write_to_file(file, file2, dir_rec2);

        fclose(file2);
      }
#endif /* CREATE_FILESYSTEM */
    }

    ptr += size2;
  }

  return 0;
}

int read_root_directory(FILE *file, iso9660_header header)
{
  int err;
#ifdef CREATE_FILESYSTEM
  err = mkdir("root", 0777);
  if (err) {
    return err;
  }

  err = chdir("root");
  if (err) {
    return err;
  }
#endif /* CREATE_FILESYSTEM */

  err = read_directory_record(file, header.root_dir_record);
  if (err) {
    return err;
  }
  

#ifdef CREATE_FILESYSTEM
  err = chdir("..");
  if (err) {
    return err;
  }
#endif /* CREATE_FILESYSTEM */
}

int read_iso9660_file(FILE *file)
{

  seek_sector(file, 16);
  read_sector_header(file);

  iso9660_header header = read_iso9660_header(file);

  read_root_directory(file, header);

  return 0;
}

int read_iso9660(char *filename)
{
  FILE *file = fopen(filename, "r");

  if (file == NULL)
    return 1;

  int err = read_iso9660_file(file);

  err = fclose(file);

  if (err)
    return err;

  return 0;
}