add_library(app_module MODULE app.cpp)
target_sources(app_module PRIVATE app.h apptype.cpp apptype.h)
target_include_directories(app_module PRIVATE ../../include)
target_link_libraries(app_module objecttype objecttypeinterface module)

add_library(resourcexml resourcexml.cpp)
target_sources(resourcexml PRIVATE resourcexml.h)
target_include_directories(resourcexml PUBLIC ../../3rd/tinyxml2)
target_include_directories(resourcexml PRIVATE ../../include)
target_link_libraries(resourcexml tinyxml2)

add_library(scenexml scenexml.cpp)
target_sources(scenexml PRIVATE scenexml.h)
target_include_directories(scenexml PUBLIC ../../3rd/tinyxml2)
target_include_directories(scenexml PRIVATE ../../include)
target_link_libraries(scenexml tinyxml2)

add_library(resource_module MODULE resource.cpp)
target_sources(resource_module PRIVATE resource.h resourcetype.cpp resourcetype.h)
target_include_directories(resource_module PRIVATE ../../include)
target_link_libraries(resource_module objecttype objecttypeinterface modulemanager resourcexml)

add_library(viewer_module MODULE viewer.cpp)
target_sources(viewer_module PRIVATE viewer.h viewertype.cpp viewertype.h)
target_include_directories(viewer_module PRIVATE ../../include)
target_link_libraries(viewer_module objecttype objecttypeinterface module scenexml threadpool pipeline camera)

find_library(vulkan ${VULKAN_LIBRARY_NAME} PATHS ${VULKAN_LIBRARY_DIRECTORY})
find_library(glfw3 glfw3 PATHS ../../lib NO_DEFAULT_PATH)

add_library(render_object_info renderobjectinfo.cpp)
target_sources(render_object_info PRIVATE renderobjectinfo.h)
target_include_directories(render_object_info PRIVATE ../../include ${VULKAN_INCLUDE_DIRECTORY})
target_link_libraries(render_object_info tinymath3d gltf)

add_library(defferred_pipeline MODULE defferredpipeline.cpp)
target_sources(defferred_pipeline PRIVATE defferredpipeline.h defferredpipelinetype.cpp defferredpipelinetype.h)
target_include_directories(defferred_pipeline PRIVATE ../../include ../../3rd/glfw/include ${VULKAN_INCLUDE_DIRECTORY})
target_link_libraries(defferred_pipeline objecttype objecttypeinterface modulemanager tinymath3d ${glfw3} ${vulkan} render_object_info gltf)

add_library(box_node MODULE boxnode.cpp)
target_sources(box_node PRIVATE boxnode.h boxnodetype.cpp boxnodetype.h)
target_include_directories(box_node PRIVATE ../../include)
target_link_libraries(box_node objecttype objecttypeinterface tinymath3d)

add_library(glTF_node MODULE glTFnode.cpp)
target_sources(glTF_node PRIVATE glTFnode.h glTFnodetype.cpp glTFnodetype.h)
target_include_directories(glTF_node PRIVATE ../../include ${VULKAN_INCLUDE_DIRECTORY})
target_link_libraries(glTF_node objecttype objecttypeinterface modulemanager gltf tinymath3d render_object_info)

add_custom_command(TARGET app_module POST_BUILD COMMAND copy $<TARGET_FILE:app_module> ${CMAKE_HOME_DIRECTORY}\\env\\Libraries\\app${MODULE_SUFFIX})
add_custom_command(TARGET resource_module POST_BUILD COMMAND copy $<TARGET_FILE:resource_module> ${CMAKE_HOME_DIRECTORY}\\env\\Libraries\\resource${MODULE_SUFFIX})
add_custom_command(TARGET viewer_module POST_BUILD COMMAND copy $<TARGET_FILE:viewer_module> ${CMAKE_HOME_DIRECTORY}\\env\\Libraries\\viewer${MODULE_SUFFIX})

add_custom_command(TARGET defferred_pipeline POST_BUILD COMMAND copy $<TARGET_FILE:defferred_pipeline> ${CMAKE_HOME_DIRECTORY}\\env\\Libraries\\defferred_pipeline${MODULE_SUFFIX})

add_custom_command(TARGET box_node POST_BUILD COMMAND copy $<TARGET_FILE:box_node> ${CMAKE_HOME_DIRECTORY}\\env\\Libraries\\box_node${MODULE_SUFFIX})
add_custom_command(TARGET glTF_node POST_BUILD COMMAND copy $<TARGET_FILE:glTF_node> ${CMAKE_HOME_DIRECTORY}\\env\\Libraries\\glTF_node${MODULE_SUFFIX})