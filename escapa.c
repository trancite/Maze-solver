#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#define DIMENSIO 4
//DAVID ESTAÚN: 1672876

/*En este programa se han realizado las funciones "borratxo", "backtracking" y BFS. Todas estan formadas por una función
  principal, una función opcions_moviment y demás funciones necesarias. Para leer el laberinto, se ha ha creado una matriz malloc.
  Siento si son demasiadas líneas.

*/

typedef struct Nodo {
    int x;
    int y;
    struct Nodo* siguiente;
} Nodo;

typedef struct {
    Nodo* frente;
    Nodo* final;
} Cola;

void imprimir_back(int *, int); //IMPRIME EL CAMINO CON EL BACKTRACKING
void imprimir_bor(int*, int); //IMPRIME EL CAMINO CON EL BORRATXO
int BFS(int**, int, int);
int* detecta_salida(int**, int, int);
void inicializarCola(Cola* cola);
int estaVacia(Cola* cola);
void insertarEnCola(Cola* cola, int, int);
void eliminarDeCola(Cola* cola);
int opcions_moviment_BFS(double[], int**, int, int);
int opcions_moviment(double[], int**, int, int, int *, int);
int BACKTRACKING(int**, int, int, int, int*, int);
int BORRACHO(int**, int, int, int);
void VerLaberinto(int**, int, int);
int opcions_moviment_backtracking(double[], int**, int, int, int*, int);


double direccions[DIMENSIO] = {0, 0, 0, 0};
int pos_x, pos_y;
int LX = 0, LY = 0;

char N;

int main(int argc, char * argv[]) {
    if (argc < 3 ||  argc > 4)
        {
        fprintf(stderr, "Introdueix un número correcte de arguments");
        return 1;
        }


//CREACIÓN DE PARÁMETROS INICIALES

    char *LABERINTO;

    FILE* LAB;
    LAB = fopen(argv[2], "r");

    if (LAB == NULL) {
        fprintf(stderr, "\nError al abrir el archivo\n\n");
        return 1;
    }

    while ((N = fgetc(LAB)) != EOF) { //BUSCAMOS EL VALOR DE LAS COLUMNAS Y FILAS DE LA MATRIZ
        if (LY == 0) {
            LX++;
        }
        if (N == '\n') {
            LY++;
        }
    }
    rewind(LAB);
    LX--; // RESTAMOS 1 A LX PORQUE EL ÚLTIMO EL SALTO DE FILA ES UN CARÁCTER

  //CREAMOS EL CHAR DEL LABERINTO PARA HACER LA MATRIZ
    LABERINTO = (char*)malloc((LX * LY + 1) * sizeof(char)); // UN ESPACIO ADICIONAL GUARDADO PARA EL CARÁCTER'\0'

    if (LABERINTO == NULL) {
        fprintf(stderr, "\nError al reservar memoria\n\n");
        return 1;
    }

    // LEEMOS LABERINTO
    int K = 0;
    while ((N = fgetc(LAB)) != EOF) {
        if (N != '\n') {
            LABERINTO[K] = N;
            K++;
        }
    }
    LABERINTO[K] = '\0'; // PONEMOS COMO HEMOS DICHO ANTERIORMENTE EL CARÁCTER /0 AL FINAL

    fclose(LAB); //

    // Reserva de memoria para la matriz
    int** matriz = (int**)malloc(LX * sizeof(int*));
    if (matriz == NULL) {
        fprintf(stderr, "\nError al reservar memoria\n\n");
        return 1;
    }
    for (int i = 0; i < LX; i++) {
        matriz[i] = (int*)malloc(LY * sizeof(int));
        if (matriz[i] == NULL) {
            fprintf(stderr, "\nError al reservar memoria\n\n");
            return 1;
        }
    }

    // INICIALIZACIÓN DE LA MATRIZ, CON UN 1 DONDE EMPIEZA LA X, UN 2 DONDE HAY OBSTÁCULOS Y 0 EN LAS DEMAS CASILLAS
    for (int i = 0; i < LX; i++) {
        for (int j = 0; j < LY; j++) {
            matriz[i][j] = 0;
        }
    }

    int tmpFila = 1;
    int tmpCol= 0;
    for (int i = 0; i < K; i++)
        {

            if (tmpCol==LX){tmpCol=0;tmpFila++;}
            tmpCol=tmpCol+1;

            if ((LABERINTO[i] == '+') || (LABERINTO[i] == '|') || (LABERINTO[i] == '-'))
                {
                    matriz[tmpCol-1][tmpFila-1]=2;
                }

            if (LABERINTO[i] == 'X')
                {
                pos_x = tmpCol-1;
                pos_y = tmpFila-1;
                }

        }

    matriz[pos_x][pos_y] = 1;


    if (matriz == NULL)
        {
            fprintf(stderr, "\n ERROR");
            return 2;
        }


//CREAMOS EL VECTOR PARA EL BACKTRACKING
int* X = malloc(LX * LY * sizeof(int));


if (strcmp(argv[1], "borratxo") == 0)
    {
     BORRACHO(matriz, LX, LY, argc);
    }
else if (strcmp(argv[1], "backtrack") == 0)
    {
     BACKTRACKING(matriz, LX, LY, 0, X, LX*LY);
    }
else if (strcmp(argv[1], "BFS") == 0)
    {
    BFS(matriz, LX, LY);
    }
}

