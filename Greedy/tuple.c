#include "graphColoring.h"

struct _tuple_t {
    unsigned int key;
    tuple_t next;
};

struct _node_t {
    u32 key;
    node_t next;
    tuple_t neightbors;
    u32 vertex_ubication;
    u32 neighbor_count;
    u32 color;
};

/* Se crean nodos usados luego en listas enlazadas */
tuple_t insert_tuple(u32 key, tuple_t tuple) {
    tuple_t tuple_aux;
    tuple_aux = calloc(1, sizeof(struct _tuple_t));
    tuple_aux->key = key;
    tuple_aux->next = NULL;
    if (tuple == NULL) {
        tuple = tuple_aux;
    } else if (tuple->next == NULL) {
        tuple->next = tuple_aux;
    } else {
        tuple_aux->next = tuple->next;
        tuple->next = tuple_aux;
    }
    return (tuple);
}

tuple_t destroy_tuple(tuple_t tuple) {
    tuple_t tuple_aux = tuple;
    while (tuple != NULL) {
        tuple_aux = tuple->next;
        tuple->next = NULL;
        free(tuple);
        tuple = tuple_aux;
    }
    return tuple;
}

node_t destroy_node(node_t node) {
    node_t node_aux = node;
    while (node != NULL) {
        node_aux = node->next;
        node->next = NULL;
        free(node);
        node = node_aux;
    }
    return node;
}
