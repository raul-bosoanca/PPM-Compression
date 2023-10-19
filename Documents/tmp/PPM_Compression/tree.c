#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "tree.h"

Tree createTree(unsigned char node_type, unsigned char red,
unsigned char blue, unsigned char green) {
    Tree root = malloc(sizeof(TreeNode));
    root->blue = blue;
    root->red = red;
    root->green = green;
    root->node_type = node_type;
    root->first_child = NULL;
    root->next_sibling = NULL;
    return root;
}

Tree insertChild(Tree parent, unsigned char node_type, unsigned char red,
unsigned char blue, unsigned char green) {
    Tree new_node = createTree(node_type, red, blue, green);
    Tree curr = parent->first_child;
    if (curr == NULL) {
        parent->first_child = new_node;
    } else {
        while (curr->next_sibling != NULL) {
            curr = curr->next_sibling;
        }
        curr->next_sibling = new_node;
    }
    return new_node;
}

RGB** initGrid(int dim) {
    RGB** grid = malloc(sizeof(RGB*) * dim);
    for (int i = 0; i < dim; i++) {
        grid[i] = malloc(sizeof(RGB) * dim);
    }
    return grid;
}

void createCompresionTree(Tree *root, RGB **grid, int start_row, int start_col,
int size, int tol, int *nr_of_squares, int *nr_of_levels, int *biggest_square_side, int start_dim) {
    unsigned long long  red_average = 0, green_average = 0, blue_average = 0;
    unsigned long long mean = 0;
    int i = 0, j = 0;
    if (log2(start_dim / size) + 1 > *nr_of_levels) {
        *nr_of_levels = log2(start_dim / size) + 1;
    }
    for (i = start_row; i < start_row + size; i++) {
        for (j = start_col; j < start_col + size; j++) {
            red_average += grid[i][j].red;
            blue_average += grid[i][j].blue;
            green_average += grid[i][j].green;
        }
    }
    red_average /= (size * size);
    green_average /= (size * size);
    blue_average /= (size * size);


    for (i = start_row; i < start_row + size; i++) {
        for (j = start_col; j < start_col + size; j++) {
            mean += ((red_average - grid[i][j].red) * (red_average - grid[i][j].red));
            mean += ((blue_average - grid[i][j].blue) * (blue_average - grid[i][j].blue));
            mean += ((green_average - grid[i][j].green) * (green_average - grid[i][j].green)); 
        }
    }
    mean /= (size * size * 3);
    if (floor(mean) <= tol) {
        (*root)->red = red_average;
        (*root)->blue = blue_average;
        (*root)->green = green_average;
        if (size > *biggest_square_side) {
            *biggest_square_side = size;
        }
    } else {
        (*nr_of_squares) += 3;
        (*root)->node_type = 0;
        Tree first_child = insertChild(*root, 1, 0, 0, 0);
        createCompresionTree(&first_child, grid, start_row, start_col,
        size / 2, tol, nr_of_squares, nr_of_levels,biggest_square_side, start_dim);
        Tree second_child = insertChild(*root, 1, 0, 0, 0);
        createCompresionTree(&second_child, grid, start_row, start_col + size / 2,
        size / 2, tol, nr_of_squares, nr_of_levels,biggest_square_side, start_dim);
        Tree third_child = insertChild(*root, 1, 0, 0, 0);
        createCompresionTree(&third_child, grid, start_row + size / 2, start_col + size / 2,
        size / 2, tol, nr_of_squares, nr_of_levels,biggest_square_side, start_dim);
        Tree forth_child = insertChild(*root, 1, 0, 0, 0);
        createCompresionTree(&forth_child, grid, start_row + size / 2, start_col,
        size / 2, tol, nr_of_squares, nr_of_levels, biggest_square_side, start_dim);
    }
}

ListNode* createNode(Tree treeNode) {
    ListNode* new_node = malloc(sizeof(ListNode));
    new_node->node = treeNode;
    return new_node;
}

Queue* createQueue() {
    Queue *new_queue = malloc(sizeof(Queue));
    new_queue->front = new_queue->rear = NULL;
    return new_queue;
}

Queue* dequeue(Queue* queue) {
    if (queue == NULL) {
        return createQueue();
    }
    if (queue->front == queue->rear) {
        if (queue->front == NULL) {
            return queue;
        }
        free(queue->front);
        queue->front = queue->rear = NULL;
        return queue;
    }
    ListNode* aux = queue->front;
    queue->front = queue->front->next;
    free(aux);
    return queue;
}

