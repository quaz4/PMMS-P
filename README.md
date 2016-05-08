# PMMS-P
Parallel Matrix Multiplication Sum: Process - Matrix multiplication sum with multiple processes
Written by William Stewart

# Compiling:
	Included is a make file, run "make" to compile
	You may also wish to run "make clean" first

# Makefile:
	The following commands are available

	make
		Compiles the program

	make clean
		Removes the executable file and all object files

# Running:
	The program takes 5 arguments and can be run in the following format

	./pmms arrayA arrayB M N K

	arrayA
		The file name where array a is stored, see the file format section for more info

	arrayB
		The file name where array b is stored, see the file format section for more info

	M
		The number of rows in array a

	N
		The number of columns in array b or the number of rows in array b

	K
		The number of columns in array b

# File format:
	An array is represented in a file as space seperated integers

	eg. 1 2 3 4
		5 6 7 8