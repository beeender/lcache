#ifndef LINK_INFO_H
#define LINK_INFO_H

#include <string>
#include <set>

class link_info
{
public:
    link_info();
    void set_linker_path(std::string&& path);
    void add_linker_param(std::string&& parm);
    void add_input_file(std::string&& file_path);
    void set_output_path(std::string&& file_path);
    std::string& get_output_path() { return output_file; }

    std::string hash();
private:
    std::string linker_path;
    std::set<std::string> linker_params;
    std::set<std::string> input_files;
    std::string output_file;
};

#endif // LINK_INFO_H
