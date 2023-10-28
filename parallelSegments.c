/* 
Implementación de un algoritmo de fuerza bruta para descifrar un texto cifrado con DES.
El algoritmo recibe como parámetro el nombre del archivo que contiene el texto cifrado.

Luis Santos
José Miguel Gonzales
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <openssl/des.h>
#include "/opt/homebrew/Cellar/libomp/16.0.6/include/omp.h"

/* 
DESCRIPCIÓN:
    Función que descifra un texto cifrado con DES.
    Recibe como parámetros:
        - key: llave para descifrar el texto.
        - ciph: texto cifrado.
        - len: tamaño del texto cifrado.
    No retorna nada.
 */
void myDecrypt(long key, char *ciph, int len) {
    // Variables para cifrado DES
    DES_key_schedule schedule;
    DES_cblock keyBlock;

    // Convertir llave long a DES_cblock
    memcpy(&keyBlock, &key, sizeof(keyBlock));
    
    // Inicializar cifrado DES
    DES_set_odd_parity(&keyBlock);
    DES_set_key_checked(&keyBlock, &schedule);
    
    // Descifrar texto en bloques de 8 bytes (64 bits)
    for (int i = 0; i < len; i += 8) {
        DES_ecb_encrypt((DES_cblock *)(ciph + i), (DES_cblock *)(ciph + i), &schedule, DES_DECRYPT);
    }
}

/* 
DESCRIPCIÓN:
    Función que prueba una llave para descifrar un texto cifrado con DES.
    Recibe como parámetros:
        - key: llave para descifrar el texto.
        - ciph: texto cifrado.
        - len: tamaño del texto cifrado.
    Retorna 1 si la llave es correcta, 0 en caso contrario.
 */
char search[] = " es una prueba de ";
int tryKey(long key, char *ciph, int len){
    // Variables para cifrado DES
    char temp[len + 1];

    // Copiar texto cifrado a variable temporal
    memcpy(temp, ciph, len);
    temp[len] = 0;
    
    // Descifrar texto temporal
    myDecrypt(key, temp, len);

    // Devolver resultado de búsqueda de texto
    return strstr((char *)temp, search) != NULL;
}

long parallelSearch(long lower, long upper, int ciphlen, char *cipher){
    long found = 0;

    # pragma omp parallel for shared(found)
    for (long i = lower; i < upper; i++) {
        if (tryKey(i, (char *)cipher, ciphlen)) {
            found = i;
        }
    }

    return found;
}

void mySearch(long mylower, long myupper, int N, int ciphlen, char *cipher, MPI_Request *req){
    long found = 0;
    int ready = 0;

    long lower = mylower;
    long upper = mylower + 1000;

    while (upper <= myupper && found == 0) {
        MPI_Test(req, &ready, MPI_STATUS_IGNORE);
        if(ready) break;  //ya encontraron, salir
        
        found = parallelSearch(lower, upper, ciphlen, cipher);

        if (found != 0) {
            for(int node = 0; node<N; node++){
                MPI_Send(&found, 1, MPI_LONG, node, 0, MPI_COMM_WORLD);
            }
        } else {
            lower = upper;
            upper += 1000;
        }
    }
}

unsigned char cipher[0];

/* 
DESCRIPCIÓN:
    Función principal del programa.
    Recibe como parámetros:
        - argc: número de argumentos.
        - argv: arreglo de argumentos.
 */
int main(int argc, char *argv[]){
    // Variables para MPI 
    int N, id;
    long upper = (1L <<56);
    long mylower, myupper;
    MPI_Status st;
    MPI_Request req;
    int flag;
    int ciphlen = strlen(cipher);
    MPI_Comm comm = MPI_COMM_WORLD;
    double start, end;

    // Inicializar MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_size(comm, &N);
    MPI_Comm_rank(comm, &id);
    
    if (id == 0) {
        printf("---------- Decrypting file ----------\n");
        start = MPI_Wtime();

        if (argc != 2) {
            printf("Usage: ./bruteforce <filename>\n");
            return 1;
        }

        // Leer archivo de texto cifrado
        char *filename = argv[1];
        FILE *fp = fopen(filename, "r");
        fseek(fp, 0L, SEEK_END);
        int fsize = ftell(fp);
        fseek(fp, 0L, SEEK_SET);

        // Leer texto cifrado en buffer
        char *buffer = malloc(fsize);
        fread(buffer, fsize, 1, fp);
        fclose(fp);

        // Read each byte from file into cipher array
        for (int i = 0; i < fsize; i++) {
            cipher[i] = buffer[i];
        }

        ciphlen = strlen(cipher);

        printf("Encrypted text: %s\n", cipher);
    }

    // Dividir espacio de búsqueda entre nodos
    long range_per_node = upper / N;
    mylower = range_per_node * id;
    myupper = range_per_node * (id+1) -1;
    if(id == N-1){
        myupper = upper;
    }

    long found = 0;
    int ready = 0;

    MPI_Irecv(&found, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &req);

    mySearch(mylower, myupper, N, ciphlen, cipher, &req);

    if(id==0){
        // Recibir llave encontrada
        MPI_Wait(&req, &st);
        myDecrypt(found, (char *)cipher, ciphlen);
        printf("Decrypted text: %s\n", cipher);
        printf("Key: %ld\n", found);

        end = MPI_Wtime();
        printf("Time: %f ms\n", (end - start) * 1000);
    }

    // Finalizar MPI
    MPI_Finalize();
}
