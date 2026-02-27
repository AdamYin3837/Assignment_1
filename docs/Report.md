# ECE 407 Assignment Report: Memory Problems

## Group Name/Number
Group 0x05 - Adam Yin

## References
[1] Valgrind Developers. (2025, Oct. 24). *Valgrind User Manual* [Online]. Available: 
https://valgrind.org/docs/manual/manual.html

[2] S. Knudsen. "ECE 407 Lecture 7 W2026." [Online]. Available: 
https://canvas.ualberta.ca/courses/30117/files/7164528?module_item_id=3593950 [Accessed Feb. 23, 2026].

[3] S. Knudsen. "ECE 407 Lecture 8 W2026." [Online]. Available: 
https://canvas.ualberta.ca/courses/30117/files/7217285?module_item_id=3602940 [Accessed Feb. 23, 2026].

[4] S. Knudsen. "ECE 407 Lecture 9 W2026." [Online]. Available: 
https://canvas.ualberta.ca/courses/30117/files/7221212?module_item_id=3603239 [Accessed Feb. 23, 2026].

[5] Raspberry Pi Ltd. (2025, Feb. 20). *RP2040 Datasheet* [Online]. Available: 
https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf

## Purpose
The purpose of this assignment is twofold:
- To investigate memory management in C by intentionally creating, identifying, and debugging memory leaks and corruption.
- To constrast how dynamic memory is handled on a standard Linux environment (using GDB and Valgrind) with how memory is statically organized on an embedded system (the Raspberry Pi Pico).

## Introduction
In desktop environments, memory leaks typically consume swap space and RAM, degrading system performance. However, these issues are ultimately mitigated because the operating system reclaims all allocated memory upon program termination.

Conversely, in embedded systems development, hardware resources are heavily constrained. A microcontroller like the RP2040 operates without an underlying OS to automatically clean up leaked memory, and it lacks a Memory Management Unit (MMU) to protect against buffer overflows and memory corruption. Because embedded systems are expected to run continuously without intervention, even a minor memory leak can lead to severe failure.

Therefore, mastering the ability to track heap allocations, prevent buffer overruns, and analyze system memory maps is important for engineers to write safe and crash-free embedded firmware.

## Pico Memory (Cross-Compilation & Memory Analysis)
In this section, we cross-compiled the C program for the Raspberry Pi Pico (an ARM Cortex-M0+ microcontroller) to observe how the code is translated and mapped to physical memory.

**1. Pico Memory Map (`.ld` Linker Script)**
The linker script defines the physical memory layout of the Pico. As shown in the generated `pico_flash_region.ld` file, the Pico has a specific region dedicated to FLASH memory:
* `FLASH(rx) : ORIGIN = 0x10000000, LENGTH = (2 * 1024 * 1024)`

This indicates that the executable read-only memory starts exactly at the absolute address `0x10000000` and has a size of 2MB. 

**2. ARM Assembly & Memory Allocation (`.dis` Disassembly)**
By examining the disassembly file `pico_memory.dis`, we can verify that our standard C code has been successfully translated into ARM assembly language.
* For instance, the `<main>:` function begins with the instruction `push {r4, lr}`. The use of specific hardware registers like `r4` and `lr` confirms the ARM architecture.
* More importantly, the memory address assigned to the `<main>:` function is `100002d4`. This address falls perfectly within the `0x10000000` range defined in the linker script. This proves that the cross-compiler and linker accurately placed our program's instructions into the Pico's physical FLASH memory.

## C Program with Memory Problems
To demonstrate common memory management errors, three distinct memory issues were simulated and debugged using Valgrind and GDB.

### Problem 1: Heap Leak (`--LEAK 1`)
A standard memory leak occurs when dynamically allocated memory is never freed.
* **Implementation:** 40 bytes (10 integers) were allocated using `malloc()`, but `free()` was never called before the function returned.
* **Valgrind Analysis:** Valgrind clearly caught this issue, reporting `40 bytes in 1 blocks are definitely lost` in the `leak_one` function.
* **GDB Inspection:** Using GDB, we stepped through the code and observed the pointer `ptr` receiving the allocated memory address (`0xaaaaaaab22a0`). Since we lost track of this address without freeing it, the memory remains occupied indefinitely.

### Problem 2: Buffer Overflow (`--LEAK 2`)
A buffer overflow happens when data is written past the boundary of the allocated memory array.
* **Implementation:** We created a 5-byte character array but used `strcpy` to force a much longer string into it, corrupting the adjacent memory.
* **GDB & Stack Canary Analysis:** In GDB, the program does not crash immediately upon executing `strcpy`. However, the moment the program executes the closing brace `}` to return from the function, the OS stack canary mechanism kicks in. It detects that the stack has been smashed and immediately terminates the program, resulting in a `SIGABRT` signal.

### Problem 3: Lost Pointer (`--LEAK 3`)
A lost pointer occurs when a pointer holding a valid memory address is overwritten with a new address, turning the original memory block inaccessible.
* **Implementation:** We allocated 80 bytes (20 integers) to `ptr`. Then, without freeing it, we reassigned `ptr` to a new 40-byte allocation. Calling `free(ptr)` at the end only frees the *second* allocation.
* **GDB Inspection:** GDB explicitly shows the exact moment the pointer is lost. After the first allocation, `ptr` points to `0xaaaaaaab22a0`. After the second allocation, the address inside `ptr` is overwritten to `0xaaaaaaab2300`. The original `0xaaaaaaab22a0` address is gone forever.
* **Valgrind Analysis:** Valgrind confirms that the original `80 bytes` are `definitely lost` at line 17, validating our lost pointer theory.

## Conclusion
This assignment practically demonstrated the critical importance of memory management in C programming. By utilizing tools like Valgrind and GDB, we could visualize invisible memory operations, trace exact crash points, and intercept stack-smashing events. 

Furthermore, cross-compiling the code for the Raspberry Pi Pico highlighted the vast differences between development environments. While a PC operating system has robust protections (like stack canaries) and gigabytes of RAM to forgive small memory leaks, a microcontroller operates on bare metal with extremely limited resources (e.g., 264KB of SRAM). On such embedded systems, even minor leaks or overflows—like the 40 bytes lost in our example—can rapidly drain the system memory and cause catastrophic failures. Proper memory handling is not just a coding standard; it is a vital necessity for system stability.