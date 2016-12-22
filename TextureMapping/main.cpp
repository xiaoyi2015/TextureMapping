//This project is to optimize the color map of the restruction of 3D human body

#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPLYReader.h>
#include <vtkBMPWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageShiftScale.h>
#include "preprocess.h"
#include "optimization.h"


void createZbuffers(){

}

int main(int argc, char *argv[])
{
  
	std::string inputFilename = "C://Users//zln//Desktop//TextureMapping//bun.ply";
    vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();

    vtkSmartPointer<vtkActor> actor =
      vtkSmartPointer<vtkActor>::New();

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renWin =
      vtkSmartPointer<vtkRenderWindow>::New();

    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();

	vtkSmartPointer<vtkPLYReader> reader =
		vtkSmartPointer<vtkPLYReader>::New();

    vtkSmartPointer<vtkWindowToImageFilter> filter =
      vtkSmartPointer<vtkWindowToImageFilter>::New();

    vtkSmartPointer<vtkBMPWriter> imageWriter =
      vtkSmartPointer<vtkBMPWriter>::New();

    vtkSmartPointer<vtkImageShiftScale> scale =
      vtkSmartPointer<vtkImageShiftScale>::New();

	vtkSmartPointer<vtkCamera> aCamera =
	vtkSmartPointer<vtkCamera>::New();

    // Read .ply file
	reader->SetFileName(inputFilename.c_str());

	//Build visualization enviroment
	mapper->SetInputConnection(reader->GetOutputPort());
	actor->SetMapper(mapper);
	renderer->AddActor(actor);
	renWin->AddRenderer(renderer);
	renWin->SetSize(800, 600);
	interactor->SetRenderWindow(renWin);
	renWin->Render();
	
  
	// Create Depth Map
	filter->SetInput(renWin);
	filter->SetMagnification(1);
	filter->SetInputBufferTypeToZBuffer();        //Extract z buffer value

	scale->SetOutputScalarTypeToUnsignedChar();
	scale->SetInputConnection(filter->GetOutputPort());
	scale->SetShift(0);
	scale->SetScale(-255);


	// Write depth map as a .bmp image
	imageWriter->SetFileName("C://Users//zln//Desktop//TextureMapping//Samples//ZBuffer//output.bmp");
	imageWriter->SetInputConnection(scale->GetOutputPort());
	imageWriter->Write();

	interactor->Start();

    return 0;
}

