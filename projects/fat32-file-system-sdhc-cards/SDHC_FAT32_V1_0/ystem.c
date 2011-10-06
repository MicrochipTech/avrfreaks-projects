
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************
//System, for reading partition information, and read/write fat table entries and fsinfo
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************



	//partition info used in master boot record
	struct partition_info_struct
	{
		unsigned char	status;				//0x80 - active partition
		unsigned char 	head_start;			//starting head
		unsigned int	cyl_sect_start;		//starting cylinder and sector
		unsigned char	type;				//partition type 
		unsigned char	head_end;			//ending head of the partition
		unsigned int	cyl_sect_end;		//ending cylinder and sector
		unsigned long	sector_first;		//total sectors between MBR & the first sector of the partition
		unsigned long	sector_total;		//size of this partition in sectors
	};

	//master boot record
	struct master_boot_record_struct
	{
		unsigned char					nothing[446];		//ignore, placed here to fill the gap in the structure
		struct partition_info_struct 	partition_info[4];	//partition info
		unsigned int					signature;			//0xaa55
	};

	//Boot sector
	struct boot_sector_struct
	{
		unsigned char jump_boot[3]; //default: 0x009000EB
		unsigned char oem_name[8];
		unsigned int bytes_per_sector; //deafault: 512
		unsigned char sectors_per_cluster;
		unsigned int total_reserved_sectors;
		unsigned char number_of_fats;
		unsigned int root_entry_count;
		unsigned int total_sectors_fat16; //must be 0 for FAT32
		unsigned char media_type;
		unsigned int fat_size_fat16; //must be 0 for FAT32
		unsigned int sectors_per_track;
		unsigned int number_of_heads;
		unsigned long hidden_sectors;
		unsigned long total_sectors_fat32;
		unsigned long fat_size_fat32; //count of sectors occupied by one FAT
		unsigned int ext_flags;
		unsigned int version; 		//0x0000 (defines version 0.0)
		unsigned long root_cluster; //first cluster of root directory (=2)
		unsigned int fsinfo_sector; //sector number of FSinfo structure (=1)
		unsigned int backup_boot_sector;
		unsigned char reserved[12];
		unsigned char drive_number;
		unsigned char reserved1;
		unsigned char boot_signature;
		unsigned long volume_serial;
		unsigned char volume_name[11]; //"NO NAME "
		unsigned char file_system_name[8]; //"FAT32"
		unsigned char boot_data[420];
		unsigned int boot_end_signature; //0xaa55
	};

	//FS info sector
	struct fsinfo_sector_struct
	{
		char signature1[4];	//"RRaA"
		char reserved1[480];
		char signature2[4];	//"rrAa"
		unsigned long total_free_clusters;
		unsigned long last_allocated_cluster;
		char reserved2[12];
		char signature3[4];	//(0x00 0x00 0x55 0xAA)
	};



