#include "preprocess.h"

#include <vtkBMPWriter.h>
#include <vtkBMPReader.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageShiftScale.h>
#include <vtkPLYWriter.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkMatrix3x3.h>

double zbuffer[num][840][840];

PreProcess::PreProcess(){
	mesh = vtkSmartPointer<vtkPolyData>::New();
	vertexs = vtkSmartPointer<vtkPoints>::New();

	reader = vtkSmartPointer<vtkPLYReader>::New();
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	actor = vtkSmartPointer<vtkActor>::New();
	renderer = vtkSmartPointer<vtkRenderer>::New();
	renWin = vtkSmartPointer<vtkRenderWindow>::New();
}

PreProcess::~PreProcess(){

}

void PreProcess::LoadMesh(){
	
	string meshFilename = "C://Users//zln//Desktop//Cui//total_usc_cleaned.ply";
	reader->SetFileName(meshFilename.c_str());
	reader->Update();
	mesh = reader->GetOutput();
	vertexs = mesh->GetPoints();
	pointColorSet.resize(vertexs->GetNumberOfPoints(), Vector4());

	cout << "load mesh success" << endl;

}


void PreProcess::LoadImages(){
	char imageFilename[128];
	int dims[3];
	for (int i = 0; i < num; i ++) 
	{
		vtkSmartPointer<vtkJPEGReader> imageReader = vtkSmartPointer<vtkJPEGReader>::New();
		sprintf(imageFilename, "C://Users//zln//Desktop//Cui//color_small//%d.jpg", i);
		vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
		imageReader->SetFileName(imageFilename);
		imageReader->Update();
		imageData = imageReader->GetOutput();
		imageSet.push_back(imageData);
		imageData->GetDimensions(dims);
		imageWidth.push_back(dims[0]);
		imageHeight.push_back(dims[1]);

		//imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
		//std::cout << "像素数:" << imageData->GetNumberOfPoints() << std::endl;

		/*for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				unsigned char* pixel = static_cast<unsigned char*>(imageData->GetScalarPointer(x, y, 0));
				if (pixel[0] > 0.0)

					std::cout << "像素zhi:" << (int)pixel[0] << " " << (int)pixel[1] << " " << (int)pixel[2] << std::endl;
			}
		}*/
		//unsigned char* pixel = static_cast<unsigned char*>(imageData->GetScalarPointer(x, y, z));
	}

	for (int i = 0; i < 30; i++){
		if (i > 23){
			imageWidth.push_back(h);
			imageHeight.push_back(w);
		}
		else
		{
			imageWidth.push_back(w);
			imageHeight.push_back(h);
		}
	}
	cout << "load images success" << endl;
}

void PreProcess::LoadMasks(){
	char maskFilename[128];
	int dims[3];
	for (int i = 0; i < num; i++)
	{
		vtkSmartPointer<vtkJPEGReader> maskReader = vtkSmartPointer<vtkJPEGReader>::New();
		sprintf(maskFilename, "C://Users//zln//Desktop//Cui//mask_small//%d.jpg", i);
		vtkSmartPointer<vtkImageData> maskData = vtkSmartPointer<vtkImageData>::New();
		maskReader->SetFileName(maskFilename);
		maskReader->Update();
		maskData = maskReader->GetOutput();
		maskSet.push_back(maskData);
		//std::cout << "像素数:" << maskData->GetNumberOfPoints() << std::endl;

		/*maskData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				unsigned char* pixel = static_cast<unsigned char*>(maskData->GetScalarPointer(x, y, 0));
				if (pixel[0] > 0.0)

				std::cout << "像素zhi:" << (int)pixel[0] << std::endl;
			}
		}*/
		//unsigned char* pixel = static_cast<unsigned char*>(imageData->GetScalarPointer(x, y, z));
	}
	cout << "load masks success" << endl;
}

