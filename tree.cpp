#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_LEN 64

typedef struct Node {
    int code;
    char message[MSG_LEN];
    struct Node* left;
    struct Node* right;
} Node;

static Node* create_node(int code, const char* msg) {
    Node* n = (Node*)malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(1);
    }
    n->code = code;
    n->left = NULL;
    n->right = NULL;
    strncpy_s(n->message, MSG_LEN, msg, MSG_LEN - 1);
    n->message[MSG_LEN - 1] = '\0';
    return n;
}

static Node* insert(Node* root, int code, const char* msg) {
    if (!root) return create_node(code, msg);
    if (code < root->code)
        root->left = insert(root->left, code, msg);
    else if (code > root->code)
        root->right = insert(root->right, code, msg);
    return root;
}

static Node* build_from_file(const char* filename) {
    FILE* fp = NULL;
    fopen_s(&fp, filename, "r");
    if (!fp) {
        printf("Cannot open file: %s\n", filename);
        exit(1);
    }
    Node* root = NULL;
    int code;
    char msg[MSG_LEN];
    while (fscanf_s(fp, "%d %63s", &code, msg, (unsigned)sizeof(msg)) == 2)
        root = insert(root, code, msg);
    fclose(fp);
    return root;
}

int main() {
    Node* root = build_from_file("data1.txt");
    if (root == NULL) {
        printf("Error: tree is empty\n");
        return 1;
    }

    printf("Tree built successfully from data1.txt\n");

    /* todo: print, search, remove, delete */
    (void)root;
    return 0;
}