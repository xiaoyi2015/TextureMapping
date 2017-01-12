
#include "optimization.h"

#include <vtkMatrix3x3.h>
#include <vtkInteractorStyleTrackballCamera.h>

Optimization::Optimization()
{
	mesh = vtkSmartPointer<vtkPolyData>::New();
	vertexs = vtkSmartPointer<vtkPoints>::New();
	reader = vtkSmartPointer<vtkPLYReader>::New();
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	actor = vtkSmartPointer<vtkActor>::New();
	renderer = vtkSmartPointer<vtkRenderer>::New();
	renWin = vtkSmartPointer<vtkRenderWindow>::New();

	
}

Optimization::~Optimization()
{
}

void Optimization::LoadPointId(){
	char idFilename[128];
	fstream f;
	for (int i = 0; i < num; i++){
		vector<int> pointIDSet;

		sprintf(idFilename, "C://Users//zln//Desktop//Cui//Id//%d.txt", i);
		
		f.open(idFilename, ios::in);
		while (!f.eof()){
			int a;
			f >> a;
			//cout << a << endl;
			pointIDSet.push_back(a);
		}
		//cout << pointIDSet.size() << endl;
		imagePointSet.push_back(pointIDSet);
		f.close();

	}
	cout << "Load point ID success" << endl;
	
}

void Optimization::LoadMesh(){

	string meshFilename = "C://Users//zln//Desktop//Cui//total_usc_cleaned.ply";
	reader->SetFileName(meshFilename.c_str());
	reader->Update();
	mesh = reader->GetOutput();
	vertexs = mesh->GetPoints();
	cout << vertexs->GetNumberOfPoints() << endl;
	pointColorSet.resize(vertexs->GetNumberOfPoints(), Vector4());

	cout << "load mesh success" << endl;

}


void Optimization::LoadImages(){
	char imageFilename[128];
	int dims[3];
	for (int i = 0; i < num; i++)
	{
		vtkSmartPointer<vtkJPEGReader> imageReader = vtkSmartPointer<vtkJPEGReader>::New();
		sprintf(imageFilename, "C://Users//zln//Desktop//Cui//color_valid//%d.jpg", i);
		vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
		imageReader->SetFileName(imageFilename);
		imageReader->Update();
		imageData = imageReader->GetOutput();
		imageSet.push_back(imageData);
		imageData->GetDimensions(dims);
		cout << dims[0] << dims[1] << endl;
		imageWidth.push_back(dims[0]);
		imageHeight.push_back(dims[1]);

		
	}
	cout << "load images success" << endl;
}

void Optimization::LoadMasks(){
	char maskFilename[128];
	int dims[3];
	for (int i = 0; i < num; i++)
	{
		vtkSmartPointer<vtkJPEGReader> maskReader = vtkSmartPointer<vtkJPEGReader>::New();
		sprintf(maskFilename, "C://Users//zln//Desktop//Cui//mask//%d.jpg", i);
		vtkSmartPointer<vtkImageData> maskData = vtkSmartPointer<vtkImageData>::New();
		maskReader->SetFileName(maskFilename);
		maskReader->Update();
		maskData = maskReader->GetOutput();
		maskSet.push_back(maskData);
		
	}
	cout << "load masks success" << endl;
}

void Optimization::LoadInMatrix(){
	ifstream file;
	char inFilename[128];
	double a;
	string s;
	for (int k = 0; k < num; k++){
		vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
		sprintf(inFilename, "C://Users//zln//Desktop//Cui//projection//proj_%d.txt", k);
		file.open(inFilename);
		file >> s;
		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 4; j++){
				file >> a;
				//cout << a << endl;
				m->SetElement(i, j, a);
			}
		}
		for (int i = 0; i < 4; i++){
			m->SetElement(3, i, 0);
		}
		file.close();
		inMat.push_back(m);
		for (int i = 0; i < 4; i++){
			//cout << m->GetElement(i, 0) << " " << m->GetElement(i, 1) <<" " << m->GetElement(i, 2) << " " << m->GetElement(i, 3) << endl;

		}
	}
	cout << "load inMats success" << endl;
}

void Optimization::LoadExMatrix(){
	ifstream file;
	char exFilename[128];
	double a;
	for (int k = 1; k < num + 1; k++){
		vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
		sprintf(exFilename, "C://Users//zln//Desktop//Cui//extrinsic//pp_pointcloud_%d.tf", k);
		file.open(exFilename);
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				file >> a;
				//cout << a << endl;
				m->SetElement(i, j, a);
			}
		}
		file.close();
		exMat.push_back(m);
		//cout << exMat.size() << endl;
		for (int i = 0; i < 4; i++){
			//cout << m->GetElement(i, 0) << " " << m->GetElement(i, 1) <<" " << m->GetElement(i, 2) << " " << m->GetElement(i, 3) << endl;
		}
	}
	cout << "load exMats success" << endl;
}

