cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)

project(myserv)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.45.0 COMPONENTS system)
find_package(Threads)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})

	add_executable(tests "main_tests.cpp" "property.h" "arg_stream.h" "fixed_messages.h" "message_reader.cpp" "tcp_connection.cpp" "tcp_server.cpp")
	target_link_libraries(tests ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

	add_executable(client "main_client.cpp" "tcp_connection.cpp" "message_reader.cpp")
	target_link_libraries(client ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

	add_executable(${PROJECT_NAME}
		"main_server.cpp" "tcp_server.cpp" "tcp_connection.cpp"
		"message_reader.cpp" "game_server.cpp"
		# headers added to compile so Qt Creator will manage them:
		"arg_stream.h" "fixed_messages.h" "property.h")
	target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()
