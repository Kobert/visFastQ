visFastQ.o: src/visFastQ.c
	gcc src/visFastQ.c src/ref_arg.c src/ref_hash.c src/ref_slidingWindow.c src/ref_math.c src/ref_io.c src/visQ_arg.c -Wall -O3 -lm -o visFastQ

