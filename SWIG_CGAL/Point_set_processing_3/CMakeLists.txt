SET (LIBSTOLINKWITH CGAL_Kernel_cpp)

find_package(Eigen3 REQUIRED CONFIG)

if (EIGEN3_FOUND OR Eigen3_FOUND)
  include(CGAL_Eigen3_support)
  set(LIBSTOLINKWITH ${LIBSTOLINKWITH} CGAL::Eigen3_support)
  if (TBB_FOUND)
    set(LIBSTOLINKWITH ${LIBSTOLINKWITH} TBB::tbb TBB::tbbmalloc Threads::Threads)
  endif()

  # Required registration libraries
  find_package(OpenGR REQUIRED)
  add_definitions(-DCGAL_LINKED_WITH_OPENGR)
  message(STATUS "Found OpenGR: ${OpenGR_LIBRARIES}")
  include_directories(${OpenGR_INCLUDE_DIR})
  set(LIBSTOLINKWITH ${LIBSTOLINKWITH} ${OpenGR_LIBRARIES})

  find_package(libpointmatcher REQUIRED)
  add_definitions(-DCGAL_LINKED_WITH_POINTMATCHER)
  message(STATUS "Found libpointmatcher")
  set(LIBSTOLINKWITH ${LIBSTOLINKWITH} pointmatcher)

  # Modules
  ADD_SWIG_CGAL_JAVA_MODULE   ( Point_set_processing_3 ${LIBSTOLINKWITH} )
  ADD_SWIG_CGAL_PYTHON_MODULE ( Point_set_processing_3 ${LIBSTOLINKWITH} "-keyword" )
  ADD_SWIG_CGAL_RUBY_MODULE   ( Point_set_processing_3 ${LIBSTOLINKWITH} )
else()
    message(STATUS "NOTICE: Point_set_processing_3 bindings requires Eigen version 3.1 or greater, and will not be compiled.")
endif()
