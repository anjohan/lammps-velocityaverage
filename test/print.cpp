#include <cstdio>
#include <cstdint>
#include <cinttypes>

int main(){
    FILE *f = fopen("v.bin", "rb");

    int Nx, Ny, Nz;
    double *data = new double[4];
    double xmin, xmax, ymin, ymax, zmin, zmax;

    fread(&Nx, sizeof(int), 1, f);
    fread(&Ny, sizeof(int), 1, f);
    fread(&Nz, sizeof(int), 1, f);

    fread(&xmin, sizeof(double), 1, f);
    fread(&xmax, sizeof(double), 1, f);
    fread(&ymin, sizeof(double), 1, f);
    fread(&ymax, sizeof(double), 1, f);
    fread(&zmin, sizeof(double), 1, f);
    fread(&zmax, sizeof(double), 1, f);


    printf("Nx = %d, Ny = %d, Nz = %d\n", Nx, Ny, Nz);
    printf("xmin = %.2f, xmax = %.2f, ymin = %.2f, ymax = %.2f, "
           "zmin = %.2f, zmax = %.2f\n", xmin, xmax, ymin, ymax, zmin, zmax);

    int64_t step;

    fread(&step, sizeof(int64_t), 1, f);
    while (!feof(f)) {
        printf("%d\n", feof(f));
        printf("step: %" PRId64 "\n", step);
        for (int i = 0; i < Nx*Ny*Nz; i++){
            fread(data, sizeof(double), 4, f);
            printf("%.2f %.2f %.2f %.2f\n", data[0], data[1], data[2], data[3]);
        }
        fread(&step, sizeof(int64_t), 1, f);
    }

    fclose(f);
}
