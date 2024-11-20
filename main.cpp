#include <iostream>
#include <chrono>
#include <iomanip>
#include <memory>

#include <openssl/hmac.h>
#include <openssl/evp.h>

inline void hmacNaive(const std::string& key, const unsigned char* buffer, size_t buffer_len, unsigned char* result, size_t& result_len) {
    EVP_MAC* mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
    EVP_MAC_CTX* ctx = EVP_MAC_CTX_new(mac);
    OSSL_PARAM params[] = {
        OSSL_PARAM_construct_utf8_string("digest", "SHA256", 0),
        OSSL_PARAM_END
    };
    EVP_MAC_init(ctx, (const unsigned char *)key.c_str(), key.size(), params);
    EVP_MAC_update(ctx, buffer, buffer_len);
    EVP_MAC_final(ctx, result, &result_len, EVP_MAX_MD_SIZE);
    EVP_MAC_CTX_free(ctx);
    EVP_MAC_free(mac);
}

struct HmacCtx {
    EVP_MAC* mac = nullptr;
    EVP_MAC_CTX* ctx = nullptr;

    ~HmacCtx() {
        EVP_MAC_CTX_free(ctx);
        EVP_MAC_free(mac);
    }

    void init(const std::string& key) {
        mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
        ctx = EVP_MAC_CTX_new(mac);
        OSSL_PARAM params[] = {
            OSSL_PARAM_construct_utf8_string("digest", "SHA256", 0),
            OSSL_PARAM_END
        };
        EVP_MAC_init(ctx, (const unsigned char *)key.c_str(), key.size(), params);
    }
};

inline void hmacReuseCtx(HmacCtx& ctx,const unsigned char* buffer, size_t buffer_len, unsigned char* result, size_t& result_len) {
    EVP_MAC_init(ctx.ctx, NULL, 0, NULL);
    EVP_MAC_update(ctx.ctx, buffer, buffer_len);
    EVP_MAC_final(ctx.ctx, result, &result_len, EVP_MAX_MD_SIZE);
}

void reportTest(const std::string& name, const std::chrono::high_resolution_clock::time_point& start_time,
    const std::chrono::high_resolution_clock::time_point& end_time, const unsigned char* result, size_t result_len) {
    constexpr size_t SEPARATOR_LEN = 60;
    std::string header = "=== " + name + " ";
    header.append(SEPARATOR_LEN - header.size(), '=');
    std::cout << "\n" << header << "\n";

    // can be verified with https://www.akto.io/tools/hmac-sha-256-hash-generator
    std::cout << "Result: ";
    for (size_t i = 0; i < result_len; ++i)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)result[i];
    std::cout << "\n";

    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Done in " << std::dec << duration.count() << " ms\n";

    std::string footer;
    footer.append(SEPARATOR_LEN, '=');
    std::cout << footer << std::endl;
}

int main() {
    const std::string key = "key";

    constexpr size_t BUFFER_SIZE = 30 * 1'000;
    auto buffer = std::make_unique<unsigned char[]>(BUFFER_SIZE);
    for (size_t i = 0; i < BUFFER_SIZE; ++i)
        buffer[i] = 'A';

    unsigned char result[EVP_MAX_MD_SIZE];
    size_t result_len = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    constexpr size_t ITERATIONS = 100'000;
    for (size_t i = 0; i < ITERATIONS; ++i)
        hmacNaive(key, buffer.get(), BUFFER_SIZE, result, result_len);
    auto end_time = std::chrono::high_resolution_clock::now();
    reportTest("Naive HMAC", start_time, end_time, result, result_len);

    start_time = std::chrono::high_resolution_clock::now();
    {
        HmacCtx ctx;
        ctx.init(key);
        for (size_t i = 0; i < ITERATIONS; ++i)
            hmacReuseCtx(ctx, buffer.get(), BUFFER_SIZE, result, result_len);
    }
    end_time = std::chrono::high_resolution_clock::now();
    reportTest("Reuse HMAC ctx", start_time, end_time, result, result_len);

    constexpr size_t CHUNK_SIZE = 1'000;
    start_time = std::chrono::high_resolution_clock::now();
    {
        HmacCtx ctx;
        ctx.init(key);
        for (size_t i = 0; i < ITERATIONS; ++i) {
            for (size_t j = 0; j < BUFFER_SIZE; j += CHUNK_SIZE)
                hmacReuseCtx(ctx, buffer.get() + j, CHUNK_SIZE, result, result_len);
        }
    }
    end_time = std::chrono::high_resolution_clock::now();
    reportTest("Reuse chunked", start_time, end_time, result, result_len);

    return 0;
}
