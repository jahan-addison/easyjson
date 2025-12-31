cmake_minimum_required(VERSION 3.16...3.29)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif()

project(
  easyjson
  VERSION 1.3.0
  LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(GNUInstallDirs)
include(CMakeDependentOption)
include(cmake/CPM.cmake)

cmake_dependent_option(EASYJSON_BUILD_EXAMPLES
    "Enable ${PROJECT_NAME} project example targets" OFF
    "EASYJSON_BUILD_EXAMPLES" OFF
)

cmake_dependent_option(EASYJSON_BUILD_TESTS
    "Enable ${PROJECT_NAME} project tests targets" OFF
    "EASYJSON_BUILD_TESTS" OFF
)

CPMAddPackage("gh:StableCoder/cmake-scripts#24.04")
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.8.0")

if(USE_SANITIZER)
    include(${cmake-scripts_SOURCE_DIR}/sanitizers.cmake)
endif()

include(${cmake-scripts_SOURCE_DIR}/tools.cmake)

# Define the header-only library
add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(EASYJSON_BUILD_TESTS)
  set(BUILD_TESTING OFF)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.10.0
  )

  FetchContent_MakeAvailable(Catch2)

  if(MSVC)
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /utf-8 /DEBUG /D_CRT_SECURE_NO_DEPRECATE /W4 /WX"
    )
  else()
      set(CMAKE_CXX_FLAGS
          "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -O2 -march=native")
  endif()


  file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
  file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc")
  add_executable(Test_Suite "${CMAKE_CURRENT_SOURCE_DIR}/test/test.cc")
  set_target_properties(Test_Suite PROPERTIES CXX_STANDARD 17)

  target_link_libraries(Test_Suite PRIVATE easyjson Catch2WithMain)

  # C++20
  add_executable(Test_Suite_CPP20 "${CMAKE_CURRENT_SOURCE_DIR}/test/test.cc")
  set_target_properties(Test_Suite_CPP20 PROPERTIES CXX_STANDARD 20)
  target_link_libraries(Test_Suite_CPP20 PRIVATE easyjson Catch2WithMain)

endif()

if(EASYJSON_BUILD_EXAMPLES)

  file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
  file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cc")

  foreach(example_source ${sources})
      get_filename_component(example_name ${example_source} NAME_WE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDEBUG")
      add_executable(${example_name} ${headers} ${example_source})
      set_target_properties(${example_name} PROPERTIES CXX_STANDARD 17)
      target_include_directories(
        ${example_name} PUBLIC $<BUILD_INTERFACE:${${example_name}_SOURCE_DIR}>
                              $<INSTALL_INTERFACE:${example_name}-${PROJECT_VERSION}>
      )

      target_link_libraries(${example_name} PRIVATE easyjson)
  endforeach()
endif()
