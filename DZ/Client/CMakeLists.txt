cmake_minimum_required(VERSION 3.6)
project(DZ_client_new)

include_directories(/diana/include)
link_directories(/diana/lib)

include_directories(/home/diana/libboost/boost_1_61_0/include/)
link_directories(/home/diana/libboost/boost_1_61_0/stage/lib)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(DZ_client_new ${SOURCE_FILES})

# откуда тут берется gmp?
# можно как-н. так: https://github.com/stevedekorte/io/blob/master/modules/FindGMP.cmake

target_link_libraries(DZ_client_new
        gmp
        gmpxx
        pthread
        boost_system
        boost_thread)