//This project is to optimize the color map of the restruction of 3D human body

#include "preprocess.h"
#include "optimization.h"


int main(int argc, char *argv[])
{
	PreProcess init = PreProcess();
	init.LoadExMatrix();
	init.LoadMesh(); 
	init.CreateZBuffer();
	//init.LoadExMatrix();
	//init.LoadImages();
	//init.LoadMasks();
	//init.InitPointSet();

	system("Pause");
    return 0;
}


