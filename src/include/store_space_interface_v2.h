#pragma once

#include "common_structure.h"
#include "store_space_template.h"
#include "store_space_manager.h"

namespace storeservice {

bool init();

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

bool get_data_metadata_with_bucket_name(size_t store_space_id, const std::string &bucket_nickname, std::vector<std::string> &file_list, std::vector<VarDesc> &var_desc_list);

bool read_var_float(size_t store_space_id, VarInfo var_info, std::vector<float> &var_data);

bool read_var_float_with_bucket_name(size_t store_space_id, VarInfo var_info, std::vector<float> &var_data);

}