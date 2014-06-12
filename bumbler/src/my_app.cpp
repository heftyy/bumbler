#include "my_app.h"
#include "gui_main.h"
#include "launcher.h"

bool MyApp::OnInit() {
	MyFrame *frame = new MyFrame("Bumbling along", wxPoint(50, 50), wxSize(450, 340));
	frame->Show(true);
	launcher::getInstance().servers_frame = frame;
	return true;
}