program tstThermophysicalProperties
 
use default_data_store_M
use r_kister_data_store_M
use thermophysical_properties_M

implicit none

CALL tst_DefaultData()
CALL tst_RKData()

contains
  !
  subroutine tst_DefaultData()
    type(Default_Data_Store) :: dflt_data
    type(Thermophysical_Properties) :: tp
    
    ! Load the default property set 
    call dflt_data%initialize()
    call dflt_data%load()
    
    ! Initialize properties instance
    call tp%initialize()
    
    ! Assign data store to
    call tp%init_data_store(dflt_data)
    if (.not.tp%is_SaltValid("LiF-BeF2-ThF4",3)) then
      print *,"Failure to validate composition!"
      stop 1
    endif

    if (tp%is_SaltValid("Li2F-BeF2-ThF4",3)) then
      print *,"Failure to validate composition!"
      stop 1
    endif
    
    if (.not.tp%set_composition("LiF-BeF2-ThF4",[0.7011_8, 0.2388_8, 0.0601_8],3)) then
      print *,"Failure to set composition!"
      stop 1
    endif
    
    print *,tp%mu(926.00_8, 101.0_8)
    if ( tp%mu(926.00_8, 101.0_8) /= 7.4825131826198357_8) then
      print *,"Failed to equate values!"
      stop 2
    endif

    if ( tp%rho(926.00_8, 101.0_8) * 1000.0 /= tp%rho_kgm3(926.00_8,101.0_8)) then
      print *,"Failed to equate values!"
      stop 3
    endif
    
    call tp%destroy()
    call dflt_data%destroy()
  end subroutine tst_DefaultData
  !
  subroutine tst_RKData()
    type(R_Kister_Data_Store) :: rk_data
    type(Thermophysical_Properties) :: tp
    
    ! Load the default property set 
    call rk_data%initialize()
    call rk_data%load()
    
    ! Initialize properties instance
    call tp%initialize()
    
    ! Assign data store to
    call tp%init_data_store(rk_data)
    if (.not.tp%is_SaltValid("LiF-BeF2-ThF4",3)) then
      print *,"Failure to validate composition!"
      stop 1
    endif

    if (tp%is_SaltValid("Li2F-BeF2-ThF4",3)) then
      print *,"Failure to validate composition!"
      stop 1
    endif
    
    if (.not.tp%set_composition("LiF-NaF-KF",[0.465_8,0.115_8,0.42_8],3)) then
      print *,"Failure to set composition!"
      stop 1
    endif
    
    print *,tp%rho(850.0_8, 101.0_8)
    if ( tp%rho(850.0_8, 101.0_8) /= 2.0406762757251267_8 ) then
      print *,"Failed to equate values!"
      stop 2
    endif
    
    call tp%destroy()
    call rk_data%destroy()
  end subroutine tst_RKData
  !
end program tstThermophysicalProperties
