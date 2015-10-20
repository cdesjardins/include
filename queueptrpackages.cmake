
file(GLOB boost_dirs "${CMAKE_SOURCE_DIR}/../external/boost/install/*")

find_file(HAVE_BOOST "boost/smart_ptr/intrusive_ptr.hpp" PATHS ${boost_dirs})
if (NOT HAVE_BOOST)
    MESSAGE(FATAL_ERROR "Need to run the makeboost.py script, could not find boost includes")
endif()

get_filename_component(HAVE_BOOST ${HAVE_BOOST} DIRECTORY)
get_filename_component(HAVE_BOOST "${HAVE_BOOST}/../../" ABSOLUTE)
add_definitions(-DBOOST_ALL_NO_LIB)
include_directories(${HAVE_BOOST})
message(STATUS "Found boost: ${HAVE_BOOST}")
