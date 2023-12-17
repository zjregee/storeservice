#pragma once

#include "common_structure.h"

namespace storeservice {

struct StoreSpaceTemplate {
    bool writable;
    size_t max_size;
    StoreSpaceKind kind;
    StoreSpaceConnectConfig connect_config;
};

}