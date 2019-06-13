cmake_minimum_required (VERSION 3.13)
project (HubballBot)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.14/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_check(VERSION 1.14.4 REQUIRED)
conan_add_remote(NAME bincrafters
                 URL https://api.bintray.com/conan/bincrafters/public-conan)

conan_cmake_run(CONANFILE conanfile.txt  # or relative build/conanfile.txt
                BASIC_SETUP)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else()
set(CMAKE_CXX_STANDARD 17)
endif(MSVC)

file(GLOB helloworld_SRC
    "CppTwitchSoundBot/*.hpp"
    "CppTwitchSoundBot/*.h"
    "CppTwitchSoundBot/*.cpp"
)

# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()

add_executable(HubballBot ${helloworld_SRC})
target_link_libraries(HubballBot ${CONAN_LIBS})
