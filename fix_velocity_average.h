/* syntax:
 * fix <fix-name> <group-name> velocityaverage <outfile> sample_frequency write_frequency
 *     Nx Ny Nz xmin xmax ymin ymax zmin zmax
 */

#ifdef FIX_CLASS

FixStyle(velocityaverage, FixVelocityAverage)

#else

#ifndef LMP_FIX_VELOCITY_AVERAGE_H
#define LMP_FIX_VELOCITY_AVERAGE_H

#include "fix.h"

namespace LAMMPS_NS {

class FixVelocityAverage : public Fix {
    public:
        FixVelocityAverage(class LAMMPS*, int, char **);
        ~FixVelocityAverage();
        virtual int setmask();
        virtual void end_of_step();
        virtual void setup(int);

    protected:
        int Nx, Ny, Nz, Ntot;
        double xmin, xmax, dx, \
               ymin, ymax, dy, \
               zmin, zmax, dz;

        FILE* outfile;
        double* data;
        double* reduced_data;

        int sample_freq, write_freq, samples, samples_per_write;

        inline int index(int, int, int, int);
        void zero_data();
};

}

#endif
#endif
