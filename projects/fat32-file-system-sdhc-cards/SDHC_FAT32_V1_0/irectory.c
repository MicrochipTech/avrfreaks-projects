//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************
// Directory
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************


// **** Functions ****

/*
Creates a new directory entry, and returns an index to it.
The new entry must be modified afterwards to reference a chain, set attributes, and size
*/
static char dir_name_add(char* name, unsigned long current_directory, unsigned int* new_index)
{
	char success=TRUE, space_found_erased=FALSE, space_found_unused=FALSE;

	char name_short[13], short_valid;
	char lfn_checksum=0, lfn_sequence;

	unsigned char entries_needed, erased_entry_count;
	unsigned int index, index_space_erased=0, total_entries;

	struct directory_struct	*directory_ptr=NULL;
	struct lfn_struct		*lfn_ptr;
	
	short_valid = dir_name_short_make(name, name_short);
	
	DEBUG_P(PSTR("FAT32 - Add entry to directory @ cluster "));
	DEBUG_ULONG((unsigned long)current_directory);
	DEBUG_P(PSTR(" \""));
	if(short_valid)
		DEBUG(name_short);
	else
		DEBUG(name);
	DEBUG_P(PSTR("\"\r\n"));
	
	if(dir_search_name(current_directory, name, NULL))
	{
		DEBUG_P(PSTR("FAT32 - ERROR cannot create entry, name exists\r\n"));
		success=FALSE;
	}
	else
	{
		//calculate number of directory entries required
		if(short_valid)
			entries_needed=1;
		else
			entries_needed=2+strlen(name)/13;
		
		//max number of entries in current directory chain
		total_entries = chain_length(current_directory, NULL) << (sectors_per_cluster_power+4);
		
		//look for space
		index=0;
		erased_entry_count=0;
		do
		{
			directory_ptr=dir_entry_get_ptr(current_directory, index);
			
			if(directory_ptr)
			{	
				//look for erased space
				if(!space_found_erased)
				{
					if(dir_type(directory_ptr) == DIR_TYPE_ERASED)
					{
						erased_entry_count++;
						//if found
						if(erased_entry_count==entries_needed)
						{
							//store index to beginning of erased space
							index_space_erased=index-entries_needed+1;
							space_found_erased=TRUE;
						};
					}
					else
						erased_entry_count=0;
				};
				
				//look for first unused entry
				if(dir_type(directory_ptr) == DIR_TYPE_UNUSED)
					space_found_unused = TRUE;
				else
					index++;
			}
			else
				success=FALSE;
		//search until we reach the end of the chain, or we have an I/O error, or we find an unused entry
		}while(index != total_entries && success && space_found_unused == FALSE);
		
		/*
		One of 3 things will happen now.
		1. We have enough space at the end of the directory table to add our entries (preferred), and still have 1 unused entry to terminate the list
		2. We have enough sequential erased entries to overwrite with our entry (next best)
		3. Neither, we must extend the directory chain to hold new entrys
		
		If we are writing to the end of the directory, we will also write a terminating unused entry at the end.
		
		*/
		
		
		// if index exceeds the end of the chain (==total_entries), and no erased space is found,
		// the chain will be extended so that the index becomes an unused entry in the next cluster
		if(success)
		{
			//if enough room before end of chain (entries required +1 for directory terminator)
			if(total_entries-index > entries_needed)
			{
				space_found_unused=TRUE;
				DEBUG_P(PSTR("FAT32 - Add entry, unused space found at index "));
				DEBUG_ULONG((unsigned long)index);
				DEBUG_P(PSTR("\r\n"));
			}
			//was erased space found?
			else if(space_found_erased)
			{
				//use erased space instead
				index=index_space_erased;
				space_found_unused=FALSE;
				DEBUG_P(PSTR("FAT32 - Add entry, erased space found at index "));
				DEBUG_ULONG((unsigned long)index);
				DEBUG_P(PSTR("\r\n"));
			}
			//if no erased space, try to extend chain
			else
			{
				DEBUG_P(PSTR("FAT32 - Add entry, no space, extending chain, new space will be at index "));
				DEBUG_ULONG((unsigned long)index);
				DEBUG_P(PSTR("\r\n"));
				success=chain_extend_by(current_directory, 1);
				if(success)
				{
					chain_init(&dir_chain_nav, current_directory);
					success=chain_rw(&dir_chain_nav, NULL, total_entries <<5, ((unsigned long)sectors_per_cluster)<<9 , FAT_ERASE);
				};
			};
		};
		//index now points to first entry of available space
		
		//index to last entry +1
		index += entries_needed;
		
		if(success)
		{
			//if we are not overwriting erased space
			if(space_found_unused)
			{
				DEBUG_P(PSTR("FAT32 - Add entry, terminating entry added at index "));
				DEBUG_ULONG((unsigned long)index);
				DEBUG_P(PSTR("\r\n"));
				
				//get pointer to last entry+1
				directory_ptr=dir_entry_get_ptr(current_directory, index);
				if(directory_ptr)
				{
					//ensure directory table is terminated with unused entry
					memset(directory_ptr, 0, sizeof(struct directory_struct));
					fatcache_dirty(directory_ptr);
				}
				else
					success=FALSE;
			};
		}
		else
		{
			DEBUG_P(PSTR("FAT32 - Add entry, ERROR, failed to get space in directory\r\n"));
		};
		
		//index to last entry (main entry)
		index --;	
		
		if(success)
		{			
			//return index of main entry
			*new_index=index;
			
			//get pointer to main entry
			directory_ptr=dir_entry_get_ptr(current_directory, index);
			if(!directory_ptr)
				success=FALSE;
			
		};
		
		if(success)
		{
			//create main directory entry
			memcpy(directory_ptr->filename_dos, name_short, 8);
			memcpy(directory_ptr->filename_ext, &name_short[9], 3);
			
			directory_ptr->attributes=0x00;
			directory_ptr->reserved1=0;
			if(fat_date_time.second & 1)
				directory_ptr->time10ms=100;
			else
				directory_ptr->time10ms=0;
			directory_ptr->create_time = LEND_UINT_WRITE(dir_time_entry(fat_date_time.hour, fat_date_time.min, fat_date_time.second));
			directory_ptr->create_date = LEND_UINT_WRITE(dir_date_entry(fat_date_time.year, fat_date_time.month, fat_date_time.day));
			directory_ptr->access_date = directory_ptr->create_date;
			directory_ptr->cluster_h = 0;
			directory_ptr->modified_time = directory_ptr->create_time;
			directory_ptr->modified_date = directory_ptr->create_date;
			directory_ptr->cluster_l = 0;
			directory_ptr->size = 0;
			fatcache_dirty(directory_ptr);
			
			lfn_checksum = dir_lfn_checksum(directory_ptr);
			
			DEBUG_P(PSTR("FAT32 - Add entry, main entry added at index "));
			DEBUG_ULONG((unsigned long)index);
			DEBUG_P(PSTR("\r\n"));
		}
		else
		{
			DEBUG_P(PSTR("FAT32 - Add entry, ERROR, failed to write main entry at index "));
			DEBUG_ULONG((unsigned long)index);
			DEBUG_P(PSTR("\r\n"));
		};
		
		//index to first entry (first lfn)
		index -= entries_needed-1;
		
		if(success)
		{	
			//need LFN?
			if(entries_needed > 1)
			{
				lfn_sequence = 0x3F + entries_needed;
				
				while(lfn_sequence && success)
				{
					//create LFN					
					lfn_ptr = (struct lfn_struct*)dir_entry_get_ptr(current_directory, index);
					
					if(lfn_ptr)
					{							
						lfn_ptr->sequence = lfn_sequence;
						lfn_ptr->attributes = 0x0F;
						lfn_ptr->reserved1 = 0;
						lfn_ptr->checksum = lfn_checksum;
						lfn_ptr->reserved2 = LEND_UINT_WRITE(0);
						dir_lfn_text_write(lfn_ptr, name, lfn_sequence);
						fatcache_dirty(lfn_ptr);
						
						DEBUG_P(PSTR("FAT32 - Add entry, LFN entry seq="));
						DEBUG_HEX(lfn_sequence);
						DEBUG_P(PSTR(" added at index "));
						DEBUG_ULONG((unsigned long)index);
						DEBUG_P(PSTR("\r\n"));
						lfn_sequence &=0x3F;
						lfn_sequence --;			
						index++;
					}
					else
					{
						success=FALSE;
						DEBUG_P(PSTR("FAT32 - Add entry, ERROR, failed to write LFN entry at index "));
						DEBUG_ULONG((unsigned long)index);
						DEBUG_P(PSTR("\r\n"));
					};
				};
			};
		};
	};
	return success;
}

