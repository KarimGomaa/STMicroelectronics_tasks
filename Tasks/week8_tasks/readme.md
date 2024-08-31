# 📊 Disk Partition Information Tool

This program is designed to read and display the partition information from a disk using either the MBR (Master Boot Record) or GPT (GUID Partition Table) partitioning scheme. It provides detailed information about each partition, including the start and end sectors, size, type, and other relevant details.

## 📋 Table of Contents

1. [✨ Features](#-features)
2. [🛠️ Requirements](#-requirements)
3. [🚀 Usage](#-usage)
4. [📝 Code Explanation](#-code-explanation)
   - [MBR Partition Handling](#mbr-partition-handling)
   - [GPT Partition Handling](#gpt-partition-handling)
   - [Helper Functions](#helper-functions)
   - [Error Handling](#error-handling)
5. [📊 Output](#-output)
6. [⚠️ Important Notes](#-important-notes)

## ✨ Features

- Reads the partition table from a specified disk device.
- Supports both MBR and GPT partition tables.
- Displays partition information such as device name, start sector, end sector, total sectors, size, type, and ID.
- Supports both primary and extended partitions for MBR.
- Provides detailed partition information for common GPT types.

## 🛠️ Requirements

- A Linux system with access to the required disk device.
- GCC compiler for building the program.

## 🚀 Usage

1. **Compile the Program:**

   ```bash
   gcc -o disk_partition_info disk_partition_info.c

1. **Run the Program:**

   ```bash
   sudo ./disk_partition_info <disk_device>
Replace <disk_device> with the path to the disk device (e.g., /dev/sda).   

## 📝 Code Explanation
- MBR Partition Handling: The program reads the MBR partition entries and displays information for each primary and extended partition. It handles up to four primary partitions and processes extended partitions recursively.

- GPT Partition Handling: For GPT, the program reads the partition entries and displays information for each entry found. It identifies common partition types like EFI System, Linux Filesystem, Microsoft Reserved, and more.

- Helper Functions:

    - getMBRPartitionType(): Returns the index of the MBR partition type for display.
    - getSuitableSizeUnit(): Determines the suitable size unit (G, M, K) based on the       number of sectors.
    - printPrimaryPartitionInfo(): Prints information for primary partitions.
    - printExtendedPartitionInfo(): Prints information for extended partitions.
    - getGPTPartitionTypeIndex(): Returns the index for GPT partition type GUIDs.
    - printGPTPartitionInfo(): Prints information for GPT partitions.

- Error Handling: The program checks for errors when opening the disk device, reading sectors, and seeking within the disk. If any error occurs, an appropriate error message is displayed.

## 📊 Output
The program outputs the following information depending on the partitioning scheme:

### For MBR Partitions:
- Device: The disk device name.
- Boot: Bootable flag (* if bootable).
- Start: The starting sector of the partition.
- End: The ending sector of the partition.
- Sectors: The total number of sectors in the partition.
- Size: The size of the partition.
- ID: The hexadecimal ID of the partition type.
- Type: The human-readable name of the partition type.

### For GPT Partitions:
- Device: The disk device name.
- Start: The starting LBA (Logical Block Address) of the partition.
- End: The ending LBA of the partition.
- Sectors: The total number of sectors in the partition.
- Size: The size of the partition.
- Type: The GUID partition type.

### 📃 Example Output
    ```bash
    Disklabel type: MBR
    Device     Boot    Start       End         Sectors     Size     ID    Type       
    /dev/sda1  *       2048        1050623     1048576     512M     83    Linux      
    /dev/sda2          1050624     2097151     1046528     511M     05    Extended   
    /dev/sda5          1050624     2097151     1046528     511M     83    Linux


## ⚠️ Important Notes
Running the program requires read access to the disk device, which typically requires root privileges.
Be cautious when specifying disk devices to avoid unintentional data loss.

##
Thank you for exploring My fdisk project. If you have any questions or feedback, feel free to reach out or contribute to the repository.

