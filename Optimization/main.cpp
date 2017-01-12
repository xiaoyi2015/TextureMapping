//This project is to optimize the color map of the restruction of 3D human body

#include "optimization.h"

using namespace std;


int main(int argc, char *argv[])
{
	Optimization init = Optimization();
	init.LoadPointId();
	init.LoadImages();
	init.LoadMasks();
	init.LoadExMatrix();
	init.LoadInMatrix();
	init.LoadMesh(); 
	init.CalProjCoord();
	init.showInitColor();
	/*for (int i = 0; i < init.pointColorSet.size(); i++){
		cout << init.pointColorSet[i].id << " " << init.pointColorSet[i].r << " " << 
			init.pointColorSet[i].g << " " << init.pointColorSet[i].b << endl;

	}
	*/

	//Optimization op = Optimization(init.pointColorSet);
	//op.InitColor();

	//cout << init.pointColorSet.size() << endl;
	system("Pause");
    return 0;
}


