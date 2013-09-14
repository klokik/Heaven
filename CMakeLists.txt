# set(INCLUDES ${INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/include})

# add all content of src folder as libraries
set(LOCAL_SOURCES "")
set(LOCAL_LIBS "")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

include(SimpleJSON.cmake)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src DIR_LIST)

if(BUILD_EXECUTABLE)
	message("Build executable")
	add_executable(heaven heaven_main.cpp ${DIR_LIST})
else()
	message("Build shared library")
	add_library(heaven SHARED heaven_main.cpp ${DIR_LIST} ${CMAKE_SOURCE_DIR}/dummy.cpp $<TARGET_OBJECTS:android_glue>)
endif()

target_link_libraries(heaven ${LOCAL_LIBS} ${AE_BINARY_NAME})
