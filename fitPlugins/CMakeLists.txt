
add_subdirectory( exp_saturation )

add_subdirectory( explin )

add_subdirectory( fitRational0 )

add_subdirectory( fitRational1 )

add_subdirectory( planck_wavelength )

if( WIN32 )
  install( TARGETS exp_saturation explin fitRational0 fitRational1 planck_wavelength
    RUNTIME DESTINATION fitPlugins
    ARCHIVE DESTINATION fitPlugins
    )
else()
  install( TARGETS exp_saturation explin fitRational0 fitRational1 planck_wavelength
    LIBRARY DESTINATION lib/AlphaPlot/plugins
    ARCHIVE DESTINATION lib/AlphaPlot/plugins
    )
endif()