void Optimization::CalProjCoord(){

	mapper->SetInputConnection(reader->GetOutputPort());
	actor->SetMapper(mapper);
	renderer->AddActor(actor);
	renWin->AddRenderer(renderer);

	for (int k = 0; k < num; k++){

		double fx, cx, cy;
		fx = inMat[k]->GetElement(0, 0);
		cx = inMat[k]->GetElement(0, 2);
		cy = inMat[k]->GetElement(1, 2);
		cout << k << ":" << fx << " " << cx << " " << cy << endl;
		vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

		double wcx = -2 * (cx - double(imageWidth[k]) / 2) / imageWidth[k];
		double wcy = 2 * (cy - double(imageHeight[k]) / 2) / imageHeight[k];
		camera->SetWindowCenter(wcx, wcy);
		double view_angle = 180 * (2.0 * std::atan2(imageHeight[k] / 2.0, fx)) / 3.1415926;
		std::cout << "view_angle = " << view_angle << std::endl;
		vtkSmartPointer<vtkMatrix4x4> m1 = vtkSmartPointer<vtkMatrix4x4>::New();
		m1->DeepCopy(exMat[k]);
		m1->Invert();
		for (int i = 0; i < 4; i++){
			//cout << m1->GetElement(i, 0) << " " << m1->GetElement(i, 1) << " " << m1->GetElement(i, 2) << " " << m1->GetElement(i, 3) << endl;
		}
		camera->SetModelTransformMatrix(m1);
		camera->SetViewAngle(view_angle);
		camera->SetViewUp(0, -1, 0);
		camera->SetPosition(0, 0, 0);
		camera->SetFocalPoint(0, 0, 1);
		camera->SetClippingRange(50, 300);
		renderer->SetActiveCamera(camera);

		renWin->SetSize(554, 830);
		renWin->Render();


		double x, y, z;
		double _x, _y, _z;
		int __x, __y;
		//int num = 0;
		maskSet[k]->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
		imageSet[k]->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

		vtkSmartPointer<vtkMatrix3x3> _m = vtkSmartPointer<vtkMatrix3x3>::New();
		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 3; j++){
				_m->SetElement(i, j, exMat[k]->GetElement(i, j));
			}
		}
		_m->Invert();

		for (int i = 0; i < imagePointSet[k].size(); i++){
			//cout << vertexs->GetPoint(i)[0] << " " << vertexs->GetPoint(i)[1] << " " << vertexs->GetPoint(i)[2] << endl;
			x = _m->GetElement(0, 0) * (vertexs->GetPoint(imagePointSet[k][i])[0] - exMat[k]->GetElement(0, 3)) + _m->GetElement(0, 1) * (vertexs->GetPoint(imagePointSet[k][i])[1] - exMat[k]->GetElement(1, 3))
				+ _m->GetElement(0, 2) * (vertexs->GetPoint(imagePointSet[k][i])[2] - exMat[k]->GetElement(2, 3));
			y = _m->GetElement(1, 0) * (vertexs->GetPoint(imagePointSet[k][i])[0] - exMat[k]->GetElement(0, 3)) + _m->GetElement(1, 1) * (vertexs->GetPoint(imagePointSet[k][i])[1] - exMat[k]->GetElement(1, 3))
				+ _m->GetElement(1, 2) * (vertexs->GetPoint(imagePointSet[k][i])[2] - exMat[k]->GetElement(2, 3));
			z = _m->GetElement(2, 0) * (vertexs->GetPoint(imagePointSet[k][i])[0] - exMat[k]->GetElement(0, 3)) + _m->GetElement(2, 1) * (vertexs->GetPoint(imagePointSet[k][i])[1] - exMat[k]->GetElement(1, 3))
				+ _m->GetElement(2, 2) * (vertexs->GetPoint(imagePointSet[k][i])[2] - exMat[k]->GetElement(2, 3));

			_x = inMat[k]->GetElement(0, 0) * x + inMat[k]->GetElement(0, 1) * y + inMat[k]->GetElement(0, 2) * z;
			_y = inMat[k]->GetElement(1, 0) * x + inMat[k]->GetElement(1, 1) * y + inMat[k]->GetElement(1, 2) * z;
			_z = inMat[k]->GetElement(2, 0) * x + inMat[k]->GetElement(2, 1) * y + inMat[k]->GetElement(2, 2) * z;

			x = _x / _z;
			y = _y / _z;
			y = imageHeight[k] - y;

			__x = CalIntColor(x);
			__y = CalIntColor(y);

			//cout << __x << " " << __y << " " << _z << endl;

			if (x >= 0 && x <= imageWidth[k]-1 && y >= 0 && y <= imageHeight[k]-1){
				unsigned char* pixel = static_cast<unsigned char*>(maskSet[k]->GetScalarPointer(__x, __y, 0));
				if ((int)pixel[0] >= 240 && (int)pixel[0] <= 255){
					//z = _z - ((near * far) / (far - (renderer->GetZ(__x, __y)*(far - near))));
					//if (z < 0.2 && z > -0.2){
					int c_x = (int)x;
					int c_y = (int)y;
					double r_x = x - c_x;
					double r_y = y - c_y;
					double p1 = (1 - r_x)*(1 - r_y);
					double p2 = r_x * (1 - r_y);
					double p3 = (1 - r_x) * r_y;
					double p4 = r_x * r_y;

						unsigned char* pixel1 = static_cast<unsigned char*>(imageSet[k]->GetScalarPointer(c_x, c_y, 0));
						unsigned char* pixel2 = static_cast<unsigned char*>(imageSet[k]->GetScalarPointer(c_x+1, c_y, 0));
						unsigned char* pixel3 = static_cast<unsigned char*>(imageSet[k]->GetScalarPointer(c_x, c_y+1, 0));
						unsigned char* pixel4 = static_cast<unsigned char*>(imageSet[k]->GetScalarPointer(c_x+1, c_y+1, 0));
						pointColorSet[imagePointSet[k][i]].cnt++;
						pointColorSet[imagePointSet[k][i]].r += CalIntColor(p1 * pixel1[0] + p2 * pixel2[0] + p3 * pixel3[0] + p4 * pixel4[0]);
						pointColorSet[imagePointSet[k][i]].g += CalIntColor(p1 * pixel1[1] + p2 * pixel2[1] + p3 * pixel3[1] + p4 * pixel4[1]);
						pointColorSet[imagePointSet[k][i]].b += CalIntColor(p1 * pixel1[2] + p2 * pixel2[2] + p3 * pixel3[2] + p4 * pixel4[2]);
						//cout << num << " " << imagePoint << " " << z << endl;
					//}
				}
			}

		}

	}

	
}