//CREAMOS LAS FUNCIONES PARA: INICIALIZAR COLAS, COMPROBAR SI ESTÁN VACÍAS, ENCOLAR, DESENCOLAR

void inicializarCola(Cola* cola) {
    cola->frente = NULL;
    cola->final = NULL;
}

int estaVacia(Cola* cola) {
    return (cola->frente == NULL);
}

void insertarEnCola(Cola* cola, int x, int y) {
    Nodo* nuevoNodo = (Nodo*)malloc(sizeof(Nodo));
    nuevoNodo->x = x;
    nuevoNodo->y = y;
    nuevoNodo->siguiente = NULL;

    if (estaVacia(cola)) {
        cola->frente = nuevoNodo;
        cola->final = nuevoNodo;
    } else {
        cola->final->siguiente = nuevoNodo;
        cola->final = nuevoNodo;
    }
}

void eliminarDeCola(Cola* cola) {
    if (estaVacia(cola)) {
        printf("La cola está vacía.\n");
        return;
    }

    Nodo* nodoEliminado = cola->frente;
    cola->frente = cola->frente->siguiente;

    if (cola->frente == NULL) {
        cola->final = NULL;
    }

    free(nodoEliminado);
}

int opcions_moviment(double u[], int *matriz[], int LX, int LY, int* LISTA, int argc) {

    int B;
    switch (matriz[pos_x][pos_y-1]) {
        case 2:
            u[0] = 1;
            break;

        default:

            if ((pos_y == 1))
                {
                imprimir_bor(LISTA, argc);
                exit(0);
                }
            else if (matriz[pos_x][pos_y-2] != 1 || pos_y == 0)
                {
                u[0] = 0;
                }
            else
                {
                u[0] = 1;
                }
    }

    switch (matriz[pos_x][pos_y+1]) {
        case 2:
            u[1] = 1;
            break;
        default:

             if ((pos_y == LY - 2))
                {
                imprimir_bor(LISTA, argc);
                exit(0);
                }

            else if (matriz[pos_x][pos_y+2] != 1 || pos_y == LY - 1)
                {
                u[1] = 0;
                }
            else
            {
                u[1] = 1;
            }
    }

    switch (matriz[pos_x+1][pos_y]) {
        case 2:
            u[2] = 1;
            break;
        default:

            if ((pos_x == LX - 2))
                {
                imprimir_bor(LISTA, argc);
                exit(0);
                }

            else if (matriz[pos_x+2][pos_y] != 1)
                {
                u[2] = 0;
                }

            else
                {
                u[2] = 1;
                }
    }
    switch (matriz[pos_x-1][pos_y]) {
        case 2:
            u[3] = 1;
            break;
        default:
            if ((pos_x == 1))
                {
                imprimir_bor(LISTA, argc);
                exit(0);
                }


            else if (matriz[pos_x-2][pos_y] != 1 || pos_x == 0)
                {
                u[3] = 0;
                }

            else
                {
                u[3] = 1;
                }
    }


    B = (4 - (u[0] + u[1] + u[2] + u[3]));
    return B;
   }

