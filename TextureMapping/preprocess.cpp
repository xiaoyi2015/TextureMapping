#include "preprocess.h"

PreProcess::PreProcess(Mesh _mesh){
	mesh = _mesh;
}

PreProcess::~PreProcess(){

}

void PreProcess::InitPointSet(){
	vertexs = mesh->GetPoints();
	for (int i = 0; i < vertexs->GetNumberOfPoints(); i++){
		Point point;
		point.x = vertexs->GetPoint(i)[0];
		point.y = vertexs->GetPoint(i)[1];
		point.z = vertexs->GetPoint(i)[2];
		point.r = 0;
		point.g = 0;
		point.b = 0;
		point.w = 0;
		pointSet.push_back(point);
	}
	
}

void PreProcess::InitImageSet(){

}

void PreProcess::DataPreProcess(){

}