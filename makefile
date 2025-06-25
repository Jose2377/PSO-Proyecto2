main:
	gcc Cifrado.c -o cifrado
	./cifrado
	mpicc Server-cluster.c -o servidor_mpi
	mpirun -np 4 ./servidor_mpi
	
server:
	mpicc Server-cluster.c -o servidor_mpi
	mpirun -np 4 ./servidor_mpi
	
cifrar:
	gcc Cifrado.c -o cifrado
	./cifrado
	
install:
	sudo apt update
	sudo apt install openmpi-bin
	sudo apt install libopenmpi-dev
	sudo apt install build-essential
