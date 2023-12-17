g++ -O2 ./tools/cmd/cmd.cpp ./src/*.cpp -I./src/include -o ./cmd -lnetcdf -lrados -lssl -lcrypto
./cmd
rm ./cmd