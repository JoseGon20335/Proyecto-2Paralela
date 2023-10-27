# install
sudo apt-get install libssl-dev

# run
mpirun -np 4 ./bruteforce './encrypted.txt'
./cypher 123456L './input.txt'     

# compile on mac
mpicc -g bruteforce.c -o bruteforce -I/opt/homebrew/Cellar/openssl@1.1/1.1.1w/include -L/opt/homebrew/Cellar/openssl@1.1/1.1.1w/lib -lssl -lcrypto
mpicc -g divideAndConquer.c -o divideAndConquer -I/opt/homebrew/Cellar/openssl@1.1/1.1.1w/include -L/opt/homebrew/Cellar/openssl@1.1/1.1.1w/lib -lssl -lcrypto
gcc-13 -I/opt/homebrew/Cellar/open-mpi/4.1.5/include -L/opt/homebrew/Cellar/open-mpi/4.1.5/lib -L/opt/homebrew/opt/libevent/lib -lmpi -g parallelSegments.c -o parallelSegments -I/opt/homebrew/Cellar/openssl@1.1/1.1.1w/include -L/opt/homebrew/Cellar/openssl@1.1/1.1.1w/lib -lssl -lcrypto -fopenmp

gcc-13 cypher.c -o cypher -I/opt/homebrew/Cellar/openssl@1.1/1.1.1w/include -L/opt/homebrew/Cellar/openssl@1.1/1.1.1w/lib -lssl -lcrypto