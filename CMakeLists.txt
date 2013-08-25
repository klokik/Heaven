# set(INCLUDES ${INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/include})

# add all content of src folder as libraries
set(LOCAL_SOURCES "")
set(LOCAL_LIBS "")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src DIR_LIST)
# foreach(file_name ${DIR_LIST})
# 	get_filename_component(lib_name ${file_name} NAME_WE)
# 	add_library(${lib_name} ${file_name})
# 	set(LOCAL_LIBS ${LOCAL_LIBS} ${lib_name})
# endforeach()

if(BUILD_EXECUTABLE)
	message("Build executable")
	add_executable(heaven heaven_main.cpp ${DIR_LIST})
	target_link_libraries(heaven ${AE_BINARY_NAME})
#	target_link_libraries(heaven ${LOCAL_LIBS} ${AE_BINARY_NAME})
else()
	message("Build shared library")
	add_library(heaven SHARED heaven_main.cpp ${DIR_LIST} ${CMAKE_SOURCE_DIR}/dummy.cpp $<TARGET_OBJECTS:android_glue>)
	target_link_libraries(heaven ${LOCAL_LIBS} ${AE_BINARY_NAME})
#	target_link_libraries(${AE_BINARY_NAME} heaven)
endif()

