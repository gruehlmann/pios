#include "fat.h"
#include "sd.h"
#include "uart.h"
#include "rprintf.h"

struct boot_sector *bs;
char bootSector[SECTOR_SIZE];
char fat_table[8*SECTOR_SIZE];

unsigned int root_sector;
unsigned int first_data_sector;

/*
 
int file_strcmp(char* s1, char* s2, unsigned int bytes) {
    int ret = 1;

    for (int i = 0; i < bytes; i++) {
        
	if (s1[i] != s2[i]) {
	    ret = 0;
	    break;
	}

    }

    return ret;
}


char to_upper(char c) {
    
    return (c >= 'a' && c <= 'z') ? c + ('A' - 'a') : c;
    
}


int get_filename(char* name_buffer, char* ext_buffer, char* path) {
    char* o_path = path;
    int ret = 0;
    int buff_i = 0;

    if (*path++ != '/') {
        return ret;
    }

    
    while (*path != '.' && *path != '\0' && *path != '/') {
	name_buffer[buff_i++] = to_upper(*path++);
    }

    
    while (buff_i < 8) {
        name_buffer[buff_i++] = ' ';
    }

    buff_i = 0;
    
    if (*path == '.') {
    
    	path++;
	while (*path != '\0' && *path != '/') {
	    ext_buffer[buff_i++] = to_upper(*path++);
	}
	    
    }
    
    
    while (buff_i < 3) {
        ext_buffer[buff_i++] = ' ';
    }

    
    if (*path == '\0' || *path == '/') {
        ret = path - o_path;
    }

    return ret;
}

/*
 * Initializes FAT16 file system.
 * Returns 1 on success, 0 on 

int fat_init() {
    int ret = 1;

    sd_readblock(0, (unsigned char*) bootSector, SECTOR_SIZE);
    bs = (struct boot_sector*) bootSector;

    if (bs->boot_signature != 0xaa55) {
        ret = 0;
	return ret;
    }

    if (!file_strcmp(bs->fs_type, "FAT16   ", 8)) {
	ret = 0;
	return ret;
    }
    
    esp_printf((func_ptr) uart_send_char, "Total Sectors: %d\n", bs->total_sectors);
    esp_printf((func_ptr) uart_send_char, "Bytes Per Sector: %d\n", bs->bytes_per_sector);
    esp_printf((func_ptr) uart_send_char, "Fat Tables: %d\n", bs->num_fat_tables);
    esp_printf((func_ptr) uart_send_char, "Number Root Directory Entries: %d\n", bs->num_root_dir_entries);

    sd_readblock(bs->num_reserved_sectors, (unsigned char*) fat_table, 8);

    root_sector = bs->num_fat_tables*bs->num_sectors_per_fat + bs->num_reserved_sectors + bs->num_hidden_sectors;
    
    first_data_sector = bs->num_reserved_sectors + (bs->num_fat_tables * bs->num_sectors_per_fat) + 
    			((bs->num_root_dir_entries * 32) + (SECTOR_SIZE - 1)) / SECTOR_SIZE;
    
    return ret;
}


 
int file_open(FILE* f, char* path) {
    int ret = 1;
    unsigned int sector = root_sector;

    struct root_directory_entry* dir_buffer;
    char buffer[8*SECTOR_SIZE];
    char name_buffer[8];     
    char ext_buffer[3];     
    
    while (*path != '\0') {

        
        int traveled = get_filename(name_buffer, ext_buffer, path);

        if (!traveled) {
	    ret = 0;
	    break;
	}

	path += traveled;

   
	sd_readblock(sector, (unsigned char*) buffer, 8);
	    
	dir_buffer = (struct root_directory_entry*) buffer;
	
	int wrong_file_type = 0;

	for (;*dir_buffer->file_name != 0; dir_buffer++) {
	        
	    
	    if (*dir_buffer->file_name == 0xe5)
	        continue;
	        
	    
	    if (file_strcmp(dir_buffer->file_name, name_buffer, 8) && 
	        file_strcmp(dir_buffer->file_extension, ext_buffer, 3)) 
	    {   
	        	
	        
	    	    
	            	        
	        if ((*path == '\0' && (dir_buffer->attribute & (1 << 4))) ||
	            (*path == '/' && !(dir_buffer->attribute & (1 << 4)))) 
	        {
	            wrong_file_type = 1;
	            break;
    		}
	       
	        sector = ((dir_buffer->cluster - 2) * bs->num_sectors_per_cluster) + first_data_sector;
	        break;
	        
	    }
        }
	    

	if (wrong_file_type || *dir_buffer->file_name == 0) {
	   ret = 0;
	   break;
	}
    }
    
    if (ret) {
        f->rde = *dir_buffer;
        f->start_cluster = dir_buffer->cluster;
    }
    
    return ret;
}

int read_file(FILE* f, void* buf, unsigned int bytes) {

    int bytes_read = 0;
    int bytes_per_cluster = bs->num_sectors_per_cluster*bs->bytes_per_sector;
    
    unsigned short curr_cluster = f->start_cluster;
    unsigned short curr_sector = ((curr_cluster - 2) * bs->num_sectors_per_cluster) + first_data_sector;
    
    unsigned char* res_buffer = (unsigned char*) buf;
    unsigned char clus_buffer[8*SECTOR_SIZE];

    while (bytes > 0) {

	
        sd_readblock(curr_sector, clus_buffer, bs->num_sectors_per_cluster);
	
	
	unsigned int bytes_needed = (bytes < bytes_per_cluster) ? bytes :  bytes_per_cluster;
	for (int i = 0; i < bytes_needed; i++) {
	    *res_buffer++ = clus_buffer[i];
	    bytes--; bytes_read++;
	    
	}

	
	unsigned int index = (curr_cluster * 2) % bs->bytes_per_sector;
	curr_cluster = *(unsigned short*)&fat_table[index];
	if (curr_cluster >= 0xfff8) {
	    break;
	}
	curr_sector = ((curr_cluster - 2) * bs->num_sectors_per_cluster) + first_data_sector;
    }

    return bytes_read;
}
