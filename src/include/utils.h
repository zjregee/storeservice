#pragma once

#include <string>
#include <random>
#include <fstream>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include <openssl/sha.h>

namespace storeservice {

std::string generate_random_string(size_t size);
std::string generate_hash_string(const std::string &base_string, size_t size);
std::string combine_path(const std::string &base_path, const std::string &sub_path);
bool get_local_file_data(const std::string &path, char* &data, size_t &size);
bool get_local_dir_file_list(const std::string &path, std::vector<std::string> &list);
bool check_local_file_exist(const std::string &path);

}