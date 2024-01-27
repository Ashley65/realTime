//
// Created by NIgel work on 09/01/2024.
//

#ifndef REALTIME_ENCRYPTANDDECRYPT_H
#define REALTIME_ENCRYPTANDDECRYPT_H
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <iostream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/rsa.h>


class cryptoHandler{

    EVP_PKEY *pkey_;


public:
    cryptoHandler(size_t keySize, size_t ivSize) : keySize_(keySize), ivSize_(ivSize) {
        key_.resize(keySize_);
        iv_.resize(ivSize_);

        generateIV();
        generateKey();
        generateKeyPair();
        initializeContexts();
    }
    cryptoHandler() = default;

    void generateKeyPair();

    ~cryptoHandler() {
        EVP_CIPHER_CTX_free(encryptCtx_);
        EVP_CIPHER_CTX_free(decryptCtx_);
    }

    void performCipherOperation(const std::vector<unsigned char>& input,
                                std::vector<unsigned char>& output, bool encrypt);

private:
    size_t keySize_;
    size_t ivSize_;
    std::vector<unsigned char> key_;
    std::vector<unsigned char> iv_;
    EVP_CIPHER_CTX* encryptCtx_;
    EVP_CIPHER_CTX* decryptCtx_;

    void generateKey();

    void generateIV();

    void initializeContexts();

};

#endif //REALTIME_ENCRYPTANDDECRYPT_H
