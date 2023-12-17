rm -rf test_space_0
rm -rf test_space_1
rm -rf tmp/file_cache

mkdir test_space_0
mkdir test_space_1
mkdir tmp/file_cache

mkdir test_space_0/test_path
mkdir test_space_1/test_path

cp -r test_data/test_data_0 test_space_0/test_path/test_data
cp -r test_data/test_data_0 test_space_1/test_path/test_data