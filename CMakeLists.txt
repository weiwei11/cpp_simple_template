cmake_minimum_required(VERSION 3.15)
project(Util)
set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_STANDARD 11)

option(BUILD_MEX "Build Mex" OFF)

# yaml
# set(Yaml_DIR /path/to/yaml-cpp-master/build)
set(Torch_DIR /path/to/libtorch/share/cmake/Torch)
# set(Hdf5_DIR /path/to/hdf5-1.10.0/hdf5)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

##########################################
# find package
##########################################
find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED 0.5.3)
find_package(Armadillo REQUIRED)
find_package(HDF5 QUIET COMPONENTS C)

##########################################
# create a library
##########################################

aux_source_directory(. DIR_SRCS)
string(REPLACE "./main.cpp" "" LIB_SRCS "${DIR_SRCS}")
# message(${LIB_SRCS})

set(LIB_NAME common_util)
add_library(${LIB_NAME} SHARED ${LIB_SRCS})
# add_library(${LIB_NAME}:library ALIAS ${LIB_NAME})

target_include_directories(${LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS} 
    ${YAML_CPP_INCLUDE_DIR}
    # ${Hdf5_DIR}/include   
)

target_link_libraries(${LIB_NAME}
    ${OpenCV_LIBS} 
    ${YAML_CPP_LIBRARIES}
    ${TORCH_LIBRARIES}
    # yaml-cpp
    ${ARMADILLO_LIBRARIES}
    ${HDF5_LIBRARIES}
    matio
    /usr/lib/x86_64-linux-gnu/libtiff.so.5
)

##########################################
# create a executable
##########################################

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    # ${OpenCV_LIBS} 
    # ${YAML_CPP_LIBRARIES}
    # ${TORCH_LIBRARIES}
    # # yaml-cpp
    # ${ARMADILLO_LIBRARIES}
    # ${HDF5_LIBRARIES}
    ${LIB_NAME}
)

# message(${TORCH_LIBRARIES})
# message(${TORCH_CXX_FLAGS})
# message(${ARMADILLO_LIBRARIES})
# message(${DIR_SRCS})
# message(${CMAKE_CXX_FLAGS})
# message(${YAML_CPP_LIBRARIES})

##########################################
# create a matlab interface
##########################################
if(BUILD_MEX)
    set(Matlab_dir /path/to/matlab)
    set(Matlab_mex ${Matlab_dir}/bin/mex)
    set(Mex_outfile ${PROJECT_SOURCE_DIR}/matlab/mex_.mexa64)
    set(Mex_srcfile ${PROJECT_SOURCE_DIR}/matlab/mex_.cpp)

    set(cflags -D_GLIBCXX_USE_CXX11_ABI=1)
    set(link_folders 
        -I/usr/local/include/opencv2 
        -I/usr/local/include/opencv 
        -I../
        -L/usr/local/lib 
        -L../
    )
    set(libflags
        -l${LIB_NAME} -lopencv_core -lopencv_calib3d -lopencv_highgui -lopencv_features2d -lopencv_imgproc -lopencv_imgcodecs
        -ltorch -lcaffe2 -lyaml-cpp -larmadillo -lhdf5
        /usr/lib/x86_64-linux-gnu/libcuda.so /usr/local/cuda/lib64/libnvrtc.so /usr/local/cuda/lib64/libnvToolsExt.so /usr/local/cuda/lib64/libcudart.so
    )

    add_custom_command(OUTPUT ${Mex_outfile} COMMAND ${Matlab_mex}
        ARGS -output ${Mex_outfile} ${Mex_srcfile} ${cflags} ${link_folders} ${libflags}
        DEPENDS ${PROJECT_NAME} COMMENT "Building Matlab interface: ${Mex_outfile}" VERBATIM)
    add_custom_target(matlab ALL DEPENDS ${Mex_outfile} SOURCES ${Mex_srcfile})
    add_custom_command(TARGET matlab POST_BUILD COMMAND cp ARGS ./lib${LIB_NAME}.so ${PROJECT_SOURCE_DIR}/matlab COMMENT "Copy lib to ${PROJECT_SOURCE_DIR}/matlab")
endif(BUILD_MEX)
##########################################
# unit test
##########################################

set(UNIT_TEST unit_tests)
# enable CTest testing
enable_testing()

aux_source_directory(test TEST_SRCS)

# Add a testing executable
add_executable(${UNIT_TEST} ${TEST_SRCS})

# Add google test lib
# set(GTest_DIR /path/to/googletest-release-1.10.0/)
# target_include_directories(${UNIT_TEST} PUBLIC
#     ${GTest_DIR}/googletest/include
# )
# target_link_directories(${UNIT_TEST} PUBLIC
#     ${GTest_DIR}/lib
# )
target_link_libraries(${UNIT_TEST}
    ${LIB_NAME}
    gtest
    gtest_main
)

add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})
add_custom_target(runtest COMMAND ${UNIT_TEST})
