# Readers
This folder contains one or more high-level interfaces to the data files written by the fix.

## Fortran
There is a module `mod_velocity_reader` containing the class `velocity_reader`, which can be used like this:
```fortran
program foo
    use mod_velocity_reader
    implicit none

    class(velocity_reader), allocatable :: reader

    reader = velocity_reader("velocityfile.bin")

    write(*,*) reader%Nx, reader%Ny, reader%Nz
    write(*,*) reader%xmin, reader%xmax ! ditto for y, z

    do while (.not. reader%EOF)

        call reader%read_step()

        write(*,*) "Current step:", reader%step

        ! reader%values has dimension 4 x Nz x Ny x Nx

        associate (counts => reader%values(1,:,:,:), &
                   velocities => reader%values(2:4,:,:,:))

            ! do stuff with counts, velocities

        end associate
    end do

    deallocate(reader) ! unless inside procedure or block
end program
```
Simply compile with
```sh
gfortran -o foo /path/to/lammps-velocityaverage/readers/reader.f90 foo.f90
```
