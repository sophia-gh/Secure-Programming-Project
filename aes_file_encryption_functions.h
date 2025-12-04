/*
*
*          AES CBC mode File Encryption functions
* 
*          implemented using the Crypto++ library https://www.cryptopp.com/wiki/Advanced_Encryption_Standard
*          must install libcryptopp-dev package on linux systems to compile and link properly
*          SecByteBlock must be defined and initialized in the calling code to use as the encryption/decryption key
*          log files are encrypted after a successful logappend function, and decrypted before a logread function then re-encrypted before closing
*
*/
#ifndef AES_FILE_ENCRYPTION_FUNCTIONS_H
#define AES_FILE_ENCRYPTION_FUNCTIONS_H

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
const byte hardcodedKeyData[CryptoPP::AES::DEFAULT_KEYLENGTH] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10 };
inline const SecByteBlock AES_KEY(hardcodedKeyData, sizeof(hardcodedKeyData)); // this is the key that will be used for encryption/decryption

// takes the un-encrypted input file, the file name for the encyrpted file, and the key to use for encryption
bool encryptFile(const std::string &inFile, const std::string &outFile, const SecByteBlock &key);

// takes the encrypted input file, the output file name, and the key to use for decryption
bool decryptFile(const std::string &inFile, const std::string &outFile, const SecByteBlock &key);

// takes two file names and returns true if the files are identical, false otherwise
bool filesAreEqual(const std::string &file1, const std::string &file2);

#endif // AES_FILE_ENCRYPTION_FUNCTIONS_H