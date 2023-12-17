#pragma once

#include <netcdf>
#include <iostream>
#include <rados/librados.h>

#include "utils.h"
#include "file_cache_pool.h"
#include "common_structure.h"

namespace storeservice {

class CephAdaptor {
public:
    CephAdaptor(StoreSpaceConnectConfig connect_config);
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
    size_t store_space_id_;
    std::string conf_file_;
    rados_t rados_;
    FileCachePool* file_cache_pool_;

    std::string generate_cache_id(const std::string &bucket_name, const std::string &file_name);
    bool put_file_in_cache(const std::string &hash_id, const char* data, size_t size);
    bool check_file_in_cache(const std::string &hash_id);
    bool read_remote_data(const std::string &pool_name, const std::string &object_name, char* &data, size_t &size);
};

}