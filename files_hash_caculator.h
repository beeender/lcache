#ifndef FILESHASHCACULATOR_H
#define FILESHASHCACULATOR_H

#include <set>
#include <string>
#include <thread>
#include <mutex>

class FilesHashCaculator
{
public:
    FilesHashCaculator(std::set<std::string> list);
    std::set<std::string> calculate_hash();

private:
    std::set<std::string> m_file_list;
    std::set<std::string> m_hash_list;
    std::mutex m_mutex;
};

#endif // FILESHASHCACULATOR_H
