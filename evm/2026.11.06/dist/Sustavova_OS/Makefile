OPTS=-O3 -mfpmath=sse -fstack-protector-all -g \
-W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors \
-Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute \
-Wformat=1 -Wwrite-strings -Wno-long-long -Woverloaded-virtual \
-Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format -std=c++14
all: a.out
a.out: a.o matrix_io.o solve.o mistake.o reduce_sum.o thread_func.o
	g++ a.o matrix_io.o solve.o mistake.o reduce_sum.o thread_func.o -o a.out

a.o: task22.cpp matrix_io.h solve.h
	g++ $(OPTS) -c task22.cpp -o a.o




matrix_io.o: matrix_io.cpp matrix_io.h
	g++ $(OPTS) -c matrix_io.cpp -o matrix_io.o

mistake.o: mistake.cpp mistake.h
	g++ $(OPTS) -c mistake.cpp -o mistake.o

solve.o: solve.cpp solve.h matrix_io.h
	g++ $(OPTS) -c solve.cpp -o solve.o

reduce_sum.o: reduce_sum.cpp reduce_sum.h
	g++ $(OPTS) -c reduce_sum.cpp -o reduce_sum.o

thread_func.o: thread_func.cpp thread_func.h matrix_io.h
	g++ $(OPTS) -c thread_func.cpp -o thread_func.o

clean:
	rm *.o*