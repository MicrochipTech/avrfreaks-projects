#include "main.h"
#include "mmc.h"

#include "spi.h"
#include "mmc_file.h"
#include "timers.h"

#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
	

//current limitations: only one partition, all files in root directory, fat16
#define P_T_Adress 0x1BE
#define ROOT_ENTRIES 0x200






word fat_startsector,dir_startsector,start_of_data,num_fat_sectors;
byte secs_per_cluster;




byte mmc_boot()
{	byte error;
	word aux;
/*#ifdef DEBUG
char outstring[30],auxstring[20];
#endif*/
	unsigned long int volumesector;

	error=mmc_read_block (sector,0);  //read boot sector
	while (!block_complete);
	spi_deselect();
	mmc_resume();
	
	if (!error)
	{	//analyse partition table 1
		memcpy (&volumesector,&sector[P_T_Adress+0x08],4);
		error=(!sector[P_T_Adress+0x04]&0x04);
		if (error)
		{	volumesector=0;
			error=0;
		}
	}	
	
	if (!error)
	{	error=mmc_read_block (sector,volumesector);
		while (!block_complete);
		spi_deselect();
		mmc_resume();
	}
	
	
	
	if (!error)
	{	error=(sector[0x3A]!='6');			//error if not FAT16 entry found
		secs_per_cluster=sector[0x0D];
		error|=!((sector[0x1FE]==0x55)&&(sector[0x1FF]==0xAA));	//Error if signature incorrect
	}
	
	if(!error)
	{	memcpy(&aux,&sector[0x0E],2);						//getting number of reserved sectors
		fat_startsector=volumesector+aux;					//computing start of FAT
		memcpy(&aux,&sector[0x16],2);						//getting sectors per FAT
		num_fat_sectors=aux;
		dir_startsector=fat_startsector+(2*aux);			//double for 2 FATs
		start_of_data=dir_startsector+32;					//valid for 512 Root dir entries (32byte per entry,512 entries);
	}

/*#ifdef DEBUG	
	if (!error)
	{
		sprintf_P(outstring,PSTR("B OK"));
		sprintf_P(auxstring,PSTR("#%u #%u\r"),secs_per_cluster,dir_startsector);
		strcat (outstring,auxstring);
	}
	else
		sprintf_P(outstring,PSTR("b nok\r"));
	while (!uart_transmit (outstring,strlen(outstring)));
	while (!uart_complete);
#endif*/

	return error;
}



byte mmc_get_next_cluster (word *current_cluster)
{	byte read;
	word cluster,error;
	unsigned long int sec_to_read;

	cluster=*current_cluster;
	sec_to_read=fat_startsector+(cluster/256);
	if (file.fast_fat)
	{	if (sec_to_read!=file.current_fatsector)
		{	file.current_fatsector=sec_to_read;
			read=1;
		}
		else
			read=0;
	}
	else
		read=1;
	if (read)
	{	error=mmc_read_block (sector,sec_to_read);
		while (!block_complete);
	}
	else 
		error=0;
		
	if (!error)
	{	
		cluster=cluster%256;
		memcpy (current_cluster,&sector[2*cluster],2);
	
	}
	spi_deselect();	
	mmc_resume();
	return error;
}



