#include "store_space_manager.h"

namespace storeservice {

StoreSpaceManager::~StoreSpaceManager() {
    for (auto &entry : storage_pool_) {
        delete entry.second;
    }
}

bool StoreSpaceManager::create_store_space(size_t content_space_id, StoreSpaceTemplate t, size_t &store_space_id) {
    std::lock_guard<std::mutex> lock(lock_);
    store_space_id = generate_store_space_id();
    bind_content_space_id(content_space_id, store_space_id);
    StoreSpaceMetadata store_space_metadata = extract_store_space_metadata(t);
    bind_store_space_id(store_space_id, store_space_metadata);
    if (t.kind == None) {
        return true;
    } else {
        StorageAdaptor* adaptor = get_storage_adaptor(store_space_id);
        if (!adaptor->get_size(store_space_metadata.size)) {
            return false;
        }
        if (!adaptor->get_capacity(store_space_metadata.capacity)) {
            return false;
        }
        bind_store_space_id(store_space_id, store_space_metadata);
        return true;
    }
}

bool StoreSpaceManager::get_store_space_ids(size_t content_space_id, std::vector<size_t> &store_space_ids) {
    std::lock_guard<std::mutex> lock(lock_);
    store_space_ids.clear();
    auto it = content_space_id_map_.find(content_space_id);
    if (it == content_space_id_map_.end()) {
        return false;
    }
    store_space_ids = it->second;
    return true;
}

bool StoreSpaceManager::get_store_space_metadata(size_t store_space_id, StoreSpaceMetadata &store_space_metadata) {
    std::lock_guard<std::mutex> lock(lock_);
    auto it = store_space_id_map_.find(store_space_id);
    if (it == store_space_id_map_.end()) {
        return false;
    }
    store_space_metadata = it->second;
    return true;
}

bool StoreSpaceManager::get_store_space_data(size_t store_space_id, std::vector<std::string> &bucket_name_list) {
    std::lock_guard<std::mutex> lock(lock_);
    bucket_name_list.clear();
    if (!check_store_space_exist(store_space_id)) {
        return false;
    }
    auto it = store_space_data_map_.find(store_space_id);
    if (it == store_space_data_map_.end()) {
        return false;
    }
    bucket_name_list = it->second;
    return true;
}

bool StoreSpaceManager::get_store_space_data_include_bucket_name(size_t store_space_id, std::vector<std::pair<std::string, std::string>> &bucket_name_pair_list) {
    std::lock_guard<std::mutex> lock(lock_);
    bucket_name_pair_list.clear();
    if (!check_store_space_exist(store_space_id)) {
        return false;
    }
    auto it = store_space_data_map_.find(store_space_id);
    if (it == store_space_data_map_.end()) {
        return false;
    }
    for (size_t i = 0; i < it->second.size(); i++) {
        bucket_name_pair_list.emplace_back(std::make_pair(it->second[i], get_bucket_nickname(store_space_id, it->second[i])));
    }
    return true;
}

bool StoreSpaceManager::register_data(size_t store_space_id, const std::string &bucket_name) {
    std::lock_guard<std::mutex> lock(lock_);
    if (!check_store_space_exist(store_space_id)) {
        return false;
    }
    bind_store_space_data(store_space_id, bucket_name);
    return true;
}

bool StoreSpaceManager::register_data_with_bucket_name(size_t store_space_id, const std::string &bucket_name, const std::string &bucket_nickname) {
    std::lock_guard<std::mutex> lock(lock_);
    if (!check_store_space_exist(store_space_id)) {
        return false;
    }
    bind_store_space_data(store_space_id, bucket_name);
    bind_bucket_name(store_space_id, bucket_name, bucket_nickname);
    bind_bucket_nickname(store_space_id, bucket_nickname, bucket_name);
    return true;
}

bool StoreSpaceManager::import_data(size_t store_space_id, const std::string &path) {
    std::lock_guard<std::mutex> lock(lock_);
    if (!check_store_space_exist(store_space_id)) {
        return false;
    }
    std::string bucket_name = generate_random_string(DEFAULT_BUCKET_NAME_SIZE);
    bind_store_space_data(store_space_id, bucket_name);
    StorageAdaptor* adaptor = get_storage_adaptor(store_space_id);
    if (!adaptor->create_bucket(bucket_name)) {
        return false;
    }
    std::vector<std::string> list;
    if (!get_local_dir_file_list(path, list)) {
        return false;
    }
    for (size_t i = 0; i < list.size(); i++) {
        char* data;
        size_t size;
        if (!get_local_file_data(combine_path(path, list[i]), data, size)) {
            return false;
        }
        if (!adaptor->insert_file_to_bucket(bucket_name, list[i], data, size)) {
            return false;
        }
        delete[] data;
    }
    return true;
}

bool StoreSpaceManager::import_data_with_bucket_name(size_t store_space_id, const std::string &path, const std::string &bucket_nickname) {
    std::lock_guard<std::mutex> lock(lock_);
    if (!check_store_space_exist(store_space_id)) {
        return false;
    }
    std::string bucket_name = generate_random_string(DEFAULT_BUCKET_NAME_SIZE);
    bind_store_space_data(store_space_id, bucket_name);
    bind_bucket_name(store_space_id, bucket_name, bucket_nickname);
    bind_bucket_nickname(store_space_id, bucket_nickname, bucket_name);
    StorageAdaptor* adaptor = get_storage_adaptor(store_space_id);
    if (!adaptor->create_bucket(bucket_name)) {
        return false;
    }
    std::vector<std::string> list;
    if (!get_local_dir_file_list(path, list)) {
        return false;
    }
    for (size_t i = 0; i < list.size(); i++) {
        char* data;
        size_t size;
        if (!get_local_file_data(combine_path(path, list[i]), data, size)) {
            return false;
        }
        if (!adaptor->insert_file_to_bucket(bucket_name, list[i], data, size)) {
            return false;
        }
        delete data;
    }
    return true;
}

bool StoreSpaceManager::get_data_metadata(size_t store_space_id, const std::string &bucket_name, std::vector<std::string> &file_list, std::vector<VarDesc> &var_desc_list) {
    std::lock_guard<std::mutex> lock(lock_);
    if (!check_store_space_exist(store_space_id)) {
        return false;
    }
    StorageAdaptor* adaptor = get_storage_adaptor(store_space_id);
    if (!adaptor->get_bucket_file_list(bucket_name, file_list)) {
        return false;
    }
    if (!adaptor->get_bucket_var_desc_list(bucket_name, var_desc_list)) {
        return false;
    }
    return true;
}

bool StoreSpaceManager::get_data_metadata_with_bucketname(size_t store_space_id, const std::string &bucket_nick_name, std::vector<std::string> &file_list, std::vector<VarDesc> &var_desc_list) {
    std::lock_guard<std::mutex> lock(lock_);
    if (!check_store_space_exist(store_space_id)) {
        return false;
    }
    std::string bucket_name = get_bucket_name(store_space_id, bucket_nick_name);
    StorageAdaptor* adaptor = get_storage_adaptor(store_space_id);
    if (!adaptor->get_bucket_file_list(bucket_name, file_list)) {
        return false;
    }
    if (!adaptor->get_bucket_var_desc_list(bucket_name, var_desc_list)) {
        return false;
    }
    return true;
}

bool StoreSpaceManager::read_var_float(size_t store_space_id, VarInfo var_info, std::vector<float> &var_data) {
    std::lock_guard<std::mutex> lock(lock_);
    if (!check_store_space_exist(store_space_id)) {
        return false;
    }
    StorageAdaptor* adaptor = get_storage_adaptor(store_space_id);
    if (!adaptor->read_var_float(var_info, var_data)) {
        return false;
    }
    return true;
}

bool StoreSpaceManager::read_var_float_with_bucket_name(size_t store_space_id, VarInfo var_info, std::vector<float> &var_data) {
    std::lock_guard<std::mutex> lock(lock_);
    if (!check_store_space_exist(store_space_id)) {
        return false;
    }
    var_info.var_addr.bucket_name = get_bucket_name(store_space_id, var_info.var_addr.bucket_name);
    StorageAdaptor* adaptor = get_storage_adaptor(store_space_id);
    if (!adaptor->read_var_float(var_info, var_data)) {
        return false;
    }
    return true;
}

bool StoreSpaceManager::split_store_space(size_t content_space_id, std::vector<size_t> split_content_space_ids, std::vector<std::vector<size_t>> split_store_space_ids) {
    std::lock_guard<std::mutex> lock(lock_);
    auto it = content_space_id_map_.find(content_space_id);
    if (it == content_space_id_map_.end()) {
        return false;
    }
    content_space_id_map_.erase(content_space_id);
    for (size_t i = 0; i < split_content_space_ids.size(); i++) {
        for (size_t j = 0; j < split_store_space_ids.size(); j++) {
            bind_content_space_id(split_content_space_ids[i], split_store_space_ids[i][j]);
        }
    }
    return true;
}

size_t StoreSpaceManager::generate_store_space_id() {
    return current_store_space_id_++;
}

bool StoreSpaceManager::check_store_space_exist(size_t store_space_id) {
    auto it = store_space_id_map_.find(store_space_id);
    return it != store_space_id_map_.end();
}

StoreSpaceMetadata StoreSpaceManager::extract_store_space_metadata(StoreSpaceTemplate t) {
    StoreSpaceMetadata store_space_metadata;
    store_space_metadata.writable = t.writable;
    store_space_metadata.max_size = t.max_size;
    store_space_metadata.kind = t.kind;
    store_space_metadata.connect_config = t.connect_config;
    return store_space_metadata;
}

StorageAdaptor* StoreSpaceManager::get_storage_adaptor(size_t store_space_id) {
    auto it = storage_pool_.find(store_space_id);
    if (it == storage_pool_.end()) {
        StorageAdaptor* adaptor = new StorageAdaptor(store_space_id_map_[store_space_id].kind, store_space_id_map_[store_space_id].connect_config);
        storage_pool_[store_space_id] = adaptor;
        return adaptor;
    } else {
        return it->second;
    }
}

void StoreSpaceManager::bind_content_space_id(size_t content_space_id, size_t store_space_id) {
    std::vector<size_t> store_space_ids;
    auto it = content_space_id_map_.find(content_space_id);
    if (it == content_space_id_map_.end()) {
        store_space_ids.emplace_back(store_space_id);
        content_space_id_map_[content_space_id] = store_space_ids;
    } else {
        store_space_ids = it->second;
        bool is_exist = false;
        for (size_t i = 0; i < store_space_ids.size(); i++) {
            if (store_space_ids[i] == store_space_id) {
                is_exist = true;
                break;
            }
        }
        if (!is_exist) {
            store_space_ids.emplace_back(store_space_id);
            content_space_id_map_[content_space_id] = store_space_ids;
        }
    }
}

void StoreSpaceManager::bind_store_space_id(size_t store_space_id, StoreSpaceMetadata store_space_metadata) {
    store_space_id_map_[store_space_id] = store_space_metadata;
}

void StoreSpaceManager::bind_store_space_data(size_t store_space_id, const std::string &bucket_name) {
    std::vector<std::string> bucket_name_lists;
    auto it = store_space_data_map_.find(store_space_id);
    if (it == store_space_data_map_.end()) {
        bucket_name_lists.emplace_back(bucket_name);
        store_space_data_map_[store_space_id] = bucket_name_lists;
    } else {
        bucket_name_lists = it->second;
        bool is_exist = false;
        for (size_t i = 0; i < bucket_name_lists.size(); i++) {
            if (bucket_name_lists[i] == bucket_name) {
                is_exist = true;
                break;
            }
        }
        if (!is_exist) {
            bucket_name_lists.emplace_back(bucket_name);
            store_space_data_map_[store_space_id] = bucket_name_lists;
        }
    }
}

void StoreSpaceManager::bind_bucket_name(size_t store_space_id, const std::string &bucket_name, const std::string &bucket_nickname) {
    store_space_bucket_name_map_[store_space_id][bucket_name] = bucket_nickname;
}

void StoreSpaceManager::bind_bucket_nickname(size_t store_space_id, const std::string &bucket_nickname, const std::string &bucket_name) {
    store_space_bucket_nickname_map_[store_space_id][bucket_nickname] = bucket_name;
}

std::string StoreSpaceManager::get_bucket_name(size_t store_space_id, const std::string &bucket_name) {
    auto first_it = store_space_bucket_nickname_map_.find(store_space_id);
    if (first_it == store_space_bucket_nickname_map_.end()) {
        return bucket_name;
    } else {
        auto second_it = first_it->second.find(bucket_name);
        if (second_it == first_it->second.end()) {
            return bucket_name;
        } else {
            return second_it->second;
        }
    }
}

std::string StoreSpaceManager::get_bucket_nickname(size_t store_space_id, const std::string &bucket_name) {
    auto first_it = store_space_bucket_name_map_.find(store_space_id);
    if (first_it == store_space_bucket_name_map_.end()) {
        return bucket_name;
    } else {
        auto second_it = first_it->second.find(bucket_name);
        if (second_it == first_it->second.end()) {
            return bucket_name;
        } else {
            return second_it->second;
        }
    }
}

}