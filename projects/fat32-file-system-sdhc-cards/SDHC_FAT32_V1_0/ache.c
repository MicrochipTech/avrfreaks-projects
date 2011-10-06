
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************
// Cache
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************
/*
In order to provide write back cache, an association between a buffer and a sector must be maintained.
A 'Clean' buffer means buffer contents matches the physical sector.
A 'Dirty' buffer means buffer contents MAY not match the physical sector.

When the sector associated with the buffer needs to be changed...
	If the buffer is dirty, it must be written to the sector it is currently associated with, fat mirrors are also updated at this point.
	Then either:
		Data from the new sector is read into the buffer (for reading)
		The association is updated to the new sector.
		The buffer is clean.
	or
		Existing data in the buffer, is associated with a new sector (for writing)
		The buffer is dirty.


Read sector X
	X != associated_sector?
	Yes
		Dirty?
		Yes
			Physical write associated_sector
		associated_sector = X
		Physical read associated_sector
		Buffer is Clean.
	Done

Write sector X
	X  != associated_sector?
	Yes
		Dirty?
		Yes
			Physical write associated_sector
		associated_sector = X
		Buffer is Dirty
	Done
*/


//********************************************************************
// Private variables
//********************************************************************

	static char				cache_buffers[CACHE_SIZE*512];		//for holding actual data
	static unsigned long 	cache_sectors[CACHE_SIZE];			//associated sectors of above buffers
	static char				cache_dirty[CACHE_SIZE];			//clean/dirty flags for each buffer
	static unsigned char	cache_age[CACHE_SIZE];				//highest value is used when a new buffer is needed




//********************************************************************
// Functions
//********************************************************************

void fatcache_init(void)
{
	unsigned char index=0;
	do
	{
		cache_sectors[index]=0xFFFFFFFFul;
		cache_dirty[index]=FALSE;
		cache_age[index]=255;
		
		index++;
	}while(index != CACHE_SIZE);
}

void* fatcache_sector(unsigned long sector, unsigned char read_write)
{
	unsigned char index;
	unsigned char selection;
	unsigned char maximum;
	unsigned long templong;

	char found=FALSE, success=TRUE;
	char *sector_ptr=NULL;
	char *temp_ptr;
	
	//increment inactivty of all buffers
	index=0;
	do
	{
		if(cache_age[index]<255)
			cache_age[index]++;
		index++;
	}while(index!=CACHE_SIZE);

	//does sector exist in buffers?
	index=0;
	temp_ptr=cache_buffers;
	do
	{	
		if(sector==cache_sectors[index])
		{
			selection=index;
			found=TRUE;
			sector_ptr=temp_ptr;
			cache_age[selection]=0;		//clear buffers cache_age counter
		}
		else
		{
			index++;
			temp_ptr+=512;
		};
	}while(!found && index!=CACHE_SIZE);
	
	//if not, use the buffer with the highest cache_age
	if(!found)
	{
		//find buffer with highest cache_age
		index=0;
		temp_ptr=cache_buffers;
		selection=0;
		maximum=0;
		do
		{
			if(cache_age[index]>maximum)
			{
				selection=index;
				sector_ptr=temp_ptr;
				maximum=cache_age[index];
			};
			index++;
			temp_ptr+=512;
		}while(index!=CACHE_SIZE);
		
		
		//clear buffers cache_age (as we are about to use it)
		cache_age[selection]=0;
		
		//if the buffer is dirty, write it back
		if(cache_dirty[selection])
		{
			templong=cache_sectors[selection];
			success = BLOCK_WRITE(sector_ptr, templong);
			
			//fat mirroring enabled?
			if(fat_size)
			{
				//provide FAT mirroring during cache write back
				if( first_fat_sector <= templong && templong < (first_fat_sector + fat_size) )	//if we just wrote to FAT0	
				{
					index=total_fats-1;
					while(index && success)
					{
						templong += fat_size;
						success = BLOCK_WRITE(sector_ptr, templong);
						index--;
					};
				};
			};
		};
		
		//if no error
		if(success)
		{
			//associate buffer with sector
			cache_sectors[selection]=sector;
			
			//read required?
			if(read_write == FAT_READ)
			{
				success=BLOCK_READ(sector_ptr, sector);
				cache_dirty[selection]=FALSE;
			}
			else
				cache_dirty[selection]=TRUE;
		};
		
		if(!success)
		{
			cache_sectors[selection]=0xFFFFFFFFul;
			cache_dirty[selection]=FALSE;
			sector_ptr=NULL;
		};	
	};
	

//	debug_status();
	return (void*)sector_ptr;
}

void fatcache_dirty(void* buffer)
{
	unsigned char index=0;
	char* sector_ptr, *target;
	unsigned int tempint;
	
	target = (char*)buffer;			//target=start address of the buffer containing the passed address
	
	tempint = target-cache_buffers;
	tempint &=~0x1FF;
	tempint += (unsigned int)cache_buffers;
	
	target = (char*)(tempint);
	
	sector_ptr=cache_buffers;
	do
	{
		if(sector_ptr==target)
		{
			cache_dirty[index]=TRUE;
			index=CACHE_SIZE;
		}
		else
		{
			index++;
			sector_ptr+=512;
		};
	}while(index != CACHE_SIZE);
//	debug_status();
}

char fatcache_clean(void)
{
	char success=TRUE;
	unsigned char index, selection=0;
	unsigned long templong;
	char *sector_ptr;
	
	sector_ptr=cache_buffers;
	do
	{
		//clear all cache_age counters
		cache_age[selection]=0;
		
		//if the buffer is dirty, write it back
		if(cache_dirty[selection])
		{
			templong=cache_sectors[selection];
			success = BLOCK_WRITE(sector_ptr, templong);
			
			//fat mirroring enabled?
			if(fat_size)
			{
				//provide FAT mirroring during cache write back
				if( first_fat_sector <= templong && templong < (first_fat_sector + fat_size) )	//if we just wrote to FAT0	
				{
					index=total_fats-1;
					while(index && success)
					{
						templong += fat_size;
						success = BLOCK_WRITE(sector_ptr, templong);
						
						index--;
					};
				};
			};
			if(success)
				cache_dirty[selection]=FALSE;
		};
		selection++;
		sector_ptr+=512;
	}while(success && selection!=CACHE_SIZE);
//	debug_status();
	return success;
}

/* old function used for debugging cache
static void debug_status(void)
{
	unsigned char index=0;
	char* temp_ptr;
	DEBUG_P(PSTR("FATCACHE - STATUS\r\n"));
	
	temp_ptr=cache_buffers;
	do
	{
		DEBUG_ULONG((unsigned long)index);
		if(cache_dirty[index])
			DEBUG_P(PSTR(" DIRTY "));
		else
			DEBUG_P(PSTR(" CLEAN "));
		
		DEBUG_P(PSTR("INAC: "));
		DEBUG_ULONG((unsigned long)cache_age[index]);
		DEBUG_P(PSTR(" SECT: "));
		DEBUG_ULONG((unsigned long)cache_sectors[index]);
		DEBUG_P(PSTR(" ADD: "));
		DEBUG_ULONG((unsigned long)(unsigned int)temp_ptr);
		DEBUG_P(PSTR("\r\n"));
		index++;
		temp_ptr+=512;
	}while(index!=CACHE_SIZE);
}
*/
