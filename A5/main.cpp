#include"raytracer.h"
int main() {
	Raytracer* raytracer = new Raytracer;
	raytracer->SetInput( "scene.txt" );
	//raytracer->SetOutput( "picture.bmp" );
	raytracer->SetOutput( "pictureT.bmp" );
	//raytracer->Run();
	raytracer->MultiThreadRun();
	//raytracer->DebugRun(740,760,410,430);
	return 0;
}
