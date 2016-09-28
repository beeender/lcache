#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    std::string m_cache_path;
    uint16_t jobs;

    static const Config& shared();
};

#endif // CONFIG_H
