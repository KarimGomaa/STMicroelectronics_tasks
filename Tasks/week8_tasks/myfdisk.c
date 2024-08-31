#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

/* Define sector size for disk read operations */
#define SECTOR_SIZE 512

/* GPT Partition Types (common types) */
#define GPT_PARTITION_TYPE_UNUSED "00000000-0000-0000-0000-000000000000"
#define GPT_PARTITION_TYPE_EFI_SYSTEM "C12A7328-F81F-11D2-BA4B-00A0C93EC93B"
#define GPT_PARTITION_TYPE_BIOS_BOOT "21686148-6449-6E6F-744E-656564454649"
#define GPT_PARTITION_TYPE_MICROSOFT_RESERVED "E3C9E316-0B5C-4DB8-817D-F92DF00215AE"
#define GPT_PARTITION_TYPE_MICROSOFT_BASIC_DATA "EBD0A0A2-B9E5-4433-87C0-68B6B72699C7"
#define GPT_PARTITION_TYPE_LINUX_FILESYSTEM "0FC63DAF-8483-4772-8E79-3D69D8477DE4"
#define GPT_PARTITION_TYPE_LINUX_SWAP "0657FD6D-A4AB-43C4-84E5-0933C84B4F4F"
#define GPT_PARTITION_TYPE_LINUX_LVM "E6D6D379-F507-44C2-A23C-238F2A3DF928"

/* MBR Partition Types (common types) */
#define LINUX_TYPE 0x83
#define EXTENDED_TYPE 0x05
#define GPT_TYPE 0xEE

/* Unit size definitions */
#define GIGA_SIZE (1024 * 1024 * 1024)
#define MEGA_SIZE (1024 * 1024)
#define KILO_SIZE 1024

/* Array holding unit symbols and corresponding size values for conversions */
static const char Units_Symbol[] = {'G', 'M', 'K'};
static const uint64_t Units_Value[] = {GIGA_SIZE, MEGA_SIZE, KILO_SIZE};

/* Partition type names for MBR and GPT */
static const char *MBR_TypeNames[] = {"Linux", "Extended", "Unknown"};
static const char *GPT_TypeNames[] = {
    "UNUSED", "EFI_SYSTEM", "BIOS_BOOT", "MICROSOFT_RESERVED", "MICROSOFT_BASIC_DATA",
    "LINUX_FILESYSTEM", "LINUX_SWAP", "LINUX_LVM", "UNKNOWN"
};

/* GPT Partition Entry structure */
typedef struct {
    uint8_t partition_type_guid[16];   // Partition type GUID
    uint8_t unique_partition_guid[16]; // Unique partition GUID
    uint64_t first_lba;                // First LBA of the partition
    uint64_t last_lba;                 // Last LBA of the partition
    uint64_t attributes;               // Partition attributes
    uint16_t partition_name[36];       // Partition name (Unicode UTF-16)
} GPTPartitionEntry;

/* MBR Partition Entry structure */
typedef struct {
    uint8_t status;
    uint8_t first_chs[3];
    uint8_t partition_type;
    uint8_t last_chs[3];
    uint32_t lba;
    uint32_t sector_count;
} MBRPartitionEntry;

/* Global variable for extended partition start sector */
uint32_t ExtendedStartSector = 0;

/* Gets the MBR partition type index */
uint8_t getMBRPartitionType(uint8_t partitionType) {
    switch (partitionType) {
        case LINUX_TYPE:    return 0;
        case EXTENDED_TYPE: return 1;
        default:            return 2;
    }
}

/* Determines the suitable unit character for the size */
uint8_t getSuitableSizeUnit(uint32_t sectorCount) {
    uint64_t byteCount = (uint64_t)sectorCount * SECTOR_SIZE;

    if (byteCount >= GIGA_SIZE) return 0;
    if (byteCount >= MEGA_SIZE) return 1;
    if (byteCount >= KILO_SIZE) return 2;
    return 2;
}

/* Prints primary partition information */
void printPrimaryPartitionInfo(uint8_t index, MBRPartitionEntry *entry, char *diskName) {
    if (entry->lba != 0) {
        uint8_t unitIndex = getSuitableSizeUnit(entry->sector_count);
        uint8_t typeIndex = getMBRPartitionType(entry->partition_type);

        printf("%s%-3d %-9c %-10u %-10u %-10u %u%c %-8X %s\n",
               diskName, index + 1, entry->status == 0x80 ? '*' : ' ',
               entry->lba, entry->lba + entry->sector_count - 1, entry->sector_count,
               (uint32_t)(((uint64_t)entry->sector_count * SECTOR_SIZE) / Units_Value[unitIndex]),
               Units_Symbol[unitIndex], entry->partition_type, MBR_TypeNames[typeIndex]);
    }
}

