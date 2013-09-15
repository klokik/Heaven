include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/SimpleJSON/src/)


set(SimpleJSON_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/libs/SimpleJSON/src/JSON.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/libs/SimpleJSON/src/JSONValue.cpp)

add_library(SimpleJSON ${SimpleJSON_SRCS})

set(LOCAL_LIBS ${LOCAL_LIBS} SimpleJSON)
