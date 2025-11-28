#ifndef VFS_H
#define VFS_H

#define VFS_NAME_LEN 32
#define VFS_PATH_LEN 64

void vfs_init(void);
int vfs_mkdir(const char *path);
int vfs_list(const char *path);
int vfs_create(const char *path, const char *initial);
int vfs_write(const char *path, const char *text);
const char *vfs_read(const char *path);
void vfs_append(const char *path, const char *line);
int vfs_exists(const char *path);
int vfs_is_dir(const char *path);

#endif
