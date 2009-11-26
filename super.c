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
#include <linux/version.h>
#include <linux/slab.h>

/* helpful if this is different than other fs */
#define SAMPLEFS_MAGIC     0x73616d70 /* "SAMP" */

static int samplefs_fill_super(struct super_block * sb, void * data, int silent)
{
	printk(KERN_ERR "samplefs not fully done\n");
	return -EINVAL;
}

static struct dentry *samplefs_mount(struct file_system_type *fs_type,
				      int flags, const char *dev_name,
				      void *data)
{
	return mount_nodev(fs_type, flags, data, samplefs_fill_super);
}

static void samplefs_kill_sb(struct super_block *sb)
{
	kfree(sb->s_fs_info);
	sb->s_fs_info = NULL;
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
