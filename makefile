main:
	mpicc Cifrado.c -o cifrado
	./cifrado
	mpicc Server-cluster.c -o servidor_mpi
	mpirun -np 4 ./servidor_mpi
	
server:
	mpicc Server-cluster.c -o servidor_mpi
	mpirun -np 4 ./servidor_mpi
	
cifrar:
	mpicc Cifrado.c -o cifrado
	./cifrado
