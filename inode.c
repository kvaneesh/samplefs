/*
 *   fs/samplefs/inode.c
 *
 *   Copyright (C) International Business Machines  Corp., 2006, 2007
 *   Author(s): Steve French (sfrench@us.ibm.com)
 *              Aneesh Kumar K.V (aneesh.kumar@linux.vnet.ibm.com)
 *
 *   Sample File System
 *
 *   Primitive example to show how to create a Linux filesystem module
 *
 *   Inode related functions
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation; either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/module.h>
#include <linux/fs.h>
#include "samplefs.h"

extern struct inode *samplefs_get_inode(struct super_block *sb,
					int mode, dev_t dev);


static int sfs_mknod(struct inode *dir, struct dentry *dentry,
		int mode, dev_t dev)
{
	struct inode * inode = samplefs_get_inode(dir->i_sb, mode, dev);
	int error = -ENOSPC;

	printk(KERN_INFO "samplefs: mknod\n");
	if (inode) {
		if (dir->i_mode & S_ISGID) {
			inode->i_gid = dir->i_gid;
			if (S_ISDIR(mode))
				inode->i_mode |= S_ISGID;
		}
		d_instantiate(dentry, inode);
		dget(dentry);   /* Extra count - pin the dentry in core */
		error = 0;
		dir->i_mtime = dir->i_ctime = CURRENT_TIME;

		/* real filesystems would normally use i_size_write function */
		dir->i_size += 0x20;  /* bogus small size for each dir entry */
	}
	return error;
}


static int sfs_mkdir(struct inode * dir, struct dentry * dentry, int mode)
{
	int retval = 0;

	retval = sfs_mknod(dir, dentry, mode | S_IFDIR, 0);

	/* link count is two for dir, for dot and dot dot */
	if (!retval)
		dir->i_nlink++;
	return retval;
}

static int sfs_create(struct inode *dir, struct dentry *dentry, int mode,
		struct nameidata *nd)
{
	return sfs_mknod(dir, dentry, mode | S_IFREG, 0);
}

static int sfs_symlink(struct inode * dir, struct dentry *dentry,
		const char * symname)
{
	struct inode *inode;
	int error = -ENOSPC;

	inode = samplefs_get_inode(dir->i_sb, S_IFLNK|S_IRWXUGO, 0);
	if (inode) {
		int l = strlen(symname)+1;
		error = page_symlink(inode, symname, l);
		if (!error) {
			if (dir->i_mode & S_ISGID)
				inode->i_gid = dir->i_gid;
			d_instantiate(dentry, inode);
			dget(dentry);
			dir->i_mtime = dir->i_ctime = CURRENT_TIME;
		} else
			iput(inode);
	}
	return error;
}


struct inode_operations sfs_file_inode_ops = {
	.getattr        = simple_getattr,
};

struct inode_operations sfs_dir_inode_ops = {
	.create         = sfs_create,
	.lookup         = simple_lookup,
	.link           = simple_link,
	.unlink         = simple_unlink,
	.symlink        = sfs_symlink,
	.mkdir          = sfs_mkdir,
	.rmdir          = simple_rmdir,
	.mknod          = sfs_mknod,
	.rename         = simple_rename,
};
