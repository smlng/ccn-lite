cmake_minimum_required(VERSION 3.7)
project(ccn-lite-relay)

set(PROJECT_LINK_LIBS libccnl-core.a libccnl-pkt.a libccnl-fwd.a libccnl-unix.a libccnl-nfn.a)
set(EXT_LINK_LIBS ssl crypto)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin)

link_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/../lib
)

include_directories(
    ../ccnl-pkt/include
    ../ccnl-fwd/include
    ../ccnl-core/include
    ../ccnl-unix/include
    ../ccnl-nfn/include
)

file(GLOB SOURCES "*.c")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LINK_LIBS} ${EXT_LINK_LIBS})
target_link_libraries(ccn-lite-relay ccnl-core ccnl-pkt ccnl-fwd ccnl-unix ccnl-nfn ccnl-pkt-compression)
