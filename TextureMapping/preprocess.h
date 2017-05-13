#ifndef PREPROCESS_H
#define PREPROCESS_H

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
#include <fstream>
#include <vector>
using namespace std;

const float near = 50;
const float far = 5000;
const int w = 3456;
const int h = 5184;
const int num = 27;

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
	void InitDataSet(int k);
	void showInitColor();
	int CalIntColor(double x);
	void WriteIDInFile();
	void WritePLYFile();

private:
	vector<vtkSmartPointer<vtkImageData>> maskSet;
	vector<vtkSmartPointer<vtkMatrix4x4>> proMat;
	vtkSmartPointer<vtkPLYReader> reader;
	vtkSmartPointer<vtkPolyDataMapper> mapper;
	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkRenderWindow> renWin;

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
	
	

};

#endif