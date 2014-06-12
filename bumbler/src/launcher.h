#pragma once

#include "gui_main.h"

class launcher
{
public:
	static launcher& getInstance()
	{
		static launcher instance;
		return instance;
	};

	MyFrame* servers_frame;

private:
	launcher() {}
	launcher(launcher const&);
	void operator=(launcher const&);
};

