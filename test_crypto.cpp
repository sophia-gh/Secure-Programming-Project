#include <cryptopp/files.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>

#include <iostream>
#include <fstream>
#include <string>

using namespace CryptoPP;

// takes the input file, the output file name, and the key to use for encryption
bool encryptFile(const std::string &inFile, const std::string &outFile, const SecByteBlock &key) {
    try {
        AutoSeededRandomPool prng; // secure random number generator
        byte iv[AES::BLOCKSIZE];  // creates an array to hold the initialization vector with the size of the aes block size
        prng.GenerateBlock(iv, sizeof(iv)); // generate the initialization vector

        CBC_Mode<AES>::Encryption enc; // create cbc mode aes encryption object
        enc.SetKeyWithIV(key, key.size(), iv); // set the key and initialization vector with the encryption object

        // Read input file as vector plaintext
        std::ifstream fin(inFile, std::ios::binary);
        std::vector<byte> plaintext((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        fin.close();
 
        //open output file and write iv
        std::ofstream fout(outFile, std::ios::binary);
        if (!fout) return false;
        fout.write((char*)iv, AES::BLOCKSIZE);

        // Encrypt file contents -- use array source instead of file source to avoid skipping parts of the file due to iv at start
        // us array source object supplied by cyrpto++ library to read from a file, initalized with input file name
        // stream transformation filter object supplied by crypto++ library for handling padding and other transformations
            // initialized with the encryption object and a file sink object, supplied by crypto++ to write to the output file
        // array source object reads from the plaintext vector, processes it through the stream transformation filter, which encrypts and writes to the output file
        ArraySource(plaintext.data(), plaintext.size(), true, new StreamTransformationFilter(enc, new FileSink(fout)));

    } catch (const Exception &e) {
        // if the above operations fail, print the error message
        std::cerr << "Encryption error: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool decryptFile(const std::string &inFile, const std::string &outFile, const SecByteBlock &key)
{
    try {
        // read the initialization vector from the beginning of the encrypted file
        std::ifstream in(inFile, std::ios::binary);
        if (!in) return false;

        byte iv[AES::BLOCKSIZE];
        in.read((char*)iv, sizeof(iv));

        // Read the rest of the file into a buffer
        std::vector<byte> ciphertext((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        in.close();

        CBC_Mode<AES>::Decryption dec;
        dec.SetKeyWithIV(key, key.size(), iv);

        // Decrypt
        ArraySource(ciphertext.data(), ciphertext.size(), true, new StreamTransformationFilter(dec, new FileSink(outFile.c_str())));

    } catch (const Exception &e) {
        std::cerr << "Decryption error: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool filesAreEqual(const std::string &file1, const std::string &file2) {
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);

    if (!f1 || !f2) return false;

    std::string content1((std::istreambuf_iterator<char>(f1)), std::istreambuf_iterator<char>());
    std::string content2((std::istreambuf_iterator<char>(f2)), std::istreambuf_iterator<char>());

    return content1 == content2;
}

// ----------------- Main Test Program -----------------

int main() {
    std::string inputFile = "log1.txt";
    std::string encryptedFile = "test_encrypted.enc";
    std::string decryptedFile = "test_decrypted.txt";


    // Generate AES-256 key
    SecByteBlock key(32);
    AutoSeededRandomPool prng;
    prng.GenerateBlock(key, key.size());

    // Encrypt
    if (!encryptFile(inputFile, encryptedFile, key)) {
        std::cerr << "Encryption failed!\n";
        return 1;
    }
    std::cout << "Encryption succeeded.\n";

    // Decrypt
    if (!decryptFile(encryptedFile, decryptedFile, key)) {
        std::cerr << "Decryption failed!\n";
        return 1;
    }
    std::cout << "Decryption succeeded.\n";

    // Verify
    if (filesAreEqual(inputFile, decryptedFile)) {
        std::cout << "SUCCESS: Decrypted file matches original.\n";
    } else {
        std::cerr << "FAILURE: Decrypted file does NOT match original.\n";
    }

    return 0;
}