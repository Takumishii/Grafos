#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _NodoG;

struct _NodoL {
  struct _NodoG *vertice;
  struct _NodoL *siguiente;
};

struct _NodoG {
  int id;
  int coloracion;
  char colorsito[20];
  struct _NodoL *adyacentes;
  struct _NodoG *siguiente;
};

struct _NodoG *agregarNodo(struct _NodoG **grafo, int *id_counter) {
  struct _NodoG *nuevoNodo = (struct _NodoG *)malloc(sizeof(struct _NodoG));
  nuevoNodo->id = (*id_counter)++;
  nuevoNodo->coloracion = -1;
  strcpy(nuevoNodo->colorsito, "");
  nuevoNodo->adyacentes = NULL;
  nuevoNodo->siguiente = NULL;

  if (*grafo == NULL) {
    *grafo = nuevoNodo;
  } else {
    struct _NodoG *actual = *grafo;
    while (actual->siguiente != NULL) {
      actual = actual->siguiente;
    }
    actual->siguiente = nuevoNodo;
  }

  return nuevoNodo;
}

bool aristaExiste(struct _NodoL *adyacentes, struct _NodoG *vertice) {

  struct _NodoL *actual = adyacentes;
  while (actual != NULL) {

    if (actual->vertice == vertice) {
      return true;
    }
    actual = actual->siguiente;
  }
  return false;
}

void agregarArista(struct _NodoG *nodo1, struct _NodoG *nodo2) {
  if (aristaExiste(nodo1->adyacentes, nodo2) || nodo1 == nodo2) {
    return;
  }

  struct _NodoL *nuevoNodo1 = (struct _NodoL *)malloc(sizeof(struct _NodoL));
  nuevoNodo1->vertice = nodo2;
  nuevoNodo1->siguiente = nodo1->adyacentes;
  nodo1->adyacentes = nuevoNodo1;

  struct _NodoL *nuevoNodo2 = (struct _NodoL *)malloc(sizeof(struct _NodoL));
  nuevoNodo2->vertice = nodo1;
  nuevoNodo2->siguiente = nodo2->adyacentes;
  nodo2->adyacentes = nuevoNodo2;
}

void eliminarArista(struct _NodoL **adyacentes, struct _NodoG *vertice) {
  struct _NodoL *actual = *adyacentes;
  struct _NodoL *anterior = NULL;
  while (actual != NULL && actual->vertice != vertice) {
    anterior = actual;
    actual = actual->siguiente;
  }
  if (actual != NULL) {
    if (anterior != NULL) {
      anterior->siguiente = actual->siguiente;
    } else {
      *adyacentes = actual->siguiente;
    }
    free(actual);
  }
}

void eliminarNodo(struct _NodoG **grafo, struct _NodoG *nodo) {
  struct _NodoG *actual = *grafo;
  struct _NodoG *anterior = NULL;

  while (actual != NULL && actual != nodo) {
    anterior = actual;
    actual = actual->siguiente;
  }

  if (actual == NULL) {
    printf("Vértice no válido.\n");
    return;
  }

  struct _NodoL *adyacente = actual->adyacentes;
  while (adyacente != NULL) {
    eliminarArista(&(adyacente->vertice->adyacentes), actual);
    struct _NodoL *temp = adyacente;
    adyacente = adyacente->siguiente;
    free(temp);
  }

  if (anterior != NULL) {
    anterior->siguiente = actual->siguiente;
  } else {
    *grafo = actual->siguiente;
  }

  free(actual);
}

bool colorearConBacktracking(struct _NodoG *grafo, struct _NodoG *nodo, char colores[][20], int num_colores) {
  if (nodo == NULL) {
    return true;
  }
  for (int color = 1; color <= num_colores; color++) {

    bool puede_colorear = true;
    struct _NodoL *actual = nodo->adyacentes;
    while (actual != NULL) {

      if (actual->vertice->coloracion == color) {

        puede_colorear = false;
        break;
      }

      actual = actual->siguiente;
    }

    if (puede_colorear) {

        nodo->coloracion = color;
        strcpy(nodo->colorsito, colores[color]);

        if (colorearConBacktracking(grafo, nodo->siguiente, colores, num_colores)) {
            return true;
      }
        nodo->coloracion = -1;
    }
  }
  return false;
}

void mostrar(struct _NodoG *grafo) {
  struct _NodoG *nodo = grafo;
  while (nodo != NULL) {
    printf("Vértice %d: Color %d (%s)\n", nodo->id, nodo->coloracion,
           nodo->colorsito);
    printf("  Conectado a: ");
    struct _NodoL *actual = nodo->adyacentes;
    while (actual != NULL) {
      printf("%d (%s) ", actual->vertice->id, actual->vertice->colorsito);
      actual = actual->siguiente;
    }
    printf("\n");
    nodo = nodo->siguiente;
  }
}