//find fat32 partition and get info
char fat_init(void)
{
	char success;
	struct boot_sector_struct *boot_sector_ptr;
	struct master_boot_record_struct *master_boot_record_ptr;
	struct partition_info_struct *partition_info_ptr;
	struct fsinfo_sector_struct *fsinfo_sector_ptr=NULL;
	unsigned long templong;
	unsigned char tempchar;
	
	fat_date_time.year=11;
	fat_date_time.month=9;
	fat_date_time.day=23;
	fat_date_time.hour=11;
	fat_date_time.min=02;
	fat_date_time.second=15;

	fatcache_init();
	
	//read first block
	boot_sector_ptr=fatcache_sector(0ul, FAT_READ);

	master_boot_record_ptr = (struct master_boot_record_struct*)boot_sector_ptr;

	//is NOT the boot sector?
	if( (boot_sector_ptr->jump_boot[0]!=0xE9) && (boot_sector_ptr->jump_boot[0]!=0xEB) )
	{
		//is it the master boot record?
		if(LEND_UINT_READ(master_boot_record_ptr->signature) == 0xAA55)
		{
			DEBUG_P(PSTR("FAT32 - Master boot record found\r\n"));
			partition_info_ptr = &master_boot_record_ptr->partition_info[0];	//get info for first partition
			
			first_reserved_sector=LEND_ULONG_READ(partition_info_ptr->sector_first);
			DEBUG_P(PSTR("FAT32 - Reading first partition boot sector at block "));
			DEBUG_ULONG(first_reserved_sector);
			DEBUG_P(PSTR("\r\n"));
			boot_sector_ptr=fatcache_sector(first_reserved_sector, FAT_READ);
		};
	}
	else
		first_reserved_sector=0;
	
	//if we still dont have the boot sector, were stuffed.
	if( (boot_sector_ptr->jump_boot[0]!=0xE9) && (boot_sector_ptr->jump_boot[0]!=0xEB) )
	{
		DEBUG_P(PSTR("FAT32 - Failed to find boot sector\r\n"));
		success=FALSE;
	}
	else
	{
		success=TRUE;
		bytes_per_sector = LEND_UINT_READ(boot_sector_ptr->bytes_per_sector);
		sectors_per_cluster = (unsigned char)LEND_UINT_READ(boot_sector_ptr->sectors_per_cluster);
		
		total_reserved_sectors = LEND_UINT_READ(boot_sector_ptr->total_reserved_sectors);
		
		first_fat_sector = first_reserved_sector+total_reserved_sectors;
		total_fats = boot_sector_ptr->number_of_fats;
		fat_size = LEND_ULONG_READ(boot_sector_ptr->fat_size_fat32);
		total_fat_sectors =  total_fats * fat_size;
		first_data_sector = first_fat_sector + total_fat_sectors;
		total_data_sectors = LEND_ULONG_READ(boot_sector_ptr->total_sectors_fat32) - total_fat_sectors - total_reserved_sectors;
		
		fsinfo_sector = LEND_ULONG_READ(boot_sector_ptr->fsinfo_sector)+first_reserved_sector;
		
		root_cluster = LEND_ULONG_READ(boot_sector_ptr->root_cluster);
		total_clusters = total_data_sectors / (unsigned long)sectors_per_cluster;
		
		fsinfo_sector_ptr=fatcache_sector(fsinfo_sector, FAT_READ);
		if(fsinfo_sector_ptr)
		{
			if(memcmp_P(fsinfo_sector_ptr->signature1, PSTR("RRaA"),4))
				success=FALSE;
			if(memcmp_P(fsinfo_sector_ptr->signature2, PSTR("rrAa"),4))
				success=FALSE;
			if(memcmp_P(fsinfo_sector_ptr->signature3, PDATA(char,{0x00,0x00,0x55,0xAA}),4))
				success=FALSE;
		}
		else
			success=FALSE;
		
		if(!success)
		{
			DEBUG_P(PSTR("FAT32 - Failed to find FS Info sector at "));
			DEBUG_ULONG(fsinfo_sector);
			DEBUG_P(PSTR("\r\n"));
		};
	};
	
	if(success)
	{
		//if total free clusters is invalid
		if(LEND_ULONG_READ(fsinfo_sector_ptr->total_free_clusters) > total_clusters)
		{
			DEBUG_P(PSTR("FAT32 - Counting free clusters...\r\n"));
			templong=cluster_count_free();
			
			fsinfo_sector_ptr=fatcache_sector(fsinfo_sector, FAT_READ);
			if(fsinfo_sector_ptr)
			{
				fsinfo_sector_ptr->total_free_clusters = templong;
				fatcache_dirty(fsinfo_sector_ptr);
				fatcache_clean();
			}
			else
				success=FALSE;
		};
		
		DEBUG_P(PSTR("FAT32 - Bytes/Sector          = "));DEBUG_ULONG(bytes_per_sector);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - Sectors/Cluster       = "));DEBUG_ULONG(sectors_per_cluster);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - FSinfo sector         = "));DEBUG_ULONG(fsinfo_sector);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - First reserved sector = "));DEBUG_ULONG(first_reserved_sector);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - Reserved sectors      = "));DEBUG_ULONG(total_reserved_sectors);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - First fat sector      = "));DEBUG_ULONG(first_fat_sector);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - Number of fat tables  = "));DEBUG_ULONG(total_fats);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - Size of fat table     = "));DEBUG_ULONG(fat_size);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - Fat sectors           = "));DEBUG_ULONG(total_fat_sectors);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - First data sector     = "));DEBUG_ULONG(first_data_sector);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - Data sectors          = "));DEBUG_ULONG(total_data_sectors);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - Root cluster          = "));DEBUG_ULONG(root_cluster);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - Total clusters        = "));DEBUG_ULONG(total_clusters);DEBUG_P(PSTR("\r\n"));
		DEBUG_P(PSTR("FAT32 - Free clusters         = "));DEBUG_ULONG(LEND_ULONG_READ(fsinfo_sector_ptr->total_free_clusters));DEBUG_P(PSTR("\r\n"));
		
		
		sectors_per_cluster_power=0;
		tempchar=sectors_per_cluster;	//always a power of 2
		while(!(tempchar & 1))
		{
			sectors_per_cluster_power++;
			tempchar>>=1;
		};
	};

	return success;
}

