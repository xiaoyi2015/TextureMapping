//This project is to optimize the color map of the restruction of 3D human body

#include "preprocess.h"
#include "optimization.h"


int main(int argc, char *argv[])
{
	PreProcess init = PreProcess();
	init.LoadImages();
	init.LoadMasks();
	init.LoadExMatrix();
	init.LoadInMatrix();
	//init.CalProMatrix();
	init.LoadMesh(); 
	init.CreateZBuffer();
	
	
	
	

	system("Pause");
    return 0;
}


