#pragma once

#include "common_structure.h"
#include "ceph_adaptor.h"
#include "luster_adaptor.h"

namespace storeservice {

class StorageAdaptor {
public:
    StorageAdaptor(StoreSpaceKind kind, StoreSpaceConnectConfig config);
    ~StorageAdaptor();
    bool get_size(size_t &size);
    bool get_capacity(size_t &size);
    bool create_bucket(const std::string &bucket_name);
    bool insert_file_to_bucket(const std::string &bucket_name, const std::string &file_name, const char* data, size_t size);
    bool get_bucket_file_list(const std::string &bucket_name, std::vector<std::string> &list);
    bool get_bucket_var_desc_list(const std::string &bucket_name, std::vector<VarDesc> &desc_list);
    bool read_var_float(VarInfo var_info, std::vector<float> &var_data);

private:
    StoreSpaceKind kind_;
    StoreSpaceConnectConfig config_;
    LusterAdaptor* luster_adaptor_;
    CephAdaptor* ceph_adaptor_;

    bool connect();
    bool is_connect();
    bool try_connect();
};

}