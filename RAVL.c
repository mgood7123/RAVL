// demo: https://godbolt.org/z/KW1TrK5ee

/*
R 1
    L 10
        L 8
            L 4
            R 3
        R 7
            L 2
    R 9
        L 6
        R 5
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#define log(fd, id) printf("%s = " id "\n", #fd, fd)
#define logb(fd) printf("%s = %s\n", #fd, (fd) ? "true" : "false")

#define logt(tag, fd, id) printf("%s: %s = " id "\n", tag, #fd, fd)
#define logbt(tag, fd) printf("%s: %s = %s\n", tag, #fd, (fd) ? "true" : "false")

struct RAVL_LAYERED_NODE {
    // data
    int key;

    // required for LL
    struct RAVL_LAYERED_NODE* next;

    // required for binary tree
    int layer_size;
    struct RAVL_LAYERED_NODE* left;
    struct RAVL_LAYERED_NODE* right;
    struct RAVL_LAYERED_NODE* parent; // points to upper layer

    // required for traversal
    bool seen;
};

struct RAVL {
    struct RAVL_LAYERED_NODE * root_layer; // points to the very first layer
    struct RAVL_LAYERED_NODE * current_layer; // points to the beginning of the current layer
    struct RAVL_LAYERED_NODE * previous_layer; // points to the beginning of the previous layer
    struct RAVL_LAYERED_NODE * current_node; // points to the current node in the current layer
    struct RAVL_LAYERED_NODE * current_parent_node; // points to the current node in the previous layer
    int layer_count;
    int nodes_in_current_layer;
    int total_nodes;
    bool next_is_left;
};

struct RAVL * root;

struct RAVL_LAYERED_NODE * createNode(int key) {
    // zero all pointers and assign key
    struct RAVL_LAYERED_NODE * node = calloc(1, sizeof(struct RAVL_LAYERED_NODE));
    node->key = key;
    return node;
}

struct RAVL * createRoot(int key) {
    struct RAVL * root = malloc(sizeof(struct RAVL));
    root->root_layer = createNode(key);
    root->root_layer->layer_size = 1;
    root->current_layer = root->root_layer;
    root->current_node = root->current_layer;
    root->current_parent_node = 0;
    root->previous_layer = 0;
    root->layer_count = 1;
    root->nodes_in_current_layer = 1;
    root->next_is_left = true;
    root->total_nodes = 1;
    return root;
}

void insert(struct RAVL * root, int key) {
    struct RAVL_LAYERED_NODE * layer = createNode(key);
    layer->layer_size = root->current_layer->layer_size;

    if(root->nodes_in_current_layer == root->current_layer->layer_size) {

        // current layer is full, move to next layer
        layer->layer_size *= 2;
        layer->parent = root->current_layer;
        layer->parent->left = layer;

        root->previous_layer = root->current_layer;
        root->current_parent_node = root->previous_layer;
        root->current_layer = layer;
        root->current_node = root->current_layer;
        root->layer_count++;
        root->nodes_in_current_layer = 1;
        root->next_is_left = false;
        root->total_nodes++;
    } else {
        // current layer is not full
        root->nodes_in_current_layer++;
        if (root->current_node == NULL) {
            printf("current node is NULL\n");
        } else {
            root->current_node->next = layer;
        }
        root->current_node = layer;
        root->total_nodes++;

        layer->parent = root->current_parent_node;
        if (root->next_is_left) {
            root->next_is_left = false;
            if (root->current_parent_node != NULL) {
                root->current_parent_node->left = layer;
            }
        } else {
            root->next_is_left = true;
            if (root->current_parent_node != NULL) {
                root->current_parent_node->right = layer;
                if (root->current_parent_node->next != NULL) {
                    // our next allocation will be in the next parent node
                    root->current_parent_node = root->current_parent_node->next;
                }
            }
        }
    }
}

void insert_and_sort(struct RAVL * root, int key) {
    printf("searching for node larger than key: %d\n", key);
    struct RAVL_LAYERED_NODE * n = root->root_layer;
    bool found = false;
    while(true) {
        printf("current node key: %d, wanted key: %d\n", n->key, key);
        if (n->key > key) {
            printf("found (R) node larger than key\n");
            found = true;
            break;
        }
        if (n->left) {
            printf("current node left key: %d, wanted key: %d\n", n->left->key, key);
            if (n->left->key > key) {
                n = n->left;
                printf("found node (L) larger than key\n");
                found = true;
                break;
            }
        }
        if (n->right) {
            printf("current node right key: %d, wanted key: %d\n", n->right->key, key);
            if (n->right->key > key) {
                n = n->right;
                printf("found node (R) larger than key\n");
                found = true;
                break;
            }
        }
        break;
    }
    if (!found) {
        printf("failed to find node larger than key: %d\n\n", key);
        insert(root, key);
    } else {
        putchar('\n');

        struct RAVL_LAYERED_NODE * layer = createNode(key);
        layer->layer_size = root->current_layer->layer_size;

        struct RAVL_LAYERED_NODE * new_node = layer;
        struct RAVL_LAYERED_NODE * target_node = n;

        // we need to insert new node at target node

        if(root->nodes_in_current_layer == root->current_layer->layer_size) {

            // current layer is full, move to next layer
            layer->layer_size *= 2;
            layer->parent = root->current_layer;
            layer->parent->left = layer;

            root->previous_layer = root->current_layer;
            root->current_parent_node = root->previous_layer;
            root->current_layer = layer;
            root->current_node = root->current_layer;
            root->layer_count++;
            root->nodes_in_current_layer = 1;
            root->next_is_left = false;
            root->total_nodes++;
        } else {
            // current layer is not full
            root->nodes_in_current_layer++;
            if (root->current_node == NULL) {
                printf("current node is NULL\n");
            } else {
                root->current_node->next = layer;
            }
            root->current_node = layer;
            root->total_nodes++;

            layer->parent = root->current_parent_node;
            if (root->next_is_left) {
                root->next_is_left = false;
                if (root->current_parent_node != NULL) {
                    root->current_parent_node->left = layer;
                }
            } else {
                root->next_is_left = true;
                if (root->current_parent_node != NULL) {
                    root->current_parent_node->right = layer;
                    if (root->current_parent_node->next != NULL) {
                        // our next allocation will be in the next parent node
                        root->current_parent_node = root->current_parent_node->next;
                    }
                }
            }
        }
    }
}

void print(struct RAVL * root_) {
    struct RAVL_LAYERED_NODE * root = root_->root_layer;

    if (root == NULL) {
        printf("NULL");
        return;
    }
    printf("R %d\n", root->key);

    root->seen = false;
    if (root->left) root->left->seen = false;
    if (root->right) root->right->seen = false;
    bool left = true;
    int i = 0;

    int increment = 4;

    while(root != NULL) {
        if (root->left == NULL && root->right == NULL && root->parent == NULL) {
            break;
        }
        if (left) {
            if (root->left != NULL && !root->left->seen) {
                root = root->left;
                if (root->left) root->left->seen = false;
                if (root->right) root->right->seen = false;
                i += increment;
                printf("%*sL %d", i, "", root->key);
                printf("\n");
                continue;
            }
            if (root->right != NULL && !root->right->seen) {
                root = root->right;
                if (root->left) root->left->seen = false;
                if (root->right) root->right->seen = false;
                i += increment;
                printf("%*sR %d", i, "", root->key);
                printf("\n");
                continue;
            }
            if (root->parent != NULL) {
                root->seen = true;
                root = root->parent;
                i -= increment;
                continue;
            }
            if (root->parent == NULL) {
                // we have climbed all the way to the top
                printf("%*sreached top\n", i, "");
                break;
            }
        }
        if (root->right != NULL && !root->right->seen) {
            root = root->right;
            if (root->left) root->left->seen = false;
            if (root->right) root->right->seen = false;
            i += increment;
            printf("%*sR %d", i, "", root->key);
            printf("\n");
            left = true;
            continue;
        }
        if (root->left != NULL && !root->left->seen) {
            root = root->left;
            if (root->left) root->left->seen = false;
            if (root->right) root->right->seen = false;
            i += increment;
            printf("%*sL %d", i, "", root->key);
            printf("\n");
            continue;
        }
        if (root->parent != NULL) {
            root->seen = true;
            root = root->parent;
            i -= increment;
            continue;
        }
        if (root->parent == NULL) {
            printf("%*sreached top\n", i, "");
            break;
        }
    }
    printf("END\n");
}


int main(void)
{
    root = createRoot(1);
    print(root);
    for (int i = 148; i != 1; i--) insert(root, i);
    print(root);
	return 0;
}
