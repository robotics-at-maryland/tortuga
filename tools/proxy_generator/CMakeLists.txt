
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.hpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})
add_executable(proxy_generator ${SOURCES} ${HEADERS})
target_link_libraries(proxy_generator
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  )
