#include <inc/lib.h>
// malloc()
//	This function use NEXT FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
struct user_heap_info {
	uint32 start_add;
	uint32 end_add;
};
struct user_heap_info user_info[97000];
uint32 Modified_page = (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE;
uint32 heap_pages = (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE;
uint32 user_arr[131072];
uint32 user_index = 0;
uint32 next = 0;

void* malloc(uint32 size) {
	uint32 first = 0, last = 0, free_size = 0;
	int counter = 0;
	uint32 Full_size = ROUNDUP(size, PAGE_SIZE);
	uint32 no_of_pages = Full_size / PAGE_SIZE;

	//TODO: [PROJECT 2022 - [9] User Heap malloc()] [User Side]
	// Write your code here, remove the panic and write your code
	//	panic("malloc() is not implemented yet...!!");
	// Steps:
	//	1) Implement NEXT FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	// 	4) Return pointer containing the virtual address of allocated space,
	//

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyNEXTFIT()
	//to check the current strategy
	if (sys_isUHeapPlacementStrategyNEXTFIT()) {

		for (uint32 i = next; i <= heap_pages; i++) {
			// LOOP BACK
			if (no_of_pages > Modified_page)
				return NULL;
			if (i == heap_pages)
				i = 0;

			if (i == next)
				counter += 1;
			if (counter == 2)
				return NULL;
            //check the presence of free consquense elements in user array and it start from fisrt element
			if (user_arr[i] == 0 && free_size == 0) {
				//store address of fisrt empty element and its index in array
				user_info[user_index].start_add = USER_HEAP_START
						+ (i * PAGE_SIZE);
				first = i;
			}
			//store address of end empty element and its index in array
			user_info[user_index].end_add = USER_HEAP_START + (i * PAGE_SIZE);
			last = i;
			free_size += PAGE_SIZE;
			if (user_arr[i] == 1) {
				free_size = 0;
			}

			if (free_size == Full_size) {
				break;
			}
		}
		uint32 no_of_user_pages = 0;
		no_of_user_pages = (user_info[user_index].end_add - user_info[user_index].start_add)
				/ PAGE_SIZE;
		if (free_size == Full_size) {
			sys_allocateMem(user_info[user_index].start_add, no_of_user_pages);
			Modified_page -= (last - first);
			for (uint32 i = first; i <= last; i++)
				user_arr[i] = 1;
		}

		next = last + 1;
		user_index += 1;
		return (void *) (user_info[user_index - 1].start_add);
	}
	return NULL;
}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable) {
	panic("smalloc() is not required ..!!");
	return NULL;
}

void* sget(int32 ownerEnvID, char *sharedVarName) {
	panic("sget() is not required ..!!");
	return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user_heap_info mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address) {
	//TODO: [PROJECT 2022 - [11] User user_heap free()] [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	uint32 first = 0, last = 0;
	int i =0;
	while (i < user_index) {
		if (user_info[i].start_add == (uint32) virtual_address) {
			first = ((uint32) virtual_address - USER_HEAP_START) / PAGE_SIZE;
			last = (user_info[i].end_add - USER_HEAP_START) / PAGE_SIZE;
			for (uint32 j = first; j <= last; j++) {
				user_arr[j] = 0;
				Modified_page += 1;
			}
			sys_freeMem((uint32) virtual_address, user_info[i].end_add);
			user_info[i].start_add = 0;
			user_info[i].end_add = 0;
			break;

		}
		i+=1;

		//you shold get the size of the given allocation using its address
		//you need to call sys_freeMem()
		//refer to the project presentation and documentation for details

	}
}

void sfree(void* virtual_address) {
	panic("sfree() is not requried ..!!");
}

//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size) {
	//TODO: [PROJECT 2022 - BONUS3] User user_heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}
