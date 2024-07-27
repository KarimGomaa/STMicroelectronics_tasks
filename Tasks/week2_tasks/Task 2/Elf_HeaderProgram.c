#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

/* Function to print ELF class */
const char* get_class(unsigned char ei_class) {
    switch (ei_class) {
        case ELFCLASSNONE: return "Invalid Class";
        case ELFCLASS32: return "32-bit objects";
        case ELFCLASS64: return "64-bit objects";
        default: return "Unknown Class";
    }
}

/* Function to print ELF data encoding */
const char* get_data(unsigned char ei_data) {
    switch (ei_data) {
        case ELFDATANONE: return "Invalid data encoding";
        case ELFDATA2LSB: return "Little-endian";
        case ELFDATA2MSB: return "Big-endian";
        default: return "Unknown data encoding";
    }
}

/* Function to print ELF OS ABI */
const char* get_osabi(unsigned char ei_osabi) {
    switch (ei_osabi) {
        case ELFOSABI_NONE: return "UNIX System V ABI";
        case ELFOSABI_HPUX: return "HP-UX ABI";
        case ELFOSABI_NETBSD: return "NetBSD ABI";
        case ELFOSABI_LINUX: return "Linux ABI";
        case ELFOSABI_SOLARIS: return "Solaris ABI";
        case ELFOSABI_IRIX: return "IRIX ABI";
        case ELFOSABI_FREEBSD: return "FreeBSD ABI";
        case ELFOSABI_TRU64: return "TRU64 UNIX ABI";
        case ELFOSABI_ARM: return "ARM architecture ABI";
        case ELFOSABI_STANDALONE: return "Stand-alone (embedded) ABI";
        default: return "Unknown OS ABI";
    }
}

/* Function to print ELF type */
const char* get_type(uint16_t e_type) {
    switch (e_type) {
        case ET_NONE: return "No file type";
        case ET_REL: return "Relocatable file";
        case ET_EXEC: return "Executable file";
        case ET_DYN: return "Shared object file";
        case ET_CORE: return "Core file";
        default: return "Unknown type";
    }
}

/* Function to print ELF machine */
const char* get_machine(uint16_t e_machine) {
    switch (e_machine) {
        case EM_NONE: return "No machine";
        case EM_M32: return "AT&T WE 32100";
        case EM_SPARC: return "SPARC";
        case EM_386: return "Intel 80386";
        case EM_68K: return "Motorola 68000";
        case EM_88K: return "Motorola 88000";
        case EM_860: return "Intel 80860";
        case EM_MIPS: return "MIPS RS3000";
        case EM_PARISC: return "HP/PA";
        case EM_SPARC32PLUS: return "SPARC with enhanced instruction set";
        case EM_PPC: return "PowerPC";
        case EM_PPC64: return "PowerPC 64-bit";
        case EM_S390: return "IBM S/390";
        case EM_ARM: return "ARM";
        case EM_SH: return "Renesas SuperH";
        case EM_SPARCV9: return "SPARC v9 64-bit";
        case EM_IA_64: return "Intel Itanium";
        case EM_X86_64: return "AMD x86-64";
        case EM_VAX: return "DEC Vax";
        default: return "Unknown machine";
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *elfFile = fopen(argv[1], "r");
    if (!elfFile) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    Elf64_Ehdr elfHeader;
    fread(&elfHeader, sizeof(elfHeader), 1, elfFile);

    printf("ELF Header:\n");
    printf("  Magic:   ");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%2.2x ", elfHeader.e_ident[i]);
    }
    printf("\n");
    printf("  Class:                             %s\n", get_class(elfHeader.e_ident[EI_CLASS]));
    printf("  Data:                              %s\n", get_data(elfHeader.e_ident[EI_DATA]));
    printf("  Version:                           %d\n", elfHeader.e_ident[EI_VERSION]);
    printf("  OS/ABI:                            %s\n", get_osabi(elfHeader.e_ident[EI_OSABI]));
    printf("  ABI Version:                       %d\n", elfHeader.e_ident[EI_ABIVERSION]);
    printf("  Type:                              %s\n", get_type(elfHeader.e_type));
    printf("  Machine:                           %s\n", get_machine(elfHeader.e_machine));
    printf("  Version:                           %d\n", elfHeader.e_version);
    printf("  Entry point address:               0x%lx\n", (unsigned long)elfHeader.e_entry);
    printf("  Start of program headers:          %ld (bytes into file)\n", (long)elfHeader.e_phoff);
    printf("  Start of section headers:          %ld (bytes into file)\n", (long)elfHeader.e_shoff);
    printf("  Flags:                             0x%x\n", elfHeader.e_flags);
    printf("  Size of this header:               %d (bytes)\n", elfHeader.e_ehsize);
    printf("  Size of program headers:           %d (bytes)\n", elfHeader.e_phentsize);
    printf("  Number of program headers:         %d\n", elfHeader.e_phnum);
    printf("  Size of section headers:           %d (bytes)\n", elfHeader.e_shentsize);
    printf("  Number of section headers:         %d\n", elfHeader.e_shnum);
    printf("  Section header string table index: %d\n", elfHeader.e_shstrndx);

    fclose(elfFile);

    return EXIT_SUCCESS;
}