//return total free clusters from fsinfo sector, or 0 if error
static unsigned long fsinfo_total_free_read(void)
{
	struct fsinfo_sector_struct *fsinfo_ptr;
	unsigned long retval=0;
	
	fsinfo_ptr=fatcache_sector(fsinfo_sector, FAT_READ);
	if(fsinfo_ptr)
		retval=LEND_ULONG_READ(fsinfo_ptr->total_free_clusters);

	return retval;
}

//increase total free clusters in fsinfo sector, returns success
static char fsinfo_total_free_increase(unsigned long increment)
{
	char success=TRUE;
	struct fsinfo_sector_struct *fsinfo_ptr;
	
	fsinfo_ptr=fatcache_sector(fsinfo_sector, FAT_READ);
	if(fsinfo_ptr)
	{
		fsinfo_ptr->total_free_clusters = LEND_ULONG_WRITE(LEND_ULONG_READ(fsinfo_ptr->total_free_clusters)+increment);
		fatcache_dirty(fsinfo_ptr);
	}
	else
		success=FALSE;
	return success;
}

//increase total free clusters in fsinfo sector, returns success
static char fsinfo_total_free_decrease(unsigned long decrement)
{
	char success=TRUE;
	struct fsinfo_sector_struct *fsinfo_ptr;
	
	fsinfo_ptr=fatcache_sector(fsinfo_sector, FAT_READ);
	if(fsinfo_ptr)
	{
		fsinfo_ptr->total_free_clusters = LEND_ULONG_WRITE(LEND_ULONG_READ(fsinfo_ptr->total_free_clusters)-decrement);
		fatcache_dirty(fsinfo_ptr);
	}
	else
		success=FALSE;
	return success;
}

//read last allocated cluster, returns 0 if error
static unsigned long fsinfo_last_allocated_read(void)
{
	unsigned long retval;
	struct fsinfo_sector_struct *fsinfo_ptr;

	fsinfo_ptr=fatcache_sector(fsinfo_sector, FAT_READ);
	if(fsinfo_ptr)
		retval=LEND_ULONG_READ(fsinfo_ptr->last_allocated_cluster);
	else
		retval=0ul;

	return retval;
}

//write last allocated cluster to fsinfo, returns success
static char fsinfo_last_allocated_write(unsigned long last_allocated)
{
	char success=TRUE;
	struct fsinfo_sector_struct *fsinfo_ptr;

	fsinfo_ptr = fatcache_sector(fsinfo_sector, FAT_READ);
	if(fsinfo_ptr)
	{
		fsinfo_ptr->last_allocated_cluster=LEND_ULONG_WRITE(last_allocated);
		fatcache_dirty(fsinfo_ptr);
	}
	else
		success=FALSE;
	
	return success;
}

