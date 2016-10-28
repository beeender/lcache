#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>
#include <libgen.h>


#include "link_info.h"
#include "config.h"
#include "version.h"

static Config s_config;

const Config& Config::shared() {
    return s_config;
}

static link_info parse(int argc, char *argv[]) {
    link_info info;
    int cur_index = 0;

    if (cur_index < argc) {
        info.set_linker_path(std::string(argv[cur_index]));
        cur_index++;
    } else {
        std::cerr << "Cannot identify the path to the linker." << std::endl;
        exit(1);
    }

    while(cur_index < argc) {
        if (strcmp("-o", argv[cur_index]) == 0) {
            cur_index++;
            if (cur_index < argc) {
                info.set_output_path(argv[cur_index]);
                cur_index++;
            } else {
                std::cerr << "No output file for '-o'." << std::endl;
                exit(1);
            }
        } else if (argv[cur_index] == strstr(argv[cur_index], "--output")) {
            const char* tmp_path = strstr(argv[cur_index], "=");
            if (tmp_path == nullptr) {
                std::cerr << "No output file for '--output'." << std::endl;
                exit(1);
            } else {
                info.set_output_path(tmp_path + 1);
            }
            cur_index++;
        } else if (argv[cur_index][0] == '-') {
            info.add_linker_param(argv[cur_index]);
            cur_index++;
        } else {
            info.add_input_file(argv[cur_index]);
            cur_index++;
        }
    }

    return info;
}

static int execute(char *const argv[])
{
    int pid = fork();
    if (pid == 0) {
        exit(execv(argv[0], argv));
    }

    int status;
    if (waitpid(pid, &status, 0) != pid) {
        std::cerr << "waitpid failed: " << strerror(errno) << std::endl;
        exit(1);
    }
    return WEXITSTATUS(status);
}

static bool from_cache(const std::string& hash_str, const std::string& output_path)
{
    std::string dir = s_config.m_cache_path + "/" + hash_str + "/";
    std::string path = dir + basename(const_cast<char*>(output_path.c_str()));
    struct stat dir_stat;
    if ((stat(path.c_str(), &dir_stat) != 0)) {
        std::cout << "Cache file for " << output_path << " doesn't exist." << std::endl;
        return false;
    }

    std::ifstream  src(path, std::ios::binary);
    std::ofstream  dst(output_path, std::ios::binary);

    dst << src.rdbuf();
    return true;
}

static void to_cache(const std::string& hash_str, const std::string& output_path, char* argv[]) {
    int status = execute(argv);
    if (WEXITSTATUS(status) != 0) {
        exit(status);
    }

    std::string dir = s_config.m_cache_path + "/" + hash_str + "/";
    std::string path = dir + basename(const_cast<char*>(output_path.c_str()));
    struct stat dir_stat;
    if ((stat(dir.c_str(), &dir_stat) != 0)) {
        status = mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (status) {
            std::cerr << "Failed to create directory " << dir << "." << std::endl;
            exit(1);
        }
    }

    std::ifstream  src(output_path, std::ios::binary);
    std::ofstream  dst(path, std::ios::binary);

    dst << src.rdbuf();
}

// Check if the given param string is path to the excutable file of ccache.
static bool is_ccache(const char param[]) {
    /*
    struct stat file_stat;
    if ((stat(param, &file_stat) != 0)) {
        // Doesn't exist?
        return false;
    }

    if (!(file_stat.st_mode & S_IXUSR)) {
        // Not executable.
        return false;
    }
    */

    // FIXME: Make a more completed version to check ccache.
    if (strstr(param, "ccache") != nullptr) {
        return true;
    }

    return false;
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp("-v", argv[1]) == 0) {
        std::cout << "lcache version: " << VERSION << std::endl;
        exit(0);
    }

    // Set the path to store cached files
    std::string lcache_dir_str;
    char* lcache_dir_env = getenv("LCACHE_DIR");
    if (lcache_dir_env == nullptr) {
        char* home_dir = getenv("HOME");
        std::stringstream ss;
        ss << (home_dir == nullptr ? "" : home_dir) << "/.lcache";
        lcache_dir_str = std::move(ss.str());
    } else {
        lcache_dir_str = lcache_dir_env;
    }


    struct stat dir_stat;
    if ((stat(lcache_dir_str.c_str(), &dir_stat) != 0) || (((dir_stat.st_mode) & S_IFMT) != S_IFDIR)) {
        int status;
        status = mkdir(lcache_dir_str.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (status) {
            std::cerr << "Failed to create directory " << lcache_dir_str << "." << std::endl;
            exit(1);
        }
    }
    s_config.m_cache_path = std::move(lcache_dir_str);

    if (argc > 0 && is_ccache(argv[1])) {
        argc -= 1;
        argv += 1;
    }

    link_info info = parse(argc - 1, argv + 1);
    std::string hash = info.hash();

    if (!from_cache(hash, info.get_output_path())) {
        to_cache(hash, info.get_output_path(), argv + 1);
    }
    return 0;
}