static int dir_time_entry(int hours, int mins, int seconds)
{
	int retval=0;
	
	retval += hours<<11;
	retval += mins<<5;
	retval += seconds>>1;
	
	return retval;
}

static int dir_date_entry(int year, int month, int day)
{
	int retval=0;
	
	retval += year<<9;
	retval += month<<5;
	retval += day;
	
	return retval;
}

//create write text to a LFN entry
static void dir_lfn_text_write(struct lfn_struct *lfn_ptr, char* name, char sequence)
{
	int *tempint_ptr;
	char terminated=FALSE;
	
	sequence &=0x3F;
	
	//address text for this part of sequence
	name=&name[(sequence-1)*13];
	
	tempint_ptr = lfn_ptr->name_first;
	
	do
	{
		//transfer character, fill unused entries with 0xFFFF
		if(terminated)
			*tempint_ptr = LEND_UINT_WRITE(0xFFFF);
		else
			*tempint_ptr = LEND_UINT_WRITE((unsigned int)(*name));
		
		//if not terminator, point to next char, else write terminator to remaining fields
		if(*name)
			name++;
		else
			terminated=TRUE;
		
		//increment int pointer through lfn fields
		tempint_ptr++;
		if(tempint_ptr == &lfn_ptr->name_first[5])
			tempint_ptr=lfn_ptr->name_middle;
		if(tempint_ptr == &lfn_ptr->name_middle[6])
			tempint_ptr=lfn_ptr->name_last;
		
	}while(tempint_ptr != &lfn_ptr->name_last[2]);
}

