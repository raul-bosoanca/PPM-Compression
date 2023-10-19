#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"

int main(int argc, char* args[]) {
    if (argc <= 3) {
        printf("Not enought parametres!\n");
        return 0;
    }
    int dim = 0, i = 0, j = 0;
    char buffer;

    /*     We check if the program has enough parametres        */
    /*  and we open the files depending on the task we solve    */

    FILE *in, *out;
    int tol = 0;
    if (argc == 4 && strcmp(args[1], "-d") == 0) {
        in = fopen(args[2], "rb");        
        out = fopen(args[3], "wt");
    } else 
    if (argc == 5) {
        in = fopen(args[3], "rb");
        if (strcmp(args[1], "-c1") == 0) {
            out = fopen(args[4], "wt");
        } else if (strcmp(args[1], "-c2") == 0) {
            out = fopen(args[4], "wb");
        }
        tol = atoi(args[2]);
    } else {
        exit(1);
    }

    if (argc == 5) {
        /*  We read the data from the binary file and   */
        /*          construct the grid matrix           */
        
        // we read until we find the first new line, so
        // we jump over the format, whici we know is "P6"
        fread(&buffer, sizeof(char), 1, in);
        while (buffer != '\n') {
            fread(&buffer, sizeof(char), 1, in);
        }
        // we read until we find the first ' ' and we build
        // the size of grid from the numbers we read 
        fread(&buffer, sizeof(char), 1, in);
        while (buffer != ' ') {
            dim = dim * 10 + buffer - '0';
            fread(&buffer, sizeof(char), 1, in);
        }
        // we read until we find the first new line, so
        // we jump over the second dimension, whici is the same
        while (buffer != '\n') {
            fread(&buffer, sizeof(char), 1, in);
        }
        // we read until we find the first new line, so
        // we jump over the max RGB value, whici is 256
        fread(&buffer, sizeof(char), 1, in);
        while (buffer != '\n') {
            //printf("%c", buffer);
            fread(&buffer, sizeof(char), 1, in);
        }
        RGB **grid = initGrid(dim);
        for (i = 0; i < dim; i++) {
            for (j = 0; j < dim; j++) {
                fread(&grid[i][j], sizeof(RGB), 1, in);
            }
        }

        Tree compresionTree = createTree(1, 0, 0, 0);
        int nr_of_squares = 1;
        int nr_of_levels = 0;
        int biggest_square_side = 0;

        //  we have to solve the 1st or the 2nd task
        createCompresionTree(&compresionTree, grid, 0, 0, dim, tol, &nr_of_squares,
        &nr_of_levels, &biggest_square_side, dim);
        Queue* levelQueue = createQueue();
        if (strcmp(args[1], "-c1") == 0) {
            // we have to solve the 1st task, so we print the values
            fprintf(out, "%d\n%d\n%d\n", nr_of_levels, nr_of_squares, biggest_square_side);
        } else if (strcmp(args[1], "-c2") == 0) {
            // we have to solve the second task
            fwrite(&dim, sizeof(int), 1, out);
            levelQueue = enqueue(levelQueue, compresionTree);
            while (isQueueEmpty(levelQueue) == 0) {   
                ListNode* curr = levelQueue->front;
                fwrite(&curr->node->node_type, sizeof(char), 1, out);
                if (curr->node->node_type == 1) {
                    fwrite(&curr->node->red, sizeof(char), 1, out);
                    fwrite(&curr->node->green, sizeof(char), 1, out);
                    fwrite(&curr->node->blue, sizeof(char), 1, out);                    
                }
                Tree child = curr->node->first_child;
                while (child != NULL) {
                    levelQueue = enqueue(levelQueue, child);
                    child = child->next_sibling;
                }
                levelQueue = dequeue(levelQueue);
            }
        }
        freeMemory(levelQueue, compresionTree, grid, dim);
        fclose(in);
        fclose(out);   
        return 0;
    }

    //  if we get here, that means we have to 
    //  solve the third task
    Tree decompresionTree = createTree(0, 0, 0, 0);
    fread(&dim, sizeof(int), 1, in);
    createDecompresionTree(&decompresionTree, in);
    createPPMFIle(decompresionTree, dim, out);
    freeTree(decompresionTree);
    fclose(in);
    fclose(out); 
    return 0;

    //TODO: Comentarii si README
}
