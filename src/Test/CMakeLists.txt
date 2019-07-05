find_library(lua53 lua53 PATHS ../../lib NO_DEFAULT_PATH)

add_executable(test_resource_module test_resource_module.cpp)
add_executable(test_lua test_lua.cpp)
target_include_directories(test_lua PRIVATE ../../include)
target_include_directories(test_lua PRIVATE ../../3rd/lua-5.3.5/src)
target_link_libraries(test_lua ${lua53} luascript)

add_executable(test_lockfreequeue test_lockfreequeue.cpp)
target_sources(test_lockfreequeue PRIVATE ../../include/lockfree.hpp ../../include/lockfreequeue.hpp)
target_include_directories(test_lockfreequeue PRIVATE ../../include)

add_executable(test_threadpool test_threadpool.cpp)
target_sources(test_threadpool PRIVATE ../../include/lockfree.hpp ../../include/lockfreequeue.hpp)
target_include_directories(test_threadpool PRIVATE ../../include)
target_link_libraries(test_threadpool threadpool)

add_executable(test_math3d test_math3d.cpp)
target_sources(test_math3d PRIVATE utMath.h)
target_include_directories(test_math3d PRIVATE ../../include)
target_link_libraries(test_math3d tinymath3d)

add_executable(test_gltf test_gltf.cpp)
target_include_directories(test_gltf PRIVATE ../../include)
target_link_libraries(test_gltf gltf)