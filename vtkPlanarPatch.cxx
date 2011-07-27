#include "vtkPlanarPatch.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"

#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkLandmarkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkDelaunay2D.h"

//for testing only
#include "vtkXMLPolyDataWriter.h"

vtkStandardNewMacro(vtkPlanarPatch);

vtkPlanarPatch::vtkPlanarPatch()
{
  this->FlatOutput = true;
}

int vtkPlanarPatch::FillInputPortInformation( int port, vtkInformation* info )
{
  if (port == 0)
    {
    info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet" );
    return 1;
    }

  return 0;
}

int vtkPlanarPatch::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // Get the input and ouptut
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkPointSet *input = vtkPointSet::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkPolyData *output = vtkPolyData::SafeDownCast(
               outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Normalize the plane normal
  double n[3];
  this->Plane->GetNormal(n);
  vtkMath::Normalize(n);
  this->Plane->SetNormal(n);

  // Project points onto the plane
  vtkSmartPointer<vtkPoints> projectedPoints =
    vtkSmartPointer<vtkPoints>::New();

  for(vtkIdType i = 0; i < input->GetNumberOfPoints(); i++)
    {
    double p[3];
    input->GetPoint(i,p);
    double projected[3];
    vtkPlane::ProjectPoint(p, this->Plane->GetOrigin(), this->Plane->GetNormal(), projected);
    projectedPoints->InsertNextPoint(projected);
    }

  // Find a coordinate system on the plane

  // Find one vector by subtracting two points
  double v0[3];
  double p0[3];
  double p1[3];
  projectedPoints->GetPoint(0, p0);
  projectedPoints->GetPoint(1, p1);

  // v0 = p1 - p0
  vtkMath::Subtract(p1, p0, v0);
  vtkMath::Normalize(v0);

  // Find the second vector by crossing the plane normal with v0
  double v1[3];
  vtkMath::Cross(this->Plane->GetNormal(), v0, v1);

  vtkMath::Normalize(v1);

  double sourceOrigin[3];
  for(unsigned int i = 0; i < 3; i++)
    {
    sourceOrigin[i] = p0[i];
    }

  double sourceP1[3];
  vtkMath::Add(p0, v0, sourceP1);

  double sourceP2[3];
  vtkMath::Add(p0, v1, sourceP2);

  vtkSmartPointer<vtkPoints> sourcePoints =
    vtkSmartPointer<vtkPoints>::New();
  sourcePoints->InsertNextPoint(sourceOrigin);
  sourcePoints->InsertNextPoint(sourceP1);
  sourcePoints->InsertNextPoint(sourceP2);

  vtkSmartPointer<vtkPoints> targetPoints =
    vtkSmartPointer<vtkPoints>::New();
  targetPoints->InsertNextPoint(0,0,0);
  targetPoints->InsertNextPoint(0,1,0);
  targetPoints->InsertNextPoint(1,0,0);

  // Find the coordinates of the projected points in the new coordinate system
  vtkSmartPointer<vtkLandmarkTransform> landmarkTransform =
    vtkSmartPointer<vtkLandmarkTransform>::New();
  landmarkTransform->SetSourceLandmarks(sourcePoints);
  landmarkTransform->SetTargetLandmarks(targetPoints);
  landmarkTransform->SetModeToRigidBody();
  landmarkTransform->Update();

  // Add the projected points to a polydata object (required for Delaunay)
  vtkSmartPointer<vtkPolyData> polydata =
    vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(projectedPoints);

  vtkSmartPointer<vtkDelaunay2D> delaunay =
    vtkSmartPointer<vtkDelaunay2D>::New();
  delaunay->SetTransform(landmarkTransform);
  delaunay->SetInput(polydata);
  delaunay->Update();

  // If FlatOutput is specified, the output is actually a region of a plane
  if(this->FlatOutput)
    {
    output->ShallowCopy(delaunay->GetOutput());
    }
  else // If FlatOutput is false, the output is a potentially non-planar mesh on the 3d points
    {
    output->ShallowCopy(input);
    output->SetPolys(delaunay->GetOutput()->GetPolys());
    }

  return 1;
}
