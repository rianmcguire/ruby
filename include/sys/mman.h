#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

// https://github.com/openbsd/src/blob/a246f7a0bc5ae64e47a2d63ae5fa3e50d02d5962/sys/sys/mman.h

/*
 * Protections are chosen from these bits, or-ed together
 */
#define	PROT_NONE	0x00	/* no permissions */
#define	PROT_READ	0x01	/* pages can be read */
#define	PROT_WRITE	0x02	/* pages can be written */
#define	PROT_EXEC	0x04	/* pages can be executed */

/*
 * Flags contain sharing type and options.
 * Sharing types; choose one.
 */
#define	MAP_SHARED	0x0001	/* share changes */
#define	MAP_PRIVATE	0x0002	/* changes are private */

/*
 * Other flags
 */
#define	MAP_FIXED	0x0010	/* map addr must be exactly as requested */
#define	__MAP_NOREPLACE	0x0800	/* fail if address not available */
#define	MAP_ANON	0x1000	/* allocated from memory, swap space */
#define	MAP_ANONYMOUS	MAP_ANON	/* alternate POSIX spelling */
#define	__MAP_NOFAULT	0x2000
#define	MAP_STACK	0x4000	/* mapping is used for a stack */
#define	MAP_CONCEAL	0x8000	/* omit from dumps */

#define	MAP_FLAGMASK	0xfff7

/*
 * Error return from mmap()
 */
#define MAP_FAILED	((void *)-1)

void *	mmap(void *, size_t, int, int, int, off_t);
int	mprotect(void *, size_t, int);
int	munmap(void *, size_t);

#endif