byte mmc_dir(char* entry,word* startcluster,unsigned long int *filesize,char* extension,word* entrynumber)
{	

	byte auxstring[15],as[5];
	word dircounter,entryadress,totalentries;
	byte error,browse;
	byte auxcount;
	
	*startcluster=0;
	browse=(strlen(entry)==0);
	if (!browse)
	{	totalentries=0;
		dircounter=0;
	}
	else
	{	dircounter=(*entrynumber)%16;
		totalentries=*entrynumber-dircounter;
	}
		
	do
	{		
		if(mmc_read_block (sector,(dir_startsector+(totalentries>>4))))
			error=10;
		else
			error=0;
/*#ifdef DEBUG
		sprintf_P(as,PSTR("Dir read sec %u\r "),(dir_startsector+(totalentries>>4)));
		strcat(auxstring,as);
		while (!uart_transmit (auxstring,strlen(auxstring)));
		while (!uart_complete);
#endif*/
		wdt_reset();
		while (!block_complete);
		spi_deselect();	
		mmc_resume();
		if (!error)
		{	do
			{	memset (auxstring,0,sizeof(auxstring));
				memset (as,0,sizeof(as));
				entryadress=(dircounter)<<5;
				auxcount=0;
				do
				{	auxstring[auxcount]=sector[entryadress+auxcount];
					auxcount++;
				}
				while ((auxcount<8)&&(sector[entryadress+auxcount]!=0x20));
				auxcount=0;
				do
				{	as[auxcount]=sector[entryadress+8+auxcount];
					auxcount++;
				}
				while ((auxcount<3)&&(sector[entryadress+8+auxcount]!=0x20));
				strcat_P(auxstring,PSTR("."));
				strcat(auxstring,as);
				if (!browse)
				{	if (!strcasecmp(auxstring,entry))
					{	memcpy (startcluster,&sector[entryadress+0x1A],2);
						memcpy (filesize,&sector[entryadress+0x1C],4);
						error=0;
					}
					else
						error=1;
				}
				else
				{	if (auxstring[0]!=0xE5)
					{	if (!strcasecmp(as,extension))
						{	error=0;
							strcpy (entry,auxstring);
							*entrynumber=totalentries+dircounter;
						}
						else
							error=1;
					}
					else
						error=1;
				}
			
			
/*#ifdef DEBUG	
			//if ((auxstring[0]!='.')&&(auxstring[0]!=0xE5))
			{	sprintf_P(as,PSTR(" #%u %u \r "),dircounter,entryadress);
				strcat(auxstring,as);
				while (!uart_transmit (auxstring,strlen(auxstring)));
				while (!uart_complete);
			}
#endif*/			
				dircounter++;
			}
			while ((error)&&(dircounter<16));
			totalentries+=dircounter;
			dircounter=0;
		}		
	}
	while ((error)&&(totalentries<ROOT_ENTRIES)&&(error!=10));
	return error;
}
	
/*
byte mmc_get_free_cluster (void)
{	byte error,retry=0;
	word fat_entry;
	word cluster,idx;
	unsigned long int sec_to_read,end_sector;
	
	//Get next cluster, strategy is "next-fit"
	end_sector=fat_startsector+num_fat_sectors-1;
	
	cluster=file.current_cluster+1;
	do
	{	sec_to_read=fat_startsector+cluster/256;
		error= mmc_read_block (sector,sec_to_read);
		while (!block_complete);
		spi_deselect();
		mmc_resume();
		if (!error)
		{	do
			{	idx=(cluster%256)*2;
				memcpy(&fat_entry,&sector[idx],2);
				if (fat_entry==0)
				{	file.next_cluster=cluster;
					error=0;
				}
				else
					error=1;
				cluster++;
			}
			while ((cluster%256)&&(error));
		}
		else
			retry++;
	}
	while ((sec_to_read<end_sector)&&(retry<20)&&(error));
	return error;
}	

*/

byte mmc_open_file (char* filename,byte control)
{	byte error,loop;
#ifdef DEBUG
//	char outstring[15];
#endif
	word counter,aux;
	unsigned long int sec_to_read,fat_sector;
	char* extension;
	word* entrynumber;
	
	*extension=0;
	*entrynumber=0;
	
	memset (&file,0,sizeof(file));
	
	error=mmc_dir(filename,&file.current_cluster,&file.filesize,extension,entrynumber);
	if (error!=10)
	{	if (control==WRITE)
			error=!error;
	}
	if (!error)
	{	if (control==WRITE)
		{	//case READ:	
		
		
	//					break;
//			case WRITE:	
						//error=mmc_get_free_cluster();
						
						//create directory entry
/*						if (!error)
						{	sec_to_read=dir_startsector;
							do
							{				
								//delay(1);
								error= mmc_read_block (sector,sec_to_read);
								counter=0;
								while (!block_complete);
								spi_deselect();
								mmc_resume();
								if (!error)
								{	do
									{	error=((sector[counter]!=0)&&(sector[counter]!=0xE5));	//Search empty or erased entries
										counter+=32;
									}
									while (error&&(counter<512));
									sec_to_read++;
								}
							}
							while (error&&(sec_to_read<(dir_startsector+32)));  //stop at 512 dir-entries
						}
						if (!error)
						{	counter-=32;
							sec_to_read--;
							file.dir_sector=sec_to_read;
							file.dir_entry=counter;
							memset (&sector[counter],0,32);
							memset (&sector[counter],0x20,11);
							loop=1;
							file.first_letter=*filename;	//save first file letter
							filename++;
							sector[counter]=0xE5;		//Mark 1st letter as erased entry		
							while ((loop<11)&&((*filename)!='.'))
							{	sector[counter+loop]=*filename;
								loop++;
								filename++;
							}
							filename++;
							memcpy (&sector[counter+8],filename,3);
							memcpy (&sector[counter+0x1A],&file.next_cluster,2);
							error=mmc_write_block(sector,sec_to_read);
							//set entry cluster
							file.current_cluster=file.next_cluster;
									fat_sector=fat_startsector+(file.current_cluster/256);
							//mark current cluster as last one to avoid unreadable 0-files
							aux=0xFFFF;
							spi_select();						
							while (mmc_read_block (sector,fat_sector));		//read fat page
							while (!block_complete);
							spi_deselect();
							mmc_resume();
							memcpy (&sector[(file.current_cluster%256)*2],&aux,2);	//mark last cluster
							while(mmc_write_block(sector,fat_sector));			//update fat


						}
						
*/						
		}
	}
	return error;
}



