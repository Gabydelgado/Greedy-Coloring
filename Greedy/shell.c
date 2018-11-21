#include <stdio.h>
#include <stdbool.h>
#include "graphColoring.h"

/*Funcion dedicada a ordenar los valores
de un arreglo respecto a otro*/
void shellsort(u32 a[], u32 b[], int num, bool flag) {

    int i, j, k;
    for (i = num / 2; i > 0; i = i / 2) {
        for (j = i; j < num; j++) {
            for (k = j - i; k >= 0; k = k - i) {

                if (!flag) {
                    if (a[k + i] >= a[k]) {
                        break;
                    } else {
                        swap(a, k, k + i);
                        swap(b, k, k + i);
                    }
                } else {
                    if (a[k + i] <= a[k]) {
                        break;
                    } else {
                        swap(a, k, k + i);
                        swap(b, k, k + i);
                    }
                }
            }
        }
    }
}

void swap(u32 a[], u32 i, u32 j) {

    u32 aux;
    aux = a[i];
    a[i] = a[j];
    a[j] = aux;
}
