#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "lib/sha1.h"


uint8_t hex2Uint(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return 10 + ch - 'A';
    return 0;
}

static int
validateTOTP(char *secret_hex, char *TOTP_string) {
    int i;
    int DIGITS_POWER[9] = {1,10,100,1000,10000,100000,1000000,10000000,100000000 };


    uint8_t uintSecret[10];
    // 512 bits = 64 bytes for SHA1
    uint8_t opad[64];
    uint8_t ipad[64];
    uint8_t inner_key[64];
    uint8_t outer_key[64];
    uint8_t message[8];

    for (i = 0; i < 20; i += 2) {
        uintSecret[i / 2] = hex2Uint(secret_hex[i]) * 16 + hex2Uint(secret_hex[i + 1]);
    }

    for (i = 0; i < 64; i++) {
        opad[i] = 0x5c;
    }
    for (i = 0; i < 64; i++) {
        ipad[i] = 0x36;
    }

    for (i = 0; i < 64; i++) {
        inner_key[i] = ipad[i];
        outer_key[i] = opad[i];
        if (i < 10) {
            inner_key[i] ^= uintSecret[i];
            outer_key[i] ^= uintSecret[i];
        }
    }

    unsigned long cur_time = (unsigned long) (time(NULL) / 30);

    for (i = 0; i < 8; ++i) {
        // reverse order for little-endian system
        message[7 - i] = (cur_time >> (i * 8)) & 0xff;
    }

    SHA1_INFO ctx1, ctx2;
    uint8_t tmp_hash[20];
    uint8_t final_hash[20];

    sha1_init(&ctx1);
    sha1_update(&ctx1, inner_key, 64);
    sha1_update(&ctx1, message, 8);
    sha1_final(&ctx1, tmp_hash);

    sha1_init(&ctx2);
    sha1_update(&ctx2, outer_key, 64);
    sha1_update(&ctx2, tmp_hash, 20);
    sha1_final(&ctx2, final_hash);

    int offset = final_hash[19] & 0xf;

    int binary =
            ((final_hash[offset] & 0x7f) << 24) |
            ((final_hash[offset + 1] & 0xff) << 16) |
            ((final_hash[offset + 2] & 0xff) << 8) |
            (final_hash[offset + 3] & 0xff);

    int otp = binary % DIGITS_POWER[6];
//    printf(">>> %d\n", otp);
    int TOTP_int = atoi(TOTP_string);

    if (otp == TOTP_int)
        return (1);

    return (0);
}


int
main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s [secretHex] [TOTP]\n", argv[0]);
        return (-1);
    }

    char *secret_hex = argv[1];
    char *TOTP_value = argv[2];

    assert(strlen(secret_hex) <= 20);
    assert(strlen(TOTP_value) == 6);

    printf("\nSecret (Hex): %s\nTOTP Value: %s (%s)\n\n",
           secret_hex,
           TOTP_value,
           validateTOTP(secret_hex, TOTP_value) ? "valid" : "invalid");

    return (0);
}
