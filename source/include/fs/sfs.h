/*---------------------------------------------------------------------*/
/*!
* @file		sfs.h
* @brief	Sanix File System
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/


#ifndef _SFS_H_
#define _SFS_H_

#include "type.h"

#define SFS_BLOCK_SIZE 512
#define SFS_SB_MAGIC	0x

//#define SFS_AUTHORITY_READ 1

/*
 * Directory
 */
struct sfs_dir {
	u32 fnode_index;	/* ëŒâûfnodeî‘çÜ */
	slist list;
	u32 name_size;
	u8 *name;			/* name */
};

struct sfs_fnode_extent {
	/* B-Tree structure */
	u32 offset_logical;		/* logical block offset */
	u32 offset_physical;	/* physical block offset s*/
	u32 size;				/* data size */
	slist list;
};

struct empty_extent {
	u32 bindex;			/* block index */
	u32 size;
	slist list;
};

struct sfs_fnode {
	u32 no;				/* fnode number */
	slist list;
	
	u8 access_type;		/* access authority */
	u16 ver;			/* version */
	
	u32 updata_time;
	
	u32 nr_ext;			/* number of extent */
	struct sfs_fnode_extent *ext_root;
	
	u8 data_type;
	u32 data_size;
	void *data;			/* actual data pointer */
};

/*
 * SFS superbock info
 * 512byte alignment
 */
struct super_block{
	u32 magic;
	u32 block_size;
	u32 update_time;
	
	u32 nr_fnode;		/* number of fnode */
	u32 nr_dir;			/* number of directory */
	
	struct sfs_fnode *fnode_root;	/* fnode root */
	struct sfs_dir dir_root;		/* derectory root */
	
	u8 padding[ 512 - 24 ];
};

/*
struct file{
	u32 file_type;
	
};

struct fs_type{
};
*/

/* get super block */
struct super_block* sb_get( void );


#endif //_SFS_H_
