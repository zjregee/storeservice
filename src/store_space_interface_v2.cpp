#include "store_space_interface_v2.h"

namespace storeservice {

static StoreSpaceManager* store_space_manager = nullptr;

bool init() {
    store_space_manager = new StoreSpaceManager();
    return true;
}

bool create_store_space(size_t content_space_id, StoreSpaceTemplate t, size_t &store_space_id) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->create_store_space(content_space_id, t, store_space_id);
}

bool get_store_space_ids(size_t content_space_id, std::vector<size_t> &store_space_ids) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->get_store_space_ids(content_space_id, store_space_ids);
}

bool get_store_space_metadata(size_t store_space_id, StoreSpaceMetadata &store_space_metadata) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->get_store_space_metadata(store_space_id, store_space_metadata);
}

bool get_store_space_data(size_t store_space_id, std::vector<std::string> &bucket_name_list) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->get_store_space_data(store_space_id, bucket_name_list);
}

bool get_store_space_data_include_bucket_name(size_t store_space_id, std::vector<std::pair<std::string, std::string>> &bucket_name_pair_list) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->get_store_space_data_include_bucket_name(store_space_id, bucket_name_pair_list);
}

bool register_data(size_t store_space_id, const std::string &bucket_name) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->register_data(store_space_id, bucket_name);
}

bool register_data_with_bucket_name(size_t store_space_id, const std::string &bucket_name, const std::string &bucket_nickname) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->register_data_with_bucket_name(store_space_id, bucket_name, bucket_nickname);
}

bool import_data(size_t store_space_id, const std::string &path) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->import_data(store_space_id, path);
}

bool import_data_with_bucket_name(size_t store_space_id, const std::string &path, const std::string &bucket_nickname) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->import_data_with_bucket_name(store_space_id, path, bucket_nickname);
}

bool get_data_metadata(size_t store_space_id, const std::string &bucket_name, std::vector<std::string> &file_list, std::vector<VarDesc> &var_desc_list) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->get_data_metadata(store_space_id, bucket_name, file_list, var_desc_list);
}

bool get_data_metadata_with_bucket_name(size_t store_space_id, const std::string &bucket_nickname, std::vector<std::string> &file_list, std::vector<VarDesc> &var_desc_list) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->get_data_metadata_with_bucketname(store_space_id, bucket_nickname, file_list, var_desc_list);
}

bool read_var_float(size_t store_space_id, VarInfo var_info, std::vector<float> &var_data) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->read_var_float(store_space_id, var_info, var_data);
}

bool read_var_float_with_bucket_name(size_t store_space_id, VarInfo var_info, std::vector<float> &var_data) {
    if (store_space_manager == nullptr) {
        return false;
    }
    return store_space_manager->read_var_float_with_bucket_name(store_space_id, var_info, var_data);
}

}