//
// Created by NIgel work on 09/01/2024.
//

#include "../../include/encrypt/encryptAndDecrypt.h"


void cryptoHandler::generateKey() {
    if (RAND_bytes(key_.data(), keySize_) != 1){
        std:: cerr<< "Error generating key."<<std::endl;
        // Handle error

    }

}

void cryptoHandler::generateIV() {
    if (RAND_bytes(iv_.data(), ivSize_) != 1){
        std:: cerr<< "Error generating IV."<<std::endl;
        // Handle error

    }
}

void cryptoHandler::initializeContexts() {
    encryptCtx_ = EVP_CIPHER_CTX_new();
    if (EVP_EncryptInit_ex(encryptCtx_, EVP_aes_256_gcm(), nullptr, key_.data(), iv_.data()) != 1){
        std:: cerr<< "Error initializing encryption context."<<std::endl;
        // Handle error

    }
    decryptCtx_ = EVP_CIPHER_CTX_new();
    if (EVP_DecryptInit_ex(decryptCtx_, EVP_aes_256_gcm(), nullptr, key_.data(), iv_.data()) != 1){
        std:: cerr<< "Error initializing decryption context."<<std::endl;
        // Handle error

    }

}

void cryptoHandler::performCipherOperation(const std::vector<unsigned char>& input,
                                           std::vector<unsigned char>& output, bool encrypt){
    EVP_CIPHER_CTX* ctx = encrypt ? encryptCtx_ : decryptCtx_;

    int updateLength = 0;
    int finalLength = 0;

    // determine the size of the output

    int maxOutputLength = input.size() + EVP_CIPHER_CTX_block_size(ctx);
    output.resize(maxOutputLength);


    if (encrypt){
        // perform encryption
        if (EVP_EncryptUpdate(ctx, output.data(), &updateLength, input.data(), input.size()) != 1){
            std:: cerr<< "Error performing encryption."<<std::endl;
            // Handle error

        }
    }else{
        // perform decryption
        if (EVP_DecryptUpdate(ctx, output.data(), &updateLength, input.data(), input.size()) != 1){
            std:: cerr<< "Error performing decryption."<<std::endl;
            // Handle error

        }
    }

    // finalize the encryption or decryption
    if (encrypt) {
        if (EVP_EncryptFinal_ex(ctx, output.data() + updateLength, &finalLength) != 1) {
            std::cerr << "Error finalizing encryption." << std::endl;
            // Handle error

        }
    }else{
        if (EVP_DecryptFinal_ex(ctx, output.data() + updateLength, &finalLength) != 1) {
            std::cerr << "Error finalizing decryption." << std::endl;
            // Handle error

        }
    }

    // set the size of the output
    output.resize(updateLength + finalLength);
}

cryptoHandler::cryptoHandler() {
}
