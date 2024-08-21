# 💻 My Heap Memory Managment

Welcome to My Custom Heap Implementation ! This README.md provides an overview of the project, features, usage, and implementation details..

## 📚 Table of Contents

- [🌟 Project Overview](#-project-overview)
- [🚀 Installation & Usage](#-installation--usage)
- [🛠 How it Works](#-how-it-works)
  - [🔧 `HmmAlloc`](#-hmmalloc)
  - [⚙️ `HmmFree`](#-hmmfree)
- [🔚 Conclusion](#-conclusion)

## 🌟 Project Overview
This project involves developing a custom heap in C, and it consists of four primary files: `my_heap.c`, `my_heap.h`, `std_types.h`, and `stress_test`. Below is an explanation of each file and its key components.

#### `my_heap.c`

- **Purpose:** 
  - Contains the implementation of the custom heap memory management functions.

- **Key Components:**
  - **`HmmAlloc`:** Function responsible for allocating a block of memory of a specified size.
  - **`HmmFree`:** Function that frees previously allocated memory and merges adjacent free blocks.
  - **Memory Management Logic:** Implements the core logic for managing memory allocation, deallocation, and merging of free blocks.

- **Details:**
  - This file includes the definitions and implementations of all the heap management functions and is crucial for the functioning of the heap system.

#### `my_heap.h`

- **Purpose:**
  - Provides the header definitions and function prototypes for the heap management functions.

- **Key Components:**
  - **Function Prototypes:** Declarations of functions such as `HmmAlloc` and `HmmFree`, allowing other files to use these functions.
  - **Data Structures:** Definitions of any data structures used by the heap functions, such as metadata structures for tracking allocated and free memory.

- **Details:**
  - This file serves as an interface between the heap implementation (`my_heap.c`) and other parts of the project, ensuring proper function declarations and type definitions.

#### `std_types.h`

- **Purpose:**
  - Defines standard data types used throughout the project.

- **Key Components:**
  - **Type Definitions:** Includes definitions for fundamental types such as `uint64`, `uint8`, and other necessary data types.
  - **Macros and Constants:** Provides common constants and macros used in the heap implementation for consistency and readability.

- **Details:**
  - This file centralizes type definitions and constants, making it easier to manage and update them across the entire project.

#### `stress_test`

- **Purpose:**
  - Contains test cases and scripts for validating the functionality and performance of the heap memory management system.

- **Key Components:**
  - **Test Cases:** Various scenarios to stress test the heap implementation, including edge cases and large-scale allocations.
  - **Validation Scripts:** Scripts to automate the testing process and verify that the heap management functions behave correctly under different conditions.

- **Details:**
  - This file is essential for ensuring the reliability and robustness of the heap system by providing a means to test and validate its behavior.

Each file plays a crucial role in the overall implementation and testing of the custom heap memory management system. Together, they provide a comprehensive solution for dynamic memory allocation and management in C.


## 🚀 Installation & Usage

1. **Clone the Repository**
   ```bash
   git clone https://github.com/KarimGomaa/STMicroelectronics_tasks

1. **Compile the Heap using Makefile**
   ```bash
   make
1. **Run the Heap**
   ```bash   
   ./my_heap


## 🛠 How it Works ?

Heap memory is a region of a computer's memory that is used for dynamic memory allocation. Unlike stack memory, which is managed automatically, heap memory must be manually allocated and freed by the programmer. The heap allows for flexible and efficient management of memory at runtime, which is essential for applications that require dynamic memory usage.

### How Heap Memory Works

1. **Dynamic Allocation:** When a program needs memory at runtime, it requests a block of memory from the heap. This is typically done using functions like `malloc` in C or `new` in C++ or `HmmAlloc` in My Heap ^_^ .
   
2. **Allocation Strategies:** The heap uses various algorithms to manage memory allocation. Common strategies include:
   - **First Fit:** Allocates the first block of sufficient size.
   - **Best Fit:** Allocates the smallest block that fits the request.
   - **Worst Fit:** Allocates the largest available block.
   
We are going to deal with First fit that Allocates the first block of sufficient size.
   
3. **Memory Fragmentation:** Over time, as memory is allocated and freed, the heap can become fragmented, leading to inefficient use of memory and increased allocation times.

4. **Freeing Memory:** It is crucial to free allocated memory when it is no longer needed to avoid memory leaks and other issues.

## 🔧 `HmmAlloc`

- **Purpose:** This function is responsible for allocating a specified amount of memory.

- **Parameters:**
  - `size`: `uint64` - The size of the memory allocation in bytes.

- **Operation:**
  - This function allocates a block of memory of the given size and returns a pointer to the start of the allocated block. If the allocation fails, it returns a null pointer.

- **Return Value:**
  - Returns a pointer to the allocated memory block, or `NULL` if the allocation fails.
  
## 

![1](https://github.com/user-attachments/assets/79e0c2b4-11e5-42b8-a37b-a76a98f45c2b)
![2](https://github.com/user-attachments/assets/38248a67-d786-48d4-a4cf-59570d9295ea)
![3](https://github.com/user-attachments/assets/268b16ea-d442-48bc-bf69-206510824472)
![4](https://github.com/user-attachments/assets/ac9980ff-fecb-4612-ab33-38f17f74d702)
![5](https://github.com/user-attachments/assets/f8b59a2f-6ac9-4ccc-b74d-565afe580436)
![6](https://github.com/user-attachments/assets/d5c4d186-bd39-4a12-a9c9-89062f67a2b5)


## ⚙️ `HmmFree`

The `HmmFree` function is responsible for releasing previously allocated memory and merging adjacent free memory blocks. Here’s a detailed description of its operation:

### Functionality

1. **Locate Free Pointer:**
   - The function begins by identifying the position of the memory block to be freed. It uses a loop to find the correct location and checks if the pointer is `NULL`. If the pointer is `NULL`, the function returns immediately with a message indicating that there is nothing to free.

2. **Update Metadata:**
   - Upon locating the pointer, the function updates the metadata. The metadata size is adjusted to account for the size of the freed memory block. This is necessary because our implementation maintains free spaces with full metadata, while allocated pointers only store the size without metadata. Therefore, we extend the metadata to include the size of the newly freed block.

3. **Merge Adjacent Free Blocks:**
   - After updating the metadata, the function enters a loop to consolidate adjacent free memory blocks. This involves merging contiguous free spaces into a single larger block, which optimizes memory usage and improves allocation efficiency.

### Summary

In summary, `HmmFree` effectively manages memory by:

- Identifying the position of the free pointer.
- Handling cases where the pointer is `NULL`.
- Updating metadata to reflect the newly freed space.
- Merging adjacent free memory blocks into a unified block.

This approach ensures efficient memory management and reduces fragmentation in the heap.

##

`How it Free the spaces ^-^`

![7](https://github.com/user-attachments/assets/1dca9498-f38c-4802-854b-aa5355c45ace)

## 🔚 Conclusion

In this project, we have developed a custom heap memory management system in C, encompassing essential functionalities for dynamic memory allocation and deallocation. 

We implemented key functions such as `HmmAlloc` for allocating memory and `HmmFree` for freeing and merging adjacent memory blocks. By understanding and applying memory management techniques, including the First Fit allocation strategy, we ensure efficient use of heap memory and reduce fragmentation.

This custom heap implementation provides valuable insights into dynamic memory management and serves as a foundation for more advanced memory management systems. We encourage further exploration and enhancements to address specific needs and optimize performance.

##
Thank you for exploring My Heap Memory Management project. If you have any questions or feedback, feel free to reach out or contribute to the repository.


