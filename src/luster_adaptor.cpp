#include "luster_adaptor.h"

namespace storeservice {

LusterAdaptor::LusterAdaptor(StoreSpaceConnectConfig connect_config) {
    root_path_ = connect_config.root_path;
    connect();
}

bool LusterAdaptor::connect() {
    return true;
}

bool LusterAdaptor::is_connect() {
    return true;
}

bool LusterAdaptor::get_size(size_t &size) {
    size = calculate_size(root_path_);
    return true;
}

bool LusterAdaptor::get_capacity(size_t &capacity) {
    capacity = std::numeric_limits<size_t>::max();
    return true;
}

bool LusterAdaptor::create_bucket(const std::string &bucket_name) {
    std::string path = combine_path(root_path_, bucket_name);
    return mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}

bool LusterAdaptor::insert_file_to_bucket(const std::string &bucket_name, const std::string &file_name, const char* data, size_t size) {
    std::string path = combine_path(combine_path(root_path_, bucket_name), file_name);
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(data, size);
    return file.good();
}

bool LusterAdaptor::get_bucket_file_list(const std::string &bucket_name, std::vector<std::string> &list) {
    list.clear();
    std::string path = combine_path(root_path_, bucket_name);
    DIR* dir = opendir(path.c_str());
    if (dir == nullptr) {
        return false;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) {
            list.emplace_back(entry->d_name);
        }
    }
    closedir(dir);
    return true;
}

bool LusterAdaptor::get_bucket_var_desc_list(const std::string &bucket_name, std::vector<VarDesc> &desc_list) {
    desc_list.clear();
    std::string path = combine_path(root_path_, bucket_name);
    std::string file_name;
    bool is_find = false;
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        return false;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type = DT_REG && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            file_name = entry->d_name;
            is_find = true;
            break;
        }
    }
    if (!is_find) {
        return true;
    }
    path = combine_path(path, file_name);
    int nc_id;
    if(nc_open(path.c_str(), NC_NOWRITE, &nc_id) != NC_NOERR) {
        return false;
    }
    int num_vars;
    if (nc_inq_nvars(nc_id, &num_vars) != NC_NOERR) {
        nc_close(nc_id);
        return false;
    }
    for (int var_id = 0; var_id < num_vars; var_id++) {
        VarDesc var_desc;
        char var_name[NC_MAX_NAME + 1];
        nc_type var_type;
        int var_ndims;
        int var_dimids[NC_MAX_VAR_DIMS];
        if (nc_inq_var(nc_id, var_id, var_name, &var_type, &var_ndims, var_dimids, NULL) != NC_NOERR) {
            nc_close(nc_id);
            return false;
        }
        var_desc.var_id = var_id;
        var_desc.var_name = var_name;
        switch (var_type) {
            case NC_BYTE:
                var_desc.var_type = "NC_BYTE";
                break;
            case NC_SHORT:
                var_desc.var_type = "NC_SHORT";
                break;
            case NC_INT:
                var_desc.var_type = "NC_INT";
                break;
            case NC_FLOAT:
                var_desc.var_type = "NC_FLOAT";
                break;
            case NC_DOUBLE:
                var_desc.var_type = "NC_DOUBLE";
                break;
            default:
                var_desc.var_type = "NC_UNKNOWN";
        }
        for (int dim = 0; dim < var_ndims; dim++) {
            size_t dim_len;
            if (nc_inq_dimlen(nc_id, var_dimids[dim], &dim_len) != NC_NOERR) {
                nc_close(nc_id);
                return false;
            }
            var_desc.var_dimensions.emplace_back(dim_len);
        }
        desc_list.emplace_back(var_desc);
    }
    nc_close(nc_id);
    return true;
}

bool LusterAdaptor::read_var_float(VarInfo var_info, std::vector<float> &var_data) {
    var_data.clear();
    if (var_info.var_desc.var_type != "NC_FLOAT") {
        return false;
    }
    std::string path = combine_path(combine_path(root_path_, var_info.var_addr.bucket_name), var_info.var_addr.file_name);
    int nc_id;
    if(nc_open(path.c_str(), NC_NOWRITE, &nc_id) != NC_NOERR) {
        return false;
    }
    size_t total_size = 1;
    for (int dim = 0; dim < var_info.var_desc.var_dimensions.size(); dim++) {
        total_size *= var_info.var_desc.var_dimensions[dim];
    }
    std::vector<float> data(total_size);
    if (nc_get_var_float(nc_id, var_info.var_desc.var_id, data.data()) != NC_NOERR) {
        nc_close(nc_id);
        return false;
    }
    var_data = data;
    return true;
}

size_t LusterAdaptor::calculate_size(const std::string &path) {
    DIR* dir = opendir(path.c_str());
    if (dir == nullptr) {
        return 0;
    }
    size_t size = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            if (entry->d_type == DT_REG) {
                std::string file_path = combine_path(path, entry->d_name);
                struct stat file_stat;
                if (stat(file_path.c_str(), &file_stat) == 0) {
                    size += file_stat.st_size;
                }
            } else if (entry->d_type == DT_DIR) {
                std::string dir_path = combine_path(path, entry->d_name);
                size += calculate_size(dir_path);
            }
        }
    }
    closedir(dir);
    return size;
}

}