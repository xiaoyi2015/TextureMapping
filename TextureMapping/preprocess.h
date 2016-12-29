#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPLYReader.h>
#include <vtkJPEGReader.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkPoints.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>


#include <vector>
using namespace std;

const float near = 50;
const float far = 300;
const int w = 554;
const int h = 830;
const int num = 30;

class PreProcess
{
public:
	PreProcess();
	~PreProcess();

	void LoadMesh();
	void LoadImages();
	void LoadMasks();
	void LoadInMatrix();
	void LoadExMatrix();
	void CreateZBuffer();
	void CalProMatrix();
	void InitDataIDSet();

private:
	vtkSmartPointer<vtkPolyData> mesh;
	vtkSmartPointer<vtkPoints> vertexs;
	vector<vtkSmartPointer<vtkMatrix4x4>> inMat;
	vector<vtkSmartPointer<vtkMatrix4x4>> exMat;
	vector<vtkSmartPointer<vtkImageData>> imageSet;
	vector<vtkSmartPointer<vtkImageData>> maskSet;

	vector<vtkSmartPointer<vtkMatrix4x4>> proMat;
	vtkSmartPointer<vtkPLYReader> reader;
	vtkSmartPointer<vtkPolyDataMapper> mapper;
	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkRenderWindow> renWin;
};

#endif