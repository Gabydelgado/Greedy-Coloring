//Barros Daniel Nicolas, email: nikocho_29@live.com.ar
//Delgado Sergio Gabriel, email: gaby.delgado11@gmail.com
//Mortarino Martinez Marcio, email: mmortarinom@hotmail.com
#ifndef _GRAPH_H
#define _GRAPH_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

typedef unsigned int u32;
typedef struct Graph *GraphColoring;
typedef struct DataItem *hash_st;
typedef struct DataItem *dataitem_t;
typedef struct _tuple_t *tuple_t;
typedef struct _node_t *node_t;
typedef struct _vertex_t *vertex_t;
typedef struct _color_t *color_t;
typedef struct _queue_t *queue_t;

//Funciones usadas en la busqueda de vertices dentro de la hash
struct DataItem *search(u32 key);
u32 hashCode(u32 key);
//Se adquieren recursos para generar listas enlazadas
tuple_t insert_tuple(u32 key, tuple_t tuple);
//Se liberan los recursos usados por las listas enlazadas
tuple_t destroy_tuple(tuple_t tuple);
node_t destroy_node(node_t node);


GraphColoring graph_empty();
/*
 * Retorna una nueva estructura grafo.
 *
 * POST: El resultado no es nulo.
 */

int DestroyGraph(GraphColoring graph);
/*
 * Libera los recursos alocados por el grafo.
 *
 * PRE: El grafo no es nulo
 *
 * POST: el resultado es nulo.
 */
GraphColoring LoadGraph();
/*Funciones de coloreo*/
u32 Greedy(GraphColoring graph);
u32 Bipartito(GraphColoring graph);

/* Prototipo de funciones de ordenacion */
void ReordenManteniendoBloqueColores(GraphColoring graph, u32 x);
void AleatorizarVertices(GraphColoring graph, u32 x);
void OrdenWelshPowell(GraphColoring graph);
void OrdenNatural(GraphColoring graph);
void shellsort(u32 a[], u32 b[], int num, bool flag);
void swap(u32 a[], u32 i, u32 j);

/*Informacion del grafo*/
u32 NumeroDeColores(GraphColoring graph);
u32 NumeroDeLados(GraphColoring graph);
u32 ColorDelVertice(GraphColoring graph, u32 label);
u32 GradoDelVertice(GraphColoring graph, u32 vertex);
u32 IesimoVerticeEnElOrden(GraphColoring graph, u32 iesimo);
u32 NumeroVerticesDeColor(GraphColoring graph, u32 color);
u32 NumeroDeVertices(GraphColoring graph);
u32 NombreDelVertice(GraphColoring graph, u32 x);
u32 IesimoVecino(GraphColoring graph, u32 x, u32 i);

/* auxiliary functions */
u32 isEmpty(const char *line);

/*BFS */
queue_t empty(queue_t queue);
bool is_emptyq(queue_t queue);
queue_t enqueue(queue_t queue, unsigned int elem);
unsigned int first(queue_t queue);
queue_t dequeue(queue_t queue);
queue_t destroyq(queue_t queue);

#endif
