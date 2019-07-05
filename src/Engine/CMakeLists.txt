add_library(appxml appxml.cpp)
target_sources(appxml PRIVATE ../../include/appxml.h)
target_include_directories(appxml PRIVATE ../../3rd/tinyxml2)
target_include_directories(appxml PRIVATE ../../include)

add_library(tinymath3d tinymath3d.cpp)
target_sources(tinymath3d PRIVATE ../../include/tinymath3d.h)
target_include_directories(tinymath3d PRIVATE ../../include)

add_library(camera camera.cpp)
target_sources(camera PRIVATE ../../include/camera.h)
target_include_directories(camera PRIVATE ../../include)
target_link_libraries(camera tinymath3d)

add_library(objecttypeinterface objecttypeinterface.cpp)
target_sources(objecttypeinterface PRIVATE ../../include/objecttypeinterface.h)
target_include_directories(objecttypeinterface PRIVATE ../../include)

add_library(objecttype objecttype.cpp)
target_sources(objecttype PRIVATE ../../include/objecttype.h)
target_include_directories(objecttype PRIVATE ../../include)
target_link_libraries(objecttype objecttypeinterface)

add_library(crossplatform crossplatform.cpp)
target_sources(crossplatform PRIVATE ../../include/crossplatform.h)
target_include_directories(crossplatform PRIVATE ../../include)

add_library(scriptinterface scriptinterface.cpp)
target_sources(scriptinterface PRIVATE ../../include/scriptinterface.h)
target_include_directories(scriptinterface PRIVATE ../../include)

add_library(pipeline pipeline.cpp)
target_sources(pipeline PRIVATE ../../include/pipeline.h)
target_include_directories(pipeline PRIVATE ../../include)
target_include_directories(pipeline PUBLIC ../../3rd/tinyxml2)
target_link_libraries(pipeline tinyxml2)

add_library(threadpool threadpool.cpp)
target_sources(threadpool PRIVATE ../../include/threadpool.h ../../include/lockfreequeue.hpp)
target_include_directories(threadpool PRIVATE ../../include)

add_library(scenenode scenenode.cpp)
target_sources(scenenode PRIVATE ../../include/scenenode.h)
target_include_directories(scenenode PRIVATE ../../include)

add_library(script script.cpp)
target_sources(script PRIVATE ../../include/script.h)
target_include_directories(script PRIVATE ../../include)
target_link_libraries(script scriptinterface)

add_library(gltf gltf.cpp)
target_sources(gltf PRIVATE ../../include/gltf.h)
target_include_directories(gltf PUBLIC ../../3rd/tinygltf)
target_include_directories(gltf PRIVATE ../../include ${VULKAN_INCLUDE_DIRECTORY})

add_library(luascript luascript.cpp)
target_sources(luascript PRIVATE ../../include/luascript.h)
target_include_directories(luascript PRIVATE ../../include)
target_include_directories(luascript PUBLIC ../../3rd/lua-5.3.5/src)
target_link_libraries(luascript script scriptinterface)

add_library(module module.cpp)
target_sources(module PRIVATE ../../include/module.h)
target_include_directories(module PRIVATE ../../include)
target_link_libraries(module crossplatform)

add_library(modulemanager modulemanager.cpp)
target_sources(modulemanager PRIVATE ../../include/modulemanager.h)
target_include_directories(modulemanager PRIVATE ../../include)
target_link_libraries(modulemanager module)

add_library(viewer viewer.cpp)

add_library(scene scene.cpp)

add_library(resource MODULE resource.cpp)
target_sources(resource PRIVATE ../../include/resource.h)
target_include_directories(resource PRIVATE ../../include)

find_library(lua53 lua53 PATHS ../../lib NO_DEFAULT_PATH)

add_library(unlike3d unlike3d.cpp)
target_include_directories(unlike3d PRIVATE ../../include)
target_sources(unlike3d PRIVATE ../../include/unlike3d.h)
target_sources(unlike3d PRIVATE ../../include/appxml.h)
target_link_libraries(unlike3d appxml modulemanager objecttype luascript tinyxml2 ${lua53})