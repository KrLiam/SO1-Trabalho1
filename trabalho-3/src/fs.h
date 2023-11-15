#ifndef FS_H
#define FS_H

#include <cmath>
#include <iostream>
#include "disk.h"

class Bitmap {
	char* bits;
    int arr_size;

public:

	Bitmap(int size) {
        arr_size = ceil(static_cast<double>(size) / 8.0);

		bits = new char[arr_size];

        for (int i = 0; i < arr_size; i++) {
		    bits[i] = 0;
        }
	}

	~Bitmap() {
		delete bits;
	}

    int get_size() {
        return arr_size * 8;
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

    int search(bool bit) {
        if (bit) {
            for (int i = 0; i < arr_size; i++) {
                char value = bits[i];
                if (value == 0) continue;

                for (int j = 0; j < 8; j++) {
                    if (value % 2 == 1) return i*8 + j;
                    value >>= 1;
                }
            }
        }
        else {
            for (int i = 0; i < arr_size; i++) {
                char value = bits[i];
                if (value == 127) continue;

                for (int j = 0; j < 8; j++) {
                    if (value % 2 == 0) return i*8 + j;
                    value >>= 1;
                }
            }
        }

        return -1;
    }
};


class INE5412_FS
{
public:
    static const unsigned int FS_MAGIC = 0xf0f03410;
    static const unsigned short int INODES_PER_BLOCK = Disk::DISK_BLOCK_SIZE / 32;
    static const unsigned short int POINTERS_PER_INODE = 5;
    static const unsigned short int POINTERS_PER_BLOCK = Disk::DISK_BLOCK_SIZE / 4;

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

    int allocate_block();
    bool deallocate_block(int blocknum);

    class fs_file {
        INE5412_FS& fs;
        int inumber;

        INE5412_FS::fs_inode inode;
        bool inode_dirty = false;

        long pos;
        fs_block block;
        int block_index = -1;
        bool block_dirty = false;

        /**
         * Salva o bloco de dados atual em memória no disco.
        */
        void save() {
            if (block_index < 0) return;

            if (block_index < INE5412_FS::POINTERS_PER_INODE) {
                int blocknum = inode.direct[block_index];
                fs.disk->write(blocknum, block.data);
                // std::cout << "salvou bloco " << blocknum << " no disco (indice " << block_index << ")." << std::endl;
            }
            else {
                // FIXME testar se tem bloco indireto
                fs_block indirect;
                fs.disk->read(inode.indirect, indirect.data);
                int blocknum = indirect.pointers[block_index - INE5412_FS::POINTERS_PER_INODE];
                fs.disk->write(blocknum, block.data);
                // std::cout << "salvou bloco " << blocknum << " no disco (indice " << block_index << ")." << std::endl;
            }

            block_dirty = false;
        }

        /**
         * Carrega o bloco de dados atual do disco para a memória.
        */
        void load() {
            // FIXME testar se posição ta dentro do tamanho do arquivo

            int block_i = pos / Disk::DISK_BLOCK_SIZE;
            // ignora se o bloco já está carregado
            if (block_i == block_index) return;

            // salvo o bloco carregado atualmente se estiver sujo
            if (block_dirty) save();

            if (block_i < INE5412_FS::POINTERS_PER_INODE) {
                int blocknum = inode.direct[block_i];

                if (!blocknum) return;
                fs.disk->read(blocknum, block.data);
                // std::cout << "carregou bloco " << blocknum << " no disco." << std::endl;
            }
            else {
                // inode não tem bloco indireto
                if (!inode.indirect) return;

                fs_block indirect;
                fs.disk->read(inode.indirect, indirect.data);

                int blocknum = indirect.pointers[block_i - INE5412_FS::POINTERS_PER_INODE];
                // ponteiro de bloco é nulo (bloco inexistente)
                if (!blocknum) return;

                fs.disk->read(blocknum, block.data);
                // std::cout << "carregou bloco " << blocknum << " no disco." << std::endl;
            }

            block_index = block_i;
        }

        void set_pos(long pos) {
            if (pos < 0) pos = 0;
            if (pos > size()) pos = size();

            this->pos = pos;
            // std::cout << "mudou pos de file stream para " << pos << std::endl;
            if (!eof()) load();
        }

        inline void incr_pos(int amount = 1) {
            set_pos(pos + amount);
        }

    public:
        fs_file(INE5412_FS& fs, int inumber) : fs(fs), inumber(inumber) {
            // FIXME seria interessante ter um método open separado
            // para carregar inode e bloco de dados para permitir
            // a abertura tardia
            inode = fs.read_inode(inumber);
            set_pos(0);
        }

