#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include"GeometryRender.h"
int main(int argc, char** argv) {

	GeometryRender app;
	app.startup(0, 0, 600, 400, "firstApp");
	app.run();
	app.shutdown();
	return 0;
}