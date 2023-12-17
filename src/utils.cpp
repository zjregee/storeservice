#include "utils.h"

namespace storeservice {

std::string generate_random_string(size_t size) {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, characters.size() - 1);
    std::string result;
    for (size_t i = 0; i < size; ++i) {
        result += characters[distribution(generator)];
    }
    return result;
}

std::string generate_hash_string(const std::string &base_string, size_t size) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, base_string.c_str(), base_string.length());
    SHA256_Final(hash, &sha256);
    std::ostringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0');
    for (size_t i = 0; i < size / 2 && i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << static_cast<int>(hash[i]);
    }
    return ss.str();
}

std::string combine_path(const std::string &base_path, const std::string &sub_path) {
    std::string new_sub_path = sub_path;
    if (!sub_path.empty() && sub_path.front() == '/') {
        if (sub_path.size() == 1) {
            new_sub_path = "";
        } else {
            new_sub_path = sub_path.substr(1);
        }
    }
    if (base_path.empty() || base_path.back() == '/') {
        return base_path + new_sub_path;
    } else {
        return base_path + "/" + new_sub_path;
    }
}

bool get_local_file_data(const std::string &path, char* &data, size_t &size) {
    struct stat fstat;
    if (stat(path.c_str(), &fstat) != 0) {
        return false;
    }
    size = fstat.st_size;
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    data = new char[size];
    file.read(data, size);
    return file.good();
}

bool get_local_dir_file_list(const std::string &path, std::vector<std::string> &list) {
    list.clear();
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

bool check_local_file_exist(const std::string &path) {
    struct stat fstat;
    return stat(path.c_str(), &fstat) == 0;
}

}