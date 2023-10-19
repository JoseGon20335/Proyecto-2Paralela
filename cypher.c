#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/des.h>

void myEncrypt(long key, char *ciph, int len) {
    DES_key_schedule schedule;
    DES_cblock keyBlock;

    // Convert long key to DES_cblock
    memcpy(&keyBlock, &key, sizeof(keyBlock));

    DES_set_odd_parity(&keyBlock);
    DES_set_key_checked(&keyBlock, &schedule);
    
    DES_ecb_encrypt((DES_cblock *)ciph, (DES_cblock *)ciph, &schedule, DES_ENCRYPT);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./cypher <key> <filename>\n");
        return 1;
    }

    char *key = argv[1];
    char *filename = argv[2];
    long keyLong;

    // Convert each character in key to long
    for (int i = 0; i < strlen(key); i++) {
        keyLong += key[i];
    }

    FILE *fp = fopen(filename, "r");
    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char *buffer = malloc(fsize);
    fread(buffer, fsize, 1, fp);
    fclose(fp);

    myEncrypt(keyLong, buffer, fsize);

    // print encrypted file to console
    for (int i = 0; i < fsize; i++) {
        printf("%d ", buffer[i]);
    }

    // Write encrypted file to file
    fp = fopen("encrypted.txt", "w");
    fwrite(buffer, fsize, 1, fp);
    fclose(fp);

    return 0;
}

    