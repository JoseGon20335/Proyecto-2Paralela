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
    
    for (int i = 0; i < len; i += 8) {
        DES_ecb_encrypt((DES_cblock *)(ciph + i), (DES_cblock *)(ciph + i), &schedule, DES_ENCRYPT);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./cypher <key> <filename>\n");
        return 1;
    }

    char *key = argv[1];
    char *filename = argv[2];
    long keyLong;
    
    printf("---------- Encrypting file ----------\n");

    sscanf(key, "%ld", &keyLong);
    printf("Key: %ld\n", keyLong);

    FILE *fp = fopen(filename, "r");
    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char *buffer = malloc(fsize);
    fread(buffer, fsize, 1, fp);
    fclose(fp);

    printf("Original text: %s\n", buffer);

    myEncrypt(keyLong, buffer, fsize);

    printf("Encrypted text: %s\n", buffer);

    // Write encrypted file to file
    fp = fopen("encrypted.txt", "w");
    fwrite(buffer, fsize, 1, fp);
    fclose(fp);

    printf("Saved encrypted file to encrypted.txt\n");

    return 0;
}

    