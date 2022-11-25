set(PROJECT_NAME OpenGL-Examples)
project(${PROJECT_NAME})

file(GLOB_RECURSE PROJ_SRC LIST_DIRECTORIES false src/*.cpp)
add_executable(${PROJECT_NAME} ${PROJ_SRC})
add_dependencies(${PROJECT_NAME} OpenGL-Core)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL-Core)
