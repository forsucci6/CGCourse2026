add_executable(code_01_setup_glfw main_01.cpp)

find_package(OpenGL)

target_link_libraries(code_01_setup_glfw
    PRIVATE
        glfw
        glad
        OpenGL::GL
)