void liberarMemoria(struct _NodoG *grafo) {
  while (grafo != NULL) {
    struct _NodoG *tempG = grafo;
    struct _NodoL *actualL = grafo->adyacentes;
    while (actualL != NULL) {
      struct _NodoL *tempL = actualL;
      actualL = actualL->siguiente;
      free(tempL);
    }
    grafo = grafo->siguiente;
    free(tempG);
  }
}

struct _NodoG *buscarNodo(struct _NodoG *grafo, int id) {
  struct _NodoG *actual = grafo;
  while (actual != NULL) {

    if (actual->id == id) {

      return actual;
    }

    actual = actual->siguiente;
  }

  return NULL;
}

void agregarNuevoNodo(struct _NodoG **grafo, int *id_counter,
                      char colores[][20], int num_colores) {
  int id_padre;
  printf("Ingrese el número del vértice padre para el nuevo nodo: ");
  scanf("%d", &id_padre);

  struct _NodoG *padre = buscarNodo(*grafo, id_padre);
  if (padre != NULL) {
    struct _NodoG *nuevoNodo = agregarNodo(grafo, id_counter);
    agregarArista(padre, nuevoNodo);

    struct _NodoG *temp = *grafo;
    while (temp != NULL) {
      temp->coloracion = -1;
      strcpy(temp->colorsito, "");
      temp = temp->siguiente;
    }
    if (!colorearConBacktracking(*grafo, *grafo, colores, num_colores)) {
      printf("No se pudo colorear el grafo con los colores disponibles.\n");
    }
  } else {
    printf("Vértice padre no válido.\n");
  }
}

int main() {
  struct _NodoG *grafo = NULL;
  char colores[20][20] = {"", "azul", "rojo", "verde"};
  int num_colores = 3;
  int id_counter = 0;

  struct _NodoG *nodo1 = agregarNodo(&grafo, &id_counter);
  struct _NodoG *nodo2 = agregarNodo(&grafo, &id_counter);
  struct _NodoG *nodo3 = agregarNodo(&grafo, &id_counter);
  struct _NodoG *nodo4 = agregarNodo(&grafo, &id_counter);
  struct _NodoG *nodo5 = agregarNodo(&grafo, &id_counter);
  struct _NodoG *nodo6 = agregarNodo(&grafo, &id_counter);
  struct _NodoG *nodo7 = agregarNodo(&grafo, &id_counter);

  agregarArista(nodo1, nodo2);
  agregarArista(nodo1, nodo3);
  agregarArista(nodo2, nodo4);
  agregarArista(nodo2, nodo5);
  agregarArista(nodo3, nodo6);
  agregarArista(nodo3, nodo7);
  agregarArista(nodo4, nodo5);
  agregarArista(nodo5, nodo7);
  agregarArista(nodo7, nodo6);
  agregarArista(nodo6, nodo4);

  if (!colorearConBacktracking(grafo, grafo, colores, num_colores)) {
    printf("No se pudo colorear la figura con los colores disponibles.\n");
  }

  printf("Figura con estructura de pentágono:\n");
  mostrar(grafo);

  int opcion;
  do {
    printf("\n1: Agregar un nuevo nodo\n2: Buscar un nodo\n3: Eliminar un "
           "nodo\n0: Salir\nSeleccione una opción: ");
    scanf("%d", &opcion);

    if (opcion == 1) {
      agregarNuevoNodo(&grafo, &id_counter, colores, num_colores);
      printf("\nFigura nueva después de agregar el vértice:\n");
      mostrar(grafo);
    } else if (opcion == 2) {
      int id;
      printf("Ingrese el número del vértice a buscar: ");
      scanf("%d", &id);
      struct _NodoG *nodo = buscarNodo(grafo, id);
      if (nodo) {
        printf("Vértice %d encontrado: Color %d (%s)\n", nodo->id,
               nodo->coloracion, nodo->colorsito);
      } else {
        printf("Vértice no encontrado.\n");
      }
    } else if (opcion == 3) {
      int id;
      printf("Ingrese el número del vértice a eliminar: ");
      scanf("%d", &id);
      struct _NodoG *nodo = buscarNodo(grafo, id);
      if (nodo) {
        eliminarNodo(&grafo, nodo);
        printf("\nFigura después de eliminar el vértice:\n");
        mostrar(grafo);
      } else {
        printf("Vértice no encontrado.\n");
      }
    }
  } while (opcion != 0);

  liberarMemoria(grafo);
  return 0;
}

