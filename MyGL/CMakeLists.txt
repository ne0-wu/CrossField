find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_library(MyGL
	Window.h
	Window.cpp
	Shader.h
	Shader.cpp
	Mesh.h
	Mesh.cpp
	LineSegment.h
	LineSegment.cpp
	Camera.h
	Camera.cpp
)

target_link_libraries(MyGL PUBLIC
	glad::glad
	glfw
	imgui::imgui
)