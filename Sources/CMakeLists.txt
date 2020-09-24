set(CMAKE_CXX_STANDARD 20)

set(
  SOURCES
  ZoomStreamDeckPlugin.cpp
  main.cpp
)

if(WIN32)
  set(SOURCES
    ${SOURCES}
    Windows.cpp
  )

  add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Windows/pyinstaller-output.log"
    COMMAND "pyinstaller" --onedir --noconfirm Windows/getZoomStatus.py
  )
  add_custom_target(generate_header ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Windows/pyinstaller-output.log")
endif()

if(APPLE)
  set(SOURCES
    ${SOURCES}
    MacOS.cpp
  )
endif()



add_executable(
  sdzoomplugin
  ${SOURCES}
)
target_link_libraries(sdzoomplugin json StreamDeckSDK)
install(TARGETS sdzoomplugin DESTINATION ${CMAKE_INSTALL_PREFIX})
if (WIN32)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/getZoomStatus DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
