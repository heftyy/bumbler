#include "my_app.h"

MyFrame* MyApp::frame_servers = NULL;

bool MyApp::OnInit() {
	frame_servers = new MyFrame("Bumbling along", wxPoint(50, 50), wxSize(450, 340));
	frame_servers->Show(true);
	return true;
}