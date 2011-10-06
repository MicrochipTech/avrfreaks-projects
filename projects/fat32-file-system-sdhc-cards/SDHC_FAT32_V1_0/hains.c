//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************
// Chain layer, to read/write linearly addressed sections within a chain
//**************************************************************************************************************************************************************
//**************************************************************************************************************************************************************

//return chain length, also returns last cluster in chain, if pointer is not NULL
static unsigned long chain_length(unsigned long cluster, unsigned long *last_cluster)
{
	unsigned long length=0;
	unsigned char type;
	char finished=FALSE;

	//determine size of chain by counting how many clusters it contains
	do
	{
		type=cluster_type_read(cluster);
		
		//if not last, follow link	
		if(type == CLUSTER_TYPE_USED)		
		{
			length++;
			cluster = cluster_link_read(cluster);
		}
		else if (type == CLUSTER_TYPE_LAST)
		{
			length++;
			finished=TRUE;
			if(last_cluster)
				*last_cluster=cluster;
		}
		else
			finished=TRUE;
		
	}while(!finished);

	return length;
}

//erase a chain from the fat, freeing clusters
static char chain_erase(unsigned long cluster)
{
	char success;
	unsigned long link;
	unsigned long count=0;
	unsigned char type;
	
	DEBUG_P(PSTR("FAT32 - Erase chain @"));
	DEBUG_ULONG(cluster);
	DEBUG_P(PSTR("\r\n"));
	
	type=cluster_type_read(cluster);
	if(type != CLUSTER_TYPE_USED && type != CLUSTER_TYPE_LAST)
	{
		DEBUG_P(PSTR("FAT32 - ERROR, Attempt to erase non-existent chain @"));	
		DEBUG_ULONG(count);
		DEBUG_P(PSTR("\r\n"));
		success=FALSE;
	}
	else
	{
		do
		{
			//get next cluster
			link=cluster_link_read(cluster);
			
			//free this cluster
			success=cluster_link_write(cluster, 0x00000000ul);	//free cluster
			
			//count freed clusters for FSinfo sector
			if(success)
				count++;
				
			//next cluster follows link
			cluster=link;
		}while(link && success);
	};
	
	if(!success)
		DEBUG_P(PSTR("FAT32 - ERROR, unable to erase chain\r\n"));
	else
	{	
		success=fsinfo_total_free_increase(count);
		if(success)
		{
			DEBUG_P(PSTR("FAT32 - Freed "));	
			DEBUG_ULONG(count);
			DEBUG_P(PSTR(" clusters\r\n"));
		}
		else
			DEBUG_P(PSTR("FAT32 - ERROR, unable to update FSinfo sector\r\n"));
	};

	return success;
}

//creates a new chain, returns base cluster of new chain, or 0 if failed
static unsigned long chain_create(unsigned long size)
{
	char success=TRUE;;
	unsigned long base;
	unsigned long end;
	unsigned long search;
	unsigned long count=0;
	
	DEBUG_P(PSTR("FAT32 - Create new chain of length "));
	DEBUG_ULONG((unsigned long) size);
	DEBUG_P(PSTR("\r\n"));
	
	search=fsinfo_last_allocated_read();
	if(search > total_clusters+1)
		search=3ul;
	
	if(search)
	{
		DEBUG_P(PSTR("FAT32 - Create new chain, searching from cluster "));
		DEBUG_ULONG(search);
		DEBUG_P(PSTR("\r\n"));
		
		if(size > fsinfo_total_free_read())
		{
			success=FALSE;
			DEBUG_P(PSTR("FAT32 - ERROR, chain create failed, not enough free clusters\r\n"));
		}
		else
		{
			//update FSinfo
			success=fsinfo_total_free_decrease(size);
			
			base=0ul;
			end=0ul;
			count++;
			while(size && success)
			{
				//find a free cluster
				while(cluster_type_read(search) != CLUSTER_TYPE_FREE && count != total_clusters)
				{
					search++;
					if(search == total_clusters+1)
						search=3ul;
					count++;
				};
				
				//fail if no more free clusters found
				if(count==total_clusters)
				{
					DEBUG_P(PSTR("FAT32 - ERROR, FAT search unable to find enough free clusters (despite FSInfo).\r\n"));
					success=FALSE;
				}
				else
				{
					//if first cluster found
					if(!base)
					{
						DEBUG_P(PSTR("FAT32 - New chain base cluster at "));
						DEBUG_ULONG(search);
						DEBUG_P(PSTR("\r\n"));
						base=search;	//remember base
						end=search;		//remember end of chain
					}
					else
					{
						success=cluster_link_write(end, search);	//link end to new cluster
						end=search;
					};
					size--;
					search++;
				};
			};
			if(success)
			{
				DEBUG_P(PSTR("FAT32 - New chain, terminating end cluster at "));
				DEBUG_ULONG(end);
				DEBUG_P(PSTR("\r\n"));
				success=cluster_link_write(end, 0x0FFFFFFFul);		//terminate chain
				if(success)
				{
					success=fsinfo_last_allocated_write(end);
					if(!success)
						DEBUG_P(PSTR("FAT32 - ERROR, unable to access FSInfo\r\n"));
				}
				else
					DEBUG_P(PSTR("FAT32 - ERROR, unable to terminate new chain\r\n"));
			}
			else
				DEBUG_P(PSTR("FAT32 - ERROR, unable to create chain.\r\n"));
		};
	}
	else
	{
		success=FALSE;
		DEBUG_P(PSTR("FAT32 - ERROR, unable to access FSinfo\r\n"));
	};
	
	if(!success)
		base=0ul;
	
	return base;
}

