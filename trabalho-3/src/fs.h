#ifndef FS_H
#define FS_H

#include <cmath>
#include <iostream>
#include <exception>
#include "disk.h"

class fs_eof : std::exception {
    std::string message;

 public:
    fs_eof() : message("reached file eof.") {}

    virtual const char* what() const throw() {
        return message.c_str();
    }
};

class fs_disk_full : std::exception {
    std::string message;

 public:
    fs_disk_full() : message("disk reached max size.") {}

    virtual const char* what() const throw() {
        return message.c_str();
    }
};

class fs_max_file_size : std::exception {
    std::string message;

 public:
    fs_max_file_size() : message("file reached max size.") {}

    virtual const char* what() const throw() {
        return message.c_str();
    }
};

class Bitmap {
	char* bits;
    int size;

public:

	Bitmap(int size) {
        this->size = size;
        int arr_size = ceil(static_cast<double>(size) / 8.0);

		bits = new char[arr_size];

        for (int i = 0; i < arr_size; i++) {
		    bits[i] = 0;
        }
	}

	~Bitmap() {
		delete[] bits;
	}

    int get_size() const {
        return size;
    }

    bool read(int blocknum) const {
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

    int search(bool bit) const {
        for (int i = 0; i < size; i++) {
            if (read(i) == bit) return i;
        }

        return -1;
    }

    friend ostream& operator<<(ostream& os, const Bitmap& dt);
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

    public:
        fs_file(INE5412_FS& fs, int inumber) : fs(fs), inumber(inumber) {
            // FIXME seria interessante ter um método open separado
            // para carregar inode e bloco de dados para permitir
            // a abertura tardia
            inode = fs.read_inode(inumber);
            seek_set(0);
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
                    if (next_block_i >= INE5412_FS::POINTERS_PER_BLOCK) throw fs_max_file_size();

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

                        // std::cout << "alocou bloco de ponteiros " << indirect_ptr << std::endl;

                        for (int j = 0; j < POINTERS_PER_BLOCK; j++) {
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

        int shrink(long bytes) {
            if (bytes < 0) bytes = 0;
            if (bytes > size()) bytes = size();

            long prev_block_amount = allocated_size() / Disk::DISK_BLOCK_SIZE;

            inode.size -= bytes;
            inode_dirty = true;

            long block_amount = allocated_size() / Disk::DISK_BLOCK_SIZE;

            fs_block indirect;
            bool indirect_dirty = false;
            if (inode.indirect) {
                fs.disk->read(inode.indirect, indirect.data);
            }

            for (int i = block_amount; i < prev_block_amount; i++) {
                if (i < INE5412_FS::POINTERS_PER_INODE) {
                    int blocknum = inode.direct[i];
                    fs.deallocate_block(blocknum);
                    inode.direct[i] = 0;
                }
                else {
                    int j = i - INE5412_FS::POINTERS_PER_INODE;

                    int blocknum = indirect.pointers[j];
                    fs.deallocate_block(blocknum);
                    indirect.pointers[j] = 0;
                    indirect_dirty = true;
                }
            }

            if (inode.indirect && indirect_dirty) {
                if (!indirect.pointers[0]) {
                    fs.deallocate_block(inode.indirect);
                    inode.indirect = 0;
                }
                else {
                    fs.disk->write(inode.indirect, indirect.data);
                }
            }

            return bytes;
        }

        void seek_set(long pos) {
            if (pos < 0) pos = 0;
            if (pos > size()) pos = size();

            this->pos = pos;
            // std::cout << "mudou pos de file stream para " << pos << std::endl;
            if (!eof()) load();
        }
        void seek_cur(long offset) {
            seek_set(pos + offset);
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
            seek_set(size() - offset);
        }

        char get_char() {
            if (eof()) throw fs_eof();

            char ch = block.data[pos % Disk::DISK_BLOCK_SIZE];
            seek_cur(1);
            
            return ch;
        }

        char put_char(char ch) {
            if (eof()) {
                bool extended = extend(1);
                // if (!extended) std::cout << "falhou em extender tamanho do arquivo" << std::endl; 
                if (!extended) throw fs_disk_full();
            }

            block.data[pos % Disk::DISK_BLOCK_SIZE] = ch;
            // std::cout << "escreveu '" << ch << "' na posição " << pos << std::endl; 
            // marca o bloco atual como sujo para ser salvo
            block_dirty = true;

            seek_cur(1);

            return ch;
        }

        int get_string(char* data, int size) {
            int count = 0;

            for (int i = 0; i < size; i++) {
                try {
                    data[i] = get_char();
                    count++;
                }
                catch (fs_eof& err) {
                    break;
                }
            }

            return count;
        }
        int put_string(const char* str, int size) {
            int count = 0;
            for (int i = 0; i < size; i++) {
                try {
                    put_char(str[i]);
                    count++;
                }
                catch (fs_disk_full& err) {
                    break; 
                }
                catch (fs_max_file_size& err) {
                    break; 
                }
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
    int  fs_shrink(int inumber, int amount);

private:
    Disk* disk = NULL;
    Bitmap* bitmap = NULL;
    fs_block superblock;
};

#endif