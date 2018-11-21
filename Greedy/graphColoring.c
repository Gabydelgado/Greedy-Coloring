#include "graphColoring.h"
//Variable global que posee el numero total de vertices del grafo.
u32 SIZE;
//Variable global dedicada a mantener las posiciones validas en la hash
u32 *hash_valid_position;

struct DataItem **hashArray;
//Estructura de un vertice
struct _vertex_t {
    u32 key;
    u32 color;
    u32 neighbor_count;
    tuple_t list_of_neighbors;
};
//Estructura de color utilizada en ReordenManteniendoBloqueColores
struct _color_t {
    u32 color;
    u32 color_count;
    tuple_t list_of_vertex;
};
//Estructura usada en listas enlazadas
struct _tuple_t {
    u32 key;
    tuple_t next;
};
//Estructura dedicada a la organizacion de vertices que hayan colisionado con otros vertices
//en una misma linea de hash
struct _node_t {
    u32 key;
    node_t next;
    tuple_t neightbors;
    u32 vertex_ubication;
    u32 neighbor_count;
    u32 color;
};

/*Estructura de la hash

Colisiones es utilizado cuando ocurre una colision y se debe almacenar un vertice
en una linea de hash donde ya existe un vertice(justamente con el cual se dio la colision).
Entonces de esta manera se evitan colisiones guardando el nuevo vertice en un campo
de la hash llamado colision, ese campo es un nodo y posee toda la informacion del vertice

Vecinos indica la cantidad de vecinos del vertice asociado a la hash
Cantidad de vecinos indica la cantidad de vecinos asociados al vertice guardado en la linea principal de la hash
*/

struct DataItem {
    node_t collisions;
    tuple_t neightbors;
    u32 neighbor_count;
    u32 key;
    u32 vertex_ubication;

    u32 color;
};

/*Estructura general del grafo
El array de etiquetas(array_of_labels) es aquel que mantiene una biyeccion con
el array general de vertices(array_of_vertex)*/

struct Graph {
    u32 vertex_count;
    u32 edges_count;
    u32 *array_of_labels;
    vertex_t array_of_vertex;
};

/*Estructura de una cola FIFO implementada con un arreglo*/

struct _queue_t {
    u32 *elems;
    u32 fst;
    u32 size;
};

void Hash_toArray(vertex_t array);

/*Funcion dedicada a la busqueda de un vertice en la hash*/
hash_st search(u32 key) {
    node_t aux;
    //Toma la linea de la hash asociada a una key
    u32 hashIndex = hashCode(key);
    //Busca dentro de la hash la key que le pasan como parametro
    //Si no encuentra la key en la linea principal, la busca como posible
    //colision
    if (hashArray[hashIndex] != NULL) {
        if (hashArray[hashIndex]->key == key) {
            return hashArray[hashIndex];
        } else {
            aux = hashArray[hashIndex]->collisions;
            while (aux != NULL) {
                if (aux->key == key) {
                    return hashArray[hashIndex];
                }
                aux = aux->next;
            }
        }
    }
    return NULL;
}