//increase the length of a chain TO a certain size, if it is not that long already
static char chain_extend_to(unsigned long cluster, unsigned long new_size)
{
	char success=TRUE;
	unsigned long chain_new=0;
	unsigned long end=0u, old_size;

	old_size=chain_length(cluster, &end);

	if(cluster_type_read(end) != CLUSTER_TYPE_LAST)
	{
		success=FALSE;
		DEBUG_P(PSTR("FAT32 - Chain extend ERROR. Chain at cluster"));
		DEBUG_ULONG(cluster);
		DEBUG_P(PSTR(" contains bad/reserved cluster "));
		DEBUG_P(PSTR("\r\n"));		
	}
	else
	{
		if(old_size<new_size)
		{
			DEBUG_P(PSTR("FAT32 - Chain extend, base="));
			DEBUG_ULONG(chain_new);
			DEBUG_P(PSTR(" end="));
			DEBUG_ULONG(end);
			DEBUG_P(PSTR("\r\n"));
			
			chain_new=chain_create(new_size-old_size);
			
			if(!chain_new)
				success=FALSE;
			else
			{
				//link old chain to new chain
				success=cluster_link_write(end, chain_new);
			};
		}
		else
			DEBUG_P(PSTR("FAT32 - Chain extention not required.\r\n"));
	};

	if(!success)
		DEBUG_P(PSTR("FAT32 - ERROR, unable to extend chain\r\n"));

	return success;
}

//increase the length of a chain by size
static char chain_extend_by(unsigned long cluster, unsigned long size)
{
	char success=TRUE;
	unsigned long chain_new=0;
	unsigned long end=0ul;

	//find the end of the chain
	chain_length(cluster, &end);

	if(cluster_type_read(end) != CLUSTER_TYPE_LAST)
	{
		success=FALSE;
		DEBUG_P(PSTR("FAT32 - Chain extend ERROR. Chain at cluster"));
		DEBUG_ULONG(cluster);
		DEBUG_P(PSTR(" contains bad/reserved cluster "));
		DEBUG_P(PSTR("\r\n"));		
	}
	else
	{
		DEBUG_P(PSTR("FAT32 - Chain extend, base="));
		DEBUG_ULONG(chain_new);
		DEBUG_P(PSTR(" end="));
		DEBUG_ULONG(end);
		DEBUG_P(PSTR("\r\n"));
		
		chain_new=chain_create(size);
		if(!chain_new)
			success=FALSE;
		else
		{
			//link old chain to new chain
			success=cluster_link_write(end, chain_new);
		};
	};

	if(!success)
		DEBUG_P(PSTR("FAT32 - ERROR, unable to extend chain\r\n"));

	return success;
}

//uses chain_rw_sector, to read or write data potentially spanning many sectors, or erase an area of a chain to 0's
static char chain_rw(struct fat_chain_struct *chain_ptr, void* target, unsigned long byte_address, unsigned long size, char read_write_erase)
{
	char success=TRUE;
	unsigned long end_address;
	int tempint;

	end_address=byte_address+size;

	while(size && success)
	{
		//if we must cross a sector boundary
		if( (byte_address & 0xFFFFFE00ul) != (end_address & 0xFFFFFE00ul) )
		{
			//calculate how much of the data is in current sector only.
			tempint = 0x200 - (int)(byte_address & 0x000001FFul);
			
			success=chain_rw_sector(chain_ptr, target, byte_address, tempint, read_write_erase);
			target+=tempint;
			byte_address += tempint;
			size-=tempint;
		}
		else
		{
			success=chain_rw_sector(chain_ptr, target, byte_address, size, read_write_erase);
			size=0;
		};
	};

	return success;
}

