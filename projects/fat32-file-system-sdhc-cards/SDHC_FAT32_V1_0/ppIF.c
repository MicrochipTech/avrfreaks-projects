//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************
// Top level routines used by application
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************

#ifdef DEBUG_ON
void fat_debug_fat(unsigned long cluster)
{
	unsigned char tempchar=20;
	
	DEBUG_P(PSTR("FAT32 - **DEBUG** FAT TABLE LIST\r\n"));
	while(tempchar--)
	{
		DEBUG_ULONG_FIXED(cluster, 10);
		switch(cluster_type_read(cluster))
		{
			case CLUSTER_TYPE_FREE:
				DEBUG_P(PSTR("  free ...\r\n"));
				break;
			case CLUSTER_TYPE_RESERVED:
				DEBUG_P(PSTR("  RESERVED\r\n"));
				break;
			case CLUSTER_TYPE_USED:
				DEBUG_P(PSTR("  LINK  -> "));
				DEBUG_ULONG_FIXED(cluster_link_read(cluster), 10);
				DEBUG_P(PSTR("\r\n"));
				break;
			case CLUSTER_TYPE_LAST:
				DEBUG_P(PSTR("  FINAL -> << CHAIN END >>\r\n"));
				break;
		};
		cluster++;
	};
}

void fat_debug_dir(unsigned long base_cluster, unsigned int index)
{
	unsigned int total_entries=0, tempint=20;

	struct directory_struct *directory_ptr;
	struct lfn_struct		*lfn_ptr;
	
	DEBUG_P(PSTR("FAT32 - **DEBUG** Directory table @ cluster "));DEBUG_ULONG(base_cluster);DEBUG_P(PSTR("\r\n\r\n"));
	
	//determine size of directory by counting how many clusters it contains
	total_entries = chain_length(base_cluster, NULL) << (sectors_per_cluster_power+4);
	DEBUG_P(PSTR("MAX DIRECTORY INDEX = "));
	DEBUG_ULONG(total_entries);
	DEBUG_P(PSTR("\r\n\r\n"));

	do
	{
		DEBUG_ULONG_FIXED(index, 6);
		directory_ptr=dir_entry_get_ptr(base_cluster, index);
		if(directory_ptr)
		{
			lfn_ptr=(struct lfn_struct*)directory_ptr;
			
			if(dir_type(directory_ptr)==DIR_TYPE_UNUSED)
			{
				DEBUG_P(PSTR(" FREE\r\n"));
			}
			else if(dir_type(directory_ptr)==DIR_TYPE_ERASED)
			{
				DEBUG_P(PSTR(" ERASED"));
				DEBUG_P(PSTR(" ATTRIB=0x"));
				DEBUG_HEX(directory_ptr->attributes);
				
				if(directory_ptr->attributes!=0x0F)
				{
					DEBUG_P(PSTR(" SIZE="));
					DEBUG_ULONG_FIXED(LEND_ULONG_READ(directory_ptr->size),10);
					DEBUG_P(PSTR(" CLUST="));
					DEBUG_ULONG_FIXED(((unsigned long)LEND_UINT_READ(directory_ptr->cluster_h)<<16) + LEND_UINT_READ(directory_ptr->cluster_l),10);
					DEBUG_P(PSTR(" \""));
					dir_name_short(directory_ptr, text);
					text[0]='_';
					DEBUG(text);
					DEBUG_P(PSTR("\""));
				}
				else
					DEBUG_P(PSTR(" (LFN)"));
				DEBUG_P(PSTR("\r\n"));
			}
			else if(dir_type(directory_ptr)==DIR_TYPE_USED)
			{
				DEBUG_P(PSTR(" USED  "));
				DEBUG_P(PSTR(" ATTRIB=0x"));
				DEBUG_HEX(directory_ptr->attributes);
				DEBUG_P(PSTR(" SIZE="));
				DEBUG_ULONG_FIXED(LEND_ULONG_READ(directory_ptr->size),10);
				DEBUG_P(PSTR(" CLUST="));
				DEBUG_ULONG_FIXED(((unsigned long)LEND_UINT_READ(directory_ptr->cluster_h)<<16) + LEND_UINT_READ(directory_ptr->cluster_l),10);
				DEBUG_P(PSTR(" \""));
				DEBUG(dir_name_short(directory_ptr, text));
				DEBUG_P(PSTR("\" CHK=0x"));
				DEBUG_HEX(dir_lfn_checksum(directory_ptr));
				DEBUG_P(PSTR("\r\n"));
			}
			else if(dir_type(directory_ptr)==DIR_TYPE_LFN)
			{
				DEBUG_P(PSTR(" LFN   "));
				DEBUG_P(PSTR(" ATTRIB=0x"));
				DEBUG_HEX(directory_ptr->attributes);
				DEBUG_P(PSTR("                                        SEQ=0x"));
				DEBUG_HEX(lfn_ptr->sequence);
				DEBUG_P(PSTR(" CHK=0x"));
				DEBUG_HEX(lfn_ptr->checksum);
				DEBUG_P(PSTR("\r\n"));
			};
		}
		else
			DEBUG_P(PSTR(" ERROR\r\n"));
		
		index++;
		
		if(index==total_entries)
			tempint=0;
		
	}while(tempint--);
}
#else
void fat_debug_fat(unsigned long cluster)
{
	cluster++;	//prevent compiler warning about un-uesd variables
};

