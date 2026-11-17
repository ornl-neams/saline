! Error retrieval for the maintained C++ exception boundary.
module saline_error_M
  use, intrinsic :: iso_c_binding, only: c_char, c_int, c_null_char
  implicit none
  private
  public :: saline_get_error
  interface
    function get_last_error(buffer, capacity) bind(C, name="saline_get_last_error") result(failed)
      import :: c_char, c_int
      character(kind=c_char), intent(out) :: buffer(*)
      integer(c_int), value :: capacity
      integer(c_int) :: failed
    end function
  end interface
contains
  ! Call immediately after the operation of interest, on the same thread.
  ! Retrieval does not clear the error; the next library operation does.
  subroutine saline_get_error(failed, message)
    logical, intent(out) :: failed
    character(len=*), intent(out) :: message
    character(kind=c_char) :: buffer(len(message) + 1)
    integer :: i
    failed = get_last_error(buffer, int(size(buffer), c_int)) /= 0
    message = ''
    do i = 1, len(message)
      if (buffer(i) == c_null_char) exit
      message(i:i) = buffer(i)
    end do
  end subroutine
end module
