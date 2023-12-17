g++ -o luster_adaptor_test ./tests/adaptor_test/luster_adaptor_test.cpp -lnetcdf
./luster_adaptor_test
rm luster_adaptor_test

g++ -o ceph_adaptor_test ./tests/adaptor_test/ceph_adaptor_test.cpp -lnetcdf -lrados
./ceph_adaptor_test
rm ceph_adaptor_test

g++ -o store_space_interface_v2_test ./tests/interface_test/store_space_interface_v2_test.cpp -lnetcdf -lrados
./store_space_interface_v2_test
rm store_space_interface_v2_test