//read or write data to a chain, limited to within 1 sector
static char chain_rw_sector(struct fat_chain_struct *chain_ptr, void* target, unsigned long byte_address, int size, char read_write_erase)
{
	char success=TRUE;
	char* buffer;

	//if we are not going to modify the entire buffer, ensure media gets read
	if(size !=512)
		buffer = chain_get_sector_ptr(chain_ptr, byte_address, FAT_READ);
	else
		buffer = chain_get_sector_ptr(chain_ptr, byte_address, read_write_erase);

	if(buffer)
	{
		//read/write/erase some data (finally whoo hoo!)
		byte_address &= 0x1FFul;
		if(read_write_erase == FAT_READ)
		{
			if(size)
				memcpy(target, &buffer[byte_address], size);
		}
		else if(read_write_erase == FAT_WRITE)
		{
			if(size)
				memcpy(&buffer[byte_address], target, size);
			fatcache_dirty(buffer);
		}
		//erase
		else
		{
			if(size)
				memset(&buffer[byte_address], 0, size);
			fatcache_dirty(buffer);
		};
	}
	else
		success=FALSE;

	return success;
}

//returns a pointer to the required sector within a chain
//if read_write_erase =! FAT_READ, then the media will not be read, and the cache buffer will be marked dirty
static void* chain_get_sector_ptr(struct fat_chain_struct *chain_ptr, unsigned long byte_address, char read_write_erase)
{
	char success=TRUE;
	
	unsigned long required_cluster_physical=0;	//0=unknown
	unsigned long required_cluster_logical;
	unsigned long required_sector_logical;
	unsigned long required_sector_physical;

	unsigned long templong;
	char* retval=NULL;
	
	//determine required logical sector from byte address/512
	required_sector_logical = byte_address >> 9;
 
	//determine required logical cluster from required logical sector (logical clusters start from 0)
	required_cluster_logical = required_sector_logical >> sectors_per_cluster_power;

	//if same logical cluster
	if(required_cluster_logical == chain_ptr->logical)
	{
		//then same physical cluster (no need to search chain)
		required_cluster_physical = chain_ptr->physical;	
	}
	//searching chain
	else
	{
		//need to search from start of chain?
		if(required_cluster_logical < chain_ptr->logical)
		{
			//search from start
			templong = required_cluster_logical;
			required_cluster_physical = chain_ptr->base;	//start with the base cluster (logical cluster 0)
		}
		else
		{
			//search from current
			templong = required_cluster_logical - chain_ptr->logical;
			required_cluster_physical = chain_ptr->physical;
		};
		
		//find required physical cluster from fat table
		while(templong-- && required_cluster_physical)
			required_cluster_physical = cluster_link_read(required_cluster_physical);	//returns 0 if fat entry does not contain a link
	};
	
	//if an error did not occur
	if(required_cluster_physical)
	{
		//determine required physical sector from sector offset within cluster
		//sectors_per_cluster is always a ^ of 2, so sector & sectors_pre_cluster-1 should give us sector offset within the cluster
		
		required_sector_physical = physec_of_phyclust(required_cluster_physical);			//first sector of required cluster
		required_sector_physical += required_sector_logical & (((unsigned long)sectors_per_cluster)-1ul);	//+sector offset within cluster
		
		//if we are writing or erasing entire sector, no need to read it first
		if(read_write_erase != FAT_READ)
			retval = fatcache_sector(required_sector_physical, FAT_WRITE);
		//else read sector into the buffer
		else
			retval = fatcache_sector(required_sector_physical, FAT_READ);
		
		if(!retval)
		{
			success=FALSE;
			DEBUG_P(PSTR("FAT32 - ERROR, Chain, unable to access physical sector "));
			DEBUG_ULONG(required_sector_physical);
			DEBUG_P(PSTR("\r\n"));
		};
		//previous = current
		chain_ptr->physical = required_cluster_physical;	
		chain_ptr->logical = required_cluster_logical;
	}
	else
	{
		DEBUG_P(PSTR("FAT32 - ERROR, Chain, unable to determine physical cluster from FAT chain.\r\n"));
		success=FALSE;
	};
	
	if(!success)
	{
		DEBUG_P(PSTR("FAT32 - ERROR, Chain "));
		if(read_write_erase==FAT_READ)
			DEBUG_P(PSTR("read"));
		else if(read_write_erase==FAT_WRITE)
			DEBUG_P(PSTR("write"));
		else
			DEBUG_P(PSTR("erase"));
		DEBUG_P(PSTR(" FAILED! base="));
		DEBUG_ULONG(chain_ptr->base);
		DEBUG_P(PSTR(" l_clust="));
		DEBUG_ULONG(required_cluster_logical);
		DEBUG_P(PSTR(" p_clust="));
		DEBUG_ULONG(required_cluster_physical);
		DEBUG_P(PSTR(" byte_add="));
		DEBUG_ULONG(byte_address);
		DEBUG_P(PSTR("\r\n"));
	};

	return (void*)retval;
}

void chain_init(struct fat_chain_struct *chain_ptr, unsigned long cluster)
{
	//new base cluster?
	if(cluster != chain_ptr->base)
	{
		chain_ptr->base=cluster;
		chain_ptr->physical=cluster;
		chain_ptr->logical=0ul;
	};
}
