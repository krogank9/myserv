cmake_minimum_required(VERSION 2.8)

project(myserv)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.45.0 COMPONENTS system)
find_package(Threads)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})

	add_executable(client "main_client.cpp" "tcp_connection.cpp")
	target_link_libraries(client ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

	add_executable(${PROJECT_NAME}
		"main_server.cpp" "tcp_server.cpp" "tcp_connection.cpp"
		"message_reader.cpp" "game_server.cpp" "property.h"
		# headers added to compile so Qt Creator will manage them:
		"arg_stream.h" "fixed_messages.h")
	target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()
