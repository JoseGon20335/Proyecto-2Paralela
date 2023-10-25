# install
sudo apt-get install libssl-dev

# run
mpirun -np 4 ./bruteforce './encrypted.txt'
./cypher 123456L './input.txt'     

# compile on mac
mpicc -g bruteforce.c -o bruteforce -I/opt/homebrew/Cellar/openssl@1.1/1.1.1w/include -L/opt/homebrew/Cellar/openssl@1.1/1.1.1w/lib -lssl -lcrypto
gcc-13 cypher.c -o cypher -I/opt/homebrew/Cellar/openssl@1.1/1.1.1w/include -L/opt/homebrew/Cellar/openssl@1.1/1.1.1w/lib -lssl -lcrypto