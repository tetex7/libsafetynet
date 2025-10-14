# Libsafetynet

**Libsafetynet** is a lightweight memory management library for C and assembly.  
It tracks allocations, prevents double frees, and adds useful metadata for debugging memory issues.   

The one thing it won't do though is stop you from blowing past the edge of an array it Just make C a little more forgiving

---

## Features

- Automatic tracking of allocations
- Prevents double frees
- Optional memory sanitization on free
- Query allocation size and last error
- Works in C and assembly
- Auto frees leftovers at exit (Optional)

---
# Dependencies
- **Runtime**
  - libc

- **Build**
  - [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/) (New enough to compile C99)
  - CMake
  - Ninja *or* Make
  - Bash

---
## C Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <libsafetynet.h>


#include <stddef.h>
#include <stdint.h>

int main()
{
    int32_t* buff = sn_malloc(sizeof(int32_t) * 10);

    if (!buff)
    {
        printf("ERROR: %s", sn_get_error_msg(sn_get_last_error()));
        exit(1);
    }

    size_t buff_size = SN_GET_ARR_SIZE(sn_query_size(buff), sizeof(int32_t));

    printf("Allocated buffer of %lu\n", sn_query_size(buff));

    for (size_t i = 0; i < buff_size; i++)
    {
        buff[i] = rand();
    }

    for (size_t i = 0; i < buff_size; i++)
    {
        printf("buff[%lu] = %i\n", i, buff[i]);
    }

    return 0;
}
```
Notes:
* `sn_malloc` tracks allocations in a linked list
* Unfreed memory is automatically freed at program exit `sn_query_size(ptr)` returns the size of an allocation
* `sn_get_last_error()` returns the last allocation error

--- 
## Assembly Example
```asm
section .data
    db "TRS", 0
    msg db "Memory allocated successfully!", 0x0A, 0
    val_msg db "test_ptr allocated at %p", 0x0A, 0
    size_msg db "test_ptr size is %lu", 0x0A, 0
    data_msg db "test_ptr is %i", 0x0A, 0
    error_msg db "error is %i", 0x0A, 0

section .bss
    test_ptr resq 1  ; Reserve space for the pointer (8 bytes for x86_64)

section .text
global _start
extern printf
extern exit

extern sn_malloc
extern sn_query_size
extern sn_get_last_error

_start:
    mov rdi, 4
    call sn_malloc
    mov [test_ptr], rax         ; Store the returned pointer in test_ptr

    test rax, rax
    jz allocation_failed   ; Jump if NULL

    lea rdi, [val_msg]     ; Argument: pointer to message
    mov rsi, [test_ptr]    ; 
    xor rax, rax           ; Clear rax for variadic function (printf)
    call printf            ; Call printf
    
    mov rdi, [test_ptr]    ; Load the address stored in test_ptr into rdi
    mov dword [rdi], 42

    mov rdi, [test_ptr]
    call sn_query_size

    lea rdi, [size_msg]
    mov rsi, rax
    xor rax, rax           ; Clear rax for variadic function (printf)
    call printf

    lea rdi, [data_msg]
    mov rsi, [test_ptr]    ; Load the address stored in test_ptr into rdi
    mov rsi, [rsi]
    xor rax, rax           ; Clear rax for variadic function (printf)
    call printf

    call sn_get_last_error
    mov rsi, rax
    lea rdi, [error_msg]
    xor rax, rax           ; Clear rax for variadic function (printf)
    call printf

    mov rdi, 0
    call exit

allocation_failed:
    call sn_get_last_error
    mov rsi, rax
    lea rdi, [error_msg]
    xor rax, rax           ; Clear rax for variadic function (printf)
    call printf

    mov rdi, 1
    call exit
```
---
## Summary

Libsafetynet is your memory “safety net.” It’s simple, reliable, and ensures clean program exits while protecting against common memory issues.
    
## Current working issues
* Fast cashing for metadata look up might be a little wonky 
  Look up times might be a little slow
* I do not plan to implement custom allocator support If you want custom allocators replace the Platform independent allocators 

### How to build
run
```bash
git clone https://github.com/tetex7/libsafetynet.git # Clone the project to your local machine
cd ./libsafetynet # Changes your current working directory to the project

./dev_setup.sh # Initializes the Cmake environment
./dev_setup.sh mk # Builds the project
./package.sh # Packages the project into a A small Folder containing headers and a library
```
