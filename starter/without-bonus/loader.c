#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
	close(fd);
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** exe) {
  	fd = open(argv[1], O_RDONLY);
  // 1. Load entire binary content into the memory from the ELF file.

	ehdr = (Elf32_Ehdr *) malloc (sizeof(Elf32_Ehdr));

	if(ehdr == NULL){
		perror("Error: ");
		return;
	}
	
	ssize_t count = read(fd, ehdr, sizeof(Elf32_Ehdr));
  
	if(count == -1){
		perror("Error: ");
		return;
	}

	// 2. Iterate through the PHDR table and find the section of PT_LOAD 
	//    type that contains the address of the entrypoint method in fib.c

	int Phdr_size = ehdr -> e_phnum; 
	for(int i = 0; i < Phdr_size; i++){
		phdr = (Elf32_Phdr *) malloc (sizeof(Elf32_Phdr));

		if(phdr == NULL){
			perror("Error: ");
			return;
		}

		int seek_through = ehdr -> e_phoff + (i * sizeof(Elf32_Phdr));
		int val = lseek(fd, seek_through, SEEK_SET);
		
		if(val == -1){
			perror("Error: ");
			return;
		}

		ssize_t check = read(fd, phdr, sizeof(Elf32_Phdr));


		// 3. Allocate memory of the size "p_memsz" using mmap function 
		//    and then copy the segment content

		if(phdr -> p_type == PT_LOAD){
			void *f_content = mmap(NULL, phdr -> p_memsz, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);			//////

			lseek(fd, phdr -> p_offset, SEEK_SET);

			read(fd, f_content, phdr -> pfilesz);
		}

		// 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
		// 5. Typecast the address to that of function pointer matching "_start" method in fib.c.

		void (*entrypoint)() = (void (*)())(segment_addr + entry_offset);
		entrypoint();

		break;

	}



  // 6. Call the "_start" method and print the value returned from the "_start"
  int result = _start();
  printf("User _start return value = %d\n",result);
}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv[1]);
  // 3. invoke the cleanup routine inside the loader  
  loader_cleanup();
  return 0;
}