void fat_debug_dir(unsigned long base_cluster, unsigned int index)
{
	base_cluster++;	//prevent compiler warning about un-uesd variables
	index++;
};
#endif


char fat_file_read(struct fat_file_struct *file_info_ptr, void* target, unsigned long address, unsigned long size)
{
	return chain_rw(&file_info_ptr->chain, target, address, size, FAT_READ);
}

char fat_file_write(struct fat_file_struct *file_info_ptr, void* source, unsigned long address, unsigned long size)
{
	char success;
	success = chain_rw(&file_info_ptr->chain, source, address, size, FAT_WRITE);
	fatcache_clean();
	return success;
}

char fat_file_append(struct fat_file_struct *file_info_ptr, void* source, unsigned long size)
{
	char success=TRUE;
	unsigned long total_size;
	unsigned long total_clusters;
	unsigned long new_chain=0;
	struct directory_struct *directory_ptr;

	//check that file_info actually contains directory information
	if(!file_info_ptr->cluster_directory)
		success=FALSE;
	else
	{
		//calculate required clusters
		total_size = file_info_ptr->size + size;
		total_clusters = clusters_to_hold_bytes(total_size);
		
		//attempt to extend chain, if there is one, otherwise create one
		if(file_info_ptr->chain.base)
			success=chain_extend_to(file_info_ptr->chain.base, total_clusters);
		else
		{
			new_chain=chain_create(total_clusters);
			if(!new_chain)
				success=FALSE;
		};
	};

	if(success)
	{
		//get pointer to directory entry
		directory_ptr=dir_entry_get_ptr(file_info_ptr->cluster_directory, file_info_ptr->directory_index);
		if(directory_ptr)
		{
			//if we created a new chain, add its base cluster to directory entry, and file_info
			if(new_chain)
			{
				directory_ptr->cluster_h = LEND_UINT_WRITE((unsigned int)(new_chain>>16));
				directory_ptr->cluster_l = LEND_UINT_WRITE((unsigned int)new_chain);
				chain_init(&file_info_ptr->chain, new_chain);
			};
			
			//update new size in directory entry
			directory_ptr->size = LEND_ULONG_WRITE(total_size);
			
			//update modified and accessed date/time in directory entry
			directory_ptr->modified_time = LEND_UINT_WRITE(dir_time_entry(fat_date_time.hour, fat_date_time.min, fat_date_time.second));
			directory_ptr->modified_date = LEND_UINT_WRITE(dir_date_entry(fat_date_time.year, fat_date_time.month, fat_date_time.day));
			directory_ptr->access_date = directory_ptr->modified_date;
			
			fatcache_dirty(directory_ptr);
			
			//write new data to chain
			success=chain_rw(&file_info_ptr->chain, source, file_info_ptr->size, size, FAT_WRITE);
			
			file_info_ptr->size=total_size;
		}
		else
			success=FALSE;
	};
	fatcache_clean();
	return success;
}

