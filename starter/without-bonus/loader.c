#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup();

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** argv);

/*
Finding ST_LOAD in phdr
*/
void loop_through_phdr(Elf32_Ehdr *ehdr, Elf32_Phdr *phdr);

/*
Calling _start function in fib.c
*/
void run_program(int (*_start)());



int main(int argc, char** argv){
  if (argc != 2) {
    printf("Usage: %s <ELF Executable> \n", argv[0]);
    exit(1);
  }
  load_and_run_elf(argv);
  loader_cleanup();
  return 0;
}


/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** argv){
  fd = open(argv[1], O_RDONLY);
  if (fd == -1){
    perror("Failed to open file");
    loader_cleanup();
    exit(1);
  }

  // Load entire binary content into the memory from the ELF file.

  ehdr = (Elf32_Ehdr *) malloc(sizeof(Elf32_Ehdr));
  if(read(fd, ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)){
    perror("Error reading ELF header");
    loader_cleanup();
    exit(1);
  }

  // Iterate through the PHDR table and find the section of PT_LOAD 
  // type that contains the address of the entrypoint method in fib.c

  phdr = (Elf32_Phdr *) malloc(ehdr -> e_phentsize * ehdr -> e_phnum);
  lseek(fd, ehdr -> e_phoff, SEEK_SET);
  if (read(fd, phdr, ehdr -> e_phentsize * ehdr -> e_phnum) != (ssize_t)(ehdr -> e_phentsize * ehdr -> e_phnum)) {
    perror("Error reading program header table");
    loader_cleanup();
    exit(1);
  }

  loop_through_phdr(ehdr, phdr);

}

void loop_through_phdr(Elf32_Ehdr *ehdr, Elf32_Phdr *phdr){
  for (int i = 0; i < ehdr -> e_phnum; ++i){
    if (phdr[i].p_type == PT_LOAD && ehdr -> e_entry >= phdr[i].p_vaddr && ehdr -> e_entry < phdr[i].p_vaddr + phdr[i].p_memsz){

      void* virtual_mem = mmap(NULL, phdr[i].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

      if (virtual_mem == MAP_FAILED) {
        perror("Error allocating memory using mmap");
        loader_cleanup();
        exit(1);
      }

      // Copy segment content to allocated memory
      
      lseek(fd, phdr[i].p_offset, SEEK_SET);

      if (read(fd, virtual_mem, phdr[i].p_memsz) != phdr[i].p_memsz) {
        perror("Error reading segment content");
        loader_cleanup();
        exit(1);
      }

      // Pointer arithmetic
      
      size_t entry_offset = ehdr -> e_entry - phdr[i].p_vaddr;

      //TypeCasting of address

      void *entry_address = (void *)((int) virtual_mem + entry_offset);
      int (*_start)() = (int (*)()) entry_address;

      run_program(_start);

      break;
    }
  }
}

void run_program(int (*_start)()){
  int result = _start();
  printf("User _start return value = %d\n", result);
}

void loader_cleanup(){
  if (ehdr)
    free(ehdr);
  if (phdr)
    free(phdr);
  if (fd != -1)
    close(fd);
}
