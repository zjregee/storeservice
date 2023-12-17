#pragma once

#include <fstream>

#include "utils.h"
#include "config.h"

namespace storeservice {

class FileCachePool {
public:
    bool get_file(const std::string &file_name, char* &data, size_t &size);
    bool put_file(const std::string &file_name, const char* data, size_t size);
};

}