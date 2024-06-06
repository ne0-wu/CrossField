cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 11)

project(CrossField)

find_package(Eigen3 REQUIRED)
find_package(OpenMesh REQUIRED)

add_compile_definitions(_USE_MATH_DEFINES)

add_executable(CrossField
	main.cpp
	Mesh.h
	CrossField.h
	CrossField.cpp
)

target_link_libraries(CrossField OpenMeshCore OpenMeshTools)

add_custom_command(TARGET CrossField POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:CrossField>/data
    COMMENT "Copying directory to output directory"
)