#ifndef FS_H
#define FS_H

#include <cmath>
#include <iostream>
#include "disk.h"

class Bitmap {
	char* bits;

public:

	Bitmap(int size) {
        int arr_size = ceil(size / 8);

		bits = new char[arr_size];

        for (int i = 0; i < arr_size; i++) {
		    bits[i] = 0;
        }
	}

	~Bitmap() {
		delete bits;
	}

    bool read(int blocknum) {
        int i = blocknum / 8;
        int pos = blocknum % 8;

        return (bits[i] >> pos) % 2;
    }

    void write(int blocknum, bool bit) {
        int i = blocknum / 8;
        int pos = blocknum % 8;

        if (bit) {
            bits[i] |= 1 << pos; 
        }
        else {
            bits[i] &= ~(1 << pos);
        }
    }
};


class INE5412_FS
{
public:
    static const unsigned int FS_MAGIC = 0xf0f03410;
    static const unsigned short int INODES_PER_BLOCK = 128;
    static const unsigned short int POINTERS_PER_INODE = 5;
    static const unsigned short int POINTERS_PER_BLOCK = 1024;

    class fs_superblock {
        public:
            unsigned int magic;
            int nblocks;
            int ninodeblocks;
            int ninodes;
    }; 

    class fs_inode {
        public:
            int isvalid;
            int size;
            int direct[POINTERS_PER_INODE];
            int indirect;
    };

    union fs_block {
        public:
            fs_superblock super;
            fs_inode inode[INODES_PER_BLOCK];
            int pointers[POINTERS_PER_BLOCK];
            char data[Disk::DISK_BLOCK_SIZE];
    };


    int get_next_inumber();

    inline int to_inumber(int blocknum, int i);
    inline int get_inumber_blocknum(int inumber);
    inline int get_inumber_index(int inumber);

    inline bool is_valid_inumber(int inumber);

    fs_inode read_inode(int inumber);
    void write_inode(int inumber, fs_inode& inode);

public:

    INE5412_FS(Disk* d);

    ~INE5412_FS();

    void fs_debug();
    int  fs_format();
    int  fs_mount();

    int  fs_create();
    int  fs_delete(int);
    int  fs_getsize(int);

    int  fs_read(int inumber, char* data, int length, int offset);
    int  fs_write(int inumber, const char* data, int length, int offset);

private:
    Disk* disk = NULL;
    Bitmap* bitmap = NULL;
    fs_block superblock;
};

#endif