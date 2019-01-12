module mod_velocity_reader
    use iso_fortran_env, only: iostat_end, int64, real64, error_unit
    implicit none

    interface velocity_reader
        module procedure :: velocity_reader_constructor
    end interface

    type :: velocity_reader
        real(real64), allocatable :: values(:,:,:,:)
        real(real64) :: xmin, xmax, ymin, ymax, zmin, zmax
        integer :: Nx, Ny, Nz
        integer(int64) :: step, next_step
        logical :: eof
        integer :: file_unit
        character(len=:), allocatable :: filename

        contains
            procedure :: read_step, get_next_step
            final :: close_reader
    end type

    contains
        function velocity_reader_constructor(filename) result(self)
            character(len=*), intent(in) :: filename
            type(velocity_reader) :: self

            integer :: stat

            open(newunit=self%file_unit, file=filename, status="old", &
                 action="read", access="stream", iostat=stat)

            call check_iostat(stat, "Could not open " // filename)

            read(self%file_unit, iostat=stat) self%Nx, self%Ny, self%Nz, &
                                              self%xmin, self%xmax, &
                                              self%ymin, self%ymax, &
                                              self%zmin, self%zmax

            call check_iostat(stat, "Could not read metadata from " // filename)

            self%step = -1
            self%filename = filename

            call self%get_next_step()

            allocate(self%values(4, self%Nz, self%Ny, self%Nx))
        end function

        subroutine close_reader(self)
            type(velocity_reader), intent(inout) :: self

            close(self%file_unit)
            deallocate(self%values)
        end subroutine

        subroutine read_step(self)
            class(velocity_reader), intent(inout) :: self
            integer :: stat

            self%step = self%next_step
            read(self%file_unit, iostat=stat) self%values
            call check_iostat(stat, "Coule not read values from " // self%filename)

            call self%get_next_step()
        end subroutine

        subroutine get_next_step(self)
            class(velocity_reader), intent(inout) :: self
            integer :: stat

            read(self%file_unit, iostat=stat) self%next_step

            self%eof = stat == iostat_end

            if ((stat /= 0) .and. (.not. self%eof)) then
                call check_iostat(stat, "Unexpected error in get_next_step" &
                                        // " from " // self%filename)
            end if
        end subroutine

        subroutine check_iostat(stat, message)
            integer, intent(in) :: stat
            character(len=*), intent(in) :: message
            if (stat /= 0) then
                write(error_unit, *) "ERROR: " // message
                write(error_unit, *) "IOSTAT = ", stat
                error stop
            end if
        end subroutine
end module