//used to locate a file or directory name, returns 0 if not found or error
//returns index within the directory
static char dir_search_name(unsigned long base_cluster, char* name_long, unsigned int *directory_index)
{
	char short_valid, name_short[13], success=TRUE, finished=FALSE;

	unsigned int index;
	unsigned int total_entries=0;
	unsigned char lfn_checksum=0;
	unsigned char lfn_sequence=0;
	unsigned char type;

	struct directory_struct *directory_ptr=NULL;
	struct lfn_struct		*lfn_ptr;

	//Work out short form name, which will either be a CRC of the long name, or the name itself if it fits within 8.3
	short_valid = dir_name_short_make(name_long, name_short);

	//determine maximum size of directory by counting how many clusters it contains
	total_entries = chain_length(base_cluster, NULL) << (sectors_per_cluster_power+4);

	DEBUG_P(PSTR("FAT32 - DIR SEARCH, base="));
	DEBUG_ULONG(base_cluster);
	if(short_valid)
	{
		DEBUG_P(PSTR(" name (8.3)=\""));
		DEBUG(name_short);
	}
	else
	{
		DEBUG_P(PSTR(" name (lfn)=\""));
		DEBUG(name_long);
	};
	DEBUG_P(PSTR("\" directory_size="));
	DEBUG_ULONG((unsigned long)total_entries);
	DEBUG_P(PSTR("\r\n"));

	index=0;
	long_file_name[0]=0;

	do
	{
		//get pointer to directory entry
		directory_ptr = dir_entry_get_ptr(base_cluster, index);
		
		if(directory_ptr)
		{
			//determine type of entry
			type=dir_type(directory_ptr);
			if(type == DIR_TYPE_USED)
			{
				//if short name matches
				if(short_valid)
				{
					//if main part and extension match
					if(!memcmp(name_short, directory_ptr->filename_dos, 8) && !memcmp(&name_short[9], directory_ptr->filename_ext, 3))
					{
						//return directory index
						if(directory_index)
							*directory_index=index;
						//end search
						finished=TRUE;
						success=TRUE;
					};
				}
				//if we have a long name from previous lfn entries, test it's checksum
				else if(long_file_name[0])		
				{
					//if checksum is correct
					if(lfn_checksum == dir_lfn_checksum(directory_ptr) )
					{
						//if long name matches (ignoring case)
						if(!strcmp_nocase(name_long, long_file_name))
						{
							//return directory index
							if(directory_index)
								*directory_index=index;					
							//end search
							finished=TRUE;
							success=TRUE;
						};
					}
					else
						//erase long file name
						long_file_name[0]=0;
				};
			}
			else if(type == DIR_TYPE_LFN && short_valid==FALSE)
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
			else if(type == DIR_TYPE_UNUSED)
			{
				//if final directory entry found, end search
				finished=TRUE;
				success=FALSE;
			};	
		}
		else
		{
			success=FALSE;
			finished=TRUE;
			DEBUG_P(PSTR("FAT32 - DIR SEARCH ERROR, UNABLE TO READ CHAIN\r\n"));
		};
		
		//if end of chain, and not finished, search is unsuccessful
		index++;
		if(index == total_entries && !finished)
		{
			success=FALSE;
			finished=TRUE;
		};
		
	}while(!finished);

	return success;
}

