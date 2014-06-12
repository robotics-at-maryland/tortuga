# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# For a large number of source files you can create it in a simpler way
# using file() function:
file(GLOB image_analyzer_SOURCES *.cpp)

# Create an executable file called helloworld from sources:
add_executable(image_analyzer ${image_analyzer_SOURCES})

target_link_libraries(image_analyzer ${ROOT_LIBRARIES})
target_link_libraries(image_analyzer ${OPEN_CV_LIBS})