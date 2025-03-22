#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }
    int arr_size;
    fscanf(input, "%d", &arr_size);
    int arr[arr_size];

    // Read integers from input file into the array
    for (int i = 0; i < arr_size; i++) {
        int data;
        fscanf(input, "%d", &data);
        arr[i] = data;
    }
    fclose(input);

    int *p_a = &arr[0];

    // array a bubble sort
    /* Original C code segment
    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i -1; j++) {
            if (*(p_a + j) > *(p_a + j + 1)) {
                int tmp = *(p_a + j);
                *(p_a + j) = *(p_a + j + 1);
                *(p_a + j + 1) = tmp;
            }
        }
    }
    */

    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i - 1; j++) {
            asm volatile(
                // Your code   
                "xori x9, x0, -1\n\t" // create an all one register (x9)
                
                /* move pointer: p_a + (j * 4) */
                "slli x20, %[j], 2\n\t" // j*4
                "add x20, %[p_a], x20\n\t" // copy base address to x20 register 

                /* Load array element */
                "lw x18, 0(x20)\n\t" // load arr[j] to x18 register
                "lw x19, 4(x20)\n\t" // load arr[j+1] to x19 register
                
                /* jump if greater or equal to */
                "slt x5, x19, x18\n\t" // if arr[j+1] < arr[j], set x5 to 1, otherwise, 0
                "beq x5, x0, Exit\n\t" // if x5 == 0, Loop

                /* Swap(arr[j], arr[j+1]) */
                "and x6, x19, x9\n\t" // copy arr[j+1] to temp register (x6)
                "and x19, x18, x9\n\t" // arr[j+1] = arr[j]
                "and x18, x6, x9\n\t" // arr[j] = temp

                /* store swapped element back to memory */
                "sw x18, 0(x20)\n\t"
                "sw x19, 4(x20)\n\t"
    
                "Exit:\n\t"

                :[p_a] "+r"(p_a), [j] "+r"(j)
            );
        }
    }
    p_a = &arr[0];
    for (int i = 0; i < arr_size; i++)
        printf("%d ", *p_a++);
    printf("\n");
    return 0;
}