//count how many free clusters there are by scanning the FAT table
static unsigned long cluster_count_free(void)
{
	unsigned long retval=0;
	unsigned long cluster=total_clusters-1;
	do
	{
		if(cluster_type_read(cluster)==CLUSTER_TYPE_FREE)
			retval++;
		cluster--;
	}while(cluster != 1ul);
	return retval;
}

//read cluster type from FAT, returns CLUSTER_TYPE_RESERVED if error
//returns CLUSTER_TYPE_FREE, CLUSTER_TYPE_RESERVED, CLUSTER_TYPE_USED, CLUSTER_TYPE_LAST
static unsigned char cluster_type_read(unsigned long cluster)
{
	unsigned char retval;
	unsigned char entry_index;	
	unsigned long *table_ptr;

	entry_index = cluster&0x0000007Ful;		//entry index within it's sector

	cluster>>=7;					//convert cluster number to sector
	cluster+=first_fat_sector;
	
	//read fat sector containing the entry we want
	table_ptr=fatcache_sector(cluster, FAT_READ);

	if(table_ptr)		
	{
		cluster=LEND_ULONG_READ(table_ptr[entry_index]);	//read entry
		
		if(cluster==0ul)
			retval=CLUSTER_TYPE_FREE;
		else if(0x00000002ul <= cluster && cluster <= 0x0FFFFFEFul)
			retval=CLUSTER_TYPE_USED;
		else if( (cluster & 0xFFFFFFF8ul) == 0x0FFFFFF8ul )	//0x0FFFFFF8 - 0x0FFFFFFF
			retval=CLUSTER_TYPE_LAST;	
		else
			retval=CLUSTER_TYPE_RESERVED;
	}
	else
		retval=CLUSTER_TYPE_RESERVED;

	return retval;
}

// mark a cluster in the FAT as being CLUSTER_TYPE_FREE, CLUSTER_TYPE_LAST, or a link
// returns success
static char cluster_link_write(unsigned long cluster, unsigned long entry)
{
	char success=TRUE;
	unsigned char entry_index;	
	unsigned long *table_ptr;

	entry_index = cluster&0x0000007Ful;		//entry index within it's sector

	cluster>>=7;					//convert cluster number to sector
	cluster+=first_fat_sector;
		
	//read fat sector containing the entry we want
	table_ptr=fatcache_sector(cluster, FAT_READ);
	if(table_ptr)		
	{
		table_ptr[entry_index]=LEND_ULONG_WRITE(entry);		//write entry
		fatcache_dirty(table_ptr);
	}
	else
	{
		success=FALSE;
		DEBUG_P(PSTR("FAT32 - ERROR, unable to update FAT tables\r\n"));
	};
	return success;
}

//read cluster link from FAT, which == the next cluster in the chain
//returns 0 if EOF or reserved or free
static unsigned long cluster_link_read(unsigned long cluster)
{
	unsigned char entry_index;	
	unsigned long *table_ptr;

	entry_index = cluster&0x0000007Ful;		//entry index within it's sector
	

	if(cluster<2)
		cluster=0;
	else
	{
		cluster>>=7;					//convert cluster number to sector
		cluster+=first_fat_sector;
		
		//read fat sector containing the entry we want
		table_ptr=fatcache_sector(cluster, FAT_READ);
		if(table_ptr)		
		{
			cluster=LEND_ULONG_READ(table_ptr[entry_index]);	//read entry
			
			if(cluster < 2ul)
				cluster=0;
			if(cluster > 0x0FFFFFEFul)
				cluster=0;
		}
		else
			cluster=0;
	};

	return cluster;
}

// return first physical sector number of a physical cluster
static unsigned long physec_of_phyclust(unsigned long cluster)
{
	cluster-=root_cluster;	
	cluster <<= sectors_per_cluster_power;

	return first_data_sector + cluster;
}

// return how many clusters are required to hold X amount of bytes
static unsigned long clusters_to_hold_bytes(unsigned long bytes)
{
	if(bytes)
	{
		bytes-=1;
		bytes >>= sectors_per_cluster_power+9;	// divide by (sectors per cluster * bytes per sector)
		bytes+=1;
	};

	return bytes;
}
