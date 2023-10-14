//bruteforce.c
//nota: el key usado es bastante pequenio, cuando sea random speedup variara

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <openssl/des.h>

void myDecrypt(long key, char *ciph, int len) {
    DES_key_schedule schedule;
    DES_cblock keyBlock;

    // Convert long key to DES_cblock
    memcpy(&keyBlock, &key, sizeof(keyBlock));
    
    DES_set_odd_parity(&keyBlock);
    DES_set_key_checked(&keyBlock, &schedule);
    
    DES_ecb_encrypt((DES_cblock *)ciph, (DES_cblock *)ciph, &schedule, DES_DECRYPT);
}

void myEncrypt(long key, char *ciph, int len) {
    DES_key_schedule schedule;
    DES_cblock keyBlock;

    // Convert long key to DES_cblock
    memcpy(&keyBlock, &key, sizeof(keyBlock));

    DES_set_odd_parity(&keyBlock);
    DES_set_key_checked(&keyBlock, &schedule);
    
    DES_ecb_encrypt((DES_cblock *)ciph, (DES_cblock *)ciph, &schedule, DES_ENCRYPT);
}

char search[] = " es una prueba de ";
int tryKey(long key, char *ciph, int len){
    char temp[len+1];
    memcpy(temp, ciph, len);
    temp[len]=0;
    myDecrypt(key, temp, len);
    return strstr((char *)temp, search) != NULL;
}

unsigned char cipher[] = {50, -12, -2, -82, -108, -75, -47, 45, 117, 110, 97, 32, 112, 114, 117, 101, 98, 97, 32, 100, 101, 32, 112, 114, 111, 121, 101, 99, 116, 111, 32, 50, 0};
int main(int argc, char *argv[]){ //char **argv
    int N, id;
    long upper = (1L <<56); //upper bound DES keys 2^56
    long mylower, myupper;
    MPI_Status st;
    MPI_Request req;
    int flag;
    int ciphlen = strlen(cipher);
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(comm, &N);
    MPI_Comm_rank(comm, &id);

    long range_per_node = upper / N;
    mylower = range_per_node * id;
    myupper = range_per_node * (id+1) -1;
    if(id == N-1){
        //compensar residuo
        myupper = upper;
    }

    long found = 0;
    int ready = 0;

    MPI_Irecv(&found, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &req);

    for(long i = mylower; i<myupper; ++i){
        MPI_Test(&req, &ready, MPI_STATUS_IGNORE);
        if(ready) break;  //ya encontraron, salir

        if(tryKey(i, (char *)cipher, ciphlen)){
            found = i;
            for(int node=0; node<N; node++){
                MPI_Send(&found, 1, MPI_LONG, node, 0, MPI_COMM_WORLD);
            }
        }
    }

    if(id==0){
        MPI_Wait(&req, &st);
        myDecrypt(found, (char *)cipher, ciphlen);
        printf("%li - %s\n", found, cipher);
    }

    MPI_Finalize();
}
