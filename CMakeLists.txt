cmake_minimum_required(VERSION 2.6)

PROJECT(vtkPlanarPatch)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

ADD_EXECUTABLE(vtkPlanarPatchExample vtkPlanarPatchExample.cxx
vtkPlanarPatch.cxx)
TARGET_LINK_LIBRARIES(vtkPlanarPatchExample vtkHybrid )

