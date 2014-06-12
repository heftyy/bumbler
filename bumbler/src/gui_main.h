#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <atan/actor_system/actor_system.h>
#include "bumbler_client_actor.h"

class MyApp : public wxApp {
public:
	virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	
private:
	void OnHello(wxCommandEvent& event);
	void OnAddServer(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
	
	std::unique_ptr<actor_system> actor_system_;
	
	std::string get_address();
};

enum {
	ID_Hello = 1,
	ID_AddServer
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Hello, MyFrame::OnHello)
EVT_MENU(ID_AddServer, MyFrame::OnAddServer)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

bool MyApp::OnInit() {
	MyFrame *frame = new MyFrame("Hello World", wxPoint(50, 50), wxSize(450, 340));
	frame->Show(true);
	return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size) {
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
			"Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&Actions");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("Welcome to wxWidgets!");
	
	actor_system_ = std::unique_ptr<actor_system>(new actor_system("bumbler_system", 8558));
}

void MyFrame::OnExit(wxCommandEvent& event) {
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event) {
	wxMessageBox("This is a wxWidgets' Hello world sample",
			"About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event) {
	wxLogMessage("Hello world from wxWidgets!");
}


void MyFrame::OnAddServer(wxCommandEvent& event) {
	std::string address = get_address();
	
	if(address.length() == 0) 
	{
		wxLogMessage("Error, wrong address!");
	}
	
	std::string rpi_actor_ref = "server_actor$bumbler_server@" + address;

	auto client_actor = std::shared_ptr<bumbler_client_actor>(new bumbler_client_actor(rpi_actor_ref, *actor_system_));
	actor_system_->add_actor(client_actor);

	client_actor->tell(1, "hello");
	
	std::chrono::milliseconds sleep_duration(1000);
	std::this_thread::sleep_for(sleep_duration);

	actor_system_->stop();
}

std::string MyFrame::get_address()
{
	wxTextEntryDialog dialog(this,
		wxT("")
		wxT("Add [ip:port]:"),
		wxT("Bumbler server"),
		wxT("127.0.0.1:8556"),
		wxOK | wxCANCEL);

	if (dialog.ShowModal() == wxID_OK)
	{
		return dialog.GetValue().ToStdString();
	}
	return "";
}