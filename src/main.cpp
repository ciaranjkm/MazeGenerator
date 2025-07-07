#include "include/application.h"

int main(int argc, char** argv[])
{
	application* app = new application();
	
	if (app != nullptr) {
		app->run();

		delete app;
		app = nullptr;

		return 0;
	}
	
	delete app;
	return -1;
}