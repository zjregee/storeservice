#include <iostream>
#include <sstream>  
#include <string>  
#include <vector>
#include <unordered_map>
#include "store_space_interface_v2.h"

bool execute_command(const std::string &command) {
    std::string sub_command;
    std::stringstream ss(command);
    std::vector<std::string> sub_commands;
    while (ss >> sub_command) {
        sub_commands.push_back(sub_command);
    }
    if (sub_commands.size() == 0) {
        return false;
    }
    if (sub_commands[0] == "create_store_space") {
        size_t content_space_id = std::stoi(sub_commands[1]);
        storeservice::StoreSpaceTemplate t;
        t.writable = true;
        t.max_size = 1024 * 1024 * 1024;
        size_t kind = std::stoi(sub_commands[2]);
        if (kind == 0) {
            t.kind = storeservice::Ceph;
            t.connect_config.conf_file = sub_commands[3];
        }
        if (kind == 1) {
            t.kind = storeservice::Luster;
            t.connect_config.root_path = sub_commands[3];
        }
        size_t store_space_id;
        if (!storeservice::create_store_space(content_space_id, t, store_space_id)) {
            return false;
        }
        std::cout << "content_space_id: " << content_space_id << std::endl;
        std::cout << "store_space_id: " << store_space_id << std::endl;
        std::cout << std::endl << "create store space successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "get_store_space_ids") {
        size_t content_space_id = std::stoi(sub_commands[1]);
        std::vector<size_t> store_space_ids;
        if (!storeservice::get_store_space_ids(content_space_id, store_space_ids)) {
            return false;
        }
        std::cout << "content_space_id: " << content_space_id << std::endl;
        std::cout << "store_space_ids:" << std::endl;
        for (size_t i = 0; i < store_space_ids.size(); i++) {
            std::cout << "store_space_id: " << store_space_ids[i] << std::endl;
        }
        std::cout << std::endl << "get store space ids successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "get_store_spacemetadata") {
        size_t store_space_id = std::stoi(sub_commands[1]);
        storeservice::StoreSpaceMetadata store_space_metadata;
        if (!storeservice::get_store_space_metadata(store_space_id, store_space_metadata)) {
            return false;
        }
        std::cout << "store_space_id: " << store_space_id << std::endl;
        std::cout << "store_space_data:" << std::endl;
        if (store_space_metadata.kind == storeservice::Ceph) {
            std::cout << "kind: Ceph" << std::endl;
            std::cout << "conf_file: " << store_space_metadata.connect_config.conf_file << std::endl;
        }
        if (store_space_metadata.kind == storeservice::Luster) {
            std::cout << "kind: Luster" << std::endl;
            std::cout << "root_path: " << store_space_metadata.connect_config.root_path << std::endl;
        }
        std::cout << "writable: " << store_space_metadata.writable << std::endl;
        std::cout << "size: " << store_space_metadata.size / 1024 / 1024 << " MB" << std::endl;
        std::cout << "capacity: " << store_space_metadata.capacity / 1024 / 1024 << " MB" << std::endl;
        std::cout << "max_size: " << store_space_metadata.max_size / 1024 / 1024 << " MB" << std::endl;
        std::cout << std::endl << "get store space metadata successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "get_store_space_data") {
        size_t store_space_id = std::stoi(sub_commands[1]);
        std::vector<std::string> bucket_name_list;
        if (!storeservice::get_store_space_data(store_space_id, bucket_name_list)) {
            return false;
        }
        std::cout << "store_space_id: " << store_space_id << std::endl;
        std::cout << "store_space_data:" << std::endl;
        for (size_t i = 0; i < bucket_name_list.size(); i++) {
            std::cout << bucket_name_list[i] << std::endl;
        }
        std::cout << std::endl << "get store space data successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "get_store_space_data_include_bucket_name") {
        size_t store_space_id = std::stoi(sub_commands[1]);
        std::vector<std::pair<std::string, std::string>> bucket_name_pair_list;
        if (!storeservice::get_store_space_data_include_bucket_name(store_space_id, bucket_name_pair_list)) {
            return false;
        }
        std::cout << "store_space_id: " << store_space_id << std::endl;
        std::cout << "store_space_data:" << std::endl;
        for (size_t i = 0; i < bucket_name_pair_list.size(); i++) {
            std::cout << "bucket_name: " << bucket_name_pair_list[i].first << " bucket_nickname: " << bucket_name_pair_list[i].second << std::endl;
        }
        std::cout << std::endl << "get store space data include bucket name successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "register_data") {
        size_t store_space_id = std::stoi(sub_commands[1]);
        std::string bucket_name = sub_commands[2];
        if (!storeservice::register_data(store_space_id, bucket_name)) {
            return false;
        }
        std::cout << std::endl << "register data successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "register_data_with_bucket_name") {
        size_t store_space_id = std::stoi(sub_commands[1]);
        std::string bucket_name = sub_commands[2];
        std::string bucket_nickname = sub_commands[3];
        if (!storeservice::register_data_with_bucket_name(store_space_id, bucket_name, bucket_nickname)) {
            return false;
        }
        std::cout << std::endl << "register data with bucket name successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "import_data") {
        size_t store_space_id = std::stoi(sub_commands[1]);
        std::string path = sub_commands[2];
        if (!storeservice::import_data(store_space_id, path)) {
            return false;
        }
        std::cout << std::endl << "import data successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "import_data_with_bucket_name") {
        size_t store_space_id = std::stoi(sub_commands[1]);
        std::string path = sub_commands[2];
        std::string bucket_nickname = sub_commands[3];
        if (!storeservice::import_data_with_bucket_name(store_space_id, path, bucket_nickname)) {
            return false;
        }
        std::cout << std::endl << "import data with bucket name successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "get_data_metadata") {
        size_t store_space_id = std::stoi(sub_commands[1]);
        std::string bucket_name = sub_commands[2];
        std::vector<std::string> file_list;
        std::vector<storeservice::VarDesc> var_desc_list;
        if (!storeservice::get_data_metadata(store_space_id, bucket_name, file_list, var_desc_list)) {
            return false;
        }
        std::cout << "file_list:" << std::endl;
        for (size_t i = 0; i < file_list.size(); i++) {
            std::cout << "file_name: " << file_list[i] << std::endl;
        }
        std::cout << "var_list:" << std::endl;
        for (size_t i = 0; i < var_desc_list.size(); i++) {
            storeservice::VarDesc var = var_desc_list[i];
            std::cout << "variable " << i << ": " << var.var_id << " " << var.var_name << " " << var.var_type << " " << var.var_dimensions.size() << " ";
            for (size_t j = 0; j < var.var_dimensions.size(); j++) {
                std::cout << var.var_dimensions[j];
                if (j == var.var_dimensions.size() - 1) {
                    std::cout << std::endl;
                } else {
                    std::cout << " ";
                }
            }
        }
        std::cout << std::endl << "get data metadata successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "get_data_metadata_with_bucket_name") {
        size_t store_space_id = std::stoi(sub_commands[1]);
        std::string bucket_nickname = sub_commands[2];
        std::vector<std::string> file_list;
        std::vector<storeservice::VarDesc> var_desc_list;
        if (!storeservice::get_data_metadata_with_bucket_name(store_space_id, bucket_nickname, file_list, var_desc_list)) {
            return false;
        }
        std::cout << "file_list:" << std::endl;
        for (size_t i = 0; i < file_list.size(); i++) {
            std::cout << "file_name: " << file_list[i] << std::endl;
        }
        std::cout << "var_list:" << std::endl;
        for (size_t i = 0; i < var_desc_list.size(); i++) {
            storeservice::VarDesc var = var_desc_list[i];
            std::cout << "variable " << i << ": " << var.var_id << " " << var.var_name << " " << var.var_type << " " << var.var_dimensions.size() << " ";
            for (size_t j = 0; j < var.var_dimensions.size(); j++) {
                std::cout << var.var_dimensions[j];
                if (j == var.var_dimensions.size() - 1) {
                    std::cout << std::endl;
                } else {
                    std::cout << " ";
                }
            }
        }
        std::cout << std::endl << "get data metadata with bucket name successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "read_var") {
        
        std::cout << std::endl << "read var successful." << std::endl;
        return true;
    } else if (sub_commands[0] == "read_var_with_bucket_name") {

        std::cout << std::endl << "read var with bucket name successful." << std::endl;
        return true;
    } else {
        std::cout << "Invalid Command." << std::endl;
        return true;
    }
}

int main() {
    std::string command;
    if (!storeservice::init()) {
        return 1;
    }
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        if (command == "exit" || command == "quit" || command == "e" || command == "q") {
            break;
        } else {
            if (!execute_command(command)) {
                std::cout << "Internal Error." << std::endl;
            }
        }
    }
    return 0;
}