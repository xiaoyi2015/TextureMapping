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

struct Point
{
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	float w;
	vector<int> image_ids;
};

struct ImageTable
{
	vector<int> point_id;
};

struct ZBuffer
{
	double buffer[5200][5200];
};

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
	void InitPointSet();
	void CreateZBuffer();
	void InitImageSet();
	void DataPreProcess();

private:
	vtkSmartPointer<vtkPolyData> mesh;
	vtkSmartPointer<vtkPoints> vertexs;
	vector<vtkSmartPointer<vtkMatrix4x4>> inMat;
	vector<vtkSmartPointer<vtkMatrix4x4>> exMat;
	vector<vtkSmartPointer<vtkImageData>> imageSet;
	vector<vtkSmartPointer<vtkImageData>> maskSet;
	vector<Point> pointSet;
	vector<ImageTable> imageTable;
	vector<ZBuffer> zBufferSet;

	vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
};

#endif