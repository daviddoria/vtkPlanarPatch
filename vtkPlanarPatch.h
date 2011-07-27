/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPlanarPatch.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPlanarPatch - Given an approximately planar point cloud and the
// plane which is a good fit to the points, create a polydata
// representation of the patch of the plane that spans the points.
// .SECTION Description
// vtkPlanarPatch produces a polygonal representation of a patch of a
// plane which spans a collection of nearly planar points.
// First, the points are projected onto the plane. A coordinate system transformation
// is then computed such that the resulting points lie in the XY plane (necessary for the
// next step). A 2D Delaunay triangulation is performed on the points. This connectivity
// information is transfered back to the non-transformed points to produce the final
// planar patch.

#ifndef __vtkPlanarPatch_h
#define __vtkPlanarPatch_h

#include "vtkPolyDataAlgorithm.h"

class vtkPlane;

class vtkPlanarPatch : public vtkPolyDataAlgorithm
{
  public:

    static vtkPlanarPatch *New();
    vtkTypeMacro(vtkPlanarPatch, vtkPolyDataAlgorithm);

    void SetPlane(vtkPlane* plane) {this->Plane = plane;}

    vtkSetMacro(FlatOutput, bool);
    vtkGetMacro(FlatOutput, bool);
    
  protected:
    vtkPlanarPatch();
    ~vtkPlanarPatch() {};

    int FillInputPortInformation( int port, vtkInformation* info );
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  private:
    vtkPlane* Plane;
    bool FlatOutput;
};

#endif
