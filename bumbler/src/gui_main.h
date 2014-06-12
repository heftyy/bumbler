#pragma once

#include "server_list.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class MyApp : public wxApp {
public:
	virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	
private:
	std::unique_ptr<wxListBox> servers;
	std::unique_ptr<server_list> server_list_ptr;

	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnAddServer(wxCommandEvent& event);
	void OnDeleteServer(wxCommandEvent& event);
	void OnConnect(wxCommandEvent& event); //Used for button and double click on server hopefully...
	wxDECLARE_EVENT_TABLE();

};

enum {
	BUTTON_AddServer = wxID_HIGHEST,
	BUTTON_Connect,
	BUTTON_DeleteServer,
	BUTTON_AddText,
	LISTBOX_Servers
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_BUTTON(BUTTON_AddServer, MyFrame::OnAddServer)
EVT_BUTTON(BUTTON_Connect, MyFrame::OnConnect)
EVT_BUTTON(BUTTON_DeleteServer, MyFrame::OnDeleteServer)
EVT_LISTBOX_DCLICK(LISTBOX_Servers, MyFrame::OnConnect)
wxEND_EVENT_TABLE()

bool MyApp::OnInit() {
	server_list = std::unique_ptr<class server_list>(new  class server_list());
	MyFrame *frame = new MyFrame("Bumbling along", wxPoint(50, 50), wxSize(450, 340));
	frame->Show(true);
	return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size) {
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&Actions");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("Welcome to wxWidgets!");

	servers = std::unique_ptr<wxListBox>(new wxListBox(this, LISTBOX_Servers, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL | wxLB_SINGLE | wxLB_NEEDED_SB));
	wxButton *addServer = new wxButton(this, BUTTON_AddServer, _T("Add Server"), wxDefaultPosition, wxDefaultSize, 0);
	wxButton *deleteServer = new wxButton(this, BUTTON_DeleteServer, _T("Delete Server"), wxDefaultPosition, wxDefaultSize, 0);
	wxButton *connect = new wxButton(this, BUTTON_Connect, _T("Connect"), wxDefaultPosition, wxDefaultSize, 0);

	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	vbox->Add(addServer, 1);
	vbox->Add(deleteServer, 1);
	vbox->Add(connect, 1);
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
	hbox->Add(servers, 10, wxEXPAND);
	hbox->Add(vbox, 1, wxEXPAND);
	SetSizer(hbox);
}

void MyFrame::OnExit(wxCommandEvent& event) {
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event) {
	wxMessageBox("This is a wxWidgets' Hello world sample",
			"About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnAddServer(wxCommandEvent& event) {
	wxTextEntryDialog dialog(this, wxT("Type type"), wxT("Random text"), wxT(""), wxOK | wxCANCEL);

	if (dialog.ShowModal() == wxID_OK)
	{
		servers->Append(dialog.GetValue().ToStdString());
	}
}

void MyFrame::OnDeleteServer(wxCommandEvent& event) {
	if (servers->GetSelection() == wxNOT_FOUND)
		wxMessageBox("No server selected", "Doh",wxOK | wxICON_INFORMATION);
	else
		servers->Delete(servers->GetSelection());
}

void MyFrame::OnConnect(wxCommandEvent& event) {
	wxString selection = wxT("Nothing selected");
	if (servers->GetSelection() != wxNOT_FOUND)
		selection = servers->GetString(servers->GetSelection());
	wxMessageBox(selection, "Lets pretend I'm connecting", wxOK | wxICON_INFORMATION);
}