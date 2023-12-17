#include "ceph_adaptor.h"

namespace storeservice {

CephAdaptor::CephAdaptor(StoreSpaceConnectConfig connect_config) {
    conf_file_ = connect_config.conf_file;
    rados_ = nullptr;
    if (rados_create(&rados_, "admin") < 0) {
        std::cout << "error" << std::endl;
    }
    if (rados_conf_read_file(rados_, conf_file_.c_str()) < 0) {
        std::cout << "error" << std::endl;
    }
    if (rados_connect(rados_) < 0) {
        std::cout << "error" << std::endl;
    }
}

bool CephAdaptor::connect() {
    return true;
}

bool CephAdaptor::is_connect() {
    return true;
}

bool CephAdaptor::get_size(size_t &size) {
    size = std::numeric_limits<size_t>::max();
    return true;
}

bool CephAdaptor::get_capacity(size_t &capacity) {
    capacity = std::numeric_limits<size_t>::max();
    return true;
}

bool CephAdaptor::create_bucket(const std::string &bucket_name) {
    return !(rados_pool_create(rados_, bucket_name.c_str()) < 0);
}

bool CephAdaptor::insert_file_to_bucket(const std::string &bucket_name, const std::string &file_name, const char* data, size_t size) {
    rados_ioctx_t io_ctx = nullptr;
    if (rados_ioctx_create(rados_, bucket_name.c_str(), &io_ctx) < 0) {
        return false;
    }
    if (rados_write_full(io_ctx, file_name.c_str(), data, size) < 0) {
        return false;
    }
    if (!put_file_in_cache(generate_cache_id(bucket_name, file_name), data, size)) {
        return false;
    }
    rados_ioctx_destroy(io_ctx);
    return true;
}

bool CephAdaptor::get_bucket_file_list(const std::string &bucket_name, std::vector<std::string> &list) {
    list.clear();
    int ret;
    rados_ioctx_t io_ctx = nullptr;
    rados_list_ctx_t list_ctx = nullptr;
    if (rados_ioctx_create(rados_, bucket_name.c_str(), &io_ctx) < 0) {
        return false;
    }
    if (rados_nobjects_list_open(io_ctx, &list_ctx) < 0) {
        return false;
    }
    const char* object_name;
    while ((ret = rados_nobjects_list_next(list_ctx, &object_name, NULL, NULL)) >= 0) {
        list.emplace_back(std::string(object_name));
    }
    if (ret != -ENOENT) {
        return false;
    }
    rados_nobjects_list_close(list_ctx);
    rados_ioctx_destroy(io_ctx);
    return true;
}

bool CephAdaptor::get_bucket_var_desc_list(const std::string &bucket_name, std::vector<VarDesc> &desc_list) {
    desc_list.clear();
    std::vector<std::string> list;
    if (!get_bucket_file_list(bucket_name, list)) {
        return false;
    }
    if (list.size() == 0) {
        return true;
    }
    std::string file_name = list[0];
    std::string cache_id = generate_cache_id(bucket_name, file_name);
    if (!check_file_in_cache(cache_id)) {
        char* data;
        size_t size;
        if (!read_remote_data(bucket_name, file_name, data, size)) {
            return false;
        }
        if (!put_file_in_cache(cache_id, data, size)) {
            return false;
        }
        delete[] data;
    }
    std::string path = combine_path(DEFAULT_REMOTE_FILE_CACHE_PATH, cache_id);
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

bool CephAdaptor::read_var_float(VarInfo var_info, std::vector<float> &var_data) {
    var_data.clear();
    if (var_info.var_desc.var_type != "NC_FLOAT") {
        return false;
    }
    std::string cache_id = generate_cache_id(var_info.var_addr.bucket_name, var_info.var_addr.file_name);
    if (!check_file_in_cache(cache_id)) {
        char* data;
        size_t size;
        if (!read_remote_data(var_info.var_addr.bucket_name, var_info.var_addr.file_name, data, size)) {
            return false;
        }
        if (!put_file_in_cache(cache_id, data, size)) {
            return false;
        }
        delete[] data;
    }
    std::string path = combine_path(DEFAULT_REMOTE_FILE_CACHE_PATH, cache_id);
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

std::string CephAdaptor::generate_cache_id(const std::string &bucket_name, const std::string &file_name) {
    std::string base_string = combine_path(std::to_string(store_space_id_), combine_path(bucket_name, file_name));
    return generate_hash_string(base_string, DEFAULT_REMOTE_FILE_CACHE_NAME_SZIE);
}

bool CephAdaptor::put_file_in_cache(const std::string &hash_id, const char* data, size_t size) {
    std::string path = combine_path(DEFAULT_REMOTE_FILE_CACHE_PATH, hash_id);
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(data, size);
    return file.good();
}

bool CephAdaptor::check_file_in_cache(const std::string &hash_id) {
    std::string path = combine_path(DEFAULT_REMOTE_FILE_CACHE_PATH, hash_id);
    return check_local_file_exist(path);
}

bool CephAdaptor::read_remote_data(const std::string &bucket_name, const std::string &file_name, char* &data, size_t &size) {
    int read_len = DEFAULT_REMOTE_FILE_MAX_SIZE;
    char* read_buf = new char[read_len + 1];
    rados_ioctx_t io_ctx = nullptr;
    rados_completion_t read_completion = nullptr;
    if (rados_ioctx_create(rados_, bucket_name.c_str(), &io_ctx) < 0) {
        return false;
    }
    if (rados_aio_create_completion(NULL, NULL, NULL, &read_completion) < 0) {
        delete[] read_buf;
        return false;
    }
    if (rados_aio_read(io_ctx, file_name.c_str(), read_completion, read_buf, read_len, 0) < 0) {
        delete[] read_buf;
        rados_aio_release(read_completion);
        return false;
    }
    rados_aio_wait_for_complete(read_completion);
    if ((size = rados_aio_get_return_value(read_completion)) < 0) {
        delete[] read_buf;
        rados_aio_release(read_completion);
        return false;
    }
    data = new char[size];
    memcpy(data, read_buf, size);
    delete[] read_buf;
    rados_aio_release(read_completion);
    rados_ioctx_destroy(io_ctx);
    return true;
}

}