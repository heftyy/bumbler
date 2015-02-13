#pragma once

#include <atan/actor_system/actor_system.h>
#include <wx/wx.h>
#include "frame_servers.h"

class MyApp : public wxApp {
public:
	virtual bool OnInit();
	static MyFrame* frame_servers;
};
