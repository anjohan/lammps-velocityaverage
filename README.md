# lammps-velocityaverage
LAMMPs fix which averages velocities over space and time.

The intent is to avoid having to either dump frequently (causing massive data files) or use the powerful-but-slow `fix ave/chunk`.
