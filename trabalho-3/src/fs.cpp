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

/**
 * Calcula o inumber com base no número de bloco e índice do inode.
 * 
 * bloco 1: inumbers 1-128
 * bloco 2: inumbers 129-256
 * ...
*/
inline int INE5412_FS::to_inumber(int blocknum, int i) {
	return 1 + (blocknum - 1) * INODES_PER_BLOCK + i;
}
inline int INE5412_FS::get_inumber_blocknum(int inumber) {
	return 1 + (inumber - 1) / INODES_PER_BLOCK;
}
inline int INE5412_FS::get_inumber_index(int inumber) {
	return (inumber - 1) % INODES_PER_BLOCK;
}

/**
 * Checa se o inumber fornecido é válido (não é menor que 0 e
 * não ultrapassa o limite de inumbers possíveis com a quantidade
 * de blocos alocados).
*/
inline bool INE5412_FS::is_valid_inumber(int inumber) {
	int blocknum = get_inumber_blocknum(inumber);
	return (
		1 <= blocknum && blocknum <= superblock.super.ninodeblocks
	);
}

/**
 * Retorna o próximo inumber livre.
*/
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


/**
 * Retorna o inode correspondente ao inumber. Retorna um inode
 * inválido ou nulo se não há um inode para o inumber correspondente.
*/
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
/**
 * Escreve os dados do inode no inumber especificado.
*/
void INE5412_FS::write_inode(int inumber, fs_inode& inode) {
	if (!is_valid_inumber(inumber)) return;

	int blocknum = get_inumber_blocknum(inumber);
	int index = get_inumber_index(inumber);

	fs_block block;
	disk->read(blocknum, block.data);

	block.inode[index] = inode;

	disk->write(blocknum, block.data);
}

/**
 * Procura o próximo bloco de dados livre, marca como
 * em uso no bitmap e retorna seu número de bloco.
 * Se o disco estiver cheio, retorna -1.
*/
int INE5412_FS::allocate_block() {
	int blocknum = bitmap->search(0);
	if (blocknum < 0) return -1;

	bitmap->write(blocknum, 1);

	return blocknum;
}
/**
 * Libera o bloco especificado, marcando como livre no bitmap.
*/
bool INE5412_FS::deallocate_block(int blocknum) {
	if (!bitmap->read(blocknum)) return false;

	bitmap->write(blocknum, 0);
	return true;
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
	block.super.ninodes = block.super.ninodeblocks * INODES_PER_BLOCK;

	disk->write(0, block.data);

	// cria um bloco todo zerado
	fs_block null_block;
	for (int i = 0; i < Disk::DISK_BLOCK_SIZE; i++) {
		null_block.data[i] = 0;
	}
	// copia o bloco zerado em todos os blocos de inode/dados para zerar todos os dados do disco
	for (int blocknum = 1 + 1; blocknum < nblocks; blocknum++) {
		disk->write(blocknum, null_block.data);
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

		std::cout << "inode " << inumber << std::endl;
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

	// marca o superbloco e os blocos de inode como usados
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
				// marca o bloco indireto como usado
				bitmap->write(inode.indirect, 1);

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


int INE5412_FS::fs_create() {
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return 0;
	}

	int inumber = get_next_inumber();
	if (!inumber) return 0;

	fs_inode inode;
	inode.isvalid = 1;
	inode.size = 0;
	inode.indirect = 0;
	for (int i = 0; i < POINTERS_PER_INODE; i++) {
		inode.direct[i] = 0;
	}

	write_inode(inumber, inode);

	return inumber;
}

int INE5412_FS::fs_delete(int inumber) {
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return 0;
	}

	fs_inode inode = read_inode(inumber);

	// tentou deletar um inode que não existe
	if (!inode.isvalid) return 0;

	inode.isvalid = 0;

	// para cada bloco direto
	for (int j = 0; j < POINTERS_PER_INODE; j++) {
		int block_ptr = inode.direct[j];
		// se for ponteiro nulo, para
		if (!block_ptr) break;

		// marca bloco como livre
		deallocate_block(block_ptr);
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
			deallocate_block(block_ptr);
		}

		deallocate_block(inode.indirect);
	}

	write_inode(inumber, inode);

	return 1;
}

int INE5412_FS::fs_getsize(int inumber) {
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return 0;
	}

	fs_inode inode = read_inode(inumber);

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

	// instancia/abre um file stream
	fs_file file(*this, inumber);

	if (!file.isvalid()) return 0;

	// move a posição da stream para offset
	file.seek_set(offset);
	// le a sequencia de chars/dados do arquivo
	// o arquivo fecha e salva quando o objeto file é destruido.
	return file.get_string(data, length);
}

int INE5412_FS::fs_write(int inumber, const char* data, int length, int offset) {
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return 0;
	}
	// instancia/abre um file stream
	fs_file file(*this, inumber);

	if (!file.isvalid()) return 0;
	// move a posição da stream para offset
	file.seek_set(offset);
	// escreve a sequencia de chars/dados do arquivo
	// o arquivo fecha e salva quando o objeto file é destruido.
	return file.put_string(data, length);
}

/**
 * Método para reduzir o tamanho de um arquivo. O arquivo perderá os 
 * `amount` últimos bytes. Desaloca os últimos blocos de dados se ficarem
 * inutilizados.
 * 
 * @param inumber: O inumber do inode que deseja-se reduzir o tamanho.
 * @param amount: A quantidade de bytes que devem ser retirados do fim
 * 		   do arquivo a fim de reduzir o tamanho do arquivo.
*/
int INE5412_FS::fs_truncate(int inumber, int amount) {
	if (superblock.super.magic != FS_MAGIC) {
		std::cout << "disk not mounted." << std::endl;
		return 0;
	}

	// instancia/abre um file stream
	fs_file file(*this, inumber);

	if (!file.isvalid()) return 0;
	// reduz o tamanho do arquivo por amount removendo
	// os últimos caracteres do arquivo.
	return file.truncate(amount);
}