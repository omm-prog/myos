#ifndef VFS_H
#define VFS_H

void vfs_init(void);
void vfs_list(void);
const char *vfs_read(const char *name);
void vfs_append(const char *name, const char *line);

#endif

