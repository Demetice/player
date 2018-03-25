cmake_minimum_required (VERSION 2.6)

project(player)

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
    /usr/local/ffmpeg/include
)

link_directories(/usr/local/ffmpeg/lib)

file(GLOB SRCS src/*.cpp)

link_libraries( 
    avformat
    avcodec
    avutil
    swscale
    SDL2
    pthread
)

add_executable(player ${SRCS})