#include <stdio.h>
#include <stdlib.h>

/* Because of padding, int will be +4, and then place pointer(8 bytes) */
typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Split the linked list into two parts
void splitList(Node *head, Node **firstHalf, Node **secondHalf)
{
    asm volatile(
        /*
        Block A (splitList), which splits the linked list into two halves
        */
        
        /* Find mid */
        "beq %[head], x0, Exit\n\t"
        "ld x28, 8(%[head])\n\t"  // fast = head->next
        "add x29, %[head], x0\n\t"  // slow = head (slow will be the mid)

        "Loop_A:\n\t" "beq x28, x0, Exit_A\n\t"  // check if fast is NULL 
                      "ld x5, 8(x28)\n\t" // firstHalf point to fast->next
                      "beq x5, x0, Exit_A\n\t"    // check if fast->next is NULL
                      
                      "ld x29, 8(x29)\n\t"  // slow = slow->next
                      "ld x28, 8(x28)\n\t"  // fast = fast->next
                      "ld x28, 8(x28)\n\t"  // fast = fast->next
                      
                      "beq x0, x0, Loop_A\n\t"  // Loop

        "Exit_A:\n\t" "add x6, %[head], x0\n\t" // copy the address head point to to x6
                      "sd x6, 0(%[firstHalf])\n\t"  // firstHalf point  to the first node              
        
                      "ld x6, 8(x29)\n\t"   // slow = slow->next (mid = mid->next)
                      "sd x6, 0(%[secondHalf])\n\t" // secondHalf point to slow->next
                      "sd x0, 8(x29)\n\t"   // cut off the list into two (set slow->next to 0)

        "Exit:\n\t"

        :[firstHalf] "+r"(firstHalf), [head] "+r"(head), [secondHalf] "+r"(secondHalf)
    );
}

// Merge two sorted linked lists
Node *mergeSortedLists(Node *a, Node *b)
{
    Node *result = NULL;
    Node *tail = NULL;

    asm volatile(
        /*
        Block B (mergeSortedList), which merges two sorted lists into one
        */
        
        "beq %[a], x0, Set_R\n\t"
        "beq %[b], x0, Set_L\n\t" 
        "lw x18, 0(%[a])\n\t"   // load a->data to x18
        "lw x19, 0(%[b])\n\t"   // load b->data to x19
        "slt x5, x18, x19\n\t"  // if a->data < b->data, set x5 to 1, otherwise, 0
        "bne x5, x0, Set_L\n\t" // if a->data < b->data, jump to Set_L (initial result & tail)
        "beq x5, x0, Set_R\n\t" // if a->data >= b->data, jump to Set_R (initial result & tail) 

        "Set_L:\n\t" "add %[result], %[a], x0\n\t"  // result point to the left and the first node
                     "add %[tail], %[a], x0\n\t"    // tail point to the left and the first node
                     "ld %[a], 8(%[a])\n\t"
                     "beq x0, x0, Loop_B\n\t"   // jump to the main Loop

        "Set_R:\n\t" "add %[result], %[b], x0\n\t"  // result point to the right and the first node
                     "add %[tail], %[b], x0\n\t"    // tail point to the right and the first node
                     "ld %[b], 8(%[b])\n\t"
                     "beq x0, x0, Loop_B\n\t"   // jump to the main Loop
                     
        "Loop_B:\n\t" "beq %[a], x0, Exit_BR\n\t"   // if "a" is empty, jump to Exit_BR (link remaining "b" to tail)
                      "beq %[b], x0, Exit_BL\n\t"   // if "b" is empty, jump to Exit_BL (link remaining "a" to tail)
                      "lw x18, 0(%[a])\n\t" // load a->data to x18 
                      "lw x19, 0(%[b])\n\t" // load b->data to x19
                      "slt x5, x18, x19\n\t"    // if a->data < b->data, set x5 to 1, otherwise, 0
                      "bne x5, x0, Add_L\n\t"   // if a->data < b->data, jump to Add_L (add the first node of "a" to result list)
                      "beq x0, x0, Add_R\n\t"   // if a->data >= b->data, jump to Add_R (add the first node of "b" to result list)
                      
        "Add_L:\n\t" "add x6, %[a], x0\n\t" // x6 = the address "a" point to
                     "ld %[a], 8(%[a])\n\t" // "a" move to the next node
                     "sd x6, 8(%[tail])\n\t"    // set tail->next to the first node of a
                     "ld %[tail], 8(%[tail])\n\t"   // tail momve to the next node (updated node)
                     "beq x0, x0, Loop_B\n\t"   // Loop until one of the lists is empty
                     
        "Add_R:\n\t" "add x6, %[b], x0\n\t" // x6 = the address "b" point to
                     "ld %[b], 8(%[b])\n\t" // "b" move to the next node
                     "sd x6, 8(%[tail])\n\t"    // set tail->next to the first node of b
                     "ld %[tail], 8(%[tail])\n\t"   // tail momve to the next node (updated node)
                     "beq x0, x0, Loop_B\n\t"   // Loop until one of the lists is empty
                     
        "Exit_BR:\n\t" "sd %[b], 8(%[tail])\n\t"    // tail->next = linked list "b" 
                       "beq x0, x0, Exit_B\n\t" // Exit

        "Exit_BL:\n\t" "sd %[a], 8(%[tail])\n\t"    // tail->next = linked list "a"  
                       "beq x0, x0, Exit_B\n\t" // Exit
                       
        "Exit_B:\n\t"               

        :[result] "+r"(result), [tail] "+r"(tail), [a] "+r"(a), [b] "+r"(b)
    );

    return result;
}

// Merge Sort function for linked list
Node *mergeSort(Node *head)
{
    if (!head || !head->next)
        return head; // Return directly if there is only one node

    Node *firstHalf, *secondHalf;
    splitList(head, &firstHalf,
              &secondHalf); // Split the list into two sublists

    firstHalf = mergeSort(firstHalf);   // Recursively sort the left half
    secondHalf = mergeSort(secondHalf); // Recursively sort the right half

    return mergeSortedLists(firstHalf, secondHalf); // Merge the sorted sublists
}

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
    int list_size;
    fscanf(input, "%d", &list_size);
    Node *head = (list_size > 0) ? (Node *)malloc(sizeof(Node)) : NULL;
    Node *cur = head;
    for (int i = 0; i < list_size; i++) {
        fscanf(input, "%d", &(cur->data));
        if (i + 1 < list_size)
            cur->next = (Node *)malloc(sizeof(Node));
        cur = cur->next;
    }
    fclose(input);

    // Linked list sort
    head = mergeSort(head);

    cur = head;
    while (cur) {
        printf("%d ", cur->data);
        asm volatile(
            /*
            Block C (Move to the next node), which updates the pointer to
            traverse the linked list
            */

            "ld %[cur], 8(%[cur])\n\t"  // move to the next node

            :[cur]  "+r"(cur)
        );
    }
    printf("\n");
    return 0;
}
