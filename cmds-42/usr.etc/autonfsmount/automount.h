/*       @(#)automount.h	1.1 88/03/07 4.0NFSSRC SMI   */

/*
 * automount.h 1.6 87/10/07 Copyright 1987 Sun Microsystems, Inc.
 */

/*
 * General queue structure 
 */
struct queue {
	struct queue	*q_next;
#define	q_head	q_next
	struct queue	*q_prev;
#define	q_tail	q_prev
};

#define	INSQUE(head, ptr) my_insque(&(head), &(ptr)->q)
#define	REMQUE(head, ptr) my_remque(&(head), &(ptr)->q)
#define HEAD(type, head) ((type *)(head.q_head))
#define NEXT(type, ptr)	((type *)(ptr->q.q_next))
#define	TAIL(type, head) ((type *)(head.q_tail))
#define PREV(type, ptr)	((type *)(ptr->q.q_prev))
	

/*
 * Types of filesystem entities (vnodes)
 * We support only one level of DIR; everything else is a symbolic LINK
 */
enum vn_type { VN_DIR, VN_LINK};
struct vnode {
	struct queue q;
	nfs_fh	vn_fh;		/* fhandle */
	struct fattr vn_fattr;	/* file attributes */
	enum vn_type vn_type;	/* type of vnode */
	caddr_t	vn_data;	/* vnode private data */
};

struct vnode *fhtovn();		/* lookup vnode given fhandle */

/*
 * Structure describing a host/filesystem/dir tuple in a YP map entry
 */
struct mapfs {
	struct mapfs *mfs_next;		/* next in entry */
	char	*mfs_host;		/* host name */
	struct in_addr mfs_addr;	/* address of host */
	char	*mfs_dir;		/* dir to mount */
	char	*mfs_subdir;		/* subdir of dir */
};

/*
 * YP entry - lookup of name in DIR gets us this
 */
struct mapent {
	char	*map_mntopts;
	struct mapfs *map_fs;
};
struct mapent *getmapent();

/*
 * Everthing we know about a mounted filesystem
 * Can include things not mounted by us (fs_mine == 0)
 */
struct filsys {
	struct queue q;			/* next in queue */
	struct autodir *fs_hostdir;	/* dir for which this fs was mounted */
	int	fs_refcnt;		/* reference count */
	int	fs_mine;		/* 1 if we mounted this fs */
	dev_t	fs_dev;			/* devnum for checking unmounts */
	char	*fs_host;		/* host name */
	char	*fs_dir;		/* dir of host mounted */
	char	*fs_mntpnt;		/* local mount point */
	char	*fs_opts;		/* mount options */
	struct nfs_args fs_nfsargs;	/* nfs mount args */
	struct sockaddr_in fs_addr;	/* host address */
	nfs_fh	fs_rootfh;		/* file handle for nfs mount */
	int	fs_mflags;		/* mount flags */
};
struct queue fs_q;
struct filsys *already_mounted(), *alloc_fs();

/*
 * Structure for recently referenced links
 */
struct link {
	struct queue q;		/* next in queue */
	struct vnode link_vnode;	/* space for vnode */
	struct autodir *link_dir;	/* dir which we are part of */
	char	*link_name;	/* this name in dir */
	struct filsys *link_fs;	/* mounted file system */
	char	*link_subdir;	/* dir within file system */
	long	link_death;	/* time when no longer valid */
};
struct link *makelink();
struct link *findlink();
	
/*
 * Descriptor for each directory served by the automounter 
 */
struct autodir {
	struct queue q;
	struct	vnode dir_vnode;	/* vnode */
	char	*dir_name;	/* mount point */
	char	*dir_map;	/* name of map for dir */
	char	*dir_opts;	/* default mount options */
	int	dir_maptype;	/* map type */
	int	dir_remove;	/* remove mount point */
	struct queue dir_head;
};
struct queue dir_q;
#define	MAPTYPE_FILE	0
#define	MAPTYPE_YP	1
#define	MAPTYPE_HOSTS	2
#define	MAPTYPE_PASSWD	3
#define MAPTYPE_FSTAB	4

char self[64];		/* my hostname */
char mydomain[64];	/* my domain name */
char mysite[64];	/* my site */
char tmpdir[200];	/* real name of /tmp */
struct in_addr my_addr;	/* my inet address */
#ifdef NeXT
int usingYP;		/* are we using YP? */
#endif

char *malloc(), *index(), *strdup();
long time_now;		/* set at start of processing of each RPC call */
int mount_timeout;	/* max seconds to wait for mount */
int max_link_time;	/* seconds to keep link around */
int nomounts;		/* don't do any mounts - for cautious servers */
nfsstat lookup(), mountall(), trymount(), nfsmount();
