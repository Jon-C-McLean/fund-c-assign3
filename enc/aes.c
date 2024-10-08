/**
 * @file aes.c
 * @author Jon McLean (13515869)
 */

#include "aes.h"

#define AES_BOX_SIZE 256U

#define AES_NUM_COLS 4U
#define AES_NUM_WORDS 4U
#define AES_NUM_ROUNDS 10U

typedef unsigned char u8;
typedef unsigned int u32;

/* #define XTIME(x) ((x<<1) ^ (((x>>7) & 1) * 0x1b)) */

u8 XTIME(u8 x) {
    return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

#define MULTIPLY(a, b) \
        ( ((b & 1) * a) ^ \
        ((b>>1 & 1) * XTIME(a)) ^ \
        ((b>>2 & 1) * XTIME(XTIME(a))) ^ \
        ((b>>3 & 1) * XTIME(XTIME(XTIME(a)))) ^ \
        ((b>>4 & 1) * XTIME(XTIME(XTIME(XTIME(a))))) ) \

#define SBOX(x) (sbox[(x)])
#define RSBOX(x) (rsbox[(x)])

typedef u8 state_t[4][4];

const u8 sbox[AES_BOX_SIZE] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

const u8 rsbox[AES_BOX_SIZE] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

static const u8 rcon[11] = { /* x^(i-1) */
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

/* Private Functions */

/**
 * @brief Perform the key expansion operations for the AES algorithm with
 * a 128-bit key (176 byte expanded key)
 * @author Jon McLean (13515869)
 * 
 * @param[inout] roundKey The round key to expand
 * @param[in] key The user's key
 */
void AES_PerformKeyExpansion(u8 *roundKey, const u8 *key) {
    unsigned i, j, k;
    u8 temp[4];
    
    for(i = 0; i < AES_NUM_WORDS; i++) {
        roundKey[(i * 4) + 0] = key[(i * 4) + 0];
        roundKey[(i * 4) + 1] = key[(i * 4) + 1];
        roundKey[(i * 4) + 2] = key[(i * 4) + 2];
        roundKey[(i * 4) + 3] = key[(i * 4) + 3];
    }

    for(i = AES_NUM_WORDS; i < AES_NUM_COLS * (AES_NUM_ROUNDS + 1); i++) {
        k = (i - 1) * 4;
        temp[0] = roundKey[k + 0];
        temp[1] = roundKey[k + 1];
        temp[2] = roundKey[k + 2];
        temp[3] = roundKey[k + 3];

        if(i % AES_NUM_WORDS == 0) {
            u8 u = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = u;

            temp[0] = SBOX(temp[0]);
            temp[1] = SBOX(temp[1]);
            temp[2] = SBOX(temp[2]);
            temp[3] = SBOX(temp[3]);

            temp[0] = temp[0] ^ rcon[i/AES_NUM_WORDS];
        }

        j = i * 4; 
        k = (i - AES_NUM_WORDS) * 4;
        roundKey[j + 0] = roundKey[k + 0] ^ temp[0];
        roundKey[j + 1] = roundKey[k + 1] ^ temp[1];
        roundKey[j + 2] = roundKey[k + 2] ^ temp[2];
        roundKey[j + 3] = roundKey[k + 3] ^ temp[3];
    }
}

/**
 * @brief Add the round key to the state matrix (XOR operation)
 * @author Jon McLean (13515869)
 * 
 * @param[in] round The current round
 * @param[inout] state The state matrix
 * @param[in] roundKey The round key to add to the state matrix
 */
void AES_AddRoundKey(u8 round, state_t *state, const u8 *roundKey) {
    u8 i, j;

    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            (*state)[i][j] ^= roundKey[(round * AES_NUM_COLS * 4) + (i * AES_NUM_COLS) + j];
        }
    }
}

/**
 * @brief Substitute the state matrix with the S-Box values (SubBytes operation)
 * @author Jon McLean (13515869)
 * 
 * @param[inout] state The state matrix to substitute values into
 */
void AES_SubBytes(state_t *state) {
    u8 i,j;
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            (*state)[j][i] = SBOX((*state)[j][i]);
        }
    }
}

