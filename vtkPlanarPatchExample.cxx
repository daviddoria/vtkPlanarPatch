#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkPointSource.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkMath.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include "vtkPlanarPatch.h"

int main(int, char *[])
{
  // Create a point cloud
  double origin[3] = {0,0,0};
  double normal[3] = {1,1,0};
  vtkSmartPointer<vtkPlaneSource> planeSource =
    vtkSmartPointer<vtkPlaneSource>::New();
  planeSource->SetCenter(origin);
  planeSource->SetNormal(normal);
  planeSource->SetXResolution(10);
  planeSource->SetYResolution(10);
  planeSource->Update();

  vtkSmartPointer<vtkPoints> points =
    vtkSmartPointer<vtkPoints>::New();
  for(vtkIdType i = 0; i < planeSource->GetOutput()->GetNumberOfPoints(); i++)
    {
    double rand = vtkMath::Random(0,1);
    if(rand < .5)
      {
      double p[3];
      planeSource->GetOutput()->GetPoint(i,p);
      points->InsertNextPoint(p);
      }
    }

  vtkSmartPointer<vtkPolyData> pointsPolyData =
    vtkSmartPointer<vtkPolyData>::New();
  pointsPolyData->SetPoints(points);

  vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  glyphFilter->SetInput(pointsPolyData);
  glyphFilter->Update();

  vtkSmartPointer<vtkPlane> plane =
      vtkSmartPointer<vtkPlane>::New();
  plane->SetNormal(normal);
  plane->SetOrigin(origin);

  vtkSmartPointer<vtkPlanarPatch> planarPatch =
      vtkSmartPointer<vtkPlanarPatch>::New();
  planarPatch->SetPlane(plane);
  planarPatch->SetInputConnection(pointsPolyData->GetProducerPort());
  planarPatch->Update();

  // Create a mapper and actor for the input points
  vtkSmartPointer<vtkPolyDataMapper> pointMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  pointMapper->SetInputConnection(glyphFilter->GetOutputPort());

  vtkSmartPointer<vtkActor> pointActor =
    vtkSmartPointer<vtkActor>::New();
  pointActor->SetMapper(pointMapper);
  pointActor->GetProperty()->SetColor(1,0,0);
  pointActor->GetProperty()->SetPointSize(3);

  // Create a mapper and actor for the planar patch
  vtkSmartPointer<vtkPolyDataMapper> patchMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  patchMapper->SetInputConnection(planarPatch->GetOutputPort());

  vtkSmartPointer<vtkActor> patchActor =
    vtkSmartPointer<vtkActor>::New();
  patchActor->SetMapper(patchMapper);
  patchActor->GetProperty()->SetEdgeColor(0,0,1);
  patchActor->GetProperty()->EdgeVisibilityOn();

  // Create a renderer, render window, and interactor
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(pointActor);
  renderer->AddActor(patchActor);
  renderer->SetBackground(.3, .6, .3); // Background color green

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();
  return EXIT_SUCCESS;
}
