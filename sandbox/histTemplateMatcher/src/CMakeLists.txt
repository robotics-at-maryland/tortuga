# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# For a large number of source files you can create it in a simpler way
# using file() function:
file(GLOB hist_template_matcher_SOURCES *.cpp)

# Create an executable file called helloworld from sources:
add_executable(hist_template_matcher ${hist_template_matcher_SOURCES})

target_link_libraries(hist_template_matcher ${ROOT_LIBRARIES})
target_link_libraries(hist_template_matcher ${OPEN_CV_LIBS})