#include <iostream>
#include <cmath>

#include "fs.h"

ostream& operator<<(ostream& os, const Bitmap& bitmap)
{
    for (int i = 0; i < bitmap.get_size(); i++) {
        std::cout << bitmap.read(i);
    }
    return os;
}


INE5412_FS::INE5412_FS(Disk* d) {
	disk = d;
	bitmap = NULL;
} 

INE5412_FS::~INE5412_FS() {
	if (bitmap) delete bitmap;
}

int INE5412_FS::fs_format()
{
	if (superblock.super.magic == FS_MAGIC) {
		std::cout << "cannot format an already mounted disk." << std::endl;
		return 0;
	}

	fs_block block;

	int nblocks = disk->size();

	block.super.magic = FS_MAGIC;
	block.super.nblocks = nblocks;
	block.super.ninodeblocks = ceil(static_cast<double>(nblocks) / 10.0);
	block.super.ninodes = 0;

	disk->write(0, block.data);

	for (int blocknum = 1; blocknum <= block.super.ninodeblocks; blocknum++) {
		// para cada bloco de inode
		fs_block iblock;

		// marca todos os inodes do bloco como invalidos
		for (int j = 0; j < INODES_PER_BLOCK; j++) {
			iblock.inode[j].isvalid = 0;
		}

		disk->write(blocknum, iblock.data);
	}

	return 1;
}

void INE5412_FS::fs_debug()
{
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return;
	}

	union fs_block block;

	disk->read(0, block.data);

	cout << "superblock:\n";
	cout << "    " << (block.super.magic == FS_MAGIC ? "magic number is valid\n" : "magic number is invalid!\n");
 	cout << "    " << block.super.nblocks << " blocks\n";
	cout << "    " << block.super.ninodeblocks << " inode blocks\n";
	cout << "    " << block.super.ninodes << " inodes\n";

	for (int inumber = 1; is_valid_inumber(inumber); inumber++) {
		fs_inode inode = read_inode(inumber);
		if (!inode.isvalid) continue;

		std::cout << "inode " << (inumber - 1) << std::endl;
		std::cout << "    size: " << fs_getsize(inumber) << std::endl;

		std::cout << "    direct blocks: ";
		for (int i = 0; i < POINTERS_PER_INODE; i++) {
			int block_ptr = inode.direct[i];
			if (!block_ptr) break;
			std::cout << block_ptr << " ";
		}
		std::cout << std::endl;


		// se o inode estiver usando o bloco indireto
		if (inode.indirect) {
			std::cout << "    indirect block: " << inode.indirect << std::endl;

			std::cout << "    indirect data blocks: ";
			// dereferencia bloco indireto no disco
			fs_block indirect;
			disk->read(inode.indirect, indirect.data);

			// para cada bloco no vetor de blocos
			for (int j = 0; j < POINTERS_PER_BLOCK; j++) {
				int block_ptr = indirect.pointers[j];
				if (!block_ptr) break;

				std::cout << block_ptr << " ";
			}
			std::cout << std::endl;
		}
	}

	std::cout << "bitmap: " << *bitmap << std::endl;
}

int INE5412_FS::fs_mount()
{
	// le superbloco no disco
	disk->read(0, superblock.data);

	// falha se disco não estiver formatado
	if (superblock.super.magic != FS_MAGIC) return 0;

	// prepara bitmap
	if (bitmap) delete bitmap;
	bitmap = new Bitmap(disk->size());

	for (int i = 0; i <= superblock.super.ninodeblocks; i++) {
		bitmap->write(i, 1);
	}

	// para cada bloco de inode
	for (int blocknum = 1; blocknum <= superblock.super.ninodeblocks; blocknum++) {
		fs_block iblock;
		disk->read(blocknum, iblock.data);

		// para cada inode no bloco
		for (int i = 0; i < INODES_PER_BLOCK; i++) {
			fs_inode& inode = iblock.inode[i];

			// pula blocos invalidos
			if (!inode.isvalid) continue;

			// para cada bloco direto
			for (int j = 0; j < POINTERS_PER_INODE; j++) {
				int block_ptr = inode.direct[j];
				// se for ponteiro nulo, para
				if (!block_ptr) break;

				// marca bloco como em uso
				bitmap->write(block_ptr, 1);
			}

			// se o inode estiver usando o bloco indireto
			if (inode.indirect) {
				// dereferencia bloco indireto no disco
				fs_block indirect;
				disk->read(inode.indirect, indirect.data);

				// para cada bloco no vetor de blocos
				for (int j = 0; j < POINTERS_PER_BLOCK; j++) {
					int block_ptr = indirect.pointers[j];
					// se for ponteiro nulo, para
					if (!block_ptr) break;

					// marca bloco como em uso
					bitmap->write(block_ptr, 1);
				}
			}
		}
	}

	return 1;
}


inline int INE5412_FS::to_inumber(int blocknum, int i) {
	return 1 + (blocknum - 1) * INODES_PER_BLOCK + i;
}
inline int INE5412_FS::get_inumber_blocknum(int inumber) {
	return 1 + (inumber - 1) / INODES_PER_BLOCK;
}
inline int INE5412_FS::get_inumber_index(int inumber) {
	return (inumber - 1) % INODES_PER_BLOCK;
}

