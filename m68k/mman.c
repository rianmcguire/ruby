// https://github.com/WebAssembly/wasi-libc/blob/212296e4fa1c59f7651b1aefb6a399ed9138d97a/libc-bottom-half/mman/mman.c

// Userspace emulation of mmap and munmap. Restrictions apply.
//
// This is meant to be complete enough to be compatible with code that uses
// mmap for simple file I/O. It just allocates memory with malloc and reads
// and writes data with pread and pwrite.

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

struct map {
    int prot;
    int flags;
    off_t offset;
    size_t length;
};

void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset) {
    // Check for unsupported flags.
    if ((flags & (MAP_PRIVATE | MAP_SHARED)) == 0 ||
        (flags & MAP_FIXED) != 0 ||
#ifdef MAP_SHARED_VALIDATE
        (flags & MAP_SHARED_VALIDATE) == MAP_SHARED_VALIDATE ||
#endif
#ifdef MAP_NORESERVE
        (flags & MAP_NORESERVE) != 0 ||
#endif
#ifdef MAP_GROWSDOWN
        (flags & MAP_GROWSDOWN) != 0 ||
#endif
#ifdef MAP_HUGETLB
        (flags & MAP_HUGETLB) != 0 ||
#endif
#ifdef MAP_FIXED_NOREPLACE
        (flags & MAP_FIXED_NOREPLACE) != 0 ||
#endif
        0)
    {
        errno = EINVAL;
        return MAP_FAILED;
    }

    // Check for unsupported protection requests.
    if (prot == PROT_NONE ||
#ifdef PROT_EXEC
        (prot & PROT_EXEC) != 0 ||
#endif
        0)
    {
        errno = EINVAL;
        return MAP_FAILED;
    }

    //  To be consistent with POSIX.
    if (length == 0) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    // Check for integer overflow.
    size_t buf_len = 0;
    if (__builtin_add_overflow(length, sizeof(struct map), &buf_len)) {
        errno = ENOMEM;
        return MAP_FAILED;
    }

    // Allocate the memory.
    struct map *map = malloc(buf_len);
    if (!map) {
        errno = ENOMEM;
        return MAP_FAILED;
    }

    // Initialize the header.
    map->prot = prot;
    map->flags = flags;
    map->offset = offset;
    map->length = length;

    // Initialize the main memory buffer, either with the contents of a file,
    // or with zeros.
    addr = map + 1;
    if ((flags & MAP_ANON) == 0) {
        // TODO: fake file mmap
        return MAP_FAILED;
    } else {
        memset(addr, 0, length);
    }

    return addr;
}

int munmap(void *addr, size_t length) {
    struct map *map = (struct map *)addr - 1;

    // We don't support partial munmapping.
    if (map->length != length) {
        errno = EINVAL;
        return -1;
    }

    // Release the memory.
    free(map);

    // Success!
    return 0;
}

int mprotect(void *addr, size_t len, int prot) {
    errno = ENOTSUP;
    return -1;
}

// Dumping ground for other missing stdlib functions below

#include <unistd.h>
#include <sys/stat.h>

long sysconf(int name)
{
    if (name == _SC_PAGESIZE) {
        return 4096;
    }

    errno = EINVAL;
    return -1;
}

uid_t getuid() {
    return 42;
}

uid_t geteuid() {
    return 42;
}

gid_t getgid() {
    return 42;
}

gid_t getegid() {
    return 42;
}

int access(const char *path, int amode)
{
    struct stat buffer;
    return stat(path, &buffer);
}

unsigned int sleep(unsigned int seconds)
{
}

int dup(int oldfd)
{
    errno = ENOTSUP;
    return -1;
}

int dup2(int oldfd, int newfd)
{
    errno = ENOTSUP;
    return -1;
}

int execl(const char *pathname, const char *arg, ...
                /*, (char *) NULL */)
{
    errno = ENOTSUP;
    return -1;
}

int execv(const char *pathname, char *const argv[])
{
    errno = ENOTSUP;
    return -1;
}

int execle(const char *pathname, const char *arg, ...
                /*, (char *) NULL, char *const envp[] */)
{
    errno = ENOTSUP;
    return -1;
}

pid_t getppid(void)
{
    return 1;
}

char *getwd(char *buf)
{
    errno = ENOTSUP;
    return NULL;
}

ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset)
{
    off_t before = lseek(fildes, 0, SEEK_CUR);
    if (before < 0) {
        return before;
    }

    if (lseek(fildes, offset, SEEK_SET) < 0) {
        errno = EOVERFLOW;
        return -1;
    }
    ssize_t ret = read(fildes, buf, nbyte);
    int reterr = errno;

    lseek(fildes, before, SEEK_SET);

    errno = reterr;
    return ret;
}

ssize_t pwrite(int fildes, const void *buf, size_t nbyte,
    off_t offset)
{
    off_t before = lseek(fildes, 0, SEEK_CUR);
    if (before < 0) {
        return before;
    }

    if (lseek(fildes, offset, SEEK_SET) < 0) {
        errno = EOVERFLOW;
        return -1;
    }
    ssize_t ret = write(fildes, buf, nbyte);
    int reterr = errno;

    lseek(fildes, before, SEEK_SET);

    errno = reterr;
    return ret;
}

#include <dirent.h>

DIR *opendir(const char *name)
{
    errno = ENOTSUP;
    return NULL;
}

int closedir(DIR *dirp)
{
    errno = ENOTSUP;
    return -1;
}


struct dirent *readdir(DIR *dirp)
{
    errno = ENOTSUP;
    return NULL;
}

void rewinddir(DIR *dirp)
{
}

int chdir(const char *path)
{
    errno = ENOTSUP;
    return -1;
}

int mkdir(const char *path, mode_t mode)
{
    errno = ENOTSUP;
    return -1;
}

int rmdir(const char *path)
{
    errno = ENOTSUP;
    return -1;
}

int pipe(int fildes[2])
{
    errno = ENOTSUP;
    return -1;
}

#include <sys/stat.h>

int chmod(const char *path, mode_t mode)
{
    return access(path, F_OK);
}

int chown(const char *path, uid_t owner, gid_t group)
{
    return access(path, F_OK);
}

mode_t umask(mode_t cmask) {
    return 0;
}

#include <utime.h>

int utime(const char *path, const struct utimbuf *times)
{
    return access(path, F_OK);
}

#include <pwd.h>

struct passwd *getpwnam(const char *name)
{
    errno = ENOTSUP;
    return NULL;
}

void endpwent()
{
}

#include <sys/resource.h>

int getrusage(int who, struct rusage *r_usage)
{
    errno = ENOTSUP;
    return -1;
}

#include <stdio.h>

FILE *popen(const char *command, const char *type)
{
    errno = ENOTSUP;
    return NULL;
}

int pclose(FILE *stream)
{
    errno = ENOTSUP;
    return -1;
}

#include <time.h>

int clock_nanosleep(clockid_t clock_id, int flags,
    const struct timespec *rqtp, struct timespec *rmtp)
{
    return 0;
}

#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *stat_loc, int options)
{
    errno = ENOTSUP;
    return -1;
}

#include <sys/select.h>

int select(int nfds, fd_set *restrict readfds,
    fd_set *restrict writefds, fd_set *restrict errorfds,
    struct timeval *restrict timeout)
{
    errno = ENOTSUP;
    return -1;
}
