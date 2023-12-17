#include "file_cache_pool.h"

namespace storeservice {

bool FileCachePool::get_file(const std::string &file_name, char* &data, size_t &size) {
    return get_local_file_data(combine_path(DEFAULT_REMOTE_FILE_CACHE_PATH, file_name), data, size);
}

bool FileCachePool::put_file(const std::string &file_name, const char* data, size_t size) {
    std::string path = combine_path(DEFAULT_REMOTE_FILE_CACHE_PATH, file_name);
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(data, size);
    return true;
}

}