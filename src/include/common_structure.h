#pragma once

#include <string>
#include <vector>

namespace storeservice {

enum StoreSpaceKind {
    Ceph,
    Luster,
    None,
};

struct StoreSpaceConnectConfig {
    std::string conf_file;
    std::string root_path;
};

struct VarDesc {
    size_t var_id;
    std::string var_name;
    std::string var_type;
    std::vector<size_t> var_dimensions;
};

struct VarAddr {
    std::string bucket_name;
    std::string file_name;
};

struct VarInfo {
    VarDesc var_desc;
    VarAddr var_addr;
};

}