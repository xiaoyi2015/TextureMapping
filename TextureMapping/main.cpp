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
	init.LoadMesh(); 
	init.CreateZBuffer();
	init.showInitColor();
	//init.WriteIDInFile();
	/*for (int i = 0; i < init.pointColorSet.size(); i++){
		cout << init.pointColorSet[i].id << " " << init.pointColorSet[i].r << " " << 
			init.pointColorSet[i].g << " " << init.pointColorSet[i].b << endl;

	}
	*/

	//Optimization op = Optimization(init.pointColorSet);
	//op.InitColor();

	cout << init.pointColorSet.size() << endl;
	system("Pause");
    return 0;
}


