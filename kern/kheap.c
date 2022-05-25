#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
struct Heap {
	uint32 Start_Add;
	uint32 End_Add;
};
struct Heap heap_info[40959];
uint32 heap_index = 0;
uint32 Next_fit_ptr = KERNEL_HEAP_START;
uint32 Best_fit_ptr = KERNEL_HEAP_START;
struct fit_blocks {
	uint32 Start_Add;
	uint32 End_Add;
	uint32 no_pages;
};
uint32 block_index = 0;
static struct fit_blocks fit_blocks_arr[40959];
void* kmalloc(unsigned int size) {
	//TODO: [PROJECT 2022 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");
	uint32 Full_size = ROUNDUP(size, PAGE_SIZE);
	uint32 modified_size = Full_size;
	struct Frame_Info *frame_info;
	int Counter = 0;
	int free_size = 0;
	uint32 *Ptr_Pd;
	//check nextfit strategy
	if (isKHeapPlacementStrategyNEXTFIT()) {
		for (int i = Next_fit_ptr; i <= KERNEL_HEAP_MAX; i += PAGE_SIZE) {
			//loopback
			if (i == Next_fit_ptr)
				Counter += 1;
			if (i == KERNEL_HEAP_MAX)
				i = KERNEL_HEAP_START;
			if (Counter == 2) {

				if (free_size != Full_size)
					return NULL;
			}
			frame_info = get_frame_info(ptr_page_directory, (void *) i,
					&Ptr_Pd);
			//cprintf("%d :::::: %x", frame_info, i, "\n");
			//looping memory twice and didnt fint any consequance empty frames
			if (frame_info == NULL) {
				if (free_size == 0) {
					//store address of fisrt empty frame
					heap_info[heap_index].Start_Add = i;
				}
				free_size += PAGE_SIZE;
				//store last frame address
				heap_info[heap_index].End_Add = i;
			} else
				free_size = 0;
			if (free_size == Full_size)
				break;

		}
		frame_info = NULL;
		for (int i = heap_info[heap_index].Start_Add;
				i <= heap_info[heap_index].End_Add; i += PAGE_SIZE)
				{
			//reserve frames in frame list
			int frame = allocate_frame(&frame_info);
			map_frame(ptr_page_directory, frame_info, (void*) i,
			PERM_PRESENT | PERM_WRITEABLE);

		}
		Next_fit_ptr = heap_info[heap_index].End_Add + PAGE_SIZE;
		heap_index += 1;
		return (void *) (heap_info[heap_index - 1].Start_Add);
		////////////////////////////////////////////////////////////////
		//check bestfit strategy
	} else if (isKHeapPlacementStrategyBESTFIT()) {
		int flag = 0;
		int block_index = -1;
		int best_index = 0;
		free_size = 0;
		for (int i = KERNEL_HEAP_START; i <= KERNEL_HEAP_MAX; i += PAGE_SIZE) {
			if (i == KERNEL_HEAP_MAX)
				break;

			frame_info = get_frame_info(ptr_page_directory,
					(void *) i, &Ptr_Pd);
			cprintf("%d :::::: %x", frame_info, i, "\n");
			if (frame_info == NULL) {

				if (free_size == 0) {
					block_index += 1;
					fit_blocks_arr[block_index].Start_Add = i; //0 //1
				}
				free_size += PAGE_SIZE;
				fit_blocks_arr[block_index].End_Add = i; //0
				fit_blocks_arr[block_index].no_pages += 1; //0
			} else
				free_size = 0;
			//if(frame_info !=NULL)return NULL;
		}
		int min = 9999;
		best_index = 0;
		for (int i = 0; i <= block_index; i++) {
			cprintf("IN LOOP");
			if (fit_blocks_arr[i].no_pages == Full_size / PAGE_SIZE) {
				best_index = i;
				break;
			}
			if (fit_blocks_arr[i].no_pages < min) {
				min = fit_blocks_arr[i].no_pages;
				best_index = i;
			}
		}

		heap_info[heap_index].Start_Add = fit_blocks_arr[best_index].Start_Add;
		heap_info[heap_index].End_Add = fit_blocks_arr[best_index].End_Add;
		frame_info = NULL;

		for (int i = heap_info[heap_index].Start_Add;
				i <= heap_info[heap_index].End_Add; i += PAGE_SIZE)
				{

			int frame = allocate_frame(&frame_info);
			map_frame(ptr_page_directory, frame_info, (void*) i,
			PERM_PRESENT | PERM_WRITEABLE);

		}
		heap_index += 1;
		return (void *) (heap_info[heap_index - 1].Start_Add);
	} else
		return NULL;
//NOTE: Allocation using NEXTFIT strategy
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
//refer to the project presentation and documentation for details

//TODO: [PROJECT 2022 - BONUS1] Implement a Kernel allocation strategy
// Instead of the Next allocation/deallocation, implement
// BEST FIT strategy
// use "isKHeapPlacementStrategyBESTFIT() ..."
// and " ..."
//functions to check the current strategy
//change this "return" according to your answer
}

void kfree(void* virtual_address) {
	//TODO: [PROJECT 2022 - [2] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
	//loop on heap struct
	for (int i = 0; i < heap_index; i++) {
		//loop from start until end address at specific heap user struct
		for (uint32 va = heap_info[i].Start_Add; va <= heap_info[i].End_Add;
				va +=
				PAGE_SIZE)
				{
			//check if the input address is equal to address in struct between start and end
			if ((uint32) virtual_address == va) {
				for (uint32 j = heap_info[i].Start_Add;
						j <= heap_info[i].End_Add; j += PAGE_SIZE)
						{
					unmap_frame(ptr_page_directory, (void *) j);
					uint32 *Ptr_Pd;
					get_page_table(ptr_page_directory, (void *) j, &Ptr_Pd);
					Ptr_Pd[PTX(j)] = 0;
				}
			}
		}
	}
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details
}

unsigned int kheap_virtual_address(unsigned int physical_address) {
	//TODO: [PROJECT 2022 - [3] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

	struct Frame_Info *frame_info = to_frame_info(physical_address);
	uint32 frame_number = to_frame_number(frame_info);
	//loop on heap struct
	for (int i = 0; i < heap_index; i++) {
		//loop from start until end address at specific heap user struct
		for (uint32 va = heap_info[i].Start_Add; va <= heap_info[i].End_Add;
				va +=
				PAGE_SIZE) {
			uint32*Ptr_Pd;
			get_page_table(ptr_page_directory, (void*) va, &Ptr_Pd);
			//check present bit at specific page table
			if ((Ptr_Pd != NULL) && (Ptr_Pd[PTX(va)] & PERM_PRESENT) == 1) {
				if (frame_number == Ptr_Pd[PTX(va)] >> 12) {
					return va;
				}
			}
		}
	}
//return the virtual address corresponding to given physical_address
//refer to the project presentation and documentation for details

//change this "return" according to your answer
	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address) {
//TODO: [PROJECT 2022 - [4] Kernel Heap] kheap_physical_address()
// Write your code here, remove the panic and write your code
//panic("kheap_physical_address() is not implemented yet...!!");
	uint32 Phy_add = 0;
	uint32 *Ptr_Pd;
	struct Frame_Info *frame_info = get_frame_info(ptr_page_directory,
			(void *) virtual_address, &Ptr_Pd);
	//check if frame is empty or not
	if (frame_info == NULL) {
		return 0;
	} else {
		//return physical address of frame at given virtual address
		Phy_add = to_physical_address(frame_info);
	}
	return Phy_add;
}

/*
 * get num of pages of givn size
 * get num of pages for the whole kernel
 *
 * */
