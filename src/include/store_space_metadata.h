#pragma once

#include "common_structure.h"

namespace storeservice {

struct StoreSpaceMetadata {
    bool writable;
    size_t size;
    size_t capacity;
    size_t max_size;
    StoreSpaceKind kind;
    StoreSpaceConnectConfig connect_config;
};

}