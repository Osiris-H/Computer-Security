#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "lib/encoding.h"

uint8_t hex2Uint(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return 10 + ch - 'A';
    return 0;
}

int
main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s [issuer] [accountName] [secretHex]\n", argv[0]);
        return (-1);
    }

    char *issuer = argv[1];
    char *accountName = argv[2];
    char *secret_hex = argv[3];

    assert(strlen(secret_hex) <= 20);

    printf("\nIssuer: %s\nAccount Name: %s\nSecret (Hex): %s\n\n",
           issuer, accountName, secret_hex);

    // Create an otpauth:// URI and display a QR code that's compatible
    // with Google Authenticator
    char encodeIssuer[strlen(issuer)];
    char encodeAccName[strlen(accountName)];
    int i;

    uint8_t uint8Secret[11];
    uint8_t encodeSecret[17];

    strcpy(encodeIssuer, urlEncode(issuer));
    strcpy(encodeAccName, urlEncode(accountName));

    for (i = 0; i < 20; i += 2) {
        uint8Secret[i / 2] = hex2Uint(secret_hex[i]) * 16 + hex2Uint(secret_hex[i + 1]);
    }
    uint8Secret[10] = '\0';
    base32_encode(uint8Secret, 10, encodeSecret, 17);

    char url[1024];
    sprintf(url, "otpauth://totp/%s?issuer=%s&secret=%s&period=30", encodeAccName, encodeIssuer, encodeSecret);
    displayQRcode(url);

    return (0);
}
