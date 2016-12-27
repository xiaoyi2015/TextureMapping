#include "preprocess.h"
#include <vtkBMPWriter.h>
#include <vtkBMPReader.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageShiftScale.h>

PreProcess::PreProcess(){
	mesh = vtkSmartPointer<vtkPolyData>::New();
	vertexs = vtkSmartPointer<vtkPoints>::New();

	reader = vtkSmartPointer<vtkPLYReader>::New();
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	actor = vtkSmartPointer<vtkActor>::New();
	renderer = vtkSmartPointer<vtkRenderer>::New();
	renWin = vtkSmartPointer<vtkRenderWindow>::New();
	interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
}

PreProcess::~PreProcess(){

}

void PreProcess::LoadMesh(){
	
	string meshFilename = "C://Users//zln//Desktop//Cui//total_usc_cleaned.ply";
	reader->SetFileName(meshFilename.c_str());
	reader->Update();
	mesh = reader->GetOutput();
	cout << "load mesh success" << endl;

}


void PreProcess::LoadImages(){
	vtkSmartPointer<vtkJPEGReader> imageReader = vtkSmartPointer<vtkJPEGReader>::New();
	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
	char imageFilename[128];
	int dims[3];
	for (int i = 0; i < 30; i ++) 
	{
		sprintf(imageFilename, "C://Users//zln//Desktop//Cui//color_small//%d.jpg", i);
		imageReader->SetFileName(imageFilename);
		imageReader->Update();
		imageData = imageReader->GetOutput();
		imageSet.push_back(imageData);
		imageData->GetDimensions(dims);

		imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
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
	vtkSmartPointer<vtkJPEGReader> maskReader = vtkSmartPointer<vtkJPEGReader>::New();
	vtkSmartPointer<vtkImageData> maskData = vtkSmartPointer<vtkImageData>::New();
	char maskFilename[128];
	int dims[3];
	for (int i = 0; i < 30; i++)
	{
		sprintf(maskFilename, "C://Users//zln//Desktop//Cui//mask_small//%d.jpg", i);
		maskReader->SetFileName(maskFilename);
		maskReader->Update();
		maskData = maskReader->GetOutput();
		maskSet.push_back(maskData);
		maskData->GetDimensions(dims);

		maskData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		//std::cout << "ÏñËØÊý:" << maskData->GetNumberOfPoints() << std::endl;

		/*for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
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
	vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
	double a;
	string s;
	for (int k = 0; k < 30; k++){
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
			for (int j = 0; j < 4; j++){
				//cout << m->GetElement(i, j) << endl;
			}
		}
	}
	cout << "load inMats success" << endl;
}

void PreProcess::LoadExMatrix(){
	ifstream file;
	char exFilename[128];
	double a;
	for (int k = 1; k < 31; k++){
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
		//for (int i = 0; i < 4; i++){
			//cout << m->GetElement(i, 0) << " " << m->GetElement(i, 1) <<" " << m->GetElement(i, 2) << " " << m->GetElement(i, 3) << endl;
		//}
	}
	cout << "load exMats success" << endl;
}

void PreProcess::InitPointSet(){
	cout << mesh->GetNumberOfPoints() << endl;
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
	cout << "init mesh success" << endl;
}

void PreProcess::InitImageSet(){

}

void PreProcess::CreateZBuffer(){

	mapper->SetInputConnection(reader->GetOutputPort());
	actor->SetMapper(mapper);
	renderer->AddActor(actor);
	renWin->AddRenderer(renderer);

	renderer->GetActiveCamera()->Roll(180);
	renderer->GetActiveCamera()->SetPosition(0, 0, 0);
	renderer->GetActiveCamera()->SetFocalPoint(0, 0, 1);
	//renderer->GetActiveCamera()->ComputeViewPlaneNormal();
	renderer->GetActiveCamera()->SetClippingRange(100, 150);
	//renderer->GetActiveCamera()->ComputeViewPlaneNormal();
	for (int i = 0; i < 1; i++){
		vtkSmartPointer<vtkMatrix4x4> m1 = vtkSmartPointer<vtkMatrix4x4>::New();
		m1->DeepCopy(exMat[i]);

		//std::cout << "matrix1:" << m1->GetElement(0, 0) << " " << m1->GetElement(0, 1) << " " << m1->GetElement(0, 2) << " " << m1->GetElement(0, 3) << endl;
		//std::cout << "matrix1:" << m1->GetElement(1, 0) << " " << m1->GetElement(1, 1) << " " << m1->GetElement(1, 2) << " " << m1->GetElement(1, 3) << endl;
		//std::cout << "matrix1:" << m1->GetElement(2, 0) << " " << m1->GetElement(2, 1) << " " << m1->GetElement(2, 2) << " " << m1->GetElement(2, 3) << endl;
		//std::cout << "matrix1:" << m1->GetElement(3, 0) << " " << m1->GetElement(3, 1) << " " << m1->GetElement(3, 2) << " " << m1->GetElement(3, 3) << endl;

		vtkSmartPointer<vtkTransform> transform =
			vtkSmartPointer<vtkTransform>::New();
		transform->SetMatrix(m1);
		transform->Update();

		renderer->GetActiveCamera()->ApplyTransform(transform);

		renWin->SetSize(554, 830);
		renWin->Render();

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

		imageWriter->SetFileName("C://Users//zln//Desktop//Cui//output1.bmp");
		imageWriter->SetInputConnection(scale->GetOutputPort());
		imageWriter->Write();

		//for (int k = 0; k < 3456; k++){
			for (int j = 0; j < 10; j++)
				//if (renderer->GetZ(k, j) < 1)
					cout << renderer->GetZ(225, 410+j) << endl;
			//}
		//}

	}
}

void PreProcess::DataPreProcess(){

}