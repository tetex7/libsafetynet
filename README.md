# Libsafetynet It's just that a safety net
it adds more metadata and clean up at exit for memory allocation
Forget a freeit's got you covered
here's an example

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

Since you used the malloc that comes with the library the address      
will be appended to a linked list and on exit if it's still there in the list it will free it     
This also prevents double free's   
<br>
<br>
it also can be used in assembly 
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
<br>
    
### How to build
run
```bash
make clean # Cleans the build environment Just in case 
make  # Builds the project 
./test.sh # Test the memory safety of the project 
```
or If you're on arch
```bash
makepkg -si  # This will build, test and install
```
or
```bash
./install.sh # This will do the same thing but more nicer
```
