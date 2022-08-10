cmake_minimum_required(VERSION 2.8)

project(Assignment2)

add_executable(RayTracer.out RayTracer.cpp Cone.cpp Cylinder.cpp Plane.cpp Ray.cpp SceneObject.cpp Sphere.cpp TextureBMP.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( RayTracer.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
