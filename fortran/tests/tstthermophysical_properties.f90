
program tstThermophysicalPropertoes

 
 
use default_data_store_M
use thermophysical_properties_M

implicit none

type(Default_Data_Store) :: data
type(Thermophysical_Properties) :: tp

! Load the default property set 
call data%initialize()

! Initialize properties instance
call tp%initialize()

! Assign data store to
call tp%init_data_store(data)

if (.not.tp%set_composition("LiF-BeF2-ThF4",[0.6998_8,0.1499_8,0.1503_8],3)) then
  print *,"Failure to set composition!"
  stop 1
endif

print *,tp%mu(823.15_8, 101.0_8)
if ( tp%mu(823.15_8, 101.0_8) < 13.5_8 .or. 13.6_8 < tp%mu(823.15_8, 101.0_8)) then

print *,"Failed to equate values!"
stop 2
endif

call tp%destroy()
call data%destroy()
end program
