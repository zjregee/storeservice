#pragma once

#include <limits>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <netcdf>

#include "utils.h"
#include "common_structure.h"

namespace storeservice {

class LusterAdaptor {
public:
    LusterAdaptor(StoreSpaceConnectConfig connect_config);
    bool connect();
    bool is_connect();
    bool get_size(size_t &size);
    bool get_capacity(size_t &capacity);
    bool create_bucket(const std::string &bucket_name);
    bool insert_file_to_bucket(const std::string &bucket_name, const std::string &file_name, const char* data, size_t size);
    bool get_bucket_file_list(const std::string &bucket_name, std::vector<std::string> &list);
    bool get_bucket_var_desc_list(const std::string &bucket_name, std::vector<VarDesc> &desc_list);
    bool read_var_float(VarInfo var_info, std::vector<float> &var_data);

private:
    size_t size_;
    size_t capacity_;
    std::string root_path_;
    
    size_t calculate_size(const std::string &path);
};

}