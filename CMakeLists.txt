cmake_minimum_required(VERSION 3.12)
project(LabCarVideoSystem)

set(CMAKE_CXX_STANDARD 17)
set(SRC_FILES
    src/video_capture.cpp
    src/can_bus.cpp
    src/manual_trigger.cpp
    src/circular_buffer.cpp
    src/overlay.cpp
    src/storage.cpp
    src/main_app.cpp
    main.cpp
)

include_directories(include)

add_executable(labcar ${SRC_FILES})
target_link_libraries(labcar pthread)