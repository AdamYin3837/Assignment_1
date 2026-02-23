# ECE 407 Assignment Report: Memory Problems

## Group Name/Number
Group 0x05 - Adam Yin

## References
1. "Valgrind User Manual," Valgrind.org. [Online]. Available: https://valgrind.org/docs/manual/manual.html. [Accessed: Feb. 23, 2026].
2. "Debugging with GDB," Sourceware.org. [Online]. Available: https://sourceware.org/gdb/current/onlinedocs/gdb.html. [Accessed: Feb. 23, 2026].
3. "RP2040 Datasheet," Raspberry Pi Ltd. [Online]. Available: https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf. [Accessed: Feb. 23, 2026].

## Purpose
The purpose of this assignment is to:
- Practically investigate memory management in C by intentionally creating, identifying, and debugging memory leaks and corruption.
- Contrast how memory is handled dynamically on a standard Linux environment using tools like GDB and Valgrind, versus how that same code's memory is statically organized on an embedded system (the Raspberry Pi Pico).

## Introduction
In desktop environments:
- Memory leaks consume swap space and RAM, degrading performance but often resolved when the OS reclaims memory upon program termination.

In embedded systems development:
- Resources are heavily constrained.
- A microcontroller like the RP2040 lacks an OS to clean up leaked memory and a MMU (Memory Management Unit) to protect against buffer overflows.

Understanding how to:
- Track heap allocations.
- Prevent buffer overruns.
- Analyze memory maps is critical for writing safe, deterministic, and crash-free embedded firmware.

## Pico Memory

### Memory Map Analysis (pico_flash_region.ld)
- **Flash (ROM):** [Insert your analysis here. Example: The .ld file shows the Flash memory begins at 0x10000000. This is where the .text section (our executable code) and .rodata (read-only data, like string literals) are stored.]
- **SRAM (RAM):** [Insert your analysis here. Example: SRAM begins at 0x20000000. This holds our .data (initialized variables) and .bss (uninitialized variables) sections, as well as the heap and stack.]

### Disassembly Analysis (memory.dis)
- [Insert your analysis here. Example: Looking at memory.dis, we can see how our functions map to specific memory addresses in the Flash memory. We can also see the exact ARM Cortex-M0+ assembly instructions generated for our memory leak functions.]

### Relevance to Memory Investigations
- This static analysis reveals the exact physical limits of our embedded hardware.
- While Valgrind on a PC simulates a massive virtual memory space, the Pico map shows exactly how little heap is available.
- A buffer overflow that corrupts the stack here will directly overwrite hardware registers or the return address in SRAM, leading to an immediate hard fault rather than a simple segmentation fault.

## C Program with Memory Problems

### Problem #1: Unfreed Memory Allocation
- **The Problem:** [Explain the code. Example: In this test case (--LEAK 1), the program allocates 100 bytes on the heap using malloc(), but the function returns without calling free(). The pointer is lost when it goes out of scope.]
- **GDB Session & Valgrind Analysis:**
    - **GDB:** [Insert screenshot or transcript of stepping through GDB, showing the malloc execution]
    - **Valgrind:** [Insert screenshot/transcript of Valgrind output showing "definitely lost: 100 bytes in 1 blocks".]
- **How to Avoid:** Always ensure every malloc() or calloc() has a corresponding free(). In embedded systems, it is often best practice to avoid dynamic memory allocation entirely, relying instead on static allocation to guarantee memory safety.

### Problem #2: Lost Pointer Reassignment
- **The Problem:** [Explain the code. Example: In --LEAK 2, a pointer is assigned a block of memory, but then immediately reassigned a new block of memory before the first is freed. The first block is orphaned.]
- **GDB Session & Valgrind Analysis:**
    - **GDB:** [Insert transcript showing the pointer address changing, leaving the old address unreferenced.]
    - **Valgrind:** [Insert Valgrind output showing the specific leak summary.]
- **How to Avoid:** Track pointer lifecycles carefully. If a pointer needs to be reassigned, ensure `free(ptr)` is called before `ptr = malloc(new_size)`, or utilize `realloc()` if resizing an existing buffer.

### Problem #3: Buffer Overflow
- **The Problem:** [Explain the code. Example: In --LEAK 3, a local array of 10 integers is declared on the stack. A for loop incorrectly iterates 15 times, writing data past the end of the array and corrupting adjacent stack memory.]
- **GDB Session & Valgrind Analysis:**
    - **GDB:** [Insert transcript showing the stack frame variables changing unexpectedly due to the overflow.]
    - **Valgrind:** [Insert Valgrind output showing "Invalid write of size 4" or similar memory corruption errors.]
- **How to Avoid:** Always enforce strict bounds checking on loops and array indices. When using standard library functions, prefer safe alternatives (e.g., `strncpy` instead of `strcpy`, or `snprintf` instead of `sprintf`).

## Conclusion
This assignment demonstrated:
- How easily memory leaks and corruption can occur in C.
- How tools like GDB and Valgrind are essential for tracing them.

In a Linux environment:
- These errors might go unnoticed until the program consumes all available RAM or segfaults.

In embedded systems:
- As evidenced by analyzing the memory map of the Raspberry Pi Pico, embedded systems have strict physical memory boundaries.
- A buffer overflow on the Pico is catastrophic.

