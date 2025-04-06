#define _XOPEN_SOURCE 500 // needed for sbrk() on cslab

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct freenode
{
	size_t size;
	struct freenode *next;
} freenode;

#define HEAP_CHUNK_SIZE 4096

// head node for the freelist
freenode *freelist = NULL;

void *begin = NULL;

/* allocate size bytes from the heap */
void *malloc(size_t size)
{
	if (size < 1)
	{
		return NULL;
	}

	// add 8 bytes for bookkeeping
	size += 8;
	
	// 32 bytes is the minimum allocation
	if (size < 32)
	{
		size = 32;
	}

	// round up to the nearest 16-byte multiple
	else if (size%16 != 0)
	{
		size = ((size/16)+1)*16;
	}
	if(freelist == NULL)
	{
		void *loc = sbrk(HEAP_CHUNK_SIZE);

		if (loc == (void *)-1)
		{
			return NULL;
		}
		freelist = loc+8;
		freelist->size = HEAP_CHUNK_SIZE-8;
		freelist->next = NULL;
	}
	freenode *prev = (freenode *)(freelist);
	freenode *temp = freelist;
	temp->size = freelist->size;
	temp->next = freelist->next;
	int in = 0;
	while(temp->next != NULL)
	{
		in = 1;
		if (temp->size >= size)	
		{
			break;	
		}
		if (temp->next == freelist)
		{
			printf("HULL");
			temp->next = NULL;
			break;
		}
		prev = temp;
		temp = temp->next;
	}

	// if there is no freenode that's large enough, allocate more memory
	if (temp->next == NULL && temp->size <= size)
	{
		int var = size/HEAP_CHUNK_SIZE+1;
		void *loc = sbrk(HEAP_CHUNK_SIZE*(var))+8;
		if (loc == (void *)-1)
		{
			return NULL;
		}
		freenode *new = (freenode*)(loc);
		new->size = temp->size+size;
		new->next = freelist;
		freelist = new;
		prev = temp;
		temp = new;
	}

	if (temp->size - size < 32)
	{
		if (freelist == temp)
		{
			freelist = freelist->next;
		}
		else
		{
			prev->next = temp->next;
		}
		return (freenode *)((char *)temp+8);
	}
	else
	{
		freenode *new2 = (freenode*)(temp+size/16);
		new2->size = temp->size - (size);
		new2->next = temp->next;
		if (temp == freelist)
		{
			freelist = new2;
			freelist->next = temp->next;
		}
		else
		{
			prev->next = new2;
		}
	}
	temp->size = size;
	
	//set the address for the user to be 8 bytes past the size
	//printf("end %zu %p %p\n", freelist->size, freelist, freelist->next);
	return (freenode *) ((char *)temp + 8);  
	//give the address back to application
}

/* return a previously allocated memory chunk to the allocator */
void free(void *ptr)
{
	if (ptr == NULL)
	{
		return;
	}

	// make a new freenode starting 8 bytes before the provided address
	freenode *new_node = (freenode *)(ptr-8);

	// the size is already in memory at the right location (ptr-8)
	// add this memory chunk back to the beginning of the freelist
	new_node->next = freelist;
	freelist = new_node;
	return;
}

