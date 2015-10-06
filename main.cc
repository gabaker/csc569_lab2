#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <mpi.h>

void is_prime(long long number) {
   printf("%lld is prime!\n", number);
}

void not_prime(long long number, long long divisor) {
   printf("%lld is not prime because %lld and %lld are divisors.\n", number, divisor, number / divisor);
}

int main(int argc, char **argv) {  
   MPI_Init(&argc, &argv);
   
   int rank = 0, size = 0;
   
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);

   unsigned long long bool_is_prime = 0;                                               //value that makes number not prime; 
   unsigned long long number = atoll(argv[1]);
   int bool_debug = 0;

   if (argc == 3)
      bool_debug = atoi (argv[2]);

   if ( rank == 0 ) {      //These checks fall under the head rank
      //printf("Max value is %lld\n", LLONG_MAX);
      if (number <= 1)     //numbers <= one are not prime
         bool_is_prime = 1;
      if (number == 2)     //two is prime
         is_prime(number);
      if (!(number % 2))   //numbers that are even are not prime (except 2)
         bool_is_prime = 2;
   }
   unsigned long long up_to_this_number = ceil(sqrtl((long double) number));  //max number needed for checking
   unsigned long long range = ceil((up_to_this_number - 3) / size);                    //range for each node
   unsigned long long start = 3;                                                       //start value for each nodes range
                                                                                       //if 0 then a value has not been found
   //Set start value of test range for each node
   if (rank != 0) //if rank isn't head rank then set start; else start is 3 for head rank
      start = (range * rank) + 3;
   if (!(start % 2)) //make sure start value is odd
      start++;

   //Set end value of test range for each node
   unsigned long long end = range + start;

   if (rank + 1 == size) //if last rank make sure range ends on highest possible value
      end = up_to_this_number;

   //DEBUG CODE HERE FOR RANK RANGES
   if (bool_debug)
      printf("Rank %d of %d checking if %lld is prime from ranges: %lld to %lld\n", rank, size, number, start, end);

   for (unsigned long long idx = start; idx <= end;  idx += 2) { //check ranges
      if (number % idx == 0) {
         bool_is_prime = idx;
         break;   
      }
   }
   if (rank == 0) {     
      unsigned long long bool_is_prime_list[size];
      bool_is_prime_list[0] = bool_is_prime;
      for (int rank_idx = 1; rank_idx < size; rank_idx++) {
         MPI_Recv(&bool_is_prime_list[rank_idx], 1, MPI_UNSIGNED_LONG_LONG, rank_idx, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //MPI_Recv in order from comm rank 1 to Max comm rank (size) 
      }
      for (int rank_idx = 0; rank_idx < size; rank_idx++) {
         if (bool_is_prime_list[rank_idx] != 0) {
            not_prime(number, bool_is_prime_list[rank_idx]);
            bool_is_prime = bool_is_prime_list[rank_idx];
            break;
         }
      }
      if (bool_is_prime == 0)
         is_prime(number);
   } else {
      MPI_Send(&bool_is_prime, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
   }
   
   MPI_Finalize();
   return 0;
}