int Optimization::CalIntColor(double x){
	int res;
	if (x - (int)x > 0.5){
		res = (int)x + 1;
	}
	else{
		res = (int)x;
	}
	return res;
}

void Optimization::showInitColor(){

	vtkSmartPointer<vtkUnsignedCharArray> colors =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");
	for (int i = 0; i < pointColorSet.size(); i++){
		unsigned char color[3];
		if (pointColorSet[i].cnt != 0){
			/*if (pointColorSet[i].cnt >3){
			cout << pointColorSet[i].cnt << endl;
			}*/
			pointColorSet[i].r = CalIntColor(pointColorSet[i].r / pointColorSet[i].cnt);
			pointColorSet[i].g = CalIntColor(pointColorSet[i].g / pointColorSet[i].cnt);
			pointColorSet[i].b = CalIntColor(pointColorSet[i].b / pointColorSet[i].cnt);

			/*pointColorSet[i].r = 125;
			pointColorSet[i].g = 0;
			pointColorSet[i].b = 0;*/
			//cout << pointColorSet[i].r << " " << pointColorSet[i].g << " " << pointColorSet[i].b << endl;
		}
		else{
			pointColorSet[i].r = 255;
			pointColorSet[i].g = 255;
			pointColorSet[i].b = 255;
		}

		color[0] = (unsigned char)pointColorSet[i].r;
		color[1] = (unsigned char)pointColorSet[i].g;
		color[2] = (unsigned char)pointColorSet[i].b;

		colors->InsertTypedTuple(i, color);
	}

	mesh->GetPointData()->SetScalars(colors);


	vtkSmartPointer<vtkPolyDataMapper> mapper1 =
		vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
	mapper1->SetInputConnection(polydata->GetProducerPort());
#else
	mapper1->SetInputData(mesh);
#endif

	vtkSmartPointer<vtkActor> actor1 =
		vtkSmartPointer<vtkActor>::New();
	actor1->SetMapper(mapper1);

	vtkSmartPointer<vtkRenderer> renderer1 =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow1 =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow1->AddRenderer(renderer1);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor1 =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor1->SetRenderWindow(renderWindow1);


	renderer1->AddActor(actor1);
	renderWindow1->SetSize(800, 600);

	auto interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactorStyle->SetDefaultRenderer(renderer1);
	renderWindowInteractor1->SetInteractorStyle(interactorStyle);
	renderWindow1->Render();
	renderWindowInteractor1->Start();
}