/**
 * @brief Shift the rows of the state matrix (ShiftRows operation)
 * @author Jon McLean (13515869)
 * 
 * @param[inout] state The state matrix that will have its rows shifted
 */
void AES_ShiftRows(state_t *state) {
    u8 temp;

    /* Rotate first row */
    temp = (*state)[0][1];
    (*state)[0][1] = (*state)[1][1];
    (*state)[1][1] = (*state)[2][1];
    (*state)[2][1] = (*state)[3][1];
    (*state)[3][1] = temp;

    /* Rotate 2 columns to left */
    temp = (*state)[0][2];
    (*state)[0][2] = (*state)[2][2];
    (*state)[2][2] = temp;

    temp = (*state)[1][2];
    (*state)[1][2] = (*state)[3][2];
    (*state)[3][2] = temp;

    /* Rotate 3 columns to left */
    temp = (*state)[0][3];
    (*state)[0][3] = (*state)[3][3];
    (*state)[3][3] = (*state)[2][3];
    (*state)[2][3] = (*state)[1][3];
    (*state)[1][3] = temp;
}

/**
 * @brief Mixes the columns of the state matrix (MixColumns operation)
 * @author Jon McLean (13515869)
 * 
 * @param[in] state The state matrix to mix columns in
 */
void AES_MixCols(state_t *state) {
    u8 i;
    u8 tmp, tm, t;

    for(i = 0; i < 4; ++i) {
        t = (*state)[i][0];
        tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3];

        tm = (*state)[i][0] ^ (*state)[i][1];
        tm = XTIME(tm);  
        (*state)[i][0] ^= tm ^ tmp;
        
        tm = (*state)[i][1] ^ (*state)[i][2];
        tm = XTIME(tm);
        (*state)[i][1] ^= tm ^ tmp;
        
        tm = (*state)[i][2] ^ (*state)[i][3];
        tm = XTIME(tm);
        (*state)[i][2] ^= tm ^ tmp;

        tm = (*state)[i][3] ^ t;
        tm = XTIME(tm);
        (*state)[i][3] ^= tm ^ tmp;
    }
}

/**
 * @brief Mixes the columns of the state matrix (MixColumns operation) 
 * in reverse for decryption
 * @author Jon McLean (13515869)
 * 
 * @param[inout] state The state matrix to mix columns in reverse
 */
void AES_InverseMixCols(state_t *state) {
    int i;
    u8 a,b,c,d;

    for(i = 0; i < 4; i++) {
        a = (*state)[i][0];
        b = (*state)[i][1];
        c = (*state)[i][2];
        d = (*state)[i][3];

        (*state)[i][0] = MULTIPLY(a, 0x0e) ^ MULTIPLY(b, 0x0b) ^ MULTIPLY(c, 0x0d) ^ MULTIPLY(d, 0x09);
        (*state)[i][1] = MULTIPLY(a, 0x09) ^ MULTIPLY(b, 0x0e) ^ MULTIPLY(c, 0x0b) ^ MULTIPLY(d, 0x0d);
        (*state)[i][2] = MULTIPLY(a, 0x0d) ^ MULTIPLY(b, 0x09) ^ MULTIPLY(c, 0x0e) ^ MULTIPLY(d, 0x0b);
        (*state)[i][3] = MULTIPLY(a, 0x0b) ^ MULTIPLY(b, 0x0d) ^ MULTIPLY(c, 0x09) ^ MULTIPLY(d, 0x0e);
    }   
}

/**
 * @brief InvSubBytes operation for decryption, this is the inverse of the 
 * SubBytes operation
 * @author Jon McLean (13515869)
 * 
 * @param[inout] state The state matrix to perform the inverse substitution on
 */
void AES_InverseSubBytes(state_t *state) {
    u8 i,j;
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            (*state)[j][i] = RSBOX((*state)[j][i]);
        }
    }
}

/**
 * @brief InverseShiftRows operation for decryption, this is the inverse of the
 * ShiftRows operation.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] state The state matrix to perform the inverse shift rows on
 */
