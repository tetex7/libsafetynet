# Libsafetynet It's just that a safety net
it adds more metadata and clean up at exit for memory allocation
Forget a freeit's got you covered
here's an example

```c
#include <stdio.h>
#include <stdlib.h>
#include <libsafetynet.h>

// Example usage of sn_malloc and sn_free
int main() 
{
    int* ptr = sn_malloc(sizeof(int));
    *ptr = 44;

    printf("int %i", *ptr);

    return 0;
}
```

Since you used the malloc that comes with the librarythat address      
will be appended to a linked list and on exit if it's still there in the list it will free it     
This also prevents double free's    