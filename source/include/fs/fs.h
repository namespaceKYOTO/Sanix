#ifndef _FS_H_
#define _FS_H_

/* File System Definition */

#include "type.h"


/* directory */
struct directory {
	u32 fnode_index;	/* number corresponding fnode */
	u32 name_size;		/* maximum name size */
	slist list;
	u8 *name;
};

struct fnode_extent {
	/* B-Tree structure */
	u32 offset_logical;		/* logical block offset */
	u32 offset_physical;	/* physical block offset (*BlockSize から 物理アドレス計算) */
	u32 size;				/* サイズ */
	slist list;
};

/*struct extent {
	u32 index;
	u32 size;
	slist list;
};*/

struct fnode {
	u32 no;				/* fnode number */
	slist list;
	
	u8 access_type;		/* access authority */
	u16 ver;			/* version */
	
	u32 updata_time;
	
	u32 data_size;
	void *data;			/* actual data pointer */
};

/* ファイル情報
 512 alignment
*/
struct super_block{
	u32 block_size;
	u32 update_time;
	
	u32 nr_fnode;		/* number of fnode */
	u32 nr_dir;			/* number of directory */
	
	struct fnode *fnode_root;	/* fnode root */
	struct directory dir_root;	/* derectory root */
	
	u8 padding[ 512 - 24 ];	/**/
};

struct file{
	u32 file_type;
	
} ;

struct fs_type{
} ;


#endif //_FS_H_
