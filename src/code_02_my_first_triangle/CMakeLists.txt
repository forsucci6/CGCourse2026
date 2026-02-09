add_executable(code_02_my_first_triangle main_02.cpp)

find_package(OpenGL)


target_include_directories(code_02_my_first_triangle PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}        # anchors relative includes here
)


target_link_libraries(code_02_my_first_triangle
    PRIVATE
        glfw
        glad
        OpenGL::GL
)
