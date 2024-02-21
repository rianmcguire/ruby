#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H
struct dirent {
    char	d_name[256];		/* File name		*/
    int		d_namlen;		/* Length of d_name */
};

typedef struct _dirdesc {} DIR;
#endif
