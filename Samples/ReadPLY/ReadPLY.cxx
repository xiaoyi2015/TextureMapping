#include <vtkPolyData.h>
#include <vtkCamera.h>
#include <vtkPLYReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

int main ( int argc, char *argv[] )
{
  
	std::string inputFilename = "C://Users//zln//Desktop//TextureMapping//bun.ply";

	vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
 
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  
	vtkSmartPointer<vtkCamera> aCamera = vtkSmartPointer<vtkCamera>::New();

	reader->SetFileName(inputFilename.c_str());
	reader->Update();

	polyData = reader->GetOutput();

	//init data class



	mapper->SetInputConnection(reader->GetOutputPort());
	actor->SetMapper(mapper);

	renderWindow->AddRenderer(renderer);
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->AddActor(actor);
	renderer->SetBackground(0.1804,0.5451,0.3412); // sea green

	renderWindow->SetSize(800, 600);
	renderWindow->Render();
	renderWindowInteractor->Start();

	return 0;
}