//locate a file
char fat_file_locate(struct fat_file_struct *file_info_ptr, unsigned long directory, char* name)
{
	char success;
	unsigned int index;
	struct directory_struct *directory_ptr=NULL;
	unsigned long templong;
	
	success = dir_search_name(directory, name, &index);
	if(success)
	{
		directory_ptr = dir_entry_get_ptr(directory, index);
		if(directory_ptr)
		{
			if( directory_ptr->attributes & 0xD0 || directory_ptr->attributes==0x0F )
				success = FALSE;
			else
			{
				templong = LEND_UINT_READ(directory_ptr->cluster_h);
				templong <<= 16;
				templong += LEND_UINT_READ(directory_ptr->cluster_l);
				chain_init(&file_info_ptr->chain, templong);
				
				file_info_ptr->cluster_directory = directory;
				file_info_ptr->size = LEND_ULONG_READ(directory_ptr->size);
				file_info_ptr->directory_index = index;
				DEBUG_P(PSTR("FAT32 - File found DIR@"));
				DEBUG_ULONG(directory);
				DEBUG_P(PSTR(" INDEX="));
				DEBUG_ULONG((unsigned long)file_info_ptr->directory_index);
				DEBUG_P(PSTR(" FILE@"));
				DEBUG_ULONG(file_info_ptr->chain.base);
				DEBUG_P(PSTR(" SIZE="));
				DEBUG_ULONG(file_info_ptr->size);
				DEBUG_P(PSTR("\r\n"));
			};
		}
		else
			success=FALSE;
	};
	return success;
}

//change to a new directory
char fat_directory_change(unsigned long* current_directory_ptr, char* name)	//can be . .. or directory name, returns success
{
	char retval=TRUE;
	unsigned long new_cluster=0ul;
	unsigned int directory_index;
	struct directory_struct *directory_ptr;

	//if we find the name
	if(dir_search_name(*current_directory_ptr, name, &directory_index))
	{
		//if we can read the directory entry (after the name search, we should already have it in cache anyway)
		if((directory_ptr = dir_entry_get_ptr(*current_directory_ptr, directory_index)))
		{
			//is it a sub-directory?
			if(directory_ptr->attributes & 0x10)
			{
				//get target cluster #
				new_cluster = (((unsigned long)LEND_UINT_READ(directory_ptr->cluster_h))<<16 )+ LEND_UINT_READ(directory_ptr->cluster_l);
				//.. entries which access root directory have target of 0, which we must convert to root target of 2
				if(!strcmp_P(name, PSTR("..")) && new_cluster==0ul)
					new_cluster=2ul;
			};
		};
	};

	if(new_cluster)
		*current_directory_ptr=new_cluster;
	else
		retval=FALSE;

	return retval;
}

