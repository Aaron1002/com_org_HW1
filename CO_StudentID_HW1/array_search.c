#include <stdio.h>

int arraySearch(int *p_a, int arr_size, int target)
{
    int result = -1;

    asm volatile(
        // Your code
        "addi x19, x0, -1\n\t" // create a index i(x19), start from -1

        "Loop:\n\t" "addi x19, x19, 1\n\t" // update index i(x19)
                    "beq x19, %[arr_size], Exit\n\t" // check index i(x19) is in range (< arr_size)
                    "slli x5, x19, 2\n\t" // index i(x19) * 4, for moving to the next element    
                    "add x5, %[p_a], x5\n\t" // p_a + i (move to the next element)

                    "lw x18, 0(x5)\n\t" // load arr[i] to x18 register
                    "bne x18, %[target], Loop\n\t" // if arr[i] != target, continue
                    "add %[result], x0, x19\n\t" // assign index to result
        "Exit:\n\t"

        :[p_a] "+r"(p_a), [target] "+r"(target), [result] "+r"(result), [arr_size] "+r"(arr_size)
    );

/*
for (int i = 0; i < arr_size; i++){
    if (arr[i] != target)
        continue;
    else 
        result = i;    
}
*/

    return result;
}

// Main function to test the implementation
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
    int target;
    fscanf(input, "%d", &target);
    fclose(input);

    int *p_a = &arr[0];

    int index = arraySearch(p_a, arr_size, target);

    // Print the result
    printf("%d ", index);
    printf("\n");

    return 0;
}