To manage and avoid these problems:
- Minimize dynamic heap allocation.
- Utilize static analysis tools.
- Carefully track pointer lifecycles.
- Rigorously enforce bounds checking on all arrays.ECE 407 Assignment Report : Memory ProblemsGroup Name/Number: [Your Group Name / Number Here]References[1] "Valgrind User Manual," Valgrind.org. [Online]. Available: https://valgrind.org/docs/manual/manual.html. [Accessed: Feb. 23, 2026].[2] "Debugging with GDB," Sourceware.org. [Online]. Available: https://sourceware.org/gdb/current/onlinedocs/gdb.html. [Accessed: Feb. 23, 2026].[3] "RP2040 Datasheet," Raspberry Pi Ltd. [Online]. Available: https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf. [Accessed: Feb. 23, 2026].PurposeThe purpose of this assignment is to practically investigate memory management in C by intentionally creating, identifying, and debugging memory leaks and corruption. Furthermore, it aims to contrast how memory is handled dynamically on a standard Linux environment using tools like GDB and Valgrind, versus how that same code's memory is statically organized on an embedded system (the Raspberry Pi Pico).IntroductionIn desktop environments, memory leaks consume swap space and RAM, which can degrade performance but is often resolved when the OS reclaims memory upon program termination. In embedded systems development, however, resources are heavily constrained. A microcontroller like the RP2040 has no operating system to clean up leaked memory and lacks a MMU (Memory Management Unit) to protect against buffer overflows. Understanding how to track heap allocations, prevent buffer overruns, and analyze memory maps is critical for writing safe, deterministic, and crash-free embedded firmware.Pico MemoryThe program memory.c was compiled for the Pi Pico to investigate its storage organization.Memory Map Analysis (pico_flash_region.ld):Flash (ROM): [Insert your analysis here. Example: The .ld file shows the Flash memory begins at 0x10000000. This is where the .text section (our executable code) and .rodata (read-only data, like string literals) are stored.]SRAM (RAM): [Insert your analysis here. Example: SRAM begins at 0x20000000. This holds our .data (initialized variables) and .bss (uninitialized variables) sections, as well as the heap and stack.]Disassembly Analysis (memory.dis):[Insert your analysis here. Example: Looking at memory.dis, we can see how our functions map to specific memory addresses in the Flash memory. We can also see the exact ARM Cortex-M0+ assembly instructions generated for our memory leak functions.]Relevance to Memory Investigations:This static analysis reveals the exact physical limits of our embedded hardware. While Valgrind on a PC simulates a massive virtual memory space, the Pico map shows exactly how little heap we have available. A buffer overflow that corrupts the stack here will directly overwrite hardware registers or the return address in SRAM, leading to an immediate hard fault rather than a simple segmentation fault.C Program with Memory ProblemsProblem #TypeShort Description1Memory LeakMalloc without a corresponding free(), causing unreachable heap allocation.2Memory LeakReassigning a dynamically allocated pointer before freeing the original address (Lost Pointer).3Memory CorruptionBuffer overflow: writing beyond the bounds of an allocated array on the stack.Repository Link: [Insert URL to your Git Repository here]Problem 1: Unfreed Memory AllocationThe Problem:[Explain the code. Example: In this test case (--LEAK 1), the program allocates 100 bytes on the heap using malloc(), but the function returns without calling free(). The pointer is lost when it goes out of scope.]GDB Session & Valgrind Analysis:GDB: [Insert screenshot or transcript of stepping through GDB, showing the malloc execution]Valgrind: [Insert screenshot/transcript of Valgrind output showing "definitely lost: 100 bytes in 1 blocks".]How to Avoid:Always ensure every malloc() or calloc() has a corresponding free(). In embedded systems, it is often best practice to avoid dynamic memory allocation entirely, relying instead on static allocation to guarantee memory safety.Problem 2: Lost Pointer ReassignmentThe Problem:[Explain the code. Example: In --LEAK 2, a pointer is assigned a block of memory, but then immediately reassigned a new block of memory before the first is freed. The first block is orphaned.]GDB Session & Valgrind Analysis:GDB: [Insert transcript showing the pointer address changing, leaving the old address unreferenced.]Valgrind: [Insert Valgrind output showing the specific leak summary.]How to Avoid:Track pointer lifecycles carefully. If a pointer needs to be reassigned, ensure free(ptr) is called before ptr = malloc(new_size), or utilize realloc() if resizing an existing buffer.Problem 3: Buffer OverflowThe Problem:[Explain the code. Example: In --LEAK 3, a local array of 10 integers is declared on the stack. A for loop incorrectly iterates 15 times, writing data past the end of the array and corrupting adjacent stack memory.]GDB Session & Valgrind Analysis:GDB: [Insert transcript showing the stack frame variables changing unexpectedly due to the overflow.]Valgrind: [Insert Valgrind output showing "Invalid write of size 4" or similar memory corruption errors.]How to Avoid:Always enforce strict bounds checking on loops and array indices. When using standard library functions, prefer safe alternatives (e.g., strncpy instead of strcpy, or snprintf instead of sprintf).ConclusionThis assignment demonstrated how easily memory leaks and corruption can occur in C, and how tools like GDB and Valgrind are essential for tracing them. In a Linux environment, these errors might go unnoticed until the program consumes all available RAM or segfaults. However, as evidenced by analyzing the memory map of the Raspberry Pi Pico, embedded systems have strict physical memory boundaries. A buffer overflow on the Pico is catastrophic. To manage and avoid these problems, embedded developers should minimize dynamic heap allocation, utilize static analysis tools, carefully track pointer lifecycles, and rigorously enforce bounds checking on all arrays.