//delete a sub-directory, only works if the sub-directory is empty
char fat_directory_delete(unsigned long parent_directory, char* name)
{
	char success=TRUE, finished;
	unsigned long child_cluster=0;
	struct directory_struct *directory_ptr;
	unsigned int parent_index, child_index, total;
	unsigned char type;

	//check ID-10-T
	if( !strcmp_P(name, PSTR("..")) || !strcmp_P(name, PSTR(".")) )
		success=FALSE;

	//find sub-directory name
	if(success)
		success=dir_search_name(parent_directory, name, &parent_index);

	//read sub-directory entry (after the name search, we should already have it in the buffer anyway)
	if(success)
	{
		directory_ptr = dir_entry_get_ptr(parent_directory, parent_index);
		if(!directory_ptr)
			success=FALSE;
	};

	//get base cluster of sub-directory
	if(success)
	{
		if(directory_ptr->attributes & 0x10)
			child_cluster = (((unsigned long)LEND_UINT_READ(directory_ptr->cluster_h))<<16 )+ LEND_UINT_READ(directory_ptr->cluster_l);
		if(!child_cluster)
			success=FALSE;
	};

	//scan sub-directory for any used entries which are not . or .. abort if found
	if(success)
	{
		total=chain_length(child_cluster, NULL) << (sectors_per_cluster_power+4);
		DEBUG_P(PSTR("FAT32 - Testing directory empty, "));
		DEBUG_ULONG((unsigned long)total);
		DEBUG_P(PSTR(" entries\r\n"));
		child_index=0;
		finished=FALSE;
		if(!total)
		{
			finished=TRUE;
			success=FALSE;
		}
		else do
		{
			directory_ptr = dir_entry_get_ptr(child_cluster, child_index);
			if(directory_ptr)
			{
				type=dir_type(directory_ptr);
				
				//if we find a used entry which does not start with a . directory is not empty
				if(type == DIR_TYPE_USED && directory_ptr->filename_dos[0] !='.')
				{
					success=FALSE;
					finished=TRUE;
				}
				else if(type == DIR_TYPE_UNUSED)
					finished=TRUE;
			}
			else
			{
				success=FALSE;
				finished=TRUE;
			};
			
			child_index++;
			if(child_index==total)
				finished=TRUE;
		}while(!finished);
	};

	//delete childs entry in parent directory, also delete any preceeding LFN entries
	if(success)
	{
		//get pointer to directory entry
		directory_ptr=dir_entry_get_ptr(parent_directory, parent_index);
		
		if(directory_ptr)
		{
			//mark main directory entry as deleted
			directory_ptr->filename_dos[0]=0xE5;
			fatcache_dirty(directory_ptr);
			
			//also erase any preceeding lfn entries
			finished=FALSE;
			if(parent_index==0)
				finished=TRUE;
			while(!finished)
			{
				parent_index--;
				directory_ptr=dir_entry_get_ptr(parent_directory, parent_index);
				if(directory_ptr)
				{
					if(dir_type(directory_ptr)==DIR_TYPE_LFN)
					{
						directory_ptr->filename_dos[0]=0xE5;
						fatcache_dirty(directory_ptr);
					}
					else
						finished=TRUE;
				}
				else
				{
					success=FALSE;
					finished=TRUE;
				};
				if(parent_index==0)
					finished=TRUE;
			};
		}
		else
			success=FALSE;
	};
	
	//erase childs chain
	if(success)
		success=chain_erase(child_cluster);
	
	fatcache_clean();
	return success;
}

/*
pass this a pointer to a structure containing the current directory cluster#, and the file size
if successful, this will return file chain, and it's index in the directory.
*/
char fat_file_create(struct fat_file_struct *file_info_ptr, unsigned long directory, char* name, unsigned long size)
{
	char success;
	struct directory_struct *directory_ptr;
	unsigned long file_cluster = 0;
	
	file_info_ptr->cluster_directory = directory;
	file_info_ptr->size=size;
	
	//create directory entry
	success = dir_name_add(name, file_info_ptr->cluster_directory, &file_info_ptr->directory_index);
	if(success)
	{
		//create file chain, if required
		if(file_info_ptr->size)
		{
			file_cluster=chain_create(clusters_to_hold_bytes(file_info_ptr->size));
			if(!file_cluster)
				success=FALSE;
			else
				chain_init(&file_info_ptr->chain, file_cluster);
		}
		else
			chain_init(&file_info_ptr->chain, 0ul);
		
		//update directory entry with file info
		if(success)
		{
			if((directory_ptr=dir_entry_get_ptr(file_info_ptr->cluster_directory, file_info_ptr->directory_index)))
			{
				directory_ptr->attributes=0x00;
				directory_ptr->size = LEND_ULONG_WRITE(file_info_ptr->size);
				directory_ptr->cluster_h = LEND_UINT_WRITE((unsigned int)(file_cluster>>16));
				directory_ptr->cluster_l = LEND_UINT_WRITE((unsigned int)file_cluster);
				fatcache_dirty(directory_ptr);
			}
			else
				success=FALSE;
		};
	};

	//if we fail, do not leave file_info structure partially initialized
	if(!success)
	{
		chain_init(&file_info_ptr->chain, 0ul);
		file_info_ptr->cluster_directory = 0;
		file_info_ptr->directory_index = 0;
		file_info_ptr->size = size;
	};

	fatcache_clean();
	return success;
}