int BORRACHO(int *matriz[], int LX, int LY, int argc)
{
   int B2;

    int *LISTA = (int*)malloc(10*(LX-1)/2*(LX-1)/2*(LY-1)/2*(LY-1)/2*sizeof(int));

    B2 = opcions_moviment(direccions, matriz, LX, LY, LISTA, argc);

    long long int k = 0;

    unsigned int eleccion;
    srand(time(NULL));



    while (B2 >= 1 && pos_x > -1 && pos_x < (LX+1) && pos_y > -1 && pos_y < (LY+1) && (k<10*(LX-1)/2*(LX-1)/2*(LY-1)/2*(LY-1)/2)) {


        eleccion = rand() % 4;


        if ((direccions[0] == 0) && (eleccion == 0)) {
            pos_y = pos_y - 2;
            matriz[pos_x][pos_y] = 3;
            LISTA[k] = 1;
            k ++;
        } else if ((direccions[1] == 0) && (eleccion == 1)) {
            pos_y = pos_y + 2;
            matriz[pos_x][pos_y] = 3;
            LISTA[k] = 2;
            k ++;
        } else if ((direccions[2] == 0) && (eleccion == 2)) {
            pos_x = pos_x+2;
            matriz[pos_x][pos_y] = 3;
            LISTA[k] = 3;
            k ++;
        } else if ((direccions[3] == 0) && (eleccion == 3)) {
            pos_x = pos_x-2;
            matriz[pos_x][pos_y]= 3;
            LISTA[k] = 4;
            k ++;
        }

        opcions_moviment(direccions, matriz, LX, LY, LISTA, argc);
    }



    if (k == 10*(LX-1)/2*(LX-1)/2*(LY-1)/2*(LY-1)/2)
        {
        fprintf(stderr, "No ha trobat la sortida en %lld iteracions\n", k);
        exit(1);
        }
return k;
}

void VerLaberinto(int *matriz[], int LX, int LY)
    {
        for (int j = 0; j <LY; j++)
                 {
                    for (int i = 0; i < LX; i++){printf("%d ", matriz[i][j]);}
                    printf("\n");
                }
    }

int opcions_moviment_backtracking(double u[], int *matriz[], int LX, int LY, int* X, int k) {

    int B;

    switch (matriz[pos_x][pos_y-1]) {
        case 2:

            u[0] = 1;
            break;

        default:

            if ((pos_y == 1))
                {
                imprimir_back(X, k);
                fprintf(stdout, "\n");
                exit(1);
                }

            else if (matriz[pos_x][pos_y-2] != 1 || pos_y == 0)
                {
                u[0] = 0;
                }
            else
                {
                u[0] = 1;
                }
    }

    switch (matriz[pos_x][pos_y+1]) {
        case 2:
            u[1] = 1;
            break;
        default:
             if ((pos_y == LY - 2))
                {
                imprimir_back(X, k);
                fprintf(stdout, "\n");
                exit(1);
                }

            else if (matriz[pos_x][pos_y+2] != 1 || pos_y == LY - 1)
                {
                u[1] = 0;
                }
            else
                {
                u[1] = 1;
                }
    }

    switch (matriz[pos_x+1][pos_y]) {
        case 2:
            u[2] = 1;
            break;
        default:

            if ((pos_x == LX - 2))
                {
                imprimir_back(X, k);
                fprintf(stdout, "\n");
                exit(1);
                }

            else if (matriz[pos_x+2][pos_y] != 1)
                {
                u[2] = 0;
                }

            else
                {
                u[2] = 1;
                }
    }
    switch (matriz[pos_x-1][pos_y]) {
        case 2:
            u[3] = 1;
            break;
        default:

            if ((pos_x == 1))
                {
                imprimir_back(X, k);
                fprintf(stdout, "\n");
                exit(1);
                }

            else if (matriz[pos_x-2][pos_y] != 1 || pos_x == 0)
                {
                u[3] = 0;
                }

            else
                {
                u[3] = 1;
                }
    }


    B = (4 - (u[0] + u[1] + u[2] + u[3]));
    return B;
}

