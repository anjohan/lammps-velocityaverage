#include "fix_velocity_average.h"
#include <cstdlib>
#include "error.h"
#include "fix.h"
#include "atom.h"
#include "update.h"
#include <cstdio>

using namespace LAMMPS_NS;

FixVelocityAverage::FixVelocityAverage(LAMMPS *lmp, int narg, char **arg) :
    Fix(lmp, narg, arg)
{
    if (narg < 15) {
        error->all(FLERR, "Illegal fix velocityaverage command");
    }


    nevery = sample_freq = atoi(arg[4]);
    write_freq = atoi(arg[5]);
    samples_per_write = write_freq/sample_freq;

    Nx = atoi(arg[6]);
    Ny = atoi(arg[7]);
    Nz = atoi(arg[8]);
    Ntot = 4*Nx*Ny*Nz;

    xmin = atof(arg[9]); xmax = atof(arg[10]);
    ymin = atof(arg[11]); ymax = atof(arg[12]);
    zmin = atof(arg[13]); zmax = atof(arg[14]);

    dx = (xmax - xmin) / Nx;
    dy = (ymax - ymin) / Ny;
    dz = (zmax - zmin) / Nz;

    data = new double[Ntot];
    zero_data();
    samples = 0;

    int rank;
    MPI_Comm_rank(world, &rank);

    if (rank == 0) {
        reduced_data = new double[Ntot];

        outfile = fopen(arg[3], "wb");

        fwrite(&Nx, sizeof(int), 1, outfile);
        fwrite(&Ny, sizeof(int), 1, outfile);
        fwrite(&Nz, sizeof(int), 1, outfile);

        fwrite(&xmin, sizeof(double), 1, outfile);
        fwrite(&xmax, sizeof(double), 1, outfile);
        fwrite(&ymin, sizeof(double), 1, outfile);
        fwrite(&ymax, sizeof(double), 1, outfile);
        fwrite(&zmin, sizeof(double), 1, outfile);
        fwrite(&zmax, sizeof(double), 1, outfile);
    }
}

int FixVelocityAverage::setmask(){
    int mask = 0;
    mask |= FixConst::END_OF_STEP;
    return mask;
}

void FixVelocityAverage::end_of_step(){
    double **x = atom->x;
    double **v = atom->v;
    int *mask = atom->mask;
    int nlocal = atom->nlocal;

    for(int a = 0; a < nlocal; a++){
        if (x[a][0] < xmin || x[a][0] >= xmax || \
            x[a][1] < ymin || x[a][1] >= ymax || \
            x[a][2] < zmin || x[a][2] >= zmax || \
            !(mask[a] & groupbit)) {
            continue;
        }

        int i = (x[a][0] - xmin)/dx;
        int j = (x[a][1] - ymin)/dy;
        int k = (x[a][2] - zmin)/dz;

        int idx = index(i,j,k,0);
        data[idx] += 1;
        data[idx + 1] += v[a][0];
        data[idx + 2] += v[a][1];
        data[idx + 3] += v[a][2];
    }

    samples++;

    if (samples < samples_per_write){
        return;
    }

    int rank;

    MPI_Comm_rank(world, &rank);
    MPI_Reduce(data, reduced_data, Ntot, MPI_DOUBLE, MPI_SUM, 0, world);

    if (rank == 0) {
        for(int i = 0; i < Nx; i++){
            for(int j = 0; j < Ny; j++){
                for(int k = 0; k < Nz; k++){
                    int idx = index(i,j,k,0);

                    if (reduced_data[idx] == 0) {
                        continue;
                    }

                    reduced_data[idx + 1] /= reduced_data[idx];
                    reduced_data[idx + 2] /= reduced_data[idx];
                    reduced_data[idx + 3] /= reduced_data[idx];
                    reduced_data[idx] /= samples_per_write;
                }
            }
        }

        fwrite(&(update->ntimestep), sizeof(bigint), 1, outfile);
        fwrite(reduced_data, sizeof(double), Ntot, outfile);
    }

    samples = 0;
    zero_data();

}

inline int FixVelocityAverage::index(int i, int j, int k, int l){
    return l + k * 4 + j * 4*Nz + i * 4*Ny*Nz;
}

void FixVelocityAverage::setup(int blah){
    samples = 0;
    zero_data();
}

FixVelocityAverage::~FixVelocityAverage(){
    delete [] data;

    int rank;
    MPI_Comm_rank(world, &rank);

    if (rank == 0) {
        fclose(outfile);
        delete [] reduced_data;
    }
}

void FixVelocityAverage::zero_data(){
    for(int i = 0; i < Ntot; i++){
        data[i] = 0;
    }
}
