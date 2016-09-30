#include <algorithm>
#include <vector>

#include "link_info.h"
#include "files_hash_caculator.h"
#include "hash_util.h"

link_info::link_info()
{
}


void link_info::set_linker_path(std::string&& path)
{
    linker_path = path;
}

void link_info::add_linker_param(std::string&& param)
{
    linker_params.insert(param);
}

void link_info::add_input_file(std::string&& file_path)
{
    input_files.insert(file_path);
}

void link_info::set_output_path(std::string&& file_path)
{
    output_file = file_path;
}

std::string link_info::hash()
{
    FilesHashCaculator hash_caculator(input_files);

    std::vector<std::string> string_list(linker_params.size() + input_files.size());
    std::copy(linker_params.begin(), linker_params.end(), string_list.begin());

    auto file_hash_list = hash_caculator.calculate_hash();
    std::copy(file_hash_list.begin(), file_hash_list.end(), string_list.begin() + linker_params.size());

    return  HashUtil::hash_string_list(string_list);
}
