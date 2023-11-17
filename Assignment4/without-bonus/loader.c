#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
size_t entry_address;
void *main_pointer;
void *virtual_mem;
int (*_address)();
int totalPages = 0;
int pageFault = 0;
size_t totalMeomorySize = 0;
size_t entry_offset;
size_t internalFragmentation = 0;
Elf64_Xword totalMemorySize = 0;

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

void segfault_handler(int signal, siginfo_t *info, void *context);

int Ceil(int i, int k);

int hex_to_decimal(const char *hex_string) {
    // Use the strtol function to convert the hexadecimal string to a decimal integer
    char *endptr;
    long decimal_value = strtol(hex_string, &endptr, 16);

    // Check for conversion errors
    if (*endptr != '\0') {
        fprintf(stderr, "Invalid hexadecimal input: %s\n", hex_string);
        return -1; // Handle the error as needed
    }

    return (int)decimal_value;
}


// this is main function
int main(int argc, char** argv){
  if (argc != 2) {
    printf("Usage: %s <ELF Executable> \n", argv[0]);
    exit(1);
  }

  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO; // Use SA_SIGINFO to receive additional signal information
  sa.sa_sigaction = segfault_handler;
  
  // Register the signal handler for SIGSEGV
  if (sigaction(SIGSEGV, &sa, NULL) == -1) {
      perror("sigaction");
      return 1;
  }

  load_and_run_elf(argv);
  loader_cleanup();
  if (munmap(virtual_mem, totalMeomorySize) == -1){
  perror("Error unmapping memory");
  loader_cleanup();
  exit(1);
  }
  
  printf("Total Number Of Page Faults : %d\n", pageFault);
  printf("Total Number Of Page Allocations : %d\n", totalPages);
  printf("Total Memory Allocated : %zu\n", totalMeomorySize);
  printf("Internal Fragmentation : %d Bytes\n", internalFragmentation);
  printf("END!\n");
  
  return 0;
}

int Ceil(int i, int k){
    if(i % k == 0) return i / k;
    return i / k + 1;
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

  // printf("starting address : %p\n", ehdr -> e_entry);

  loop_through_phdr(ehdr, phdr);
}

void loop_through_phdr(Elf32_Ehdr *ehdr, Elf32_Phdr *phdr){

  // Pointer arithmetic
  entry_offset = ehdr -> e_entry;

  //TypeCasting of address
  entry_address = (entry_offset);

  _address = (int (*)()) entry_address;

  run_program(_address);
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

void segfault_handler(int signal, siginfo_t *info, void *context) {
  ucontext_t *uc = (ucontext_t *)context;
  void *fault_address = (void *)info->si_addr;

  // Print the faulting address
  // printf("Segmentation fault at address: %p\n", fault_address);

  for (int i = 0; i < ehdr->e_phnum; i++){
    if((int)fault_address <= phdr[i].p_vaddr + phdr[i].p_memsz && (int)fault_address >= phdr[i].p_vaddr){

      int mem_size = phdr[i].p_memsz;
      int pageCounter = Ceil(mem_size, 4096);
      size_t allocationSize = pageCounter * 4096;

      virtual_mem = mmap((void*)phdr[i].p_vaddr, allocationSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
      lseek(fd, phdr[i].p_offset, SEEK_SET);
      read(fd, virtual_mem, phdr[i].p_memsz);

      // printf("---------------------------------%d %d\n", allocationSize, phdr[i].p_memsz);

      internalFragmentation += (allocationSize - ((int)phdr[i].p_memsz));
      // totalMemorySize += phdr[i].p_vaddr / 1024;
      totalPages += pageCounter;
      ++pageFault;
      totalMeomorySize += allocationSize;
    }
  }
}
