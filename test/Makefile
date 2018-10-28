all: print

print: print.f90
	gfortran -o $@ -Wall -O3 $<

v.profile: in.test
	mpirun lmp -in $<