char fat_file_delete(struct fat_file_struct *file_info_ptr)
{
	char success=TRUE, finished;
	struct directory_struct *directory_ptr;
	unsigned char index;
	
	index=file_info_ptr->directory_index;
	
	//get pointer to directory entry
	directory_ptr=dir_entry_get_ptr(file_info_ptr->cluster_directory, index);
	
	if(directory_ptr)
	{
		//mark main directory entry as deleted
		directory_ptr->filename_dos[0]=0xE5;
		fatcache_dirty(directory_ptr);
		
		//also erase any preceeding lfn entries
		finished=FALSE;
		if(index==0)
			finished=TRUE;
		while(!finished)
		{
			index--;
			directory_ptr=dir_entry_get_ptr(file_info_ptr->cluster_directory, index);
			if(directory_ptr)
			{
				if(dir_type(directory_ptr)==DIR_TYPE_LFN)
				{
					directory_ptr->filename_dos[0]=0xE5;
					fatcache_dirty(directory_ptr);
				}
				else
					finished=TRUE;
			}
			else
			{
				success=FALSE;
				finished=TRUE;
			};
			if(index==0)
				finished=TRUE;
		};
		
		//erase file chain, if there is one
		if(success)
		{
			if(file_info_ptr->chain.base)
			{
				success=chain_erase(file_info_ptr->chain.base);
			};
		};
	}
	else
		success=FALSE;
	
	if(success)
	{
		chain_init(&file_info_ptr->chain, 0ul);
		file_info_ptr->cluster_directory=0;
		file_info_ptr->size=0;
		file_info_ptr->directory_index=0;
	};

	fatcache_clean();
	return success;
}

/*
create a new sub-directory under the current directory, returns success
*/
char fat_directory_create(unsigned long parent_directory, char* name)
{
	char success;
	unsigned int index;
	struct directory_struct *directory_ptr;
	unsigned long child_directory=0;

	//create parent directorys entry for child directorys name
	success=dir_name_add(name, parent_directory, &index);

	//create new chain to hold child directory
	if(success)
		child_directory=chain_create(1);
	if(!child_directory)
		success=FALSE;

	//erase new chain to 0's
	if(success)
	{
		DEBUG_P(PSTR("FAT32 - CreateDIR, erasing new chain\r\n"));
		chain_init(&dir_chain_nav, child_directory);
		success=chain_rw(&dir_chain_nav, NULL, 0ul, ((unsigned long)sectors_per_cluster)<<9, FAT_ERASE);
	};

	//read child directorys entry in parent directory
	if(success)
	{
		DEBUG_P(PSTR("FAT32 - CreateDIR, reading entry in parent DIR\r\n"));
		directory_ptr=dir_entry_get_ptr(parent_directory, index);
		if(!directory_ptr)
			success=FALSE;
	};

	//connect child directorys entry in parent directory to child directorys new chain
	if(success)
	{
		DEBUG_P(PSTR("FAT32 - CreateDIR, linking entry in parent DIR to new chain\r\n"));
		DEBUG_P(PSTR("FAT32 - CreateDIR, creating . entry\r\n"));
		directory_ptr->attributes=0x10;
		directory_ptr->size=0ul;
		directory_ptr->cluster_h=LEND_UINT_WRITE((unsigned int)(child_directory>>16));
		directory_ptr->cluster_l=LEND_UINT_WRITE((unsigned int)child_directory);
		fatcache_dirty(directory_ptr);
		
		//add child directory entry "."
		success=dir_name_add(".", child_directory, &index);
	};

	//read child directorys "." entry
	if(success)
	{
		DEBUG_P(PSTR("FAT32 - CreateDIR, reading . entry\r\n"));
		directory_ptr=dir_entry_get_ptr(child_directory, index);
		if(!directory_ptr)
			success=FALSE;
	};

	//connect schild directorys "." entry to child directorys chain
	if(success)
	{
		DEBUG_P(PSTR("FAT32 - CreateDIR, connecting . entry to new chain\r\n"));
		DEBUG_P(PSTR("FAT32 - CreateDIR, creating .. entry\r\n"));
		directory_ptr->attributes=0x10;
		directory_ptr->size=0;
		directory_ptr->cluster_h=LEND_UINT_WRITE((unsigned int)(child_directory>>16));
		directory_ptr->cluster_l=LEND_UINT_WRITE((unsigned int)child_directory);
		fatcache_dirty(directory_ptr);
		
		//add child directory entry ".."
		success=dir_name_add("..", child_directory, &index);
	};
	
	//read child directorys ".." entry
	if(success)
	{
		DEBUG_P(PSTR("FAT32 - CreateDIR, reading .. entry\r\n"));
		directory_ptr=dir_entry_get_ptr(child_directory, index);
		if(!directory_ptr)
			success=FALSE;
	};
	
	//connect child directorys ".." entry to parent directorys chain
	if(success)
	{
		DEBUG_P(PSTR("FAT32 - CreateDIR, connecting .. entry to parent chain\r\n"));
		directory_ptr->attributes=0x10;
		directory_ptr->size=0;
		if(parent_directory != 2ul)
		{
			directory_ptr->cluster_h=LEND_UINT_WRITE((unsigned int)(parent_directory>>16));
			directory_ptr->cluster_l=LEND_UINT_WRITE((unsigned int)parent_directory);
		}
		else
		{
			directory_ptr->cluster_h=0;
			directory_ptr->cluster_l=0;
		};
		fatcache_dirty(directory_ptr);
	};
	
	fatcache_clean();
	return success;
}