unsigned long get_sector()
{	return ((unsigned long)file.current_cluster-2)*secs_per_cluster+file.sec_in_cluster+start_of_data;
}



byte mmc_read_sector ()
{	

	if (!file.chars_read)
	{	file.current_sector=get_sector();
		file.eof=mmc_read_block (sector,file.current_sector);
		while (!block_complete);
		spi_deselect();
		mmc_resume();
	}
	
	if (!file.eof)
	{	file.chars_read+=512;
		file.eof=(file.chars_read>(file.filesize-1));		//end of file reached?
		if (!file.eof)
		{	if ((++file.sec_in_cluster)>=secs_per_cluster)
			{	file.sec_in_cluster=0;
				mmc_get_next_cluster (&file.current_cluster);	//lookup fat for next cluster
			}	
			file.current_sector=get_sector();
			file.eof=mmc_read_block (sector,file.current_sector);
			while (!block_complete);
			spi_deselect();
			mmc_resume();
		}
	}
	return file.eof;
}


byte mmc_read_line (char* line,byte max_chars)
{	byte counter,eol,eof,start;
		
	
	if (!file.chars_read)		//get first sector
	{	file.current_sector=get_sector();
		eof=mmc_read_block (sector,file.current_sector);
		while (!block_complete);
		spi_deselect();
		mmc_resume();
	}
	else
		eof=0;
	counter=0;
	eol=0;
	
	if (!eof)
	{	do
		{	*line=sector[file.adress_in_sector];			//read line
/*			eol=(sector[file.adress_in_sector]==0x0D);  //check for CR/LF
			if (eol)
			{	*line=0;
				file.chars_read++;
				file.adress_in_sector++;
			}
*/		
			file.adress_in_sector++;
			line++;
			counter++;
			eof=((++file.chars_read)>=(file.filesize-1));		//end of file reached?
			if ((file.adress_in_sector>511)&&(!eof))						//end of sector?
			{	file.adress_in_sector=0;
				if ((++file.sec_in_cluster)>=secs_per_cluster)
				{	file.sec_in_cluster=0;
					mmc_get_next_cluster (&file.current_cluster);	//lookup fat for next cluster
				}	
				file.current_sector=get_sector();
				eof=mmc_read_block (sector,file.current_sector);
				while (!block_complete);
				spi_deselect();
				mmc_resume();
			}
			
		}
		while ((!eol)&&(counter<max_chars)&&(!eof));
	}
	return eof;
}



