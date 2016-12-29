#include "preprocess.h"

#include <vtkBMPWriter.h>
#include <vtkBMPReader.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageShiftScale.h>

#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkMatrix3x3.h>

double zbuffer[num][h][h];

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
		//imageData->GetDimensions(dims);

		//imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
		//std::cout << "ÏñËØÊý:" << imageData->GetNumberOfPoints() << std::endl;

		/*for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				unsigned char* pixel = static_cast<unsigned char*>(imageData->GetScalarPointer(x, y, 0));
				if (pixel[0] > 0.0)

					std::cout << "ÏñËØzhi:" << (int)pixel[0] << " " << (int)pixel[1] << " " << (int)pixel[2] << std::endl;
			}
		}*/
		//unsigned char* pixel = static_cast<unsigned char*>(imageData->GetScalarPointer(x, y, z));
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
		std::cout << "ÏñËØÊý:" << maskData->GetNumberOfPoints() << std::endl;

		/*maskData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				unsigned char* pixel = static_cast<unsigned char*>(maskData->GetScalarPointer(x, y, 0));
				if (pixel[0] > 0.0)

				std::cout << "ÏñËØzhi:" << (int)pixel[0] << std::endl;
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

	mapper->SetInputConnection(reader->GetOutputPort());
	actor->SetMapper(mapper);
	renderer->AddActor(actor);
	renWin->AddRenderer(renderer);
	
	char name[128];
	for (int k = 11; k < 12; k++){
		double fx, cx, cy;
		fx = inMat[k]->GetElement(0, 0) / 5184 * h;
		cx = inMat[k]->GetElement(0, 2) / 3456 * w;
		cy = inMat[k]->GetElement(1, 2) / 5184 * h;
		cout << fx << " " << cx << " " << cy << endl;
		vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

		double wcx = -2 * (cx - double(w) / 2) / w;
		double wcy = 2 * (cy - double(h) / 2) / h;
		camera->SetWindowCenter(wcx, wcy);
		double view_angle = 180 * (2.0 * std::atan2(h / 2.0, fx)) / 3.1415926;
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

		/*vtkSmartPointer<vtkTransform> transform =
			vtkSmartPointer<vtkTransform>::New();
		transform->SetMatrix(m1);
		transform->Update();

		renderer->GetActiveCamera()->ApplyTransform(transform);*/

		renWin->SetSize(554, 830);
		renWin->Render();
		//

		//vtkSmartPointer<vtkWindowToImageFilter> filter =
		//	vtkSmartPointer<vtkWindowToImageFilter>::New();

		//vtkSmartPointer<vtkBMPWriter> imageWriter =
		//	vtkSmartPointer<vtkBMPWriter>::New();

		//vtkSmartPointer<vtkImageShiftScale> scale =
		//	vtkSmartPointer<vtkImageShiftScale>::New();

		//filter->SetInput(renWin);
		//filter->SetMagnification(1);
		//filter->SetInputBufferTypeToZBuffer();        //Extract z buffer value

		//scale->SetOutputScalarTypeToUnsignedChar();
		//scale->SetInputConnection(filter->GetOutputPort());
		//scale->SetShift(0);
		//scale->SetScale(-255);

		//
		//sprintf(name, "C://Users//zln//Desktop//Cui//%d.bmp", k);

		//imageWriter->SetFileName(name);
		//imageWriter->SetInputConnection(scale->GetOutputPort());
		//imageWriter->Write();
		double x, y ,z;
		double _x, _y, _z;
		int __x, __y;
		int num = 0;
		maskSet[k]->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
		vtkSmartPointer<vtkMatrix3x3> _m = vtkSmartPointer<vtkMatrix3x3>::New();
		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 3; j++){
				_m->SetElement(i, j, exMat[k]->GetElement(i,j));
			}
		}
		_m->Invert();

		for (int i = 0; i < vertexs->GetNumberOfPoints(); i++){
			x = _m->GetElement(0, 0) * (vertexs->GetPoint(i)[0] - exMat[k]->GetElement(0, 3)) + _m->GetElement(0, 1) * (vertexs->GetPoint(i)[1] - exMat[k]->GetElement(1, 3))
				+ _m->GetElement(0, 2) * (vertexs->GetPoint(i)[2] - exMat[k]->GetElement(2, 3));
			y = _m->GetElement(1, 0) * (vertexs->GetPoint(i)[0] - exMat[k]->GetElement(0, 3)) + _m->GetElement(1, 1) * (vertexs->GetPoint(i)[1] - exMat[k]->GetElement(1, 3))
				+ _m->GetElement(1, 2) * (vertexs->GetPoint(i)[2] - exMat[k]->GetElement(2, 3));
			z = _m->GetElement(2, 0) * (vertexs->GetPoint(i)[0] - exMat[k]->GetElement(0, 3)) + _m->GetElement(2, 1) * (vertexs->GetPoint(i)[1] - exMat[k]->GetElement(1, 3))
				+ _m->GetElement(2, 2) * (vertexs->GetPoint(i)[2] - exMat[k]->GetElement(2, 3));

			_x = inMat[k]->GetElement(0, 0) * x + inMat[k]->GetElement(0, 1) * y + inMat[k]->GetElement(0, 2) * z;
			_y = inMat[k]->GetElement(1, 0) * x + inMat[k]->GetElement(1, 1) * y + inMat[k]->GetElement(1, 2) * z;
			_z = inMat[k]->GetElement(2, 0) * x + inMat[k]->GetElement(2, 1) * y + inMat[k]->GetElement(2, 2) * z;

			x = _x / 3456 / z * w;
			y = _y / 5184 / z * h;
			__x = (int)x;
			__y = (int)y;
			//cout << __x << " " << __y << " " << _z << endl;

			if (__x >= 0 && __x < w && __y >= 0 && __y < h){
				unsigned char* pixel = static_cast<unsigned char*>(maskSet[k]->GetScalarPointer(__x, __y, 0));
				if ((int)pixel[0] > 0){
					z = z - ((near * far) / (far - (renderer->GetZ(__x, __y)*(far - near))));
					if (z < 0.5 && z > -0.5){
						num++;
						cout << num << " " << __x << " " << __y << " " << z << endl;
					}
				}
			}

		}
		//vertexs->GetNumberOfPoints();
		
		//for (int i = 0; i < vertexs->GetNumberOfPoints(); i++){
		//	//cout << vertexs->GetPoint(i)[0] << " " << vertexs->GetPoint(i)[1] << " " << vertexs->GetPoint(i)[2] << endl;;

		//	x = proMat[k]->GetElement(0, 0) * vertexs->GetPoint(i)[0] + proMat[k]->GetElement(0, 1) * vertexs->GetPoint(i)[1] + proMat[k]->GetElement(0, 2) * vertexs->GetPoint(i)[2] + proMat[k]->GetElement(0, 3) * 1.0;
		//	y = proMat[k]->GetElement(1, 0) * vertexs->GetPoint(i)[0] + proMat[k]->GetElement(1, 1) * vertexs->GetPoint(i)[1] + proMat[k]->GetElement(1, 2) * vertexs->GetPoint(i)[2] + proMat[k]->GetElement(1, 3) * 1.0;
		//	z = proMat[k]->GetElement(2, 0) * vertexs->GetPoint(i)[0] + proMat[k]->GetElement(2, 1) * vertexs->GetPoint(i)[1] + proMat[k]->GetElement(2, 2) * vertexs->GetPoint(i)[2] + proMat[k]->GetElement(2, 3) * 1.0;
		//	x = x / 3456 / z * w;
		//	y = y / 5184 / z * h;
		//	__x = (int)x;
		//	__y = (int)y;
		//	//cout << num << " " << _x << " " << _y << " " << z << endl;
		//	if (__x >= 0 && __x < w && __y >= 0 && __y < h){
		//		unsigned char* pixel = static_cast<unsigned char*>(maskSet[k]->GetScalarPointer(__x, __y, 0));
		//		if ((int)pixel[0] > 0){
		//			z = z - ((near * far) / (far - (renderer->GetZ(__x, __y)*(far - near))));
		//			if (z < 2 && z > -2){
		//				num++;
		//				cout << num << " " << __x << " " << __y << " " << z << endl;
		//			}
		//		}
		//	}
		//}


	}
}

void PreProcess::CalProMatrix(){
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
}


void PreProcess::InitDataIDSet(){

}