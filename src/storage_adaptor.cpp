#include "storage_adaptor.h"

namespace storeservice {

StorageAdaptor::StorageAdaptor(StoreSpaceKind kind, StoreSpaceConnectConfig config) {
        kind_ = kind;
        config_ = config;
        if (kind_ == Luster) {
            luster_adaptor_ = new LusterAdaptor(config);
        } else if (kind_ == Ceph) {
            ceph_adaptor_ = new CephAdaptor(config);
        }
    }

StorageAdaptor::~StorageAdaptor() {
        delete luster_adaptor_;
        delete ceph_adaptor_;
    }

bool StorageAdaptor::get_size(size_t &size) {
    if (!try_connect()) {
        return false;
    }
    if (kind_ == Luster) {
        return luster_adaptor_->get_size(size);
    } else if (kind_ == Ceph) {
        return ceph_adaptor_->get_size(size);
    } else {
        return false;
    }
}

bool StorageAdaptor::get_capacity(size_t &size) {
    if (!try_connect()) {
        return false;
    }
    if (kind_ == Luster) {
        return luster_adaptor_->get_capacity(size);
    } else if (kind_ == Ceph) {
        return ceph_adaptor_->get_capacity(size);
    } else {
        return false;
    }
}

bool StorageAdaptor::create_bucket(const std::string &bucket_name) {
    if (!try_connect()) {
        return 0;
    }
    if (kind_ == Luster) {
        return luster_adaptor_->create_bucket(bucket_name);
    } else if (kind_ == Ceph) {
        return ceph_adaptor_->create_bucket(bucket_name);
    } else {
        return false;
    }
}

bool StorageAdaptor::insert_file_to_bucket(const std::string &bucket_name, const std::string &file_name, const char* data, size_t size) {
    if (!try_connect()) {
        return 0;
    }
    if (kind_ == Luster) {
        return luster_adaptor_->insert_file_to_bucket(bucket_name, file_name, data, size);
    } else if (kind_ == Ceph) {
        return ceph_adaptor_->insert_file_to_bucket(bucket_name, file_name, data, size);
    } else {
        return false;
    }
}

bool StorageAdaptor::get_bucket_file_list(const std::string &bucket_name, std::vector<std::string> &list) {
    if (!try_connect()) {
        return 0;
    }
    if (kind_ == Luster) {
        return luster_adaptor_->get_bucket_file_list(bucket_name, list);
    } else if (kind_ == Ceph) {
        return ceph_adaptor_->get_bucket_file_list(bucket_name, list);
    } else {
        return false;
    }
}

bool StorageAdaptor::get_bucket_var_desc_list(const std::string &bucket_name, std::vector<VarDesc> &desc_list) {
    if (!try_connect()) {
        return 0;
    }
    if (kind_ == Luster) {
        return luster_adaptor_->get_bucket_var_desc_list(bucket_name, desc_list);
    } else if (kind_ == Ceph) {
        return ceph_adaptor_->get_bucket_var_desc_list(bucket_name, desc_list);
    } else {
        return false;
    }
}

bool StorageAdaptor::read_var_float(VarInfo var_info, std::vector<float> &var_data) {
    if (!try_connect()) {
        return false;
    }
    if (kind_ == Luster) {
        return luster_adaptor_->read_var_float(var_info, var_data);
    } else if (kind_ == Ceph) {
        return luster_adaptor_->read_var_float(var_info, var_data);
    } else {
        return false;
    }
}

bool StorageAdaptor::connect() {
    if (kind_ == Luster) {
        return luster_adaptor_->connect();
    } else if (kind_ == Ceph) {
        return ceph_adaptor_->connect();
    } else {
        return false;
    }
}

bool StorageAdaptor::is_connect() {
    if (kind_ == Luster) {
        return luster_adaptor_->is_connect();
    } else if (kind_ == Ceph) {
        return ceph_adaptor_->is_connect();
    } else {
        return false;
    }
}

bool StorageAdaptor::try_connect() {
    if (!is_connect()) {
        return connect();
    }
    return true;
}

}