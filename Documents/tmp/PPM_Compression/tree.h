typedef struct RGB {
    unsigned char red, green, blue;
} RGB;

typedef struct treeNode {
    unsigned char node_type;
    unsigned char red, green, blue;
    struct treeNode *first_child, *next_sibling;
} TreeNode, *Tree;

Tree createTree(unsigned char node_type, unsigned char red,
unsigned char blue, unsigned char green);

Tree insertChild(Tree parent, unsigned char node_type, unsigned char red,
unsigned char blue, unsigned char green);

RGB** initGrid(int dim);

void createCompresionTree(Tree *root, RGB **grid, int start_row, int start_col,
int size, int tol, int *nr_of_squares, int *nr_of_levels, int *bigest_square, int start_dim);

void createDecompresionTree(Tree *root, FILE* in);

typedef struct listNode {
    Tree node;
    struct listNode *next;
} ListNode;

typedef struct queue {
    ListNode *front, *rear;
} Queue;

ListNode* createNode(Tree treeNode);

Queue* createQueue();

Queue* dequeue(Queue* queue);

Queue* enqueue(Queue* queue, Tree treeNode);

char isQueueEmpty(Queue* queue);

void createPPMFIle(Tree root, int dim, FILE* out);

void completeGrid(RGB **grid, Tree root, int dim, int start_row, int start_col);

void freeMemory(Queue* queue, Tree root, RGB** grid, int dim);

void freeTree(Tree root);