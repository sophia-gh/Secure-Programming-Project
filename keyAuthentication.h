/*
    This files stores the hash for the system key and the function to validate it. 
*/


#include "external/bcrypt/bcrypt.h"
#include <string>

#ifndef GENERATED_HASH
#define GENERATED_HASH
const std::string generatedHash = "$2a$12$KctpkL51wGqtlQQpFut0ZuxZkp/d/Wr08BYepLsCZyi5g7vfNhGsS";
#endif

bool validateKey(const std::string & key) {
    return bcrypt::validatePassword(key, generatedHash);
}   
