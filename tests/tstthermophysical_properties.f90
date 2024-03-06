program tstThermophysicalProperties
 
use default_data_store_M
use r_kister_data_store_M
use thermophysical_properties_M

implicit none
logical :: l_found = .false.

!TODO just moving the football, but this could be polished
INQUIRE(FILE="tstData.csv",EXIST=l_found)
if( .not. l_found) then
  call gen_test_data()
endif
CALL tst_DefaultData()
CALL tst_RKData()

contains
  !
  subroutine tst_DefaultData()
    type(Default_Data_Store) :: dflt_data
    type(Thermophysical_Properties) :: tp
    
    ! Load the default property set 
    call dflt_data%initialize()
    call dflt_data%load("tstData.csv")
    
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
    
    if ( tp%mu(926.00_8, 101.0_8) /= 7.4825131826198357_8) then
      print *,"Failed to equate mu values!"
      stop 1
    endif

    if ( tp%rho(926.00_8, 101.0_8) * 1000.0 /= tp%rho_kgm3(926.00_8,101.0_8)) then
      print *,"Failed to equate rho values!"
      stop 1
    endif
    
    if ( tp%t_melt() /= 828.3_8) then
      print *,"Failed to equate melt values!"
      stop 1
    endif

    if ( tp%t_boil() /= 0.0_8) then
      print *,"Failed to equate boil values!"
      stop 1
    endif

    if ( tp%set_Composition("LiF-BeF2-ThF4",[0.727_8,0.157_8,0.116_8],3) ) then
      if (tp%valid_rho()) then
        print *,"valid check rho failed"
        stop 1
      end if
      if (tp%valid_k()) then
        print *,"valid check k failed"
        stop 1
      end if
      if (tp%valid_cp()) then
        print *,"valid check cp failed"
        stop 1
      end if
      if ( .not. tp%valid_mu()) then
        print *,"valid check mu failed"
        stop 1
      end if
    endif
    if ( tp%set_Composition("LiF-BeF2-ThF4",[0.6998_8,0.1499_8,0.1503_8],3) ) then
      if ( tp%valid_mu()) then
        print *,"valid check mu failed"
        stop 1
      end if
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
    call rk_data%load("tstRKData.csv","tstData.csv")
    
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
    if ( tp%rho(850.0_8, 101.0_8) /= 2.0486000000000004_8 ) then
      write(*,*) tp%rho(850.0_8,101.0_8)
      print *,"Failed to equate values!"
      stop 2
    endif
    
    if ( tp%set_Composition("LiF-UF3-ZrF4",[0.727_8,0.157_8,0.116_8],3) ) then
      if (tp%valid_rho()) then
        print *,"valid check rho failed"
        stop 3
      endif
      if (tp%valid_k()) then
        print *,"valid check k failed"
        stop 3
      endif
      if (tp%valid_cp()) then
        print *,"valid check cp failed"
        stop 3
      endif
      if (tp%valid_mu()) then
        print *,"valid check mu failed"
        stop 3
      endif
    endif

    call tp%destroy()
    call rk_data%destroy()
  end subroutine tst_RKData
  !
  subroutine gen_test_data()
    OPEN(UNIT=500,FILE="tstData.csv")
    write(500,*) ""
    write(500,*) "//Pure Salts"
    write(500,*) "//System      , id , Mol Mass , Mol Frac             , Melt(K) , Var(%) , Reference   , Boil(K) , Var(%) , Reference      , rho_a     , rho_b     , range(K)      , Var(%) , Reference     , mu1_a     , mu2_b     , mu2_a      , mu2_b      , mu2_c     , range(K)      , Var(%) , Reference    , k_a        , k_b        , range(K)     , Var(%) , Reference   , cp_a      , cp_b       , cp_c       , cp_d      , Var(%) , Reference"
    write(500,*) "LiF           , 1  , 25.9390  , 1.0                  , 1121.20 , 1.00   , Douglas1954 , 1943.00 , 0.00   , Ruff1922       , 2.371E+00 , 5.000E-04 , 1123.6-1367.5 , 1.00   , Hill1967      , 1.149E-01 , 2.699E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1133-1772     , 1.00   , Abe1981      , 1.882E+00  , -3.990E-04 , 1118.5-1900  , 20.00  , Gheribi2014 , 6.489E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1.50   , Douglas1954"
    write(500,*) "NaF           , 2  , 41.9882  , 1.0                  , 1268.00 , 0.50   , Cantor1961  , 1978.00 , 0.00   , Ruff1922       , 2.755E+00 , 6.360E-04 , 1273-1373     , 0.20   , Paucirova1970 , 1.196E-01 , 2.649E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1273-1373     , 1.00   , Brockner1979 , 1.259E+00  , -2.800E-04 , 1268.15-1800 , 20.00  , Gheribi2014 , 6.862E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.10   , O'Brien1957"
    write(500,*) "KF            , 3  , 58.0967  , 1.0                  , 1131.20 , 0.50   , Johnson1958 , 1771.00 , 0.00   , Ruff1922       , 2.646E+00 , 6.515E-04 , 1154.2-1310.2 , 1.00   , Yaffe1956     , 1.068E-01 , 2.378E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1141.2-1327.6 , 2.00   , Janz1988     , 8.552E-01  , -2.540E-04 , 1129.15-1800 , 20.00  , Gheribi2014 , 7.060E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 18.00  , Beilmann2013"
    write(500,*) "BeF2          , 4  , 47.0100  , 1.0                  , 821.15  , 1.00   , Thoma1960   , 1442.00 , 0.00   , Cantor1965     , 1.972E+00 , 1.450E-05 , 1073-1123     , 0.50   , Cantor1969    , 0.000E+00 , 0.000E+00 , -8.119E+00 , 1.149E+04  , 6.390E+05 , 846.9-1252.2  , 3.00   , Moynihan1968 , 8.007E-01  , -2.120E-06 , 1070.15-0.0  , 20.00  , Gheribi2014 , 1.027E+02 , -1.539E-03 , -1.565E+07 , 3.000E-09 , 0.00   , Glushko1994"
    write(500,*) "ThF4          , 5  , 308.0300 , 1.0                  , 0.00    , 0.00   , ----        , 0.00    , 0.00   , ----           , 7.080E+00 , 0.000E+00 , 0-0           , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.00   , ----"
    write(500,*) "UF3           , 6  , 295.0200 , 1.0                  , 1768.15 , 20.00  , D'Eye1957   , 2550.00 , 0.00   , Glushko1994    , 0.000E+00 , 0.000E+00 , 0.0-0.0       , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 1.300E+02 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 5.00   , Beilmann2013a"
    write(500,*) "ZrF4          , 7  , 167.2100 , 1.0                  , 0.00    , 0.00   , ----        , 0.00    , 0.00   , ----           , 5.368E+00 , 5.3677    , 0-0           , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.00   , ----"
    write(500,*) "LiF-NaF-KF    , 8  , 41.2909  , 0.465-0.115-0.42     , 735.00  , 2.00   , Rogers1982  , 1843.15 , 0.0    , Williams 2006b , 2.579E+00 , 6.240E-04 , 933-1163      , 1.00   , Cibulkova2006 , 0.000E+00 , 0.000E+00 , 2.130E-01  , -1.200E+03 , 1.350E+06 , 770-970       , 2.00   , Toerklep1980 , -3.500E-01 , 1.300E-03  , 773-973      , 3.50   , An2015      , 4.032E+01 , 4.388E-02  , 0.000E+00  , 0.000E+00 , 2.00   , Rogers1982"
    write(500,*) "LiF-BeF2-ThF4 , 9  , 47.9244  , 0.7011-0.2388-0.0601 , 828.30  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 3.295E+00 , 6.707E-04 , 828.3-980.6   , 1.00   , Cantor1973    , 6.602E-02 , 3.642E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 799-926       , 15.00  , Cantor1973   , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----"
    write(500,*) "LiF-BeF2-ThF4 , 10 , 63.5179  , 0.7006-0.1796-0.1198 , 806.40  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 4.044E+00 , 8.064E-04 , 806.4-1014.4  , 1.00   , Cantor1973    , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.0    , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----"
    write(500,*) "LiF-BeF2-ThF4 , 11 , 71.4958  , 0.6998-0.1499-0.1503 , 816.60  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 4.441E+00 , 9.526E-04 , 816.6-1022.7  , 1.00   , Cantor1973    , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.0    , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----"
    write(500,*) "LiF-BeF2-ThF4 , 12 , 61.9697  , 0.727-0.157-0.116    , 826.20  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 0.0       , 0.0       , 0.0-0.0       , 0.0    , ----          , 1.094E-01 , 3.402E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 826-946       , 15.00  , Cantor1973   , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----"
    CLOSE(500)
    OPEN(UNIT=600,FILE="tstRKData.csv")
    write(600,*) "//RK parameters"
    write(600,*) "//C 1 , C 2     , A1            , B1            , A2            , B2            , A3           , B3           , T min        , T max        , Reference"
    write(600,*) "KF    , LiF     , -5.383100E-03 , -4.142700E-05 , +0.000000E+00 , +0.000000E+00 , 0.000000E+00 , 0.000000E+00 , 1.006150E+03 , 1.314150E+03 , 'Taniuchi, K.; Kanai, T. Density of Binary Molten Salts of Lithium Fluoride-Potassium Fluoride and Lithium Fluoride-Calcium Fluoride Systems. Denki Kagaku oyobi Kogyo Butsuri Kagaku 1977, 45 (6), 401-404. https://doi.org/10.5796/kogyobutsurikagaku.45.401'"
    write(600,*) "KF    , NaF     , -3.747500E-01 , +2.354000E-04 , +0.000000E+00 , +0.000000E+00 , 0.000000E+00 , 0.000000E+00 , 1.050000E+03 , 1.350000E+03 , 'Porter, B., and Meaker, R. E., United States Department of the Interior, Report of Investigations 6838, 1966.'"
    write(600,*) "LiF   , NaF     , -6.998400E-02 , +1.073900E-05 , +0.000000E+00 , +0.000000E+00 , 0.000000E+00 , 0.000000E+00 , 1.130000E+03 , 1.320000E+03 , 'Matiasovsky, K., Private communication, 1968.'"
    write(600,*) "NaF   , BeF2    , +7.850000E-01 , -7.850000E-04 , -1.600000E+00 , +1.540000E-04 , 0.000000E+00 , 0.000000E+00 , 8.730000E+02 , 1.073000E+03 , 'Blanke, B. C., Bousquet, L. V., Jones, L. V., Murphy, E. L., & Vallee, R. E. (1958). Density of fused mixtures of sodium fluoride, beryllium fluoride, and uranium fluoride. Miamisburg, Ohio: Mound Laboratory, Monsanto Chemical Company, U.S. Atomic Energy Commission.'  "
    write(600,*) "LiF   , BeF2    , +8.930000E-01 , -8.960000E-04 , +3.780000E-01 , -1.160000E-04 , 0.000000E+00 , 0.000000E+00 , 8.000000E+02 , 1.130000E+03 , 'Cantor, S., Ward, W. T., & Moynihan, C. T. (1969). Viscosity and Density in Molten BeF 2 – LiF Solutions. J Chem. Phys., 50(7), 2874-2879.   Cantor, S. (1970). Density of Molten Fuorides of Reactor Interest. In M. W. Rosenthal, R. B. Brigss, & P. R. Kasten, U.S.A.E.C Semiannual Progress Report ORNL-4449 (pp. 145-146). Oak Ridge, TN: Oak Ridge National Laboratory.' "
    write(600,*) "ThF4  , KF      , -4.757300E-01 , -4.099400E-06 , +1.626800E+00 , -4.418000E-04 , 0.000000E+00 , 0.000000E+00 , 1.046000E+03 , 1.325000E+03 , 'Desyatnik, V. N.; Klimenkov, A. A.; Kurbatov, N. N.; Nechaev, A. I.; Raspopin, S. P.; Chervinskii, Y. F. Density and Kinematic Viscosity of NaF-ThF4 and KF-ThF4 Melts. Sov. At. Energy 1981, 51 (6), 807-810. https://doi.org/10.1007/BF01121687.'"
    write(600,*) "ThF4  , LiF     , +9.640500E-01 , -8.240700E-04 , +0.000000E+00 , +0.000000E+00 , 0.000000E+00 , 0.000000E+00 , 1.016650E+03 , 1.508150E+03 , 'Hill D. G., Cantor S., Ward W. T. Molar volumes in the LiF-ThF4 system. J. Inorg. Nucl. Chem. 1967, 29, 241-243.'"
    write(600,*) "ThF4  , NaF     , +8.059200E-01 , -9.775800E-04 , +2.479900E+00 , -1.254100E-03 , 0.000000E+00 , 0.000000E+00 , 1.003000E+03 , 1.337000E+03 , 'Desyatnik, V. N.; Klimenkov, A. A.; Kurbatov, N. N.; Nechaev, A. I.; Raspopin, S. P.; Chervinskii, Y. F. Density and Kinematic Viscosity of NaF-ThF4 and KF-ThF4 Melts. Sov. At. Energy 1981, 51 (6), 807810. https://doi.org/10.1007/BF01121687.'"
    write(600,*) "LiF   , ZrF4    , -1.282600E+00 , -4.689000E-04 , +7.520000E-01 , +2.322500E-04 , 0.000000E+00 , 0.000000E+00 , 9.430000E+02 , 1.280000E+03 , 'Katyshev, S. F.; Artemov, V. V; Desyatnik, V. N. Density and Surface Tension of Melts of Zirconium and Hafnium Fluorides with Lithium Fluoride. Sov. At. Energy 1987, 63 (6), 929-930. https://doi.org/10.1007/BF01126108.'"
    write(600,*) "ZrF4  , NaF     , -1.186600E+00 , +2.107500E-04 , +2.082200E-01 , -4.726400E-04 , 0.000000E+00 , 0.000000E+00 , 9.530000E+02 , 1.280000E+03 , 'Artemov, V. V.; Katyshev, S. F.; Desyatnik, V. N. Density and Surface Tension of Sodium Halide Melts with Zirconium and Hafnium Tetrafluorides. Zhurnal Fiz. Khimii 1990, 64 (4), 1113-1115.'"
    CLOSE(600)
  end subroutine gen_test_data
  !
end program tstThermophysicalProperties