void PreProcess::LoadInMatrix(){
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
				file >> a ;
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

void PreProcess::LoadExMatrix(){
	ifstream file;
	char exFilename[128];
	double a;
	for (int k = 1; k < num+1; k++){
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

void PreProcess::CreateZBuffer(){

	for (int i = 0; i < num; i++){
		vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkSmartPointer<vtkMatrix4x4> m2 = vtkSmartPointer<vtkMatrix4x4>::New();

		m->DeepCopy(exMat[i]);
		m->Invert();
		for (int i = 0; i < 4; i++){
			//cout << m->GetElement(i, 0) << " " << m->GetElement(i, 1) << " " << m->GetElement(i, 2) << " " << m->GetElement(i, 3) << endl;
		}
		vtkMatrix4x4::Multiply4x4(inMat[i], m, m2);
		proMat.push_back(m2);
		for (int i = 0; i < 4; i++){
			//cout << m2->GetElement(i, 0) << " " << m2->GetElement(i, 1) << " " << m2->GetElement(i, 2) << " " << m2->GetElement(i, 3) << endl;
		}
	}
	mapper->SetInputConnection(reader->GetOutputPort());
	actor->SetMapper(mapper);
	renderer->AddActor(actor);
	renWin->AddRenderer(renderer);
	
	char name[128];
	for (int k = 0; k < num; k++){
		double fx, cx, cy;
		fx = inMat[k]->GetElement(0, 0) / imageWidth[num + k] * imageWidth[k];
		cx = inMat[k]->GetElement(0, 2) / imageWidth[num + k] * imageWidth[k];
		cy = inMat[k]->GetElement(1, 2) / imageHeight[num + k] * imageHeight[k];
		cout << fx << " " << cx << " " << cy << endl;
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
		camera->SetClippingRange(near, far);
		renderer->SetActiveCamera(camera);
		
		//std::cout << "matrix1:" << m1->GetElement(0, 0) << " " << m1->GetElement(0, 1) << " " << m1->GetElement(0, 2) << " " << m1->GetElement(0, 3) << endl;
		//std::cout << "matrix1:" << m1->GetElement(1, 0) << " " << m1->GetElement(1, 1) << " " << m1->GetElement(1, 2) << " " << m1->GetElement(1, 3) << endl;
		//std::cout << "matrix1:" << m1->GetElement(2, 0) << " " << m1->GetElement(2, 1) << " " << m1->GetElement(2, 2) << " " << m1->GetElement(2, 3) << endl;
		//std::cout << "matrix1:" << m1->GetElement(3, 0) << " " << m1->GetElement(3, 1) << " " << m1->GetElement(3, 2) << " " << m1->GetElement(3, 3) << endl;

		//设置camera矩阵
		/*vtkSmartPointer<vtkTransform> transform =
			vtkSmartPointer<vtkTransform>::New();
		transform->SetMatrix(m1);
		transform->Update();

		renderer->GetActiveCamera()->ApplyTransform(transform);*/

		renWin->SetSize(imageWidth[k], imageHeight[k]);
		renWin->Render();

		//生成深度图并保存
		vtkSmartPointer<vtkWindowToImageFilter> filter =
			vtkSmartPointer<vtkWindowToImageFilter>::New();

		vtkSmartPointer<vtkBMPWriter> imageWriter =
			vtkSmartPointer<vtkBMPWriter>::New();

		vtkSmartPointer<vtkImageShiftScale> scale =
			vtkSmartPointer<vtkImageShiftScale>::New();

		filter->SetInput(renWin);
		filter->SetMagnification(1);
		filter->SetInputBufferTypeToZBuffer();        //Extract z buffer value

		scale->SetOutputScalarTypeToUnsignedChar();
		scale->SetInputConnection(filter->GetOutputPort());
		scale->SetShift(0);
		scale->SetScale(-255);

		
		sprintf(name, "C://Users//zln//Desktop//Cui//output//%d.bmp", k);

		imageWriter->SetFileName(name);
		imageWriter->SetInputConnection(scale->GetOutputPort());
		imageWriter->Write();
		
		InitDataSet(k);

		//利用投影矩阵求z
		//vertexs->GetNumberOfPoints();
		//double x, y, z;
		//double _x, _y, _z;
		//int __x, __y;

		//maskSet[k]->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
		//imageSet[k]->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
		//vector<int> imagePoints;

		//for (int i = 0; i < vertexs->GetNumberOfPoints(); i++){
		//	//cout << vertexs->GetPoint(i)[0] << " " << vertexs->GetPoint(i)[1] << " " << vertexs->GetPoint(i)[2] << endl;;

		//	x = proMat[k]->GetElement(0, 0) * vertexs->GetPoint(i)[0] + proMat[k]->GetElement(0, 1) * vertexs->GetPoint(i)[1] + proMat[k]->GetElement(0, 2) * vertexs->GetPoint(i)[2] + proMat[k]->GetElement(0, 3) * 1.0;
		//	y = proMat[k]->GetElement(1, 0) * vertexs->GetPoint(i)[0] + proMat[k]->GetElement(1, 1) * vertexs->GetPoint(i)[1] + proMat[k]->GetElement(1, 2) * vertexs->GetPoint(i)[2] + proMat[k]->GetElement(1, 3) * 1.0;
		//	z = proMat[k]->GetElement(2, 0) * vertexs->GetPoint(i)[0] + proMat[k]->GetElement(2, 1) * vertexs->GetPoint(i)[1] + proMat[k]->GetElement(2, 2) * vertexs->GetPoint(i)[2] + proMat[k]->GetElement(2, 3) * 1.0;
		//	x = x / imageWidth[num + k] / z * imageWidth[k];
		//	y = y / imageWidth[num + k] / z * imageWidth[k];
		//	__x = (int)x;
		//	__y = -(int)y + imageHeight[k];
		//	//cout << num << " " << _x << " " << _y << " " << z << endl;
		//	if (__x >= 0 && __x < imageWidth[k] && __y >= 0 && __y < imageHeight[k]){
		//		//unsigned char* pixel = static_cast<unsigned char*>(maskSet[k]->GetScalarPointer(__x, __y, 0));
		//		//if ((int)pixel[0] > 0){
		//			z = z - ((near * far) / (far - (renderer->GetZ(__x, __y)*(far - near))));
		//			//if (z < 10 && z > -10){
		//			unsigned char* pixel1 = static_cast<unsigned char*>(imageSet[k]->GetScalarPointer(__x, __y, 0));
		//			int imagePoint;
		//			imagePoint = i;
		//			imagePoints.push_back(imagePoint);
		//			//num++;
		//			pointColorSet[i].cnt++;
		//			pointColorSet[i].r += (int)pixel1[0];
		//			pointColorSet[i].g += (int)pixel1[1];
		//			pointColorSet[i].b += (int)pixel1[2];
		//			//cout << num << " " << imagePoint << " " << z << endl;
		//			//}
		//		//}
		//	}
		//}
		//imagePointSet.push_back(imagePoints);

	}
}

void PreProcess::InitDataSet(int k){

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
	vector<int> imagePoints;

	for (int i = 0; i < vertexs->GetNumberOfPoints(); i++){
		//cout << vertexs->GetPoint(i)[0] << " " << vertexs->GetPoint(i)[1] << " " << vertexs->GetPoint(i)[2] << endl;
		x = _m->GetElement(0, 0) * (vertexs->GetPoint(i)[0] - exMat[k]->GetElement(0, 3)) + _m->GetElement(0, 1) * (vertexs->GetPoint(i)[1] - exMat[k]->GetElement(1, 3))
			+ _m->GetElement(0, 2) * (vertexs->GetPoint(i)[2] - exMat[k]->GetElement(2, 3));
		y = _m->GetElement(1, 0) * (vertexs->GetPoint(i)[0] - exMat[k]->GetElement(0, 3)) + _m->GetElement(1, 1) * (vertexs->GetPoint(i)[1] - exMat[k]->GetElement(1, 3))
			+ _m->GetElement(1, 2) * (vertexs->GetPoint(i)[2] - exMat[k]->GetElement(2, 3));
		z = _m->GetElement(2, 0) * (vertexs->GetPoint(i)[0] - exMat[k]->GetElement(0, 3)) + _m->GetElement(2, 1) * (vertexs->GetPoint(i)[1] - exMat[k]->GetElement(1, 3))
			+ _m->GetElement(2, 2) * (vertexs->GetPoint(i)[2] - exMat[k]->GetElement(2, 3));

		_x = inMat[k]->GetElement(0, 0) * x + inMat[k]->GetElement(0, 1) * y + inMat[k]->GetElement(0, 2) * z;
		_y = inMat[k]->GetElement(1, 0) * x + inMat[k]->GetElement(1, 1) * y + inMat[k]->GetElement(1, 2) * z;
		_z = inMat[k]->GetElement(2, 0) * x + inMat[k]->GetElement(2, 1) * y + inMat[k]->GetElement(2, 2) * z;

		x = _x / imageWidth[num + k] / _z * imageWidth[k];
		y = _y / imageHeight[num + k] / _z * imageHeight[k];

		__x = CalIntColor(x);
		__y = CalIntColor(imageHeight[k] - y);
		
		//cout << __x << " " << __y << " " << _z << endl;

		if (__x >= 0 && __x < imageWidth[k] && __y >= 0 && __y < imageHeight[k]){
			unsigned char* pixel = static_cast<unsigned char*>(maskSet[k]->GetScalarPointer(__x, __y, 0));
			if ((int)pixel[0] > 254){
 				z = _z - ((near * far) / (far - (renderer->GetZ(__x, __y)*(far - near))));
				if (z < 0.2 && z > - 0.2){
					unsigned char* pixel1 = static_cast<unsigned char*>(imageSet[k]->GetScalarPointer(__x, __y, 0));
					int imagePoint;
					imagePoint = i;
					imagePoints.push_back(imagePoint);
					//num++;
					pointColorSet[i].cnt++;
					pointColorSet[i].r += (int)pixel1[0];
					pointColorSet[i].g += (int)pixel1[1];
					pointColorSet[i].b += (int)pixel1[2];
					//cout << num << " " << imagePoint << " " << z << endl;
				}
			}
		}

	}
	imagePointSet.push_back(imagePoints);
}

int PreProcess::CalIntColor(double x){
	int res;
	if (x - (int)x > 0.5){
		res = (int)x + 1;
	}
	else{
		res = (int)x;
	}
	return res;
}

void PreProcess::showInitColor(){

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
		}else{
			pointColorSet[i].r = 0;
			pointColorSet[i].g = 0;
			pointColorSet[i].b = 255;
		}
		
		color[0] = (unsigned char)pointColorSet[i].r;
		color[1] = (unsigned char)pointColorSet[i].g;
		color[2] = (unsigned char)pointColorSet[i].b;
		colors->InsertTypedTuple(i, color);
	}
		
	
	
	mesh->GetPointData()->SetScalars(colors);


	/*string ss = "C://Users//zln//Desktop//Cui//output.ply";
	vtkSmartPointer<vtkPLYWriter> plyWriter =
		vtkSmartPointer<vtkPLYWriter>::New();
	plyWriter->SetFileName(ss.c_str());
	plyWriter->SetInputConnection(reader->GetOutputPort());
	plyWriter->Write();*/
	

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

	//char name[128];
	//
	//	double fx, cx, cy;
	//	fx = inMat[0]->GetElement(0, 0) / imageWidth[num] * imageWidth[0];
	//	cx = inMat[0]->GetElement(0, 2) / imageWidth[num] * imageWidth[0];
	//	cy = inMat[0]->GetElement(1, 2) / imageWidth[num] * imageWidth[0];
	//	cout << fx << " " << cx << " " << cy << endl;
	//	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

	//	double wcx = -2 * (cx - double(imageWidth[0]) / 2) / imageWidth[0];
	//	double wcy = 2 * (cy - double(imageHeight[0]) / 2) / imageHeight[0];
	//	camera->SetWindowCenter(wcx, wcy);
	//	double view_angle = 180 * (2.0 * std::atan2(imageHeight[0] / 2.0, fx)) / 3.1415926;
	//	std::cout << "view_angle = " << view_angle << std::endl;
	//	vtkSmartPointer<vtkMatrix4x4> m1 = vtkSmartPointer<vtkMatrix4x4>::New();
	//	m1->DeepCopy(exMat[0]);
	//	m1->Invert();
	//	for (int i = 0; i < 4; i++){
	//		//cout << m1->GetElement(i, 0) << " " << m1->GetElement(i, 1) << " " << m1->GetElement(i, 2) << " " << m1->GetElement(i, 3) << endl;
	//	}
	//	camera->SetModelTransformMatrix(m1);
	//	camera->SetViewAngle(view_angle);
	//	camera->SetViewUp(0, -1, 0);
	//	camera->SetPosition(0, 0, 0);
	//	camera->SetFocalPoint(0, 0, 1);
	//	camera->SetClippingRange(near, far);
	//	renderer1->SetActiveCamera(camera);

	renderer1->AddActor(actor1);
	renderWindow1->SetSize(800, 600);
	
	auto interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactorStyle->SetDefaultRenderer(renderer1);
	renderWindowInteractor1->SetInteractorStyle(interactorStyle);
	renderWindow1->Render();
	renderWindowInteractor1->Start();
}


void PreProcess::WriteIDInFile(){
	char idFilename[128];
	for (int i = 0; i < num; i++){
		sprintf(idFilename, "C://Users//zln//Desktop//Cui//Id//%d.txt", i);

		fstream f(idFilename, ios::out);
		for (int j = 0; j < imagePointSet[i].size(); j++)
			f << imagePointSet[i][j] << endl; //写入数据
		f.close();
		
	}
}