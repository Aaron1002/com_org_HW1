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
        "ld %[firstHalf], 0(%[head])\n\t" // firstHalf = head
        "ld %[secondHalf], 8(%[head])\n\t"  // secondHalf = head -> next
        
        "Loop:\n\t" "beq %[firstHalf], x0, Exit\n\t"    // if firstHalf == NULL, break
                    "ld x5, 8(%[firstHalf])\n\t"    // x5 = firstHalf -> next
                    "beq x5, x0, Exit\n\t"  // if firstHalf -> next == NULL, break
                    
                    "ld %[firstHalf], 16(%[firstHalf])\n\t" // firstHalf = firstHalf -> next -> next
                    "ld %[secondHalf], 8(%[secondHalf])\n\t"    // secondHalf = secondHalf -> next
        
        "Exit:\n\t"
                    "ld %[firstHalf], 0(%[head])\n\t" // recover the firstHalf 

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
        
        "Loop:\n\t" "beq %[a], x0, Exit\n\t"    // check if L is NULL
                    "beq %[b], x0, Exit\n\t"    // check if R is NULL

                    "ld x5, 0(%[a])\n\t"    // load L[i] to compare to R[j]
                    "ld x6, 0(%[b])\n\t"    // load R[j] to compare to L[i]

                    "slt x7, x6, x5\n\t"    // if R[j] < L[i], set x7 as 1, otherwise 0
                    "beq x7, x0, Set_L\n\t" // if R[j] >= L[i], place L[i] in the result list 
                    "beq x0, x0, Set_R\n\t" // if R[j] < L[i], place R[j] in the result list

        "Set_R:\n\t" "ld %[result], 0(%[b])\n\t"    // result point to the smallest number of node
                     "ld %[tail], 8(%[result])\n\t" // move to the next node
                     "beq x0, x0, \n\t"

        "Set_L:\n\t" "ld %[result], 0(%[a])\n\t"    // result point to the smallest number of node
                     "ld %[tail], 8(%[result])" // move to the next node
                     "beq x0, x0, \n\t"

        "Choose:\n\t" "beq %[tail], x0, Exit\n\t"             
                      ""

        "Exit:\n\t"
    
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
            "ld %[cur], 8(%[cur])\n\t"

            :[cur]  "+r"(cur)
        );
    }
    printf("\n");
    return 0;
}
