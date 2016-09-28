#ifndef HASHUTIL_H
#define HASHUTIL_H

#include <string>
#include <vector>

class HashUtil
{
public:
    static std::string hash_file(std::string file_path);
    static std::string hash_string_list(std::vector<std::string> string_list);
private:
    HashUtil() = delete;
};

#endif // HASHUTIL_H
