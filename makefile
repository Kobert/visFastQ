visFastQ.o: visFastQ.c
	gcc visFastQ.c ref_arg.c ref_primes.c ref_hash.c ref_slidingWindow.c ref_math.c ref_io.c visQ_arg.c -Wall -O3 -lm -o visFastQ
