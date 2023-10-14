# install
sudo apt-get install libssl-dev

# run
mpirun ./bruteforce

# compile on mac
mpicc -g -o bruteforce bruteforce.c -I/opt/homebrew/Cellar/openssl@1.1/1.1.1w/include -L/opt/homebrew/Cellar/openssl@1.1/1.1.1w/lib -lssl -lcrypto