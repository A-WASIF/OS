#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
int (*_address)();
int totalPages = 0;
int pageFault = 0;
size_t entry_address;
size_t totalMeomorySize = 0;
size_t entry_offset;
size_t internalFragmentation = 0;
void *virtual_mem;
void *updatedAddress;
int phdrTable[1000][2]; // This Table stores Memory Size of every segments in Phdr table as it's 1st index, stores 0 at 0th index (1 else) if that segment has been accessed while we are looping through it in segmentation fault handler so that we make sure we don't lseek its pointer to begining of that segment

// Function declarations

/*
 * release memory and other cleanups
 */
void loader_cleanup();

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char **argv);

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

int hex_to_decimal(const char *hex_string);

// this is main function
int main(int argc, char **argv)
{
    // Check for correct usage
    if (argc != 2)
    {
        printf("Usage: %s <ELF Executable> \n", argv[0]);
        exit(1);
    }

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = segfault_handler;

    // Register the signal handler for SIGSEGV
    if (sigaction(SIGSEGV, &sa, NULL) == -1)
    {
        perror("sigaction");
        return 1;
    }

    printf("\n");
    // Load and run ELF file
    load_and_run_elf(argv);
    // Clean up and print statistics
    loader_cleanup();

    // Unmap virtual memory
    if (munmap(virtual_mem, totalMeomorySize) == -1)
    {
        perror("Error unmapping memory");
        loader_cleanup();
        exit(1);
    }
    // Print statistics

    printf("Total Number Of Page Faults : %d\n", pageFault);
    printf("Total Number Of Page Allocations : %d\n", totalPages);
    printf("Internal Fragmentation : %d KB\n", internalFragmentation);
    printf("END!\n");

    return 0;
}

// Function to calculate ceil value of i divided by k
int Ceil(int i, int k)
{
    if (i % k == 0)
        return i / k;
    return i / k + 1;
}

// Function to convert hexadecimal string to decimal
int hex_to_decimal(const char *hex_string)
{
    char *endptr;
    long decimal_value = strtol(hex_string, &endptr, 16);

    if (*endptr != '\0')
    {
        fprintf(stderr, "Invalid hexadecimal input: %s\n", hex_string);
        return -1;
    }

    return (int)decimal_value;
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char **argv)
{
    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Failed to open file");
        loader_cleanup();
        exit(1);
    }

    // Load entire binary content into the memory from the ELF file.
    ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
    if (read(fd, ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr))
    {
        perror("Error reading ELF header");
        loader_cleanup();
        exit(1);
    }

    // Iterate through the PHDR table and find the section of PT_LOAD
    // type that contains the address of the entrypoint method in fib.c
    phdr = (Elf32_Phdr *)malloc(ehdr->e_phentsize * ehdr->e_phnum);
    lseek(fd, ehdr->e_phoff, SEEK_SET);

    if (read(fd, phdr, ehdr->e_phentsize * ehdr->e_phnum) != (ssize_t)(ehdr->e_phentsize * ehdr->e_phnum))
    {
        perror("Error reading program header table");
        loader_cleanup();
        exit(1);
    }

    // printf("starting address : %p\n", ehdr -> e_entry);

    loop_through_phdr(ehdr, phdr);
}
// Function to loop through program headers
void loop_through_phdr(Elf32_Ehdr *ehdr, Elf32_Phdr *phdr)
{

    for (int i = 0; i < ehdr->e_phnum; i++)
    {
        phdrTable[i][0] = 0;
        phdrTable[i][1] = phdr[i].p_memsz;
    }

    // Pointer arithmetic
    entry_offset = ehdr->e_entry;

    // TypeCasting of address
    entry_address = (entry_offset);

    _address = (int (*)())entry_address;

    run_program(_address);
}
// Signal handler for segmentation fault
void segfault_handler(int signal, siginfo_t *info, void *context)
{
    ucontext_t *uc = (ucontext_t *)context;
    void *fault_address = (void *)info->si_addr;
    // Check which segment caused the segmentation fault
    for (int i = 0; i < ehdr->e_phnum; i++)
    {
        if ((int)fault_address <= phdr[i].p_vaddr + phdr[i].p_memsz && (int)fault_address >= phdr[i].p_vaddr)
        {

            int mem_size = phdr[i].p_memsz;
            int pageCounter = Ceil(mem_size, 4096);
            size_t allocationSize = 4096;

            if (phdrTable[i][0] == 0)
            {
                updatedAddress = (void *)phdr[i].p_vaddr;
                lseek(fd, phdr[i].p_offset, SEEK_SET);
                phdrTable[i][0] = 1;
            }
            // Map virtual memory
            virtual_mem = mmap(updatedAddress, allocationSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
            if (virtual_mem == MAP_FAILED) {
                perror("mmap failed");
                // Handle the error, possibly by exiting the program
                return;
            }

            int sizeUsing = phdrTable[i][1] > 4096 ? 4096 : phdrTable[i][1];
            updatedAddress += (sizeUsing + 1);
              // Read data into memory
            if (phdrTable[i][1] - 4096 < 0 && phdrTable[i][1] > 0)
            {
                read(fd, virtual_mem, phdrTable[i][1]);
                internalFragmentation += (allocationSize - phdrTable[i][1]);
                phdrTable[i][1] = 0;
            }
            else if (phdrTable[i][1] - 4096 > 0)
            {
                phdrTable[i][1] -= 4096;
                read(fd, virtual_mem, 4096);
            }

            ++totalPages;
            ++pageFault;
            totalMeomorySize += allocationSize;
        }
    }
}
// run the program 
void run_program(int (*_start)())
{
    int result = _start();
    printf("User _start return value = %d\n", result);
}
//loader cleanup
void loader_cleanup()
{
    if (ehdr)
        free(ehdr);
    if (phdr)
        free(phdr);
    if (fd != -1)
        close(fd);
}
