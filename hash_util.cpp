#include <iostream>
#include <sstream>
#include <openssl/md5.h>

#include "hash_util.h"

static std::string md5_digest_to_string(unsigned char digest[MD5_DIGEST_LENGTH])
{
    std::stringstream ss;
    for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
        ss << std::hex << (int)digest[i];
    return ss.str();
}

std::string HashUtil::hash_file(std::string file_path)
{
    FILE *inFile = fopen (file_path.c_str(), "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == nullptr) {
        std::cerr << "Input file '" << file_path << "' cannot be opened." << std::endl;
        exit(1);
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0) {
        MD5_Update (&mdContext, data, bytes);
    }
    unsigned char checksum[MD5_DIGEST_LENGTH];
    MD5_Final (checksum, &mdContext);
    fclose (inFile);

    return md5_digest_to_string(checksum);
}

std::string HashUtil::hash_string_list(std::vector<std::string> string_list)
{
    MD5_CTX mdContext;

    MD5_Init (&mdContext);
    for (auto& it : string_list) {
        MD5_Update (&mdContext, it.c_str(), it.size());
    }
    unsigned char checksum[MD5_DIGEST_LENGTH+1];
    checksum[MD5_DIGEST_LENGTH] = 0;
    MD5_Final (checksum, &mdContext);

    return md5_digest_to_string(checksum);
}