/* Prints extended partition information */
void printExtendedPartitionInfo(uint8_t index, MBRPartitionEntry *entry, char *diskName) {
    if (entry->lba != 0) {
        uint8_t unitIndex = getSuitableSizeUnit(entry->sector_count);
        uint8_t typeIndex = getMBRPartitionType(entry->partition_type);

        printf("%s%-3d %-9c %-10u %-10u %-10u %u%c %-8X %s\n",
               diskName, index, entry->status == 0x80 ? '*' : ' ',
               entry->lba + ExtendedStartSector, entry->lba + ExtendedStartSector + entry->sector_count - 1,
               entry->sector_count,
               (uint32_t)(((uint64_t)entry->sector_count * SECTOR_SIZE) / Units_Value[unitIndex]),
               Units_Symbol[unitIndex], entry->partition_type, MBR_TypeNames[typeIndex]);

        // Update extended partition start sector
        ExtendedStartSector = entry[1].lba == 0 ? 0 : ExtendedStartSector + entry[1].lba;
    }
}

/* Determines the GPT partition type index */
uint8_t getGPTPartitionTypeIndex(uint8_t *partition_type_guid) {
    if (memcmp(partition_type_guid, GPT_PARTITION_TYPE_UNUSED, 16) == 0) return 0;
    if (memcmp(partition_type_guid, GPT_PARTITION_TYPE_EFI_SYSTEM, 16) == 0) return 1;
    if (memcmp(partition_type_guid, GPT_PARTITION_TYPE_BIOS_BOOT, 16) == 0) return 2;
    if (memcmp(partition_type_guid, GPT_PARTITION_TYPE_MICROSOFT_RESERVED, 16) == 0) return 3;
    if (memcmp(partition_type_guid, GPT_PARTITION_TYPE_MICROSOFT_BASIC_DATA, 16) == 0) return 4;
    if (memcmp(partition_type_guid, GPT_PARTITION_TYPE_LINUX_FILESYSTEM, 16) == 0) return 5;
    if (memcmp(partition_type_guid, GPT_PARTITION_TYPE_LINUX_SWAP, 16) == 0) return 6;
    if (memcmp(partition_type_guid, GPT_PARTITION_TYPE_LINUX_LVM, 16) == 0) return 7;
    return 8;
}

/* Prints GPT partition information */
void printGPTPartitionInfo(uint8_t index, GPTPartitionEntry *entry, char *diskName) {
    uint8_t typeIndex = getGPTPartitionTypeIndex(entry->partition_type_guid);
    uint64_t sectors = entry->last_lba - entry->first_lba + 1;
    uint8_t unitIndex = getSuitableSizeUnit((uint32_t)sectors);

    printf("%s%-3d %-10" PRIu64 " %-10" PRIu64 " %-10" PRIu64 " %" PRIu64 "%c %-8s\n",
           diskName, index + 1, entry->first_lba, entry->last_lba, sectors,
           (uint64_t)(sectors * SECTOR_SIZE) / Units_Value[unitIndex], Units_Symbol[unitIndex],
           GPT_TypeNames[typeIndex]);
}

int main(int argc, char **argv) {
    char buffer[SECTOR_SIZE];
    
    if (argc < 2) {
        printf("Usage: %s <disk_device>\n", argv[0]);
        return 1;
    }

    // Open the specified disk device
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening disk device");
        return 1;
    }

    // Read the MBR sector
    if (read(fd, buffer, SECTOR_SIZE) == -1) {
        perror("Error reading MBR");
        close(fd);
        return 1;
    }

    // Pointer to the partition table in the MBR
    MBRPartitionEntry *partition_table = (MBRPartitionEntry *)(buffer + 446);

    // Check for GPT partition type
    if (partition_table->partition_type == GPT_TYPE) {
        printf("Disklabel type: GPT\n");
        printf("%-10s %-10s %-10s %-10s %-10s %-8s\n", "Device", "Start", "End", "Sectors", "Size", "Type");

        if (lseek(fd, SECTOR_SIZE * 2, SEEK_SET) != -1) {
            GPTPartitionEntry gpt_entry;
            uint8_t index = 0;

            while (read(fd, &gpt_entry, sizeof(GPTPartitionEntry)) > 0 && gpt_entry.first_lba != 0) {
                printGPTPartitionInfo(index++, &gpt_entry, argv[1]);
            }
        } else {
            perror("Error seeking to GPT partition entries");
            close(fd);
            return 1;
        }
    } else {
        printf("Disklabel type: MBR\n");
        printf("%-10s %-10s %-10s %-10s %-10s %-10s %-8s\n", "Device", "Boot", "Start", "End", "Sectors", "Size", "Type");

        // Print primary partition information
        for (int i = 0; i < 4; i++) {
            printPrimaryPartitionInfo(i, &partition_table[i], argv[1]);

            if (partition_table[i].partition_type == EXTENDED_TYPE) {
                ExtendedStartSector = partition_table[i].lba;
            }
        }

        // Print extended partition information
        uint8_t extendedIndex = 5;
        while (ExtendedStartSector != 0) {
            if (lseek(fd, ExtendedStartSector * SECTOR_SIZE, SEEK_SET) != -1 && read(fd, buffer, SECTOR_SIZE) != -1) {
                partition_table = (MBRPartitionEntry *)(buffer + 446);
                printExtendedPartitionInfo(extendedIndex++, partition_table, argv[1]);
            } else {
                perror("Error reading extended partition table");
                close(fd);
                return 1;
            }
        }
    }

    close(fd);
    return 0;
}