int BACKTRACKING(int *matriz[], int LX, int LY, int k, int *X, int D)

    {
    int B2 = 1;

    while (B2 >= 1 && pos_x > -1 && pos_x < (LX+1) && pos_y > -1 && pos_y < (LY+1)) {

    B2 = opcions_moviment_backtracking(direccions, matriz, LX, LY, X, k);

        if (direccions[0] == 0)
            {
            pos_y = pos_y - 2;
            matriz[pos_x][pos_y] = 1;
            X[k] = 0;
            k++;
            }

        else if (direccions[1] == 0)
            {
            pos_y = pos_y + 2;
            matriz[pos_x][pos_y] = 1;
            X[k] = 1;
            k++;
            }

        else if (direccions[2] == 0)
            {
            pos_x = pos_x+2;
            matriz[pos_x][pos_y] = 1;
            X[k] = 2;
            k++;
            }

        else if (direccions[3] == 0)
            {
            pos_x = pos_x-2;
            matriz[pos_x][pos_y]= 1;
            X[k] = 3;
            k++;
            }

        opcions_moviment_backtracking(direccions, matriz, LX, LY, X, k);
    }


    if (k == 0 && B2 == 0)
        {
        fprintf(stderr, "\nEl laberint no té solució\n\n");
        exit(1);
        }


    if (B2 == 0) {

    if (X[k-1] == 0)
            {
            pos_y = pos_y + 2;
            X[k-1] = 5;
            k--;
            BACKTRACKING(matriz, LX, LY, k, X, D);
            }

    else if (X[k-1] == 1)
            {
            pos_y = pos_y - 2;
            X[k-1] = 5;
            k--;
            BACKTRACKING(matriz, LX, LY, k, X, D);
            }

    else if (X[k-1] == 2)
            {
            pos_x = pos_x - 2;
            X[k-1] = 5;
            k--;
            BACKTRACKING(matriz, LX, LY, k, X, D);
            }

    else if (X[k-1] == 3)
            {
            pos_x = pos_x + 2;
            X[k-1] = 5;
            k--;
            BACKTRACKING(matriz, LX, LY, k, X, D);
            }

    }
    return 0;
}

int opcions_moviment_BFS(double u[], int *matriz[], int LX, int LY) {

    int B;

    switch (matriz[pos_x][pos_y-1]) {
        case 2:
            u[0] = 1;
            break;

        default:

            if ((pos_y == 1))
                {
                //fprintf(stdout, "Hem arribat al final del mapa\n");
                exit(1);
                }

            else if (matriz[pos_x][pos_y-2] != 1 || pos_y == 0)
                {
                u[0] = 0;
                }
            else
                {
                u[0] = 1;
                }
    }

    switch (matriz[pos_x][pos_y+1]) {
        case 2:
            u[1] = 1;
            break;
        default:

             if ((pos_y == LY - 2))
                {
                //fprintf(stdout, "Hem arribat al final del mapa\n");
                exit(1);
                }

            else if (matriz[pos_x][pos_y+2] != 1 || pos_y == LY - 1)
                {
                u[1] = 0;
                }
            else
            {
                u[1] = 1;
            }
    }

    switch (matriz[pos_x+1][pos_y]) {
        case 2:
            u[2] = 1;
            break;
        default:

            if ((pos_x == LX - 2))
                {
                //fprintf(stdout, "Hem arribat al final del mapa\n");
                exit(1);
                }

            else if (matriz[pos_x+2][pos_y] != 1)
                {
                u[2] = 0;
                }

            else
                {
                u[2] = 1;
                }
    }
    switch (matriz[pos_x-1][pos_y]) {
        case 2:
            u[3] = 1;
            break;
        default:

            if ((pos_x == 1))
                {
                //fprintf(stdout, "Hem arribat al final del mapa\n");
                exit(1);
                }

            else if (matriz[pos_x-2][pos_y] != 1 || pos_x == 0)
                {
                u[3] = 0;
                }

            else
                {
                u[3] = 1;
                }
    }


    B = (4 - (u[0] + u[1] + u[2] + u[3]));
    return B;
   }

