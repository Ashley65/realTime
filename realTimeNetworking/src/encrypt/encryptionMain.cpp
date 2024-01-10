#include "../../include/encrypt/encryptAndDecrypt.h"

int main(){
    size_t keySize = 32; // 256 bits
    size_t ivSize = 12;  // 96 bits

    cryptoHandler crypto(keySize, ivSize);

    // Example: Encrypt
    std::vector<unsigned char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
    std::vector<unsigned char> output;
    crypto.performCipherOperation(input, output, true);

    // Example: Decrypt
    std::vector<unsigned char> decryptedOutput;
    crypto.performCipherOperation(output, decryptedOutput, false);

    std::cout << "Plaintext: ";
    for (auto byte : input){
        std::cout << byte;

    }
    std::cout << std::endl;

    std::cout << "Encrypted: ";

    for (auto byte : output){
        std::cout << byte;

    }
    std::cout << std::endl;

    std::cout << "Decrypted: ";
    for (auto byte : decryptedOutput){
        std::cout << byte;

    }
    std::cout << std::endl;

    return 0;

}