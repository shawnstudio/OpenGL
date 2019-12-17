#include"GeometryRender.h"
#include "ModelRender.h"
#include <Windows.h>
GLApplication* GLApplication::application = nullptr;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

extern GLApplication* createSpring();
extern GLApplication* createTessllaton();

void _sleep(long ms) {
	if (ms <= 0)return;
	Sleep(ms);
}


int main(int argc, char** argv) {

	GLApplication* app = createTessllaton();
	app->startup(0, 0, 1200, 400, "firstApp");
	app->run();
	app->shutdown();
	delete app;
	return 0;
}