cmake_minimum_required(VERSION 3.10)
project(dcm2hdf)

set(CMAKE_CXX_STANDARD 11)

# specify that we have a config file. This is required, since DCMTK package for Ubuntu 14.04
# uses /usr/include/dcmtk/config/cfunix.h to setup dcmtk. The file is only loaded by
# DCMTK's /usr/include/dcmtk/config/osconfig.h when HAVE_CONFIG_H is set.
add_definitions(-DHAVE_CONFIG_H)


# define location of the source code. This is used so that we can refer in the example
# code to the DCMIMAGES folder relatively to the source folder.
add_definitions(-DSOURCE_CODE_LOCATION="${CMAKE_SOURCE_DIR}")

# search for DCMTK library and header files
find_package(DCMTK REQUIRED)

# specify DCMTK header include directories
include_directories(${DCMTK_INCLUDE_DIRS})

# search for Boost library and header files
FIND_PACKAGE(Boost REQUIRED COMPONENTS system filesystem)

# search for OpenCV library and header
find_package(OpenCV REQUIRED )

# search for hdft libary and header
find_package(HDF5 COMPONENTS C CXX HL REQUIRED)

# search for OpenMP library
find_package(OpenMP REQUIRED)


# specificy Boost header include directories
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

include_directories(${HDF5_INCLUDE_DIRS})

link_directories( ${HDF5_LIBRARY_DIRS} )

add_executable(dcm2hdf lib/main.cpp lib/ct.cpp lib/ct.h lib/roi.cpp lib/roi.h lib/roi_set.cpp lib/roi_set.h lib/patient.cpp lib/patient.h)

# link DCMTK library files
target_link_libraries(dcm2hdf ${DCMTK_LIBRARIES})

# link to Boost library files
TARGET_LINK_LIBRARIES( dcm2hdf ${Boost_LIBRARIES} )

target_link_libraries( dcm2hdf ${OpenCV_LIBS} )

target_link_libraries(dcm2hdf ${HDF5_LIBS})

target_link_libraries( dcm2hdf ${HDF5_CXX_LIBRARIES} )

target_link_libraries(dcm2hdf OpenMP::OpenMP_CXX)
