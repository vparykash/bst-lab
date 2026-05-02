#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_LEN 64
#define STACK_CAP 32

typedef struct Node {
    int code;
    char message[MSG_LEN];
    struct Node* left;
    struct Node* right;
} Node;

typedef struct {
    Node* data[STACK_CAP];
    int top;
} Stack;

static void stack_init(Stack* s) {
    s->top = -1;
}

static int stack_empty(const Stack* s) {
    return s->top < 0;
}

static void stack_push(Stack* s, Node* n) {
    if (s->top < STACK_CAP - 1)
        s->data[++(s->top)] = n;
}

static Node* stack_pop(Stack* s) {
    return stack_empty(s) ? NULL : s->data[(s->top)--];
}

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

static void print_inorder(const Node* root) {
    if (!root) return;
    print_inorder(root->left);
    printf("  [%4d]  %s\n", root->code, root->message);
    print_inorder(root->right);
}

static void print_tree_visual(const Node* root, int depth) {
    if (!root) return;
    print_tree_visual(root->right, depth + 1);
    for (int i = 0; i < depth; i++) printf("        ");
    printf("[%d]\n", root->code);
    print_tree_visual(root->left, depth + 1);
}

static void search_by_code(const Node* root, int target) {
    const Node* cur = root;
    while (cur) {
        if (target == cur->code) {
            printf("Found code %d: %s\n", cur->code, cur->message);
            return;
        }
        cur = (target < cur->code) ? cur->left : cur->right;
    }
    printf("Code %d not found in the tree.\n", target);
}

static int has_one_child(const Node* n) {
    return (n->left != NULL) ^ (n->right != NULL);
}

static Node* delete_one_child_node(Node* root, int code) {
    Node* parent = NULL, *cur = root;
    while (cur && cur->code != code) {
        parent = cur;
        cur = (code < cur->code) ? cur->left : cur->right;
    }
    if (!cur) return root;
    Node* child = cur->left ? cur->left : cur->right;
    if (!parent) {
        free(cur);
        return child;
    }
    if (parent->left == cur)
        parent->left = child;
    else
        parent->right = child;
    free(cur);
    return root;
}

static Node* remove_single_child_nodes(Node* root) {
    if (!root) return NULL;
    int codes[STACK_CAP];
    int count = 0;
    Stack s;
    stack_init(&s);
    Node* cur = root;

    while (cur || !stack_empty(&s)) {
        while (cur) {
            stack_push(&s, cur);
            cur = cur->left;
        }
        cur = stack_pop(&s);
        if (has_one_child(cur) && count < STACK_CAP)
            codes[count++] = cur->code;
        cur = cur->right;
    }

    if (count == 0) {
        printf("No nodes with exactly one child found.\n");
        return root;
    }

    printf("Nodes with exactly one child (to be removed):\n");
    for (int i = 0; i < count; i++)
        printf("  Code: %d\n", codes[i]);

    for (int i = 0; i < count; i++)
        root = delete_one_child_node(root, codes[i]);

    return root;
}


int main() {
    const char* files[] = { "data1.txt", "data2.txt" };
    for (int f = 0; f < 2; f++) {
        printf("\n=== Dataset: %s ===\n", files[f]);
        Node* root = build_from_file(files[f]);
        if (!root) continue;

        printf("\n--- In-order traversal ---\n");
        print_inorder(root);

        printf("\n--- Visual tree ---\n");
        print_tree_visual(root, 0);

        printf("\n--- Search by code ---\n");
        printf("Enter code to search: ");
        int target;
        scanf_s("%d", &target);
        search_by_code(root, target);

        printf("\n--- Remove nodes with exactly one child ---\n");
        root = remove_single_child_nodes(root);

        printf("\n--- Tree after removal (in-order) ---\n");
        print_inorder(root);

        printf("\n--- Updated visual tree ---\n");
        print_tree_visual(root, 0);
    }
    return 0;
}