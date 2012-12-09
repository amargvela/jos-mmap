#include <inc/lib.h>

#define debug	0

// Maximum number of mmapped regions. Default is 256, as the required metadata
// fill fit on a single page.
#define MAXMMAP	256

#define	MMAPMETADATA	

// Returns the metadata struct for index i.
#define INDEX2MMAP(i)	((struct mmap_metadata*) (MMAPTABLE + (i)*0x1000))

// Struct for storing the metadata about each mmapped region.
struct mmap_metadata {
	int mmmd_fileid;
	uint32_t mmmd_fileoffset;
	void *mmmd_startaddr;
	void *mmmd_endaddr;
};


// Implementation of mmap(), which maps address space to a memory object.
// Sets up a mapping between a section of a process' virtual address space,
// starting at addr, and some memory object represented by fd with offset off,
// continuing for len bytes. Returns a pointer to the mapped address if
// successful; otherwise, returns a negative pointer.
void *
mmap(void *addr, size_t len, int prot, int flags, int fd_num, off_t off)
{
	uint32_t retva, i;

	// Sanity check for offset, which must be a multiple of PGSIZE.
	if ((off % PGSIZE) != 0) return (void *)-E_INVAL;

	// Sanity check for prot bit.
	if ((prot & ~PTE_W) != 0) return (void *) -E_INVAL;

	// Attempt to find a contiguous region of memeory of size len
	cprintf("mmap() - find free memory \n");
	retva = sys_page_block_alloc(0, addr, len / PGSIZE, PTE_U|prot);
	if (retva < 0) {
		cprintf("mmap() - failure from sys_page_block_alloc: %d \n", retva);
		return (void *)retva;
	}
	cprintf("mmap() - start memory address: %p, UTOP: %p \n", (uint32_t)retva, UTOP); // va>UTOP??

	if ((flags & MAP_SHARED) != 0) {
		// Install handler for MAP_SHARE
		sys_env_set_region_handler(0, mmap_shared_handler, (void *) retva,
				       (void *) retva + len/PGSIZE);
	} else {
		// Install handler for MAP_PRIVATE
		sys_env_set_region_handler(0, mmap_private_handler, (void *) retva,
				       (void *) retva + len/PGSIZE);
	}

	// TODO: Allocate page(s) for mmap metadata if necessary.
	// Allocates a page to hold mmaped_region structs
	//if ((r = sys_page_alloc(0, (void *) MMAPTABLE, PTE_P|PTE_W|PTE_U)) < 0)
	//	panic("sys_page_alloc: %e", r);

	
	// Finds the smallest i that isn't already used by a mmap_metadata
	// struct, fills in its values, and returns retva. Returns negative on
	// failure.
	for (i = 0; i < MAXMMAP; i++) {
		// TODO: Check if the struct is truly free.
		// TODO: Create mmap_metadata struct and assign values.
		return (void *) retva;
	}
	
	// Couldn't map correctly.
	return (void *) -E_INVAL;


}

int
munmap(void *addr, size_t len)
{
	return 0;
}

// Handler for pages mmapped with the MAP_SHARED flag
static void
mmap_shared_handler(struct UTrapframe *utf)
{
	//union Fsipc fsipcbuf;

	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;

	//fsibcbuf.breq.req_fileid = 0x0;
	//fsibcbuf.breq.req_offset = 0x1;
	//fsibcbuf.breq.req_perm = 0x1;
	
	// TODO: Look up mmap region meta data using va
	// 	 Not sure the best way to do this, maybe redo the struct?
	
	// TODO: Determine fault type r/w and set appropriate perms.
	//	 If 0, then 0. If PTE_W, then PTE_W

	// TODO: Make the appropriate IPC call to the FS.
	 
}

// Handler for pages mmapped with the MAP_PRIVATE flag
static void
mmap_private_handler(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;

	// TODO: Look up mmap region meta data using va

	// TODO: Determine fault type r/w and set appropriate perms.
	//	 If 0, then 0. If PTE_W, then copy perms and PTE_W

	// TODO: Make the appropriate IPC call to the FS.
}
