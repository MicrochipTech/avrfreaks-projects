/*
 * Dynamic memory allocator
 * Simple implementation of a linked free list
 *
 * A section refers to an area of free or allocated memory INCLUDING its meta data
 *
 * Each free section begins with
 * uint16_t size				(size of this section)
 * void* next     				(pointer to the next free section, or null if this is the last free section)
 *
 * The beginning of the heap has a free section of unusable size,
 * It exists only to keep the beginning of the free list at the start of the heap
 * It links to the first usable free section, and is never extended.
 *
 * Each allocated section begins with
 * uint16_t size (size of allocated section)
 *
 *
 * Heap inspector/stats
 *
 * Broken/intact
 * X bytes used in X allocations
 * Largest allocation
 * X bytes free in X free sections*
 * Largest free section
 * Minimum largest free section since reset
 *
 */

	#include "includes.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define	HEAP_SIZE	380

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	//meta data used for free and allocated sections
	//these can be extended with extra info if desired

	struct free_meta_struct
	{
		uint16_t	size;
		void*		next;
	};

	struct used_meta_struct
	{
		uint16_t	size;
	};

	//pointer casts
	#define FREE_META(arg1)	((struct free_meta_struct*)(arg1))
	#define USED_META(arg1)	((struct used_meta_struct*)(arg1))

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

	static uint8_t		initialized = FALSE;
	static uint8_t		heap_space[HEAP_SIZE];

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

//********************************************************************************************************
// Public functions
//********************************************************************************************************

void heap_init(void)
{
	struct free_meta_struct *first, *second;

	initialized=TRUE;

	//Create the first two sections,
	first = (void*)heap_space;
	first->size = sizeof(struct free_meta_struct);
	first->next = heap_space + sizeof(struct free_meta_struct);

	second = first->next;
	second->size = HEAP_SIZE - sizeof(struct free_meta_struct);  //(heap - above section)
	second->next = NULL;
}

void* heap_allocate(uint16_t new_size)
{
	struct free_meta_struct *previous_section;
	struct free_meta_struct *section=NULL;

	uint8_t finished = FALSE;
	uint8_t failed	 = FALSE;

	while(!initialized);		//hang if called without initializing heap
	while(!new_size);			//hang if called to allocate 0 bytes

	//start at beginning of heap
	previous_section = (void*)heap_space;

	//new section size includes meta data for allocated section
	new_size += sizeof(struct used_meta_struct);

	//minimum size of new section must be big enough to return to the free list
	if(new_size < sizeof(struct free_meta_struct))
		new_size = sizeof(struct free_meta_struct);

	if(previous_section->next == NULL)	//fail if there are no usable free sections left
	{
		finished = TRUE;
		failed   = TRUE;
	};

	while(!finished)
	{
		//inspect next section
		section = previous_section->next;
		
		//If partially used, the section must hold:
		//   used meta data + used space + free meta data + free space(at least 1 byte)
		if(section->size >= new_size + sizeof(struct free_meta_struct) + 1)
		{
			//Reduce the free sections size
			section->size -= new_size;
			
			//Move the free section forward
			memmove((uint8_t*)section + new_size, section, sizeof(struct free_meta_struct));
			previous_section->next = (uint8_t*)(previous_section->next) + new_size;
			
			//Record the section size of the new allocation
			USED_META(section)->size = new_size;
			
			finished=TRUE;
		}
		//If used entirely, the section must hold:
		//   used meta data + used space
		else if(section->size >= new_size)
		{
			//Increase allocation size to use entire section if needed
			new_size = section->size;

			//Remove this section from the free list
			previous_section->next = section->next;

			//Record the section size of the new allocation
			USED_META(section)->size = new_size;

			finished=TRUE;
		}
		//If neither of the above, the section cannot be used for this allocation
		else
		{
			//is the next section the last free section?
			if(section->next == NULL)
			{
				//yes, failed to allocate
				finished=TRUE;
				failed=TRUE;
			}
			else
				//no, advance to the next section
				previous_section = section;
		};
	};

	if(failed)
		section = NULL;
	else
		section = (void*)((uint8_t*)section + sizeof(struct used_meta_struct));

	return section;
}

void heap_free(void* section)
{
	//Add free section to the list
	//Extend free section into the next free section if possible
	//Extend lower free section into this section if possible

	struct free_meta_struct *target_section;
	struct free_meta_struct *previous_section;

	uint16_t size;

	if(section)
	{
		//point to the target section
		section = (uint8_t*)section - sizeof(struct used_meta_struct);
		target_section = section;
		
		//Find previous section
		previous_section = (void*)heap_space;
		while(	previous_section->next
			&&	(previous_section->next < (void*)(target_section)) )
		{
			previous_section = previous_section->next;
		};
		
		//create free section meta data
		size = USED_META(target_section)->size;
		target_section->size = size;
		
		//insert new section into the free list
		target_section->next = previous_section->next;
		previous_section->next = target_section;
		
		//Can new free section be extended into the next free section?
		if((uint8_t*)target_section + target_section->size == target_section->next)
		{
			//New section size extended into next section
			target_section->size += FREE_META(target_section->next)->size;
			
			//new sections link == the next sections link
			target_section->next = FREE_META(target_section->next)->next;
		};
		
		//If previous section is NOT the first section in the heap
		if((void*)previous_section != heap_space)
		{
			//Can previous section be extended into this section?
			if((uint8_t*)previous_section + previous_section->size == (void*)target_section)
			{
				//Previous section size extended
				previous_section->size += target_section->size;
				
				//previous sections link == this sections link
				previous_section->next = target_section->next;
			};
		};
	};
}


//********************************************************************************************************
// Private functions
//********************************************************************************************************
