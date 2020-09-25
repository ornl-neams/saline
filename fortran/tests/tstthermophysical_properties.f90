
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

if (.not.tp%set_composition("LiF-BeF2-ThF4",[0.7011_8, 0.2388_8, 0.0601_8],3)) then
  print *,"Failure to set composition!"
  stop 1
endif

print *,tp%mu(926.00_8, 101.0_8)
if ( tp%mu(926.00_8, 101.0_8) /= 7.4799345781764925_8) then
  print *,"Failed to equate values!"
  stop 2
endif

call tp%destroy()
call data%destroy()
end program
