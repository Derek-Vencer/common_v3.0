/***************************************************************************
 *
 * Copyright 2015-2024 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#if defined(UTILS_ESHELL_EN) && defined(ENABLE_MBEDTLS)

#include "plat_addr_map.h"
#if defined(SHANHAI_MBEDTLS)
#include "config-shanhai.h"
#else
#include "mbedtls_ports_config.h"
#endif

#if defined(MBEDTLS_CIPHER_C)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hexdump.h"
#include "mbedtls/debug.h"
#include "mbedtls/cipher.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/platform.h"
#include "aes.h"
#include "eshell.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"
#include "plat_types.h"

#ifdef SHANHAI_MBEDTLS
#include "pal_heap.h"
#include "pal_log.h"
#endif

#define MBEDTLS_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

typedef void (* AES_FUNC_T)(mbedtls_operation_t, uint8_t *, size_t, uint8_t *, size_t, uint8_t *, size_t);
typedef void (* HASH_FUNC_T)(unsigned char *, size_t);
AES_FUNC_T aes_func;
HASH_FUNC_T hash_func;

struct MBEDTLS_LIST_T {
    char *mbedtls_name;
    void *mbedtls_func;
};

struct MD_TYPE_LIST_T {
    char *md_name;
    mbedtls_md_type_t md_type;
};

static void aes_ecb_test(mbedtls_operation_t operation,
                         uint8_t *key, size_t key_len,
                         uint8_t *iv, size_t iv_len,
                         uint8_t *input, size_t in_len);

static void aes_cbc_test(mbedtls_operation_t operation,
                         uint8_t *key, size_t key_len,
                         uint8_t *iv, size_t iv_len,
                         uint8_t *input, size_t in_len);

static void aes_ctr_test(mbedtls_operation_t operation,
                         uint8_t *key, size_t key_len,
                         uint8_t *iv, size_t iv_len,
                         uint8_t *input, size_t in_len);

static void sha1_test(unsigned char *msg, size_t msg_len);
static void sha224_test(unsigned char *msg, size_t msg_len);
static void sha256_test(unsigned char *msg, size_t msg_len);
static void sha384_test(unsigned char *msg, size_t msg_len);
static void sha512_test(unsigned char *msg, size_t msg_len);

static void unitest_mbedtls_usage(void);

struct MBEDTLS_LIST_T mbedtls_list[] = {
    {"aes-ecb", (void *)aes_ecb_test},
    {"aes-cbc", (void *)aes_cbc_test},
    {"aes-ctr", (void *)aes_ctr_test},
    {"sha-1", (void *)sha1_test},
    {"sha-224", (void *)sha224_test},
    {"sha-256", (void *)sha256_test},
    {"sha-384", (void *)sha384_test},
    {"sha-512", (void *)sha512_test},
};

struct MD_TYPE_LIST_T md_list[] = {
    {"hmac-sha1",MBEDTLS_MD_SHA1},
    {"hmac-sha256",MBEDTLS_MD_SHA256},
    {"hmac-sha512",MBEDTLS_MD_SHA512},
};

#define OUT_BUF_SIZE  100

#ifdef SHANHAI_MBEDTLS
#define BUF_SIZE (4 * 1024)
static unsigned char __attribute__((aligned(32))) alloc_buf[BUF_SIZE];
#endif

int mbedtls_atoh(const char* str, uint8_t len) {
    int result = 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    while (len--) {
        char digit = *str;
        int value;

        if (digit >= '0' && digit <= '9') {
            value = digit - '0';
        } else if (digit >= 'a' && digit <= 'f') {
            value = 10 + digit - 'a';
        } else if (digit >= 'A' && digit <= 'F') {
            value = 10 + digit - 'A';
        } else {
            break;
        }

        result = result * 16 + value;
        str++;
    }

    return result;
}

static void shanhai_init(void) {
#ifdef SHANHAI_MBEDTLS
    mbedtls_platform_context ctx;

    memset(&ctx, 0, sizeof(ctx));
    ctx.buff_addr = alloc_buf;
    ctx.buff_size = BUF_SIZE;
    mbedtls_platform_setup( &ctx );
#endif
}

static void aes_setkey(mbedtls_operation_t operation, mbedtls_aes_context *ctx, uint8_t *key, size_t key_len )
{
    if (operation == MBEDTLS_ENCRYPT) {
        mbedtls_aes_setkey_enc( ctx, key, key_len * 8 );
    } else if (operation == MBEDTLS_DECRYPT) {
        mbedtls_aes_setkey_dec( ctx, key, key_len * 8 );
    }
}

static void aes_ecb_test(mbedtls_operation_t operation,
                         uint8_t *key, size_t key_len,
                         uint8_t *iv, size_t iv_len,
                         uint8_t *input, size_t in_len)
{
    int ret;
    static uint8_t plain_data[OUT_BUF_SIZE] = {0};
    static uint8_t cipher_data[OUT_BUF_SIZE] = {0};

    mbedtls_aes_context ctx;
    mbedtls_aes_init( &ctx );

    aes_setkey(operation, &ctx, key, key_len);

    mbedtls_aes_crypt_ecb( &ctx, operation, input, cipher_data);

    eshell_putstring("plaintext:%s", input);
    eshell_putstring("ciphertext: Start of dump,len:%d", in_len);
    dump_buffer(cipher_data, in_len);
    eshell_putstring("ciphertext: End of dump");

    aes_setkey(!operation, &ctx, key, key_len);

    memset(plain_data, 0, in_len);

    mbedtls_aes_crypt_ecb( &ctx, !operation, cipher_data, plain_data);

    ret = memcmp((void *)plain_data, (void *)input, in_len);
    if(ret == 0) {
        eshell_putstring("compare passed!!!!");
    } else {
        eshell_putstring("compare failed!!!!");
    }

    mbedtls_aes_free( &ctx );
}

static void aes_cbc_test(mbedtls_operation_t operation,
                         uint8_t *key, size_t key_len,
                         unsigned char *iv, size_t iv_len,
                         unsigned char *input, size_t in_len)
{
    int ret;

    static uint8_t plain_data[OUT_BUF_SIZE] = {0};
    static uint8_t cipher_data[OUT_BUF_SIZE] = {0};
    static uint8_t iv_temp[16];
    memcpy(iv_temp, iv, iv_len);
    mbedtls_aes_context ctx;
    mbedtls_aes_init( &ctx );

    aes_setkey(operation, &ctx, key, key_len);

    mbedtls_aes_crypt_cbc( &ctx, operation, in_len, iv_temp, input, cipher_data);

    eshell_putstring("plaintext:%s", input);
    eshell_putstring("ciphertext: Start of dump,len:%d", in_len);
    dump_buffer(cipher_data, in_len);
    eshell_putstring("ciphertext: End of dump");

    aes_setkey(!operation, &ctx, key, key_len);

    memset(plain_data, 0, in_len);
    memcpy(iv_temp, iv, 16);

    mbedtls_aes_crypt_cbc( &ctx, !operation, in_len, iv_temp, cipher_data, plain_data);

    ret = memcmp((void *)plain_data, (void *)input, in_len);
    if(ret == 0) {
        eshell_putstring("compare passed!!!!");
    } else {
        eshell_putstring("compare failed!!!!");
    }

    mbedtls_aes_free( &ctx );
}

static void aes_ctr_test(mbedtls_operation_t operation,
                         uint8_t *key, size_t key_len,
                         unsigned char *iv, size_t iv_len,
                         unsigned char *input, size_t in_len)
{
    int ret;

    static uint8_t plain_data[OUT_BUF_SIZE] = {0};
    static uint8_t cipher_data[OUT_BUF_SIZE] = {0};
    static uint8_t stream_block[16] = {0};
    static uint8_t iv_temp[16];
    size_t nc_off = 0;
    memcpy(iv_temp, iv, iv_len);
    mbedtls_aes_context ctx;
    mbedtls_aes_init( &ctx );

    aes_setkey(operation, &ctx, key, key_len);

    mbedtls_aes_crypt_ctr( &ctx, in_len, &nc_off, iv_temp, stream_block, input, cipher_data);

    eshell_putstring("plaintext:%s", input);
    eshell_putstring("ciphertext: Start of dump,len:%d", in_len);
    dump_buffer(cipher_data, in_len);
    eshell_putstring("ciphertext: End of dump");

    aes_setkey(!operation, &ctx, key, key_len);
    memset(plain_data, 0, in_len);
    memcpy(iv_temp, iv, 16);
    nc_off = 0;
    mbedtls_aes_crypt_ctr( &ctx, in_len, &nc_off, iv_temp, stream_block, cipher_data, plain_data);

    ret = memcmp((void *)plain_data, (void *)input, in_len);
    if(ret == 0) {
        eshell_putstring("compare passed!!!!");
    } else {
        eshell_putstring("compare failed!!!!");
    }

    mbedtls_aes_free( &ctx );
}

static void sha1_test(unsigned char *msg, size_t msg_len)
{
    static unsigned char sha1sum[20];

    mbedtls_sha1_context ctx;
    mbedtls_sha1_init(&ctx);

    mbedtls_sha1_starts(&ctx);
    mbedtls_sha1_update(&ctx, msg, msg_len);
    mbedtls_sha1_finish(&ctx, sha1sum);

    eshell_putstring("sha1 digest: Start of dump,len:%d", 20);
    dump_buffer(sha1sum, 20);
    eshell_putstring("ciphertext: End of dump");

}

static void sha224_test(unsigned char *msg, size_t msg_len)
{
    static unsigned char sha224sum[28];

    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);

    mbedtls_sha256_starts(&ctx,1);
    mbedtls_sha256_update(&ctx, msg, msg_len);
    mbedtls_sha256_finish(&ctx, sha224sum);

    eshell_putstring("sha224 digest: Start of dump,len:%d", 28);
    dump_buffer(sha224sum, 28);
    eshell_putstring("ciphertext: End of dump");
}

static void sha256_test(unsigned char *msg, size_t msg_len)
{
    static unsigned char sha256sum[32];

    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);

    mbedtls_sha256_starts(&ctx,0);
    mbedtls_sha256_update(&ctx, msg, msg_len);
    mbedtls_sha256_finish(&ctx, sha256sum);

    eshell_putstring("sha256 digest: Start of dump,len:%d",32);
    dump_buffer(sha256sum, 32);
    eshell_putstring("ciphertext: End of dump");
}

static void sha384_test(unsigned char *msg, size_t msg_len)
{
    static unsigned char sha384sum[48];

    mbedtls_sha512_context ctx;
    mbedtls_sha512_init(&ctx);

    mbedtls_sha512_starts(&ctx,1);
    mbedtls_sha512_update(&ctx, msg, msg_len);
    mbedtls_sha512_finish(&ctx, sha384sum);

    eshell_putstring("sha384 digest: Start of dump,len:%d",48);
    dump_buffer(sha384sum, 48);
    eshell_putstring("ciphertext: End of dump");
}

static void sha512_test(unsigned char *msg, size_t msg_len)
{
    static unsigned char sha512sum[64];

    mbedtls_sha512_context ctx;
    mbedtls_sha512_init(&ctx);

    mbedtls_sha512_starts(&ctx,0);
    mbedtls_sha512_update(&ctx, msg, msg_len);
    mbedtls_sha512_finish(&ctx, sha512sum);

    eshell_putstring("sha512 digest: Start of dump,len:%d",64);
    dump_buffer(sha512sum, 64);
    eshell_putstring("ciphertext: End of dump");
}

static void hmac_test(mbedtls_md_type_t md_type, uint8_t *msg, size_t msg_len, uint8_t *key, size_t key_len)
{
    size_t len;
    uint8_t digest[64];

    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t *info;

    mbedtls_md_init(&ctx);

    info = mbedtls_md_info_from_type(md_type);

    mbedtls_md_setup(&ctx, info, 1);

    mbedtls_md_hmac_starts(&ctx, (unsigned char *)key, key_len);

    mbedtls_md_hmac_update(&ctx, (unsigned char *)msg, msg_len);

    mbedtls_md_hmac_finish(&ctx, digest);

    eshell_putstring("Input message:%s", msg);
    eshell_putstring("  ");

    len = mbedtls_md_get_size(info);
    eshell_putstring("%s digest,len:%d", mbedtls_md_get_name(info), len);
    dump_buffer(digest, len);
    eshell_putstring("  ");

    mbedtls_md_free(&ctx);
}

static void trng_test(const char *pers, size_t buf_len)
{
    static uint8_t buf[OUT_BUF_SIZE];

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *) pers, strlen(pers));

    eshell_putstring("generate random data...");
    mbedtls_ctr_drbg_random(&ctr_drbg, buf, buf_len);
    eshell_putstring("  ");
    eshell_putstring("random data,len:%d", buf_len);
    dump_buffer(buf, buf_len);

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

static void unitest_mbedtls_aes_test(int argc, char *argv[])
{
    mbedtls_cipher_mode_t cipher_mode;
    mbedtls_operation_t operation = 0;
    uint8_t key[32] = {0};
    size_t key_len = 0;
    uint8_t iv[16] = {0};
    size_t iv_len = 0;
    uint8_t input[OUT_BUF_SIZE] = {0};
    size_t in_len = 0;

    if (argc < 6)
        goto usage;

    memset(&cipher_mode, 0, sizeof(mbedtls_cipher_mode_t));

    for (int i = 0; i < MBEDTLS_ARRAY_SIZE(mbedtls_list); i++) {
        if (strncmp(argv[1], mbedtls_list[i].mbedtls_name, 7) == 0) {
            aes_func = (AES_FUNC_T)mbedtls_list[i].mbedtls_func;
            break;
        }
    }

    if (strncmp(argv[2], "enc", 3) == 0) {
        operation = MBEDTLS_ENCRYPT;
    } else if (strncmp(argv[2], "dec", 3) == 0) {
        operation = MBEDTLS_DECRYPT;
    }

    int m = 0;
    for (int i = 0; i < strlen(argv[3]); i = i+2) {
        key[m] = mbedtls_atoh(argv[3] + i, 2);
        m++;
    }
    key_len = strlen(argv[3]) / 2;

    m = 0;
    for (int i = 0; i < strlen(argv[4]); i = i+2) {
        iv[m] = mbedtls_atoh(argv[4] + i, 2);
        m++;
    }
    iv_len = strlen(argv[4]) / 2;

    in_len = strlen(argv[5]);
    memcpy(input, argv[5], in_len);

    aes_func(operation, key, key_len, iv, iv_len, input, in_len);

    return;
usage:
    unitest_mbedtls_usage();
}

static void unitest_mbedtls_hash_test(int argc, char *argv[])
{
    uint8_t *msg;
    size_t msg_len;

    if (argc < 3)
        goto usage;

    for (int i = 0; i < MBEDTLS_ARRAY_SIZE(mbedtls_list); i++) {
        if (strncmp(argv[1], mbedtls_list[i].mbedtls_name, 7) == 0) {
            hash_func = (HASH_FUNC_T)mbedtls_list[i].mbedtls_func;
            break;
        }
    }

    msg = (uint8_t *)argv[2];
    msg_len = strlen(argv[2]);

    hash_func(msg, msg_len);

    return;
usage:
    unitest_mbedtls_usage();
}

static void unitest_mbedtls_hmac_test(int argc, char *argv[])
{
    uint8_t *msg;
    size_t msg_len;
    uint8_t key[32];
    size_t key_len = 0;
    mbedtls_md_type_t md_type = 0;

    if (argc < 4) {
        goto usage;
    }

    for (int i = 0; i < MBEDTLS_ARRAY_SIZE(md_list); i++) {
        if (strncmp(argv[1], md_list[i].md_name, 11) == 0) {
            md_type = md_list[i].md_type;
            break;
        }
    }
    msg = (uint8_t *)argv[2];
    msg_len = strlen(argv[2]);

    int m = 0;
    for (int i = 0; i < strlen(argv[3]); i = i+2) {
        key[m] = mbedtls_atoh(argv[3] + i, 2);
        m++;
    }
    key_len = strlen(argv[3]) / 2;

    hmac_test(md_type, msg, msg_len, key, key_len);

    return;
usage:
    unitest_mbedtls_usage();
}

static void unitest_mbedtls_trng_test(int argc, char *argv[])
{
    if (argc < 4) {
        goto usage;
    }

    char *pers = argv[2];
    size_t buf_len = atoi(argv[3]);

    trng_test(pers, buf_len);

    return;
usage:
    unitest_mbedtls_usage();
}

static void unitest_mbedtls_usage(void)
{
    eshell_putstring("Usage:\r\n");
    eshell_putstring("  utest_mbedtls aes-ecb/cbc/ctr enc/dec key iv plain_text\r\n");
    eshell_putstring("  utest_mbedtls sha-1/224/256/384/512 plain_text\r\n");
    eshell_putstring("  utest_mbedtls hamc-sha1/256/512 plain_text key\r\n");
    eshell_putstring("  utest_mbedtls trng pers len\r\n");
    eshell_putstring("Example:\r\n");
    eshell_putstring("  utest_mbedtls aes-ecb enc 6162636465666768696a6b6c6d6e6f70 6162636465666768696a6b6c6d6e6f70 abcdefghijklmnop\r\n");
    eshell_putstring("  utest_mbedtls sha-1 abcdefghijklmnop\r\n");
    eshell_putstring("  utest_mbedtls hmac-sha1 abcdefghijklmnop 6162636465666768696a6b6c6d6e6f70\r\n");
    eshell_putstring("  utest_mbedtls trng crbg_20221223 80\r\n");
    eshell_putstring(" \r\n");
    eshell_putstring("  the ecb plaintext input must be 16 bytes long!!!\r\n");
    eshell_putstring("  the cbc plaintext input must be a multiple of 16 bytes!!!\r\n");
    eshell_putstring("  the key and iv input are hex format!!!\r\n");
}

static void unitest_mbedtls(int argc, char *argv[])
{
    shanhai_init();

    if (strncmp(argv[1], "aes", 3) == 0) {
        unitest_mbedtls_aes_test(argc, argv);
    } else if (strncmp(argv[1], "sha", 3) == 0) {
        unitest_mbedtls_hash_test(argc, argv);
    } else if (strncmp(argv[1], "hmac", 4) == 0) {
        unitest_mbedtls_hmac_test(argc, argv);
    } else if (strncmp(argv[1], "trng", 4) == 0) {
        unitest_mbedtls_trng_test(argc, argv);
    } else {
        unitest_mbedtls_usage();
    }
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_UNIT_TEST, "utest_mbedtls", "usage: utest_mbedtls help",
                   unitest_mbedtls);

#endif
#endif
