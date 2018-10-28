program print
    use iso_c_binding, only: c_int, c_long_long, c_double

    integer(c_int) :: Nx, Ny, Nz
    integer(c_long_long) :: step
    integer :: u, s, i, j, k

    real(c_double), allocatable :: data(:,:,:,:)
    real(c_double) :: xmin, xmax, ymin, ymax, zmin, zmax

    open(newunit=u, file="v.bin", access="stream", action="read")

    read(u) Nx, Ny, Nz, xmin, xmax, ymin, ymax, zmin, zmax

    allocate(data(4, Nz, Ny, Nx))

    do
        read(u, iostat=s) step
        if (s /= 0) exit

        read(u) data

        write(*,*) step

        do i = 1, Nx
            do j = 1, Ny
                do k = 1, Nz
                    write(*,"(*(f15.6))") data(:, k, j, i)
                end do
            end do
        end do
    end do
end program