/* Crea una entrada a la tabla hash */
void insert_hashline(u32 keyA, tuple_t tuple, u32 color, u32 index) {
    //Toma la linea de la hash asociada a una key
    u32 hashIndex = hashCode(keyA);
    /*Se mueve dentro de la hash fuera de una entrada nula o una celda borrada
    Si encuentra ya un key en esa posicion agrega el nuevo vertice
    dentro de un nodo asociado a la hash dandole los atributos correspondientes.
    De esta manera si 2 vertices caen dentro de la misma linea de hash, uno quedara
    en el arreglo principal de la hash, y otro quedara asociado en una estructura colision.*/
    if (hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != 0) {
        node_t node = calloc(1, sizeof(struct _node_t));
        node->key = keyA;
        node->neightbors = tuple;
        node->color = color;
        node->neighbor_count = 1;
        node->vertex_ubication = index;
        node->next = hashArray[hashIndex]->collisions;
        hashArray[hashIndex]->collisions = node;
    } else {
        hash_st item = calloc(1, sizeof(struct DataItem));
        item->neightbors = tuple;
        item->key = keyA;
        item->color = color;
        item->neighbor_count = 1;
        item->vertex_ubication = index;
        hashArray[hashIndex] = item;
        hash_valid_position[hashIndex] = 1;
    }
}
/*Funcion dedicada a liberar recursos utilizados durante
la ejecucion del programa*/
int DestroyGraph(GraphColoring graph) {
    u32 ret = 65453;
    for (u32 i = 0; i < graph->vertex_count; i++) {
         graph->array_of_vertex[i].list_of_neighbors =
            destroy_tuple(graph->array_of_vertex[i].list_of_neighbors);
    }
    free(hashArray);
    free(hash_valid_position);
    free(graph->array_of_labels);
    free(graph->array_of_vertex);
    free(graph);
    if (graph == NULL) {
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}
/*Funcion principal encargada de la carga del grafo*/
GraphColoring LoadGraph() {
    u32 vertex_count = 0, edges_count = 0;
    GraphColoring graph = NULL;
    dataitem_t fst_search = NULL, snd_search = NULL;
    node_t aux = NULL;

    char line[80];
    while (((fgets(line, sizeof line, stdin) != NULL)
            && (isEmpty(line) == 0)) || line[0] != 'c') {
        // Se lee la linea que contiene aristas/vertices
        if (sscanf(line, "p edge %u %u\n", &vertex_count, &edges_count) == 2) {
            fflush(stdout);
            break;
        }
    }

    SIZE = vertex_count;
    //Alloca memoria para la hash, el arreglo de posiciones validas,
    //el arreglo fijo de vertices y el arreglo de etiquetas
    hashArray = calloc(SIZE, sizeof(hash_st));
    hash_valid_position = calloc(SIZE, sizeof(u32));
    vertex_t array_of_vertex = calloc(SIZE, sizeof(struct _vertex_t));
    u32 *array_of_labels = calloc(SIZE, sizeof(u32));

    u32 color = 1;
    u32 index = 0;
    //Lectura de cada arista
    for (u32 i = 0; i <= edges_count; i++) {
        u32 left = 0, right = 0;
        if ((fgets(line, sizeof line, stdin) != NULL)
            && (isEmpty(line) == 0)) {
            if (sscanf(line, "e %u %u\n", &left, &right) == 2) {
                fflush(stdout);
            }
            //Se indexa en la hash ambos vertices asociados a una arista
            fst_search = search(left);
            snd_search = search(right);
            //Si el vertice existe en la hash, solo se agrega al correspondiente vecino
            if (fst_search != NULL) {
                if (fst_search->key != left) {
                    aux = fst_search->collisions;
                    //Recorre hasta llegar a la ultima posicion de la lista de vertices colisionados
                    //para poder agregar al nuevo vertice.
                    while (aux != NULL && aux->key != left) {
                        aux = aux->next;
                    }
                    insert_tuple(right, aux->neightbors);
                    aux->neighbor_count += 1;
                } else {
                    insert_tuple(right, fst_search->neightbors);
                    hashArray[left % SIZE]->neighbor_count += 1;
                }
            } else {
                insert_hashline(left, insert_tuple(right, NULL), color, index);
                array_of_vertex[index].key = left;
                array_of_vertex[index].color = color;
                if (hashArray[left % SIZE] != NULL) {
                    color++;
                    index++;
                }
            }
            //Se repite el procedimiento anterior pero esta ves para el segundo vertice
            //asociado a la ariasta si el vertice existe en la hash,
            //solo se agrega al correspondiente vecino
            if (snd_search != NULL) {
                if (snd_search->key != right) {
                    aux = snd_search->collisions;
                    //Recorre hasta llegar a la ultima posicion de la lista de vertices colisionados
                    //para poder agregar al nuevo vertice.
                    while (aux != NULL && aux->key != right) {
                        aux = aux->next;
                    }
                    insert_tuple(left, aux->neightbors);
                    aux->neighbor_count += 1;
                } else {
                    insert_tuple(left, snd_search->neightbors);
                    hashArray[right % SIZE]->neighbor_count += 1;
                }
            } else {
                insert_hashline(right, insert_tuple(left, NULL), color, index);
                array_of_vertex[index].key = right;
                array_of_vertex[index].color = color;
                if (hashArray[right % SIZE] != NULL) {
                    color++;
                    index++;
                }
            }
        }
    }

    //Se transfieren los datos de la hash, hacia un arreglo de estructuras
    Hash_toArray(array_of_vertex);

    //Se generan las etiquetas y ademas se destruye la hash.
    for (u32 i = 0; i < SIZE; i++) {
        if (hash_valid_position[i] == 1) {
            hashArray[i]->neightbors = destroy_tuple(hashArray[i]->neightbors);
            node_t nodo_colision =  hashArray[i]->collisions;
            while(nodo_colision != NULL){
                nodo_colision->neightbors = destroy_tuple(nodo_colision->neightbors);
                nodo_colision = nodo_colision->next;
            }
            hashArray[i]->collisions = destroy_node(hashArray[i]->collisions);
        }
        free(hashArray[i]);
        array_of_labels[i] = i;
    }

    graph = graph_empty();
    graph->array_of_vertex = array_of_vertex;
    graph->array_of_labels = array_of_labels;
    graph->vertex_count = vertex_count;
    graph->edges_count = edges_count;
    return (graph);
}

u32 hashCode(u32 key) {
    return key % SIZE;
}

void Hash_toArray(vertex_t array_of_vertex) {
    u32 valid_poss = 0, index = 0, neighbor = 0;
    node_t search_collision = NULL;
    hash_st ubication_neighbor;
    bool cut_while = true;

    while (valid_poss < SIZE) {
        //Chequea si la posicion eligida es valida en la hash
        if (hash_valid_position[valid_poss] == 1) {
            //Se toma un index el cual se utilizara como indice del arreglo de vertices
            //a fin de obtener en una primera instancia el primer vecino del vertice asociado a index
            //Ademas se crea un puntero el cual sera utilizado para recorrer los vecinos
            //de dicho vertice
            index = hashArray[valid_poss]->vertex_ubication;
            tuple_t neightbors_of_key = hashArray[valid_poss]->neightbors;
            //Se recorren todos los vecinos del vertice
            while (neightbors_of_key != NULL) {
                cut_while = true;
                neighbor = neightbors_of_key->key;
                ubication_neighbor = search(neighbor);
                //Si el vecino es igual a la key que esta en la linea de hash, entonces
                //lo guardo en la lista de vecinos de la key.
                if (ubication_neighbor->key == neighbor) {
                    array_of_vertex[index].list_of_neighbors =
                        insert_tuple(ubication_neighbor->vertex_ubication,
                                     array_of_vertex[index].list_of_neighbors);
                //En caso contrario ese vecino se encuntra en la lista de colisiones asociada a la misma
                //linea de hash
                } else {
                    search_collision = ubication_neighbor->collisions;
                    while (search_collision != NULL && cut_while) {
                        if (search_collision->key == neighbor) {
                            array_of_vertex[index].list_of_neighbors =
                                insert_tuple(search_collision->
                                             vertex_ubication,
                                             array_of_vertex[index].
                                             list_of_neighbors);
                            cut_while = false;
                        }
                        search_collision = search_collision->next;
                    }
                }
                neightbors_of_key = neightbors_of_key->next;
            //hasta este punto se busco y se agrego en el array un vecino
            }
            array_of_vertex[index].neighbor_count =
                hashArray[valid_poss]->neighbor_count;
            //Si existen colisiones en la misma linea repite el procedimiento descripto
            //a partir de la linea 288-316
            if (hashArray[valid_poss]->collisions != NULL) {
                node_t collision = hashArray[valid_poss]->collisions;
                while (collision != NULL) {
                    index = collision->vertex_ubication;
                    tuple_t neightbors_collision = collision->neightbors;
                    while (neightbors_collision != NULL) {
                        cut_while = true;
                        neighbor = neightbors_collision->key;
                        ubication_neighbor = search(neighbor);
                        if (ubication_neighbor->key == neighbor) {
                            array_of_vertex[index].list_of_neighbors =
                                insert_tuple(ubication_neighbor->
                                             vertex_ubication,
                                             array_of_vertex[index].
                                             list_of_neighbors);
                        } else {
                            search_collision = ubication_neighbor->collisions;
                            while (search_collision != NULL && cut_while) {
                                if (search_collision->key == neighbor) {
                                    array_of_vertex[index].list_of_neighbors =
                                        insert_tuple(search_collision->
                                                     vertex_ubication,
                                                     array_of_vertex[index].
                                                     list_of_neighbors);
                                    cut_while = false;
                                }
                                search_collision = search_collision->next;
                            }
                        }
                        neightbors_collision = neightbors_collision->next;
                    }
                    array_of_vertex[index].neighbor_count =
                        collision->neighbor_count;
                    collision = collision->next;
                }
            }
        }
        valid_poss++;
    }
}

GraphColoring graph_empty() {
    //Se inicializan los campos de la estructura asociada al grafo.
    GraphColoring graph = calloc(1, sizeof(struct Graph));
    assert(graph != NULL);
    graph->vertex_count = 0;
    graph->edges_count = 0;
    graph->array_of_labels = NULL;
    graph->array_of_vertex = NULL;
    return (graph);
}

u32 Greedy(GraphColoring graph) {
    tuple_t neightbors = NULL;
    u32 max_color = 0, j = 0, neighbor = 0, color = 0;
    u32 pos = 0;
    bool *array_of_color = calloc((graph->vertex_count + 1), sizeof(bool));
    //Se inicializa el array de colores que al marcarlos como falso no van a poder
    //utilizarse en la busqueda de color de un vertice
    //Ademas se descolorea el grafo
    for (u32 i = 0; i < graph->vertex_count; i++) {
        array_of_color[i] = true;
        graph->array_of_vertex[i].color = 0;
    }
    array_of_color[graph->vertex_count] = true;
    //El color cero no puede ser usado, se setea a falso el primer color usado
    //El primer vertice ya posee color
    array_of_color[0] = false;
    graph->array_of_vertex[graph->array_of_labels[0]].color = 1;
    max_color = 0;
    //Se comienza a recorrer a partir del segundo vertice,
    //buscando y recordando los colores de cada vecino de dicho vertice
    //con la finalidad de encontrar un color disponible para asignarle
    for (u32 i = 1; i < graph->vertex_count; i++) {
        pos = graph->array_of_labels[i];
        neightbors = graph->array_of_vertex[pos].list_of_neighbors;
        //Se busca el color de cada vecino y se lo setea a falso en el arreglo de colores
        while (neightbors != NULL) {
            neighbor = neightbors->key;
            color = graph->array_of_vertex[neighbor].color;
            array_of_color[color] = false;
            neightbors = neightbors->next;
        }
        //Se busca el primer color no usado, es decir seteado a "true"
        //y se lo asigna al vertice
        //Ademas se va guardando el valor de cada color usado,
        //quedandose con el mayor, de manera de que al terminar la ejecucion
        //de greedy se posea un resultado
        j = 1;
        while (j < graph->vertex_count + 1) {
            if (array_of_color[j] == true) {
                graph->array_of_vertex[pos].color = j;
                if (max_color < j) {
                    max_color = j;
                }
                j = graph->vertex_count + 1;
            }
            j++;
        }
        //Se setean todos los colores a true (salvo el cero) para poder usarlos nuevamente
        //en un nuevo vertice.
        j = 1;
        while (j < max_color + 1) {
            array_of_color[j] = true;
            j++;
        }
    }
    //se liberan recursos utilizados durante la ejecucion
    free(array_of_color);
    return (max_color);
}

u32 ColorDelVertice(GraphColoring graph, u32 label) {
    return (graph->array_of_vertex[label].color);
}

u32 GradoDelVertice(GraphColoring graph, u32 label) {
    return (graph->array_of_vertex[label].neighbor_count);
}

u32 NumeroVerticesDeColor(GraphColoring graph, u32 color) {
    u32 count = 0;
    for (u32 i = 0; i < SIZE; i++) {
        if (graph->array_of_vertex[i].color == color) {
            count++;
        }
    }
    return (count);
}
u32 NumeroDeColores(GraphColoring graph) {
    u32 max_color = 0;
    for (u32 i = 0; i < SIZE; i++) {
        if (max_color < graph->array_of_vertex[i].color) {
            max_color = graph->array_of_vertex[i].color;
        }
    }
    return (max_color);
}

void OrdenNatural(GraphColoring graph) {
    u32 *array = calloc(graph->vertex_count, sizeof(u32));
    for (u32 i = 0; i < graph->vertex_count; i++) {
        graph->array_of_labels[i] = i;
        array[i] = graph->array_of_vertex[i].key;
    }
    shellsort(array, graph->array_of_labels, graph->vertex_count, false);
    free(array);
}

void OrdenWelshPowell(GraphColoring graph) {

    u32 *array = calloc(graph->vertex_count, sizeof(unsigned int));
    for (u32 i = 0; i < graph->vertex_count; i++) {
        graph->array_of_labels[i] = i;
        array[i] = graph->array_of_vertex[i].neighbor_count;
    }
    shellsort(array, graph->array_of_labels, graph->vertex_count, true);
    free(array);
}

void AleatorizarVertices(GraphColoring graph, u32 x) {
    srand(x);
    u32 random_fst = 0, random_snd = 0;
    for (u32 i = 0; i < SIZE; i++) {
        graph->array_of_labels[i] = i;
    }
    for (u32 i = 0; i < graph->vertex_count; i++) {
        random_fst = rand() % graph->vertex_count;
        random_snd = rand() % graph->vertex_count;
        swap(graph->array_of_labels, random_fst, random_snd);
    }
}

void ReordenManteniendoBloqueColores(GraphColoring graph, u32 x) {
    //Creo un arreglo de estructura color_t, donde guardo todos los colores que tiene el grafo
    color_t array_bloq_color = calloc(SIZE + 1, sizeof(struct _color_t));
    u32 *array_result = calloc(SIZE, sizeof(u32));
    u32 maximo = 0, position = 0;
    tuple_t tupla = NULL;
    for (u32 i = 0; i < graph->vertex_count; i++) {
        //Recorro todo el el grafo y los voy guardando respecto a su color
        array_bloq_color[graph->array_of_vertex[i].color].color =
            graph->array_of_vertex[i].color;
        array_bloq_color[graph->array_of_vertex[i].color].color_count += 1;
        array_bloq_color[graph->array_of_vertex[i].color].list_of_vertex =
            insert_tuple(i,
                         array_bloq_color[graph->array_of_vertex[i].color].
                         list_of_vertex);
        //Recuerdo el color mas grande
        if (maximo < graph->array_of_vertex[i].color) {
            maximo = graph->array_of_vertex[i].color;
        }
        //Inicializo el arreglo resultado en 0
        array_result[i] = 0;
    }

    if (x == 0) {
        tupla = array_bloq_color[maximo].list_of_vertex;
        //Coloco las key del color r, en las primeras posiciones
        while (tupla != NULL) {
            array_result[position] = tupla->key;
            position++;
            tupla = tupla->next;
        }
        //Completo el arreglo result con las demas key
        for (u32 i = 1; i < maximo; i++) {
            tupla = array_bloq_color[i].list_of_vertex;
            while (tupla != NULL) {
                array_result[position] = tupla->key;
                position++;
                tupla = tupla->next;
            }
            array_bloq_color[i].list_of_vertex = destroy_tuple(array_bloq_color[i].list_of_vertex);
        }
    }
    if (x == 1) {
        //Rellenoi el arreglo de forma r, r-1,...,1
        for (u32 i = maximo; i > 0; i--) {
            tupla = array_bloq_color[i].list_of_vertex;
            while (tupla != NULL) {
                array_result[position] = tupla->key;
                position++;
                tupla = tupla->next;
            }
        array_bloq_color[i].list_of_vertex = destroy_tuple(array_bloq_color[i].list_of_vertex);
        }
    }
    if (x == 2) {
        //Creo un arreglo donde guardo la cantidad de vertices que tiene cada color
        u32 *array_of_color_count = calloc(maximo, sizeof(u32));
        //Este arreglo guarda la posicion del color dentro del arreglo de bloques de colores
        u32 *array_of_index = calloc(maximo, sizeof(u32));
        for (u32 i = 1; i <= maximo; i++) {
            array_of_color_count[i - 1] = array_bloq_color[i].color_count;
            array_of_index[i - 1] = i;
        }
        //Ordeno el array_of_index con respecto al orden ascendente del arreglo array_of_color_count
        shellsort(array_of_color_count, array_of_index, maximo, false);
        //Completo el arreglo result
        for (u32 i = 0; i < maximo; i++) {
            tupla = array_bloq_color[array_of_index[i]].list_of_vertex;
            while (tupla != NULL) {
                array_result[position] = tupla->key;
                position++;
                tupla = tupla->next;
            }
            array_bloq_color[array_of_index[i]].list_of_vertex =
                destroy_tuple(array_bloq_color[array_of_index[i]].list_of_vertex);
        }
        free(array_of_color_count);
        free(array_of_index);
    }
    if (x == 3) {
        //Creo un arreglo donde guardo la cantidad de vertices que tiene cada color
        u32 *array_of_color_count = calloc(maximo, sizeof(u32));
        //Este arreglo guarda la posicion del color dentro del arreglo de bloques de colores
        u32 *array_of_index = calloc(maximo, sizeof(u32));
        for (u32 i = 1; i <= maximo; i++) {
            array_of_color_count[i - 1] = array_bloq_color[i].color_count;
            array_of_index[i - 1] = i;
        }
        //Ordeno el array_of_index con respecto al orden descendente del arreglo array_of_color_count
        shellsort(array_of_color_count, array_of_index, maximo, true);
        //Completo el arreglo result
        for (u32 i = 0; i < maximo; i++) {
            tupla = array_bloq_color[array_of_index[i]].list_of_vertex;
            while (tupla != NULL) {
                array_result[position] = tupla->key;
                position++;
                tupla = tupla->next;
            }
            array_bloq_color[array_of_index[i]].list_of_vertex =
                destroy_tuple(array_bloq_color[array_of_index[i]].list_of_vertex);
        }
        free(array_of_color_count);
        free(array_of_index);
    }
    if (x > 3) {
        //Aca vamos a usar la misma idea que usamos en la funcion AleatorizarVertices
        u32 *array_index_random = calloc(maximo, sizeof(u32));
        u32 rand_fst = 0, rand_snd = 0;
        srand(x);
        for (u32 i = 0; i < maximo; i++) {
            array_index_random[i] = i + 1;
        }
        for (u32 i = 0; i < maximo; i++) {
            rand_fst = rand() % maximo;
            rand_snd = rand() % maximo;
            swap(array_index_random, rand_fst, rand_snd);
        }
        for (u32 i = 0; i < maximo; i++) {
            tupla = array_bloq_color[array_index_random[i]].list_of_vertex;
            while (tupla != NULL) {
                array_result[position] = tupla->key;
                position++;
                tupla = tupla->next;
            }
            array_bloq_color[array_index_random[i]].list_of_vertex =
                destroy_tuple(array_bloq_color[array_index_random[i]].list_of_vertex);
        }
        free(array_index_random);
    }

    for (u32 i = 0; i < graph->vertex_count; i++) {
        graph->array_of_labels[i] = array_result[i];
        if(array_bloq_color[i].list_of_vertex != NULL) {
            array_bloq_color[i].list_of_vertex = destroy_tuple(array_bloq_color[i].list_of_vertex);
        }
    }
    array_bloq_color[SIZE].list_of_vertex = destroy_tuple(array_bloq_color[SIZE].list_of_vertex);
    free(array_bloq_color);
    free(array_result);
}

u32 IesimoVerticeEnElOrden(GraphColoring graph, u32 iesimo) {
    return (graph->array_of_labels[iesimo]);
}

u32 NumeroDeVertices(GraphColoring graph) {
    return graph->vertex_count;
}

u32 NumeroDeLados(GraphColoring graph) {
    return graph->edges_count;
}

u32 NombreDelVertice(GraphColoring graph, u32 label) {
    return graph->array_of_vertex[label].key;
}

u32 IesimoVecino(GraphColoring graph, u32 x, u32 i) {
    tuple_t tuple = graph->array_of_vertex[x].list_of_neighbors;
    for (u32 j = 0; j < i; i++) {
        tuple = tuple->next;
    }
    return tuple->key;
}

u32 isEmpty(const char *line) {
    //Chequea si el string consiste solo de espacios
    while (*line != '\0') {
        if (isspace(*line) == 0)
            return 0;
        line++;
    }
    return 1;
}

// BFS Y COLA

queue_t empty(queue_t queue) {
    queue_t aux = calloc(1, sizeof(struct _queue_t));
    aux->elems = calloc(SIZE, sizeof(u32));
    aux->fst = 0;
    aux->size = 0;
    queue = aux;
    return queue;
}

queue_t enqueue(queue_t queue, u32 elem) {
    queue->elems[(queue->fst + queue->size) % SIZE] = elem;
    queue->size += 1;
    return queue;
}

u32 first(queue_t queue) {
    return (queue->elems[queue->fst]);
}

queue_t dequeue(queue_t queue) {
    queue->size -= 1;
    queue->fst = (queue->fst + 1) % SIZE;
    return queue;
}

bool is_emptyq(queue_t queue) {
    bool b = false;
    b = (queue->size == 0);
    return b;
}

queue_t destroyq(queue_t queue) {
    free(queue->elems);
    free(queue);
    return queue;
}

u32 Bipartito(GraphColoring graph) {
    queue_t q = NULL;
    u32 p = 0, j = 0;
    tuple_t neighbor = NULL;
    //Se descolorea el grafo
    for (u32 i = 0; i < graph->vertex_count; i++) {
        graph->array_of_vertex[i].color = 0;
    }
    while (j < graph->vertex_count) {
        if (graph->array_of_vertex[j].color == 0) {
            graph->array_of_vertex[j].color = 1;
            q = empty(q);
            q = enqueue(q, j);
            while (!is_emptyq(q)) {
                p = first(q);
                q = dequeue(q);
                neighbor = graph->array_of_vertex[p].list_of_neighbors;
                for (u32 i = 0; i < graph->array_of_vertex[p].neighbor_count;
                     i++) {
                    if (graph->array_of_vertex[neighbor->key].color == 0) {
                        q = enqueue(q, neighbor->key);
                        graph->array_of_vertex[neighbor->key].color =
                            3 - graph->array_of_vertex[p].color;
                    }
                    neighbor = neighbor->next;
                }
            }
            q = destroyq(q);
        }
        j++;
    }
    for (u32 i = 0; i < graph->vertex_count; i++) {
        neighbor = graph->array_of_vertex[i].list_of_neighbors;
        for (u32 j = 0; j < graph->array_of_vertex[i].neighbor_count; j++) {
            if (neighbor != NULL
                && graph->array_of_vertex[i].color ==
                graph->array_of_vertex[neighbor->key].color) {
                return 0;
            }
            neighbor = neighbor->next;
        }
    }
    return 1;
}