        ~fs_file() {
            // std::cout << "fechando arquivo de inumber " << inumber << std::endl;
            if (block_dirty) {
                save();
            }
            if (inode_dirty) {
                // std::cout << "salvou inode " << inumber << std::endl;
                fs.write_inode(inumber, inode);
            }
        }

        int isvalid() { return inode.isvalid; }
        long size() { return inode.size; }

        long allocated_size() {
            long blocks_used = ceil(static_cast<long double>(size()) / Disk::DISK_BLOCK_SIZE);
            return blocks_used * Disk::DISK_BLOCK_SIZE;
        }

        bool eof() {
            return pos >= size();
        }

        bool extend(long bytes) {
            if (bytes <= 0) return false;

            // marca o inode como sujo para ser salvado 
            // em disco ao fechar o arquivo
            inode_dirty = true;

            // aloca novos blocos de dados até ter tamanho o suficiente
            // para extender tamanho do arquivo
            if (size() + bytes > allocated_size()) {
                int blocknum = fs.allocate_block();
                if (blocknum < 0) return false;
                
                int next_block_i = allocated_size() / Disk::DISK_BLOCK_SIZE;
                // std::cout << "alocou bloco para dados " << blocknum << " no indice " << next_block_i << std::endl;

                if (next_block_i < INE5412_FS::POINTERS_PER_INODE) {
                    inode.direct[next_block_i] = blocknum;
                }
                else {
                    fs_block indirect;

                    // carrega vetor de ponteiros se presente
                    if (inode.indirect) {
                        fs.disk->read(inode.indirect, indirect.data);
                    }
                    // caso ainda não tenha um vetor de ponteiros,
                    // aloca um novo e inicializa todo nulo
                    else {
                        int indirect_ptr = fs.allocate_block();
                        // falhou em alocar um novo bloco
                        if (indirect_ptr < 0) return false;

                        inode.indirect = indirect_ptr;
                        fs.disk->read(inode.indirect, indirect.data);
                        // std::cout << "alocou bloco de ponteiros " << indirect_ptr << std::endl;

                        for (int j = 0; j < POINTERS_PER_INODE; j++) {
                            indirect.pointers[j] = 0;
                        }
                    }


                    int ptr_i = next_block_i - INE5412_FS::POINTERS_PER_INODE;
                    indirect.pointers[ptr_i] = blocknum;

                    fs.disk->write(inode.indirect, indirect.data);
                    // std::cout << "salvou bloco de ponteiros " << inode.indirect << std::endl;
                }
            }

            // recarrega bloco atual caso o antigo fim do
            // arquivo esteja em um novo bloco
            if (eof()) load();

            // atualiza fim/tamanho do arquivo
            inode.size += bytes;
            // std::cout << "tamanho de arquivo aumentou para " << inode.size << "(tamanho alocado " << allocated_size() << ")" << std::endl;

            return true;
        }

        void seek_set(long offset) {
            set_pos(offset);
        }
        void seek_cur(long offset) {
            set_pos(pos + offset);
        }

        /**
         * Posiciona o indicador de posição a partir
         * do fim do arquivo.
         * ```
         * seek_end() // posição vazia após o último byte
         * seek_end(1) // último byte do arquivo
         * seek_end(2) // penúltimo byte do arquivo
         * ```
        */
        void seek_end(long offset = 0) {
            set_pos(size() - offset);
        }

        char get_char() {
            if (block_index < 0) return 0;

            if (eof()) return 0;

            char ch = block.data[pos % Disk::DISK_BLOCK_SIZE];
            incr_pos();
            
            return ch;
        }
        char put_char(char ch) {
            if (eof()) {
                bool extended = extend(1);
                if (!extended) // std::cout << "falhou em extender tamanho do arquivo" << std::endl; 
                if (!extended) return 0;
            }

            block.data[pos % Disk::DISK_BLOCK_SIZE] = ch;
            // std::cout << "escreveu '" << ch << "' na posição " << pos << std::endl; 
            // marca o bloco atual como sujo para ser salvo
            block_dirty = true;

            incr_pos();

            return ch;
        }

        int get_string(char* data, int size) {
            if (block_index < 0) return 0;

            int count = 0;
            for (int i = 0; i < size; i++) {
                char ch = get_char();
                if (!ch) break;

                data[i] = ch;
                count++;
            }

            return count;
        }
        int put_string(const char* str, int size) {
            int count = 0;
            for (int i = 0; i < size; i++) {
                if (!put_char(str[i])) break;
                count++;
            }

            return count;
        }

        // template<typename T>
        // int read(T* ptr, int member_amount) {
        //     unsigned long member_size = sizeof(T); 

        //     if (block_index < 0) return 0;
        // }
        // template<typename T>
        // int write(T* str, int member_amount) {
        //     unsigned long member_size = sizeof(T); 

        //     if (block_index < 0) return 0;
        // }
    };


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