Queue* enqueue(Queue* queue, Tree treeNode) {
    if (queue == NULL) {
        queue = createQueue();
    }
    ListNode* new_node = createNode(treeNode);
    if (queue->front == NULL) {
        queue->front = queue->rear = new_node;
        return queue;
    }
    queue->rear->next = new_node;
    queue->rear = new_node;
    return queue;
}

char isQueueEmpty(Queue* queue) {
    return queue->front == NULL;
}

void createDecompresionTree(Tree *root, FILE* in) {
    if (*root == NULL) {
        *root = createTree(0,0,0,0);
    }
    Queue *levelsQueue = createQueue();
    levelsQueue = enqueue(levelsQueue, *root);
    Tree curr, first_child, second_child, third_child, forth_child;
    while (isQueueEmpty(levelsQueue) == 0) {
        curr = levelsQueue->front->node;
        fread(&(curr->node_type), sizeof(char), 1, in);
        if (curr->node_type == 1) {
            fread(&(curr->red), sizeof(char), 1, in);
            fread(&(curr->green), sizeof(char), 1, in);
            fread(&(curr->blue), sizeof(char), 1, in);
        } else {
            first_child = insertChild(curr, 1, 0, 0, 0);
            levelsQueue = enqueue(levelsQueue, first_child);
            second_child = insertChild(curr, 1, 0, 0, 0);
            levelsQueue = enqueue(levelsQueue, second_child);
            third_child = insertChild(curr, 1, 0, 0, 0);
            levelsQueue = enqueue(levelsQueue, third_child);
            forth_child = insertChild(curr, 1, 0, 0, 0);
            levelsQueue = enqueue(levelsQueue, forth_child);
        }
        levelsQueue = dequeue(levelsQueue);
    }
    free(levelsQueue);
}

void createPPMFIle(Tree root, int dim, FILE* out) {
    int dim_copy = dim;
    char type[] = "P6\n";
    fwrite(type, sizeof(type) - 1, 1, out);
    int reverse_dim = 0;
    int dim_lenght = 0;
    while (dim > 0) {
        reverse_dim = reverse_dim * 10 + dim % 10;
        dim /= 10;
        dim_lenght++;
    }
    char *dim_to_string = malloc(dim_lenght + 1);
    int i = 0;
    for (i = 0; i < dim_lenght; i++) {
        dim_to_string[i] = reverse_dim % 10 + '0';
        reverse_dim /= 10;
    }
    dim_to_string[dim_lenght] = ' ';
    fwrite(dim_to_string, dim_lenght + 1, 1, out);
    dim_to_string[dim_lenght] = '\n';
    fwrite(dim_to_string, dim_lenght + 1, 1, out);
    free(dim_to_string);
    char max[] = "255\n";
    fwrite(max, sizeof(max) - 1, 1, out);
    dim = dim_copy;
    RGB **grid = initGrid(dim);
    completeGrid(grid, root, dim, 0 , 0);
    for (int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            fwrite(&grid[i][j].red, sizeof(char), 1, out);
            fwrite(&grid[i][j].green, sizeof(char), 1, out);
            fwrite(&grid[i][j].blue, sizeof(char), 1, out);

        }
    }
    for (i = 0; i < dim; i++) {
        free(grid[i]);
    }
    free(grid);

}

void completeGrid(RGB **grid, Tree root, int dim, int start_row, int start_col) {
    if (root->node_type == 1) {
        int i = 0, j = 0;
        for (i = start_row; i < start_row + dim; i++) {
            for (j = start_col; j < start_col + dim; j++) {
                grid[i][j].red = root->red;
                grid[i][j].green = root->green;
                grid[i][j].blue = root->blue;
            }
        }
    } else {
        Tree curr = root->first_child;
        completeGrid(grid, curr, dim / 2, start_row, start_col);
        curr = curr->next_sibling;
        completeGrid(grid, curr, dim / 2, start_row, start_col + dim / 2);
        curr = curr->next_sibling;
        completeGrid(grid, curr, dim / 2, start_row + dim / 2, start_col + dim / 2);
        curr = curr->next_sibling;
        completeGrid(grid, curr, dim / 2, start_row + dim / 2, start_col);
    }
}
void freeTree(Tree root) {
    if (root == NULL) {
        return;
    }
    Tree first_child = root->first_child;
    if (first_child != NULL) {
        Tree second_child = first_child->next_sibling;
        Tree third_child = second_child->next_sibling;
        Tree forth_child = third_child->next_sibling;
        freeTree(forth_child);
        freeTree(third_child);
        freeTree(second_child);
    }
    freeTree(first_child);
    free(root);
}

void freeMemory(Queue* queue, Tree root, RGB** grid, int dim) {
    free(queue);
    freeTree(root);
    int i = 0;
    for (i = 0; i < dim; i++) {
        free(grid[i]);
    }
    free(grid);
}
    