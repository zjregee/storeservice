#pragma once

#include <unordered_map>
#include <vector>
#include <mutex>
#include <utility>
#include <fstream>

#include "store_space_metadata.h"
#include "store_space_template.h"
#include "storage_adaptor.h"
#include "utils.h"
#include "config.h"
#include "file_cache_pool.h"

namespace storeservice {

class StoreSpaceManager {
public:
    ~StoreSpaceManager();
    bool create_store_space(size_t content_space_id, StoreSpaceTemplate t, size_t &store_space_id);
    bool get_store_space_ids(size_t content_space_id, std::vector<size_t> &store_space_ids);
    bool get_store_space_metadata(size_t store_space_id, StoreSpaceMetadata &store_space_metadata);
    bool get_store_space_data(size_t store_space_id, std::vector<std::string> &bucket_name_list);
    bool get_store_space_data_include_bucket_name(size_t store_space_id, std::vector<std::pair<std::string, std::string>> &bucket_name_pair_list);
    bool register_data(size_t store_space_id, const std::string &bucket_name);
    bool register_data_with_bucket_name(size_t store_space_id, const std::string &bucket_name, const std::string &bucket_nickname);
    bool import_data(size_t store_space_id, const std::string &path);
    bool import_data_with_bucket_name(size_t store_space_id, const std::string &path, const std::string &bucket_nickname);
    bool get_data_metadata(size_t store_space_id, const std::string &bucket_name, std::vector<std::string> &file_list, std::vector<VarDesc> &var_desc_list);
    bool get_data_metadata_with_bucketname(size_t store_space_id, const std::string &bucket_nick_name, std::vector<std::string> &file_list, std::vector<VarDesc> &var_desc_list);
    bool read_var_float(size_t store_space_id, VarInfo var_info, std::vector<float> &var_data);
    bool read_var_float_with_bucket_name(size_t store_space_id, VarInfo var_info, std::vector<float> &var_data);
    bool split_store_space(size_t content_space_id, std::vector<size_t> split_content_space_ids, std::vector<std::vector<size_t>> split_store_space_ids);

private:
    std::mutex lock_;
    size_t current_store_space_id_ = 0;
    FileCachePool* file_cache_pool_;
    std::unordered_map<size_t, std::vector<size_t>> content_space_id_map_;                                     // content_space_id -> [store_space_id]
    std::unordered_map<size_t, StoreSpaceMetadata> store_space_id_map_;                                        // store_space_id -> store_space_metadata
    std::unordered_map<size_t, std::vector<std::string>> store_space_data_map_;                                // store_space_id -> [bucket_name]
    std::unordered_map<size_t, std::unordered_map<std::string, std::string>> store_space_bucket_name_map_;     // bucket_name -> bucket_nickname
    std::unordered_map<size_t, std::unordered_map<std::string, std::string>> store_space_bucket_nickname_map_; // bucket_nickname -> bucket_name
    std::unordered_map<size_t, StorageAdaptor*> storage_pool_;

    size_t generate_store_space_id();
    bool check_store_space_exist(size_t store_space_id);
    StoreSpaceMetadata extract_store_space_metadata(StoreSpaceTemplate t);
    StorageAdaptor* get_storage_adaptor(size_t store_space_id);
    void bind_content_space_id(size_t content_space_id, size_t store_space_id);
    void bind_store_space_id(size_t store_space_id, StoreSpaceMetadata store_space_metadata);
    void bind_store_space_data(size_t store_space_id, const std::string &bucket_name);
    void bind_bucket_name(size_t store_space_id, const std::string &bucket_name, const std::string &bucket_nickname);
    void bind_bucket_nickname(size_t store_space_id, const std::string &bucket_nickname, const std::string &bucket_name);
    std::string get_bucket_name(size_t store_space_id, const std::string &bucket_name);
    std::string get_bucket_nickname(size_t store_space_id, const std::string &bucket_name);
};

}