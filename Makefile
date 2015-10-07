
all:
	mpic++ -lm main_broad.cc -o mpi_prime_broad
	mpic++ -lm main_send.cc -o mpi_prime_send
clean:
	rm mpi_prime_broad mpi_prime_send
