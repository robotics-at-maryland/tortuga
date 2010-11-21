
set(BOOST_ROOT ${RAM_ROOT_DIR})
find_package(Boost 1.34 REQUIRED COMPONENTS filesystem date_time program_options python regex serialization signals thread)

add_definitions(-Wno-deprecated)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
