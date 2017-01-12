#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPLYReader.h>
#include <vtkJPEGReader.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>
#include <vtkUnsignedCharArray.h>
#include "vector4.h"
#include <algorithm>
#include <vector>
#include <fstream>
using namespace std;

const int num = 30;

class Optimization
{
public:
	Optimization();
	~Optimization();

	void LoadPointId();
	void LoadMesh();
	void LoadImages();
	void LoadMasks();
	void LoadInMatrix();
	void LoadExMatrix();
	void CalProjCoord();
	int  CalIntColor(double x);
	void showInitColor();

public:
	vector<int> imageWidth;
	vector<int> imageHeight;
	vector<vector<int>> imagePointSet;
	vector<Vector4> pointColorSet;
	vtkSmartPointer<vtkPolyData> mesh;
	vtkSmartPointer<vtkPoints> vertexs;
	vector<vtkSmartPointer<vtkMatrix4x4>> inMat;
	vector<vtkSmartPointer<vtkMatrix4x4>> exMat;
	vector<vtkSmartPointer<vtkImageData>> imageSet;
	vector<vtkSmartPointer<vtkImageData>> maskSet;

	vtkSmartPointer<vtkPLYReader> reader;
	vtkSmartPointer<vtkPolyDataMapper> mapper;
	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkRenderWindow> renWin;

};


#endif