//list directory entries, short name, starting cluster, size, and long name if available
void fat_directory_list(unsigned long base_cluster)
{
	char success=TRUE, finished=FALSE;
	unsigned int total_entries=0;
	unsigned int index;
	unsigned char lfn_checksum=0;
	unsigned char lfn_sequence=0;
	
	struct directory_struct *directory_ptr=NULL;
	struct lfn_struct		*lfn_ptr;

	DIRTXT_P(PSTR("Directory @ cluster "));DIRTXT_ULONG(base_cluster);DIRTXT_P(PSTR("\r\n\r\n"));

	//determine size of directory by counting how many clusters it contains
	total_entries = chain_length(base_cluster, NULL) << (sectors_per_cluster_power+4);

	index=0;
	long_file_name[0]=0;
	do
	{
		directory_ptr = dir_entry_get_ptr(base_cluster, index);
		
		if(directory_ptr)
		{
			if(dir_type(directory_ptr)==DIR_TYPE_USED)
			{
				if(directory_ptr->attributes & 0x10)
					DIRTXT_P(PSTR("<DIR>  "));
				else
					DIRTXT_P(PSTR("<FILE> "));
				
				DIRTXT(dir_name_short(directory_ptr, text));
				DIRTXT_P(PSTR(" @CLUST "));
				DIRTXT_ULONG_FIXED(((unsigned long)LEND_UINT_READ(directory_ptr->cluster_h)<<16) + LEND_UINT_READ(directory_ptr->cluster_l),10);
				
				if(directory_ptr->attributes & 0x10)
					DIRTXT_P(PSTR("                "));
				else
				{
					DIRTXT_P(PSTR(" SIZE="));
					DIRTXT_ULONG_FIXED(LEND_ULONG_READ(directory_ptr->size),10);
				};
				
				if(long_file_name[0])
				{
					if(lfn_checksum == dir_lfn_checksum(directory_ptr))
					{
						DIRTXT_P(PSTR(" \""));
						DIRTXT(long_file_name);
						DIRTXT_P(PSTR("\""));
					};
				};
				DIRTXT_P(PSTR("\r\n"));
			}
			else if(dir_type(directory_ptr)==DIR_TYPE_LFN)
			{
				lfn_ptr = (struct lfn_struct*)directory_ptr;
				//sequence start
				if(lfn_ptr->sequence & 0x40)
				{
					//store initial text, init checksum and sequence
					lfn_sequence=lfn_ptr->sequence & 0x3F;
					lfn_checksum=lfn_ptr->checksum;
					long_file_name[lfn_sequence*13]=0;
					lfn_sequence--;
					dir_lfn_text_get(lfn_ptr, &long_file_name[lfn_sequence*13]);
				}
				//valid sequence?
				else if(lfn_ptr->sequence == lfn_sequence && lfn_ptr->checksum == lfn_checksum)
				{
					lfn_sequence--;
					dir_lfn_text_get(lfn_ptr, &long_file_name[lfn_sequence*13]);
				}
				//invalid lfn
				else
				{
					long_file_name[0]=0;
				};
			}
			else if(dir_type(directory_ptr)==DIR_TYPE_UNUSED)
			{
				finished=TRUE;
			};
		}	
		else
		{
			DEBUG_P(PSTR("FAT32 - DIR ERROR, UNABLE TO READ CHAIN\r\n"));
			success=FALSE;
			finished=TRUE;
		};
		
		index++;
		
		if(index==total_entries)
			finished=TRUE;
		
	}while(!finished);
}