void AES_InverseShiftRows(state_t *state) {
    u8 temp;

    temp = (*state)[3][1];
    (*state)[3][1] = (*state)[2][1];
    (*state)[2][1] = (*state)[1][1];
    (*state)[1][1] = (*state)[0][1];
    (*state)[0][1] = temp;

    temp = (*state)[0][2];
    (*state)[0][2] = (*state)[2][2];
    (*state)[2][2] = temp;

    temp = (*state)[1][2];
    (*state)[1][2] = (*state)[3][2];
    (*state)[3][2] = temp;

    temp = (*state)[0][3];
    (*state)[0][3] = (*state)[1][3];
    (*state)[1][3] = (*state)[2][3];
    (*state)[2][3] = (*state)[3][3];
    (*state)[3][3] = temp;
}

/**
 * @brief Encrypt the state matrix through performing:
 * - AddRoundKey
 * - SubBytes
 * - ShiftRows
 * - MixCols
 * - AddRoundKey
 * for each round of the AES algorithm, except for the last round where
 * MixCols is not performed. This is the main encryption function for AES.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] state The state matrix to encrypt
 * @param[in] roundKey The round key to use for encryption
 */
void AES_Cipher(state_t *state, const u8 *roundKey) {
    u8 round = 0;

    AES_AddRoundKey(0, state, roundKey);
    for(round = 1; ; round++) {
        AES_SubBytes(state);
        AES_ShiftRows(state);
        if(round == AES_NUM_ROUNDS) {
            break;
        }
        AES_MixCols(state);
        AES_AddRoundKey(round, state, roundKey);
    }

    AES_AddRoundKey(AES_NUM_ROUNDS, state, roundKey);
}

/**
 * @brief The main AES decryption function, this function decrypts the state
 * matrix and performs the inverse of the AES cipher function.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] state The state matrix to decrypt
 * @param[in] roundKey The round key to use for decryption
 */
void AES_InverseCipher(state_t *state, const u8 *roundKey) {
    u8 round = 0;

    AES_AddRoundKey(AES_NUM_ROUNDS, state, roundKey);

    for(round = AES_NUM_ROUNDS - 1; ; round--) {
        AES_InverseShiftRows(state);
        AES_InverseSubBytes(state);
        AES_AddRoundKey(round, state, roundKey);
        if(round == 0) {
            break;
        }
        AES_InverseMixCols(state);
    }
}

/**
 * @brief XOR the buffer with the initialization vector for AES CBC mode
 * @author Jon McLean (13515869)
 * 
 * @param[inout] buffer The buffer to XOR with the init vector
 * @param iv 
 */
void AES_XorIV(u8* buffer, const u8 *iv) {
    u8 i;
    for(i = 0; i < AES_BLOCK_SIZE; i++) {
        buffer[i] ^= iv[i];
    }
}

/* Public Functions */
void AES_InitContext(aes_context_t *ctx, unsigned char *key, unsigned char *iv) {
    AES_PerformKeyExpansion(ctx->roundKey, key);
    memcpy(ctx->iv, iv, AES_BLOCK_SIZE);
}

void AES_Encrypt(aes_context_t *ctx, unsigned char *input, size_t size) {
    size_t i;
    u8 *iv = ctx->iv;

    for(i = 0; i < size; i += AES_BLOCK_SIZE) {
        AES_XorIV(input, iv);
        AES_Cipher((state_t *)input, ctx->roundKey);
        iv = input;
        input += AES_BLOCK_SIZE;
    }

    memcpy(ctx->iv, iv, AES_BLOCK_SIZE);
}

void AES_Decrypt(aes_context_t *ctx, unsigned char *input, size_t size) {
    size_t i;
    u8 nextIv[AES_BLOCK_SIZE];

    for(i = 0; i < size; i += AES_BLOCK_SIZE) {
        memcpy(nextIv, input, AES_BLOCK_SIZE);
        AES_InverseCipher((state_t *)input, ctx->roundKey);
        AES_XorIV(input, ctx->iv);
        memcpy(ctx->iv, nextIv, AES_BLOCK_SIZE);
        input += AES_BLOCK_SIZE;
    }
}

