/**
 * @file aes.h
 * @author Jon McLean (13515869)
 */

#ifndef ENC_AES_H_
#define ENC_AES_H_

#define AES_BLOCK_SIZE 16U
#define AES_KEY_SIZE 128U // 128 bit key size (16 bytes)
#define AES_EXP_KEY_SIZE 176U // 176 bytes for expanded key

typedef struct {
    unsigned char roundKey[AES_EXP_KEY_SIZE]; /* AES Round Key */
    unsigned char iv[AES_BLOCK_SIZE]; /* Initialization Vector */
} aes_context_t;

/**
 * @brief Initialize the AES context with the key and IV
 * 
 * @param[inout] ctx The context to initialize
 * @param[in] key The key to use
 * @param[in] iv The IV to use
 */
void AES_InitContext(aes_context_t *ctx, unsigned char *key, unsigned char *iv);

/**
 * @brief Encrypt the input using the AES context (i.e. key and IV)
 * 
 * @param[inout] ctx 
 */
void AES_Encrypt(aes_context_t *ctx, unsigned char *input, unsigned char *output);

/**
 * @brief Decrypt the input buffer using the AES context (i.e. key and IV)
 * 
 * @param[inout] ctx The context to use
 */
void AES_Decrypt(aes_context_t *ctx, unsigned char *input, unsigned char *output);

#endif