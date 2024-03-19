#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHA256_BLOCK_SIZE 64
#define SHA256_DIGEST_SIZE 32

// Funções básicas de manipulação de bits
#define ROTRIGHT(word, bits) (((word) >> (bits)) | ((word) << (32 - (bits))))
#define ROTLEFT(word, bits) (((word) << (bits)) | ((word) >> (32 - (bits))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define EP1(x) (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define SIG0(x) (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))

// Estrutura de contexto SHA256
typedef struct {
    unsigned char data[64];
    unsigned int datalen;
    unsigned long long bitlen;
    unsigned int state[8];
} SHA256_CTX;

// Tabela de constantes para SHA256
static const unsigned int k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// Inicializa o contexto SHA256
void sha256_init(SHA256_CTX *ctx) {
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

// Processa os dados através do algoritmo SHA256
void sha256_transform(SHA256_CTX *ctx, const unsigned char data[]) {
    unsigned int a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    // Preparação das mensagens
    for (i = 0, j = 0; i < 16; ++i, j += 4) {
        m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    }

    for ( ; i < 64; ++i) {
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
    }

    // Inicialização dos valores iniciais
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    // Loop principal
    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    // Atualiza o estado
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

// Adiciona os dados ao contexto SHA256
void sha256_update(SHA256_CTX *ctx, const unsigned char data[], size_t len) {
    for (size_t i = 0; i < len; ++i) {
        ctx->data[ctx->datalen] = data[i];
        ctx->datalen++;
        if (ctx->datalen == SHA256_BLOCK_SIZE) {
            sha256_transform(ctx, ctx->data);
            ctx->bitlen += 512;
            ctx->datalen = 0;
        }
    }
}

// Finaliza o algoritmo SHA256 e gera o digest
void sha256_final(SHA256_CTX *ctx, unsigned char hash[]) {
    unsigned int i = ctx->datalen;

    // Padrao: 1 bit seguido de 0's, até que o comprimento seja congruente a 448 mod 512
    ctx->data[i++] = 0x80;
    while (i < SHA256_BLOCK_SIZE) {
        ctx->data[i++] = 0x00;
    }

    // Comprimento original em bits
    ctx->bitlen += ctx->datalen * 8;
    ctx->data[SHA256_BLOCK_SIZE - 8] = (ctx->bitlen >> 56);
    ctx->data[SHA256_BLOCK_SIZE - 7] = (ctx->bitlen >> 48);
    ctx->data[SHA256_BLOCK_SIZE - 6] = (ctx->bitlen >> 40);
    ctx->data[SHA256_BLOCK_SIZE - 5] = (ctx->bitlen >> 32);
    ctx->data[SHA256_BLOCK_SIZE - 4] = (ctx->bitlen >> 24);
    ctx->data[SHA256_BLOCK_SIZE - 3] = (ctx->bitlen >> 16);
    ctx->data[SHA256_BLOCK_SIZE - 2] = (ctx->bitlen >> 8);
    ctx->data[SHA256_BLOCK_SIZE - 1] = (ctx->bitlen);

    // Processa o último bloco
    sha256_transform(ctx, ctx->data);

    // Gera o hash
    for (i = 0; i < 8; ++i) {
        hash[i * 4] = (ctx->state[i] >> 24) & 0xff;
        hash[i * 4 + 1] = (ctx->state[i] >> 16) & 0xff;
        hash[i * 4 + 2] = (ctx->state[i] >> 8) & 0xff;
        hash[i * 4 + 3] = ctx->state[i] & 0xff;
    }
}

// Função para calcular HMAC SHA256
void hmac_sha256(const unsigned char *data, int data_len, const unsigned char *key, int key_len, unsigned char *digest) {
    // Preencha a chave para ajustá-la ao tamanho do bloco SHA256
    unsigned char padded_key[SHA256_BLOCK_SIZE];
    if (key_len > SHA256_BLOCK_SIZE) {
        // Se a chave for maior do que o tamanho do bloco, faça o hash da chave
        SHA256_CTX key_ctx;
        sha256_init(&key_ctx);
        sha256_update(&key_ctx, key, key_len);
        sha256_final(&key_ctx, padded_key);
        key_len = SHA256_DIGEST_SIZE;
    } else {
        // Se a chave for menor do que o tamanho do bloco, preencha com zeros
        memcpy(padded_key, key, key_len);
        memset(padded_key + key_len, 0, SHA256_BLOCK_SIZE - key_len);
    }

    // Calcula o valor externo e interno da chave
    unsigned char o_key_pad[SHA256_BLOCK_SIZE];
    unsigned char i_key_pad[SHA256_BLOCK_SIZE];
    for (int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
        o_key_pad[i] = 0x5c ^ padded_key[i];
        i_key_pad[i] = 0x36 ^ padded_key[i];
    }

    // Hash do valor interno
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, i_key_pad, SHA256_BLOCK_SIZE);
    sha256_update(&ctx, data, data_len);
    unsigned char i_hash[SHA256_DIGEST_SIZE];
    sha256_final(&ctx, i_hash);

    // Hash do valor externo
    sha256_init(&ctx);
    sha256_update(&ctx, o_key_pad, SHA256_BLOCK_SIZE);
    sha256_update(&ctx, i_hash, SHA256_DIGEST_SIZE);
    sha256_final(&ctx, digest);
}

// Função para verificar se a mensagem original corresponde ao hash fornecido
int hmac_sha256_verify(const unsigned char *data, int data_len, const unsigned char *key, int key_len, const unsigned char *encoded_data) {
    unsigned char computed_encoded_data[SHA256_DIGEST_SIZE];

    // Recalcula o HMAC-SHA256 com a chave original e a mensagem original
    hmac_sha256(data, data_len, key, key_len, computed_encoded_data);

    // Compara os hashes resultantes
    return memcmp(computed_encoded_data, encoded_data, SHA256_DIGEST_SIZE) == 0;
}

int main() {
    const unsigned char *key = (const unsigned char *)"minhachave";
    const unsigned char *data = (const unsigned char *)"mensagem secreta";
    unsigned char encoded_data[SHA256_DIGEST_SIZE];

    // Codifica a mensagem
    hmac_sha256(data, strlen((char *)data), key, strlen((char *)key), encoded_data);

    // Exibe a mensagem codificada
    printf("Mensagem codificada: ");
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        printf("%02x", encoded_data[i]);
    }
    printf("\n");

    // Verifica se a mensagem original corresponde ao hash fornecido
    if (hmac_sha256_verify(data, strlen((char *)data), key, strlen((char *)key), encoded_data)) {
        printf("A mensagem original foi verificada com sucesso.\n");
    } else {
        printf("A mensagem original não corresponde ao hash fornecido.\n");
    }

    return 0;
}
