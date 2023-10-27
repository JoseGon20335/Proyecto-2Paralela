/* 
Implementación de un algoritmo de cifrado con DES.

Luis Santos
José Miguel Gonzales
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/des.h>

/* 
DESCRIPCIÓN:
    Función que cifra un texto con DES.
    Recibe como parámetros:
        - key: llave para cifrar el texto.
        - ciph: texto a cifrar.
        - len: tamaño del texto a cifrar.
    No retorna nada.
 */
 */
void myEncrypt(long key, char *ciph, int len) {
    DES_key_schedule schedule;
    DES_cblock keyBlock;

    // Convert long key to DES_cblock
    memcpy(&keyBlock, &key, sizeof(keyBlock));

    DES_set_odd_parity(&keyBlock);
    DES_set_key_checked(&keyBlock, &schedule);
    
    for (int i = 0; i < len; i += 8) {
        DES_ecb_encrypt((DES_cblock *)(ciph + i), (DES_cblock *)(ciph + i), &schedule, DES_ENCRYPT);
    }
}

/* 
DESCRIPCIÓN:
    Función principal del programa.
 */
int main(int argc, char *argv[]) {
    // Validación de parámetros
    if (argc != 3) {
        printf("Usage: ./cypher <key> <filename>\n");
        return 1;
    }

    // Obtener parámetros
    char *key = argv[1];
    char *filename = argv[2];
    long keyLong;
    
    printf("---------- Encrypting file ----------\n");

    sscanf(key, "%ld", &keyLong);
    printf("Key: %ld\n", keyLong);

    // Leer archivo
    FILE *fp = fopen(filename, "r");
    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char *buffer = malloc(fsize);
    fread(buffer, fsize, 1, fp);
    fclose(fp);

    printf("Original text: %s\n", buffer);

    // Encriptar texto
    myEncrypt(keyLong, buffer, fsize);

    printf("Encrypted text: %s\n", buffer);

    // Guardar texto encriptado
    fp = fopen("encrypted.txt", "w");
    fwrite(buffer, fsize, 1, fp);
    fclose(fp);

    printf("Saved encrypted file to encrypted.txt\n");

    return 0;
}

    