cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)

project(CrossField)

find_package(Eigen3 CONFIG REQUIRED)
find_package(OpenMesh CONFIG REQUIRED)

find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)


add_compile_definitions(_USE_MATH_DEFINES)

add_executable(CrossField
	main.cpp
	Mesh.h
	CrossField.h
	CrossField.cpp
	MyGL/Window.h
	MyGL/Window.cpp
	MyGL/Shader.h
	MyGL/Shader.cpp
	MyGL/Mesh.h
	MyGL/Mesh.cpp
	MyGL/LineSegment.h
	MyGL/LineSegment.cpp
	MyGL/Camera.h
	MyGL/Camera.cpp
)

target_link_libraries(CrossField
	OpenMeshCore
	OpenMeshTools
	glad::glad
	glm::glm
	glfw
	imgui::imgui
)

add_custom_command(TARGET CrossField POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:CrossField>/data
    COMMENT "Copying directory to output directory"
)