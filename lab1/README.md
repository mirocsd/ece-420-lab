# Extracting and Compiling on lab machine
1. Download zip
2. `cd Downloads`
3. `unzip ece-420-lab-main.zip`
4. `scp -r -4 -i ~/.ssh/id_ed25519_ece420 * user-10@kubectl.ece.ualberta.ca:~/src`
5. ssh into machine (`ssh  -4 -i ~/.ssh/id_ed25519_ece420 user-10@kubectl.ece.ualberta.ca`)
6. enter build environment (`podman run -it --rm -v ~/src:/src registry.cloud.ist.ualberta.ca/courses/mpich/builder:latest bash`)
7. `cd src`
8. `make clean && make`
9. `cp main data_input ../`
10. `exit`
11. `kubectl apply -f job-pthread.yaml`
12. wait