byte get_id3(char* song,char* artist)
{	byte eof,found,dest_index;
	word counter,source_index,clusterlength;
	unsigned long fileposition,stop_position;
	
	clusterlength=512*secs_per_cluster;
	fileposition=0;
	stop_position=file.filesize-2*clusterlength-128;
	file.fast_fat=1;
	file.sec_in_cluster=0;
	
	do
	{	mmc_get_next_cluster(&file.current_cluster);
		fileposition+=clusterlength;
	}
	while (fileposition<stop_position);	//look for pre-last cluster	
	
	file.fast_fat=0;

	do
	{	eof=mmc_read_sector();
		counter=0;
		if (!eof)
		{	do
			{	found=!strncmp_P(&sector[counter++],PSTR("TAG"),3);
				fileposition++;
			}	
			while ((!found)&&counter<512);
			eof=((file.filesize-fileposition)<128);
		}

	}	
	while ((!eof)&&(!found));
	
	
	if (found)
	{	source_index=counter+2;				//set startpostion
		for (dest_index=0;dest_index<30;dest_index++)
		{	if (source_index>511)
			{	source_index=0;
				eof!=mmc_read_sector();
			}
			*(song++)=sector[source_index++];
		}
		*song=0;
		for (dest_index=0;dest_index<30;dest_index++)
		{	if (source_index>511)
			{	source_index=0;
				eof!=mmc_read_sector();
			}
			*(artist++)=sector[source_index++];
		}
		*artist=0;
	}
	
	return eof;
}	

	
/*	
byte mmc_write_line (char* buffer, byte buffersize)
{	byte counter;
	unsigned long int fat_sector;
#ifdef DEBUG
//	static char debugstring[20];
#endif
	
	counter=0;
	file.eof=0;
	file.current_sector=get_sector();
				
	do
	{	sector[file.adress_in_sector++]=*(buffer++);
		counter++;
		file.chars_read++;
		if (!file.chars_read)		//check for max file size 0xFFFFFFFF
		{	file.eof=1;
			file.chars_read--;
		}

#ifdef DEBUG
		sprintf_P(debugstring,PSTR("Letter %u/%u\r"),counter,buffersize);
		while (!uart_transmit (debugstring,strlen(debugstring)));	
		while (!uart_complete);
#endif
		if (file.adress_in_sector>=512)
		{	file.adress_in_sector=0;
			if (!file.eof)
			{	file.eof=mmc_write_block(sector,file.current_sector);	//write data sector;
				if ((++file.sec_in_cluster)>=secs_per_cluster)
				{	
					file.sec_in_cluster=0;
					fat_sector=fat_startsector+(file.current_cluster/256);
					file.eof=mmc_get_free_cluster (); //eof=Disk full
					if (file.eof)
						file.next_cluster=0xFFFF;	//mark current cluster as last one
					else
					{	file.eof=mmc_read_block (sector,fat_sector); //get fat sector
						while (!block_complete);
						spi_deselect();
						mmc_resume();				
						if (!file.eof)
						{	memcpy(&sector[(file.current_cluster%256)*2],&file.next_cluster,2); //update fat
							file.eof=mmc_write_block(sector,fat_sector);	//write fat sector;
						}
//						while(mmc_write_block(sector,fat_sector+num_fat_sectors));	//write fat copy sector;
						file.current_cluster=file.next_cluster;				
					}
				}	
					
#ifdef DEBUG
				while (!uart_complete);
				sprintf_P(debugstring,PSTR("clu%u s%u\r"),file.current_cluster,file.current_sector);
				while (!uart_transmit (debugstring,strlen(debugstring)));	
					
#endif
			}
			file.current_sector=get_sector();

		}		
	}
	while ((!file.eof)&&(counter<buffersize));
	return file.eof;
}
	


byte mmc_close_file()

{	byte loop,error;
	word aux,a_date;
	unsigned long int fat_sector;
#ifdef DEBUG
	//char outstring[30];
	
#endif	
	//flush sector
	if (!file.eof)
	{	error=mmc_write_block(sector,file.current_sector);
		if (!error)
		{	fat_sector=fat_startsector+(file.current_cluster/256);
			aux=0xFFFF;
			error=mmc_read_block (sector,fat_sector);		//read fat page
			if (!error)
			{	while (!block_complete);
				memcpy (&sector[(file.current_cluster%256)*2],&aux,2);	//mark last cluster
			}
			spi_deselect();
			mmc_resume();
		}
		if (!error)
		{	error=mmc_write_block(sector,fat_sector);			//update fat
		}
	}

#ifdef DEBUG
	sprintf_P(outstring,PSTR("Flush: start:%u  Inh:%u\r"),file.current_sector,file.fat_chain[file.cluster_index]);
	while (!uart_transmit (outstring,strlen(outstring)));	
	while (!uart_complete);
#endif
	if (!error)
	{	error=mmc_read_block (sector,file.dir_sector);		//read directory
		while (!block_complete);
	}
	spi_deselect();
	mmc_resume();
	if (!error)
	{	sector[file.dir_entry]=file.first_letter;		//restore first letter of filename
		memcpy (&sector[file.dir_entry+0x1C],&file.chars_read,4);
		//set file date/time
		a_date=(time.day&0x1F)+((time.month&0x0F)<<5)+((time.year&0x7F)<<9);
		memcpy (&sector[file.dir_entry+0x10],&a_date,2);
		memcpy (&sector[file.dir_entry+0x12],&a_date,2);
		memcpy (&sector[file.dir_entry+0x18],&a_date,2);
		a_date=((time.sec&0x3F)>>1)+((time.min&0x3F)<<5)+((time.hour&0x1F)<<11);
		memcpy (&sector[file.dir_entry+0x0E],&a_date,2);
		memcpy (&sector[file.dir_entry+0x16],&a_date,2);
		sector[file.dir_entry+0x0B]=0x20;  //label entry as archive	
		error=mmc_write_block(sector,file.dir_sector);		//update directory
	}
	return error;
}

*/


	
