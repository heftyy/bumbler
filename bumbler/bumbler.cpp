// bumbler.cpp : Defines the entry point for the console application.
//
#include "src/gui_main.h"

int main(int argc, char* argv[])
{
	wxApp::SetInstance(new MyApp());
	wxEntryStart(argc, argv);
	wxTheApp->OnInit();

	std::cout << "bumbler  test" << std::endl;

	wxTheApp->OnRun();
	wxTheApp->OnExit();
	wxEntryCleanup();

	return 0;
}

