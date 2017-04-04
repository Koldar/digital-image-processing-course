
#include in the build all the content inside the directory
include_directories("../include")
#you might want to add the sources via the following command: set(SOURCES src/mainapp.cpp src/Student.cpp)
#but with GLOB is all much easier; include in the build all the content filtered by the pattern
file(GLOB SOURCES "*.cpp")


#find openCV libraries
find_package( OpenCV REQUIRED )
#create an executable from the file specified
add_executable(${PROJECT_NAME} ${SOURCES})
#link openCV libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} )

#copy the contents of src/main/resources inside build/XXX
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/../../src/main/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>)