inline bool INE5412_FS::is_valid_inumber(int inumber) {
	int blocknum = get_inumber_blocknum(inumber);
	return (
		1 <= blocknum && blocknum <= superblock.super.ninodeblocks
	);
}

int INE5412_FS::get_next_inumber() {
	// para cada bloco de inode
	for (int blocknum = 1; blocknum <= superblock.super.ninodeblocks; blocknum++) {
		fs_block iblock;
		disk->read(blocknum, iblock.data);

		// para cada inode no bloco
		for (int i = 0; i < INODES_PER_BLOCK; i++) {
			fs_inode& inode = iblock.inode[i];

			if (!inode.isvalid) return to_inumber(blocknum, i);
		}
	}

	return 0;
}


INE5412_FS::fs_inode INE5412_FS::read_inode(int inumber) {
	if (!is_valid_inumber(inumber)) {
		fs_inode inode;
		inode.isvalid = 0;
		inode.size = 0;
		return inode;
	};

	int blocknum = get_inumber_blocknum(inumber);
	int index = get_inumber_index(inumber);

	fs_block block;
	disk->read(blocknum, block.data);
	return block.inode[index];
}
void INE5412_FS::write_inode(int inumber, fs_inode& inode) {
	if (!is_valid_inumber(inumber)) return;

	int blocknum = get_inumber_blocknum(inumber);
	int index = get_inumber_index(inumber);

	fs_block block;
	disk->read(blocknum, block.data);

	block.inode[index] = inode;

	disk->write(blocknum, block.data);
}

int INE5412_FS::allocate_block() {
	int blocknum = bitmap->search(0);
	if (blocknum < 0) return -1;

	bitmap->write(blocknum, 1);

	return blocknum;
}
bool INE5412_FS::deallocate_block(int blocknum) {
	if (!bitmap->read(blocknum)) return false;

	bitmap->write(blocknum, 0);
	return true;
}


int INE5412_FS::fs_create() {
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return 0;
	}

	int inumber = get_next_inumber();
	if (!inumber) return 0;

	int blocknum = get_inumber_blocknum(inumber);
	int index = get_inumber_index(inumber);

	fs_block block;

	fs_inode inode;
	inode.isvalid = 1;
	inode.size = 0;
	inode.indirect =0;
	for (int i = 0; i < POINTERS_PER_INODE; i++) {
		inode.direct[i] = 0;
	}

	block.inode[index] = inode;
	disk->write(blocknum, block.data);

	superblock.super.ninodes++;
	disk->write(0, superblock.data);

	return inumber;
}

int INE5412_FS::fs_delete(int inumber) {
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return 0;
	}

	if (!is_valid_inumber(inumber)) return 0;

	int blocknum = get_inumber_blocknum(inumber);
	int index = get_inumber_index(inumber);

	fs_block block;
	disk->read(blocknum, block.data);
	fs_inode inode = block.inode[index];

	// tentou deletar um inode que não existe
	if (!inode.isvalid) return 0;

	inode.isvalid = 0;

	// para cada bloco direto
	for (int j = 0; j < POINTERS_PER_INODE; j++) {
		int block_ptr = inode.direct[j];
		// se for ponteiro nulo, para
		if (!block_ptr) break;

		// marca bloco como livre
		bitmap->write(block_ptr, 0);
	}

	// se o inode estiver usando o bloco indireto
	if (inode.indirect) {
		// dereferencia bloco indireto no disco
		fs_block indirect;
		disk->read(inode.indirect, indirect.data);

		// para cada bloco no vetor de blocos
		for (int j = 0; j < POINTERS_PER_BLOCK; j++) {
			int block_ptr = indirect.pointers[j];
			// se for ponteiro nulo, para
			if (!block_ptr) break;

			// marca bloco como em uso
			bitmap->write(block_ptr, 0);
		}

		bitmap->write(inode.indirect, 0);
	}

	block.inode[index] = inode;
	disk->write(blocknum, block.data);

	// decrementa numero de inodes no superbloco
	superblock.super.ninodes--;
	disk->write(0, superblock.data);

	return 1;
}

int INE5412_FS::fs_getsize(int inumber) {
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return 0;
	}

	if (!is_valid_inumber(inumber)) return 0;

	int blocknum = get_inumber_blocknum(inumber);
	int index = get_inumber_index(inumber);

	fs_block block;
	disk->read(blocknum, block.data);
	fs_inode inode = block.inode[index];

	// tentou verificar o tamanho de um
	// inode que não existe
	if (!inode.isvalid) return -1;

	return inode.size;
}

int INE5412_FS::fs_read(int inumber, char* data, int length, int offset) {
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return 0;
	}

	fs_file file(*this, inumber);

	if (!file.isvalid()) return 0;

	file.seek_set(offset);
	return file.get_string(data, length);	
}

int INE5412_FS::fs_write(int inumber, const char* data, int length, int offset) {
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return 0;
	}

	fs_file file(*this, inumber);

	if (!file.isvalid()) return 0;

	file.seek_set(offset);
	return file.put_string(data, length);
}