int BFS(int* matriz[], int LX, int LY) {

    int* soluciones = (int*)malloc(((LX - 1) / 2) * ((LY - 1) / 2) * sizeof(int));

    Cola cola;
    inicializarCola(&cola);
    int ix = pos_x;
    int iy = pos_y;

    insertarEnCola(&cola, pos_x, pos_y);

    int* posiciones = detecta_salida(matriz, LX, LY);
    int SX = posiciones[0];
    int SY = posiciones[1];
    int SMX;
    int SMY;

    while (estaVacia(&cola) == 0) {
        pos_x = cola.frente->x;
        pos_y = cola.frente->y;
        eliminarDeCola(&cola);
        SMX = SX;
        SMY = SY;
        if (pos_x == SX && pos_y == SY) {
            int k = 0;
            while(SMX != ix || SMY != iy)
                {
                if (matriz[SMX][SMY] == 1)
                    {
                    soluciones[k] = 1;
                    SMY = SMY+2;
                    }
                else if (matriz[SMX][SMY] == 2)
                    {
                    soluciones[k] = 2;
                    SMY = SMY-2;
                    }
                else if (matriz[SMX][SMY] == 3)
                    {
                    soluciones[k] = 3;
                    SMX = SMX-2;
                    }
                else if (matriz[SMX][SMY] == 4)
                    {
                    soluciones[k] = 4;
                    SMX = SMX+2;
                    }
                k++;
                }
                soluciones[k] = matriz[ix][iy];


           for (int r = k-1 ; r > -1; r--)
                {
                if (soluciones[r] == 1)
                    {
                    fprintf(stdout, "nord ");
                    }

                else if (soluciones[r] == 2)
                    {
                    fprintf(stdout, "sud ");
                    }

                else if (soluciones[r] == 3)
                    {
                    fprintf(stdout, "est ");
                    }

                else if (soluciones[r] == 4)
                    {
                    fprintf(stdout, "oest ");
                    }
                }

             exit(0);
        }


        opcions_moviment_BFS(direccions, matriz, LX, LY);

        if (direccions[0] == 0 && matriz[pos_x][pos_y - 2] == 0) {
            insertarEnCola(&cola, pos_x, pos_y - 2);
            matriz[pos_x][pos_y - 2] = 1;
        }

        if (direccions[1] == 0 && matriz[pos_x][pos_y + 2] == 0) {
            insertarEnCola(&cola, pos_x, pos_y + 2);
            matriz[pos_x][pos_y + 2] = 2;
        }

        if (direccions[2] == 0 && matriz[pos_x + 2][pos_y] == 0) {
            insertarEnCola(&cola, pos_x + 2, pos_y);
            matriz[pos_x + 2][pos_y] = 3;
        }

        if (direccions[3] == 0 && matriz[pos_x - 2][pos_y] == 0) {
            insertarEnCola(&cola, pos_x - 2, pos_y);
            matriz[pos_x - 2][pos_y] = 4;
        }

    if (estaVacia(&cola) == 1)
        {
        fprintf(stderr, "No hi ha sortida");
        exit(1);
        }

    }


    return 0;
}

int* detecta_salida(int* matriz[], int LX, int LY) {
    int* posiciones = (int*)malloc(2 * sizeof(int));
    posiciones[0] = -1;
    posiciones[1] = -1;

    for (int i = 0; i < LY-1; i++) {
        if (matriz[0][i] == 0) {
            posiciones[0] = 1;
            posiciones[1] = i;
            break;
        }
        else if (matriz[LX-1][i] == 0) {
            posiciones[0] = LX-2;
            posiciones[1] = i;
            break;
        }
    }

    for (int j = 0; j < LX-1; j++) {
        if (matriz[j][0] == 0) {
            posiciones[0] = j;
            posiciones[1] = 1;
            break;
        }
        else if (matriz[j][LY-1] == 0) {
            posiciones[0] = j;
            posiciones[1] = LY-2;
            break;
        }
    }

    return posiciones;
}

void imprimir_bor(int *LISTA, int argc)
    {

    long long int j = 0;
    long long int N = 0;
    long long int S = 0;
    long long int E = 0;
    long long int O = 0;


    while (LISTA[j] == 1 || LISTA[j] == 2 || LISTA[j] == 3 || LISTA[j] == 4)
            {
                if (LISTA[j] == 1)
                        {
                        fprintf(stdout,"nord ");
                        N++;
                        }
                else if (LISTA[j] == 2)
                        {
                        fprintf(stdout,"sud ");
                        S++;
                        }
                else if (LISTA[j] == 3)
                        {
                        fprintf(stdout,"est ");
                        E++;
                        }
                else if (LISTA[j] == 4)
                        {
                        fprintf(stdout,"oest ");
                        O++;
                        }
            j ++;
            }

    long long int PN = 100*N;
    long long int PS = 100*S;
    long long int PE = 100*E;
    long long int PO = 100*O;

    long long int NN = PN/j;
    long long int SS = PS/j;
    long long int EE = PE/j;
    long long int OO = PO/j;

    if (argc == 4)
        {
        fprintf(stdout, "Ha resolt el laberint en %lld iteracions\n, %lld per cent al nord, %lld per cent al sud, %lld per cent al est, %lld per cent al o", j, NN, SS, EE, OO);
        }
    }


void imprimir_back(int *X, int k)
        {
        for (int j = 0; j<k; j++)
                    {
                    if (X[j] == 0) {
                        fprintf(stdout, "Nord ");
                    }
                    else if (X[j] == 1) {
                        fprintf(stdout, "Sud ");
                    }
                    else if (X[j] == 2) {
                        fprintf(stdout, "Est ");
                    }
                    else if (X[j] == 3) {
                        fprintf(stdout, "Oest ");
                    }
        }
        }

