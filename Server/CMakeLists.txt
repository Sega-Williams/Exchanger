CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(Server)


FIND_PACKAGE(Boost 1.74 COMPONENTS system thread REQUIRED)

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(PostgreSQL REQUIRED)
FIND_PACKAGE(libpqxx 7.4.1 REQUIRED)

SET(CMAKE_CXX_STANDARD 17)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

ADD_EXECUTABLE(Server Server.cpp ../common.hpp ../json.hpp)
TARGET_LINK_LIBRARIES(Server PRIVATE Threads::Threads ${Boost_LIBRARIES} pqxx)

#ADD_EXECUTABLE(Client ../src/Client.cpp ../src/Common.hpp ../src/json.hpp)
#TARGET_LINK_LIBRARIES(Client PRIVATE Threads::Threads ${Boost_LIBRARIES})
