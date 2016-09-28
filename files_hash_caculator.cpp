#include "files_hash_caculator.h"
#include "hash_util.h"

FilesHashCaculator::FilesHashCaculator(std::set<std::string> list)
    : m_file_list(list)
{
}

std::set<std::string> FilesHashCaculator::calculate_hash()
{
    for (auto& it : m_file_list) {
        m_hash_list.insert(HashUtil::hash_file(it));
    }

    return m_hash_list;
}
