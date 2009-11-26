/*
 *   fs/samplefs/super.c
 *
 *   Copyright (C) International Business Machines  Corp., 2006,2007,2008,2009
 *   Author(s): Steve French (sfrench@us.ibm.com)
 *              Aneesh Kumar K.V (aneesh.kumar@linux.vnet.ibm.com)
 *
 *   Sample File System
 *
 *   Primitive example to show how to create a Linux filesystem module
 *
 *   superblock related and misc. functions
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
#include <linux/pagemap.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include "samplefs.h"

/* helpful if this is different than other fs */
#define SAMPLEFS_MAGIC     0x73616d70 /* "SAMP" */

static void samplefs_put_super(struct super_block *sb)
{
	struct samplefs_sb_info *sfs_sb;

	sfs_sb = SFS_SB(sb);
	if (sfs_sb == NULL) {
		/* Empty superblock info passed to unmount */
		return;
	}
	/* FS-FILLIN your fs specific umount logic here */
	kfree(sfs_sb);
	sb->s_fs_info = NULL;
	return;
}

struct super_operations samplefs_super_ops = {
	.statfs         = simple_statfs,
	.drop_inode     = generic_delete_inode, /* Not needed, is the default */
	.put_super      = samplefs_put_super,
};

struct inode *samplefs_get_inode(struct super_block *sb, int mode, dev_t dev)
{
	struct inode * inode = new_inode(sb);

	if (inode) {
		inode->i_mode = mode;
		inode->i_uid = current_fsuid();
		inode->i_gid = current_fsgid();
		inode->i_blocks = 0;
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		switch (mode & S_IFMT) {
			default:
				init_special_inode(inode, mode, dev);
				break;
			case S_IFREG:
				printk(KERN_INFO "file inode\n");
				inode->i_op = &simple_dir_inode_operations;
				break;
			case S_IFDIR:
				inode->i_op = &simple_dir_inode_operations;

				/* link == 2 (for initial ".." and "." entries) */
				inode->i_nlink++;
				break;
		}
	}
	return inode;
}

static int samplefs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *root;
	struct samplefs_sb_info * sfs_sb;

	sb->s_maxbytes = MAX_LFS_FILESIZE; /* NB: may be too large for mem */
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = SAMPLEFS_MAGIC;
	sb->s_op = &samplefs_super_ops;
	sb->s_time_gran = 1; /* 1 nanosecond time granularity */

	printk(KERN_INFO "samplefs: fill super\n");
	root = samplefs_get_inode(sb, S_IFDIR | 0755, 0);
	if (!root)
		return -ENOMEM;

	sb->s_fs_info = kzalloc(sizeof(struct samplefs_sb_info), GFP_KERNEL);
	sfs_sb = SFS_SB(sb);
	if (!sfs_sb) {
		iput(root);
		return -ENOMEM;
	}
	printk(KERN_INFO "samplefs: about to alloc dentry root inode\n");
	sb->s_root = d_alloc_root(root);
	if (!sb->s_root) {
		iput(root);
		kfree(sfs_sb);
		sb->s_fs_info = NULL;
		return -ENOMEM;
	}
	/* FS-FILLIN your filesystem specific mount logic/checks here */
	return 0;
}

static struct dentry *samplefs_mount(struct file_system_type *fs_type,
				      int flags, const char *dev_name,
				      void *data)
{
	return mount_nodev(fs_type, flags, data, samplefs_fill_super);
}

static void samplefs_kill_sb(struct super_block *sb)
{
	kill_litter_super(sb);
}

static struct file_system_type samplefs_fs_type = {
	.owner = THIS_MODULE,
	.name = "samplefs",
	.mount = samplefs_mount,
	.kill_sb = samplefs_kill_sb,
	/*  .fs_flags */
};


static int __init init_samplefs_fs(void)
{
	printk(KERN_INFO "init samplefs\n");
	return register_filesystem(&samplefs_fs_type);
}

static void __exit exit_samplefs_fs(void)
{
	printk(KERN_INFO "unloading samplefs\n");
	unregister_filesystem(&samplefs_fs_type);
}

module_init(init_samplefs_fs)
module_exit(exit_samplefs_fs)
MODULE_LICENSE("GPL");
