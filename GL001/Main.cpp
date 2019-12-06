#include"GeometryRender.h"
#include "ModelRender.h"
#include <Windows.h>
GLApplication* GLApplication::application = nullptr;

void _sleep(long ms) {
	if (ms <= 0)return;
	Sleep(ms);
}


int main(int argc, char** argv) {

	ModelRender app;
	app.startup(0, 0, 800, 600, "firstApp");
	app.run();
	app.shutdown();
	return 0;
}