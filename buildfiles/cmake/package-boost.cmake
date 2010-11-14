
set(BOOST_ROOT /opt/ram/local)
find_package(Boost 1.34 REQUIRED COMPONENTS filesystem date_time program_options python regex serialization signals thread)

add_definitions(-Wno-deprecated)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