//create a short name 8.3 from long name, returns true if long name can be represented by 8.3
static char dir_name_short_make(char* long_name, char* short_name)
{
	int length_ext=0, length;
	char retval;

	strcpy_P(short_name, PSTR("            "));
	length=strlen(long_name);

	//if not ".."
	if(!(long_name[length-2]=='.' && long_name[length-1]=='.'))
	{
		//find length of extension (1,2, or 3)
		if(long_name[length-2]=='.')
			length_ext=1;
		else if(long_name[length-3]=='.')
			length_ext=2;
		else if(long_name[length-4]=='.')
			length_ext=3;
	};

	//calculate length of main name (before .)
	if(length_ext)
		length -= length_ext+1;

	//valid for 8.3 entry only?
	if(length <= 8)
	{
		retval=TRUE;
		//put extension in short version of name
		if(length_ext)
		{
			short_name[8]='.';
			memcpy(&short_name[9], &long_name[length+1], length_ext);
		};
		memcpy(short_name, long_name, length);
	}
	else
	//hash ID required
	{
		retval=FALSE;
		if(length_ext)
			length += length_ext+1;
		crc32_generate(NULL,0);
		ascii_ulong(crc32_generate(long_name, length), short_name, 16, 8, '0');
		strcpy_P(&short_name[8], PSTR("    "));
	};

	//convert to upper case
	length=12;
	while(length--)
	{
		if('a' <= short_name[length] && short_name[length] <= 'z')
			short_name[length]-=0x20;
	};

	return retval;
}

//calculate checksum for LFN
static unsigned char dir_lfn_checksum(struct directory_struct *directory_ptr)
{
    int i;
    unsigned char sum=0;
	char* name;

	name = directory_ptr->filename_dos;
    for(i=8; i; i--)
		sum = ((sum & 1) << 7) + (sum >> 1) + *name++;

	name = directory_ptr->filename_ext;
    for(i=3; i; i--)
		sum = ((sum & 1) << 7) + (sum >> 1) + *name++;

    return sum;
}

// get text from a lfn entry
// text will be 13 bytes, only text from sequence 0x4# will be terminated, and 0xFF bytes will follow terminator
static char* dir_lfn_text_get(struct lfn_struct *lfn, char *text)
{
	int count;
	int *utf;
	char *retval;
	retval=text;

	utf=lfn->name_first;
	count=5;
	while(count--)
		*text++ = (char)LEND_UINT_READ(*utf++);

	utf=lfn->name_middle;
	count=6;
	while(count--)
		*text++ = (char)LEND_UINT_READ(*utf++);

	utf=lfn->name_last;
	count=2;
	while(count--)
		*text++ = (char)LEND_UINT_READ(*utf++);

	return retval;	
}

//returns short name from directory entry
static char* dir_name_short(struct directory_struct *directory, char* text)
{
	memcpy(text, directory->filename_dos, 8);
	text[8]='.';
	memcpy(&text[9], directory->filename_ext, 3);
	text[12]=0x00;
	
	if(text[9]==' ' && text[10]==' ' && text[11]==' ')
		text[8]=' ';	//if no extention, change . to a space
	return text;
}

//returns DIR_TYPE_UNUSED DIR_TYPE_USED DIR_TYPE_ERASED DIR_TYPE_LFN
static unsigned char dir_type(struct directory_struct *directory)
{
	int retval;

	if(directory->filename_dos[0] == 0xE5)
		retval=DIR_TYPE_ERASED;
	else if(directory->filename_dos[0] == 0x00)
		retval=DIR_TYPE_UNUSED;
	else if(directory->attributes==0x0F)
		retval=DIR_TYPE_LFN;
	else
		retval=DIR_TYPE_USED;
	return retval;
}

//read the sector containing the directory entry we want, and return a pointer to that entry
struct directory_struct* dir_entry_get_ptr(unsigned long base_cluster, unsigned int index)
{
	struct directory_struct *directory_ptr = NULL;

	chain_init(&dir_chain_nav, base_cluster);
	directory_ptr = chain_get_sector_ptr(&dir_chain_nav, ((unsigned long)index) << 5, FAT_READ);
	if(directory_ptr)
		directory_ptr = &directory_ptr[index & 0x0F];

	return directory_ptr;
}
