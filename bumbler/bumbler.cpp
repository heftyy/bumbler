// bumbler.cpp : Defines the entry point for the console application.
//
#include "src/my_app.h"

int main(int argc, char* argv[])
{
	wxApp::SetInstance(new MyApp());
	wxEntryStart(argc, argv);
	wxTheApp->OnInit();

	std::cout << "bumbler test" << std::endl;

	wxTheApp->OnRun();
	wxTheApp->OnExit();
	wxEntryCleanup();

	return 0;
}

