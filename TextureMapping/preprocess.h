#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vector>
#include <vtkPoints.h>

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

struct Image
{
	vector<int> point_id;
	//内外参，数据
};

typedef vtkSmartPointer<vtkPolyData> Mesh;
typedef vtkSmartPointer<vtkPoints> Vertexs;

class PreProcess
{
public:
	PreProcess(Mesh _mesh);
	~PreProcess();


	void InitPointSet(); 
	void InitImageSet();
	void DataPreProcess();

private:
	Mesh mesh;
	Vertexs vertexs;
	vector<Point> pointSet;
	vector<Image> imageSet;
};

#endif