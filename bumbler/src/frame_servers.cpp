#include "frame_servers.h"

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

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size) {
	server_list_ = std::unique_ptr<server_list>(new server_list());

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

	servers_ = std::unique_ptr<wxListBox>(new wxListBox(this, LISTBOX_Servers, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL | wxLB_SINGLE | wxLB_NEEDED_SB));
	wxButton *addServer = new wxButton(this, BUTTON_AddServer, _T("Add Server"), wxDefaultPosition, wxDefaultSize, 0);
	wxButton *deleteServer = new wxButton(this, BUTTON_DeleteServer, _T("Delete Server"), wxDefaultPosition, wxDefaultSize, 0);
	wxButton *connect = new wxButton(this, BUTTON_Connect, _T("Connect"), wxDefaultPosition, wxDefaultSize, 0);

	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	vbox->Add(addServer, 1);
	vbox->Add(deleteServer, 1);
	vbox->Add(connect, 1);
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
	hbox->Add(servers_.get(), 10, wxEXPAND);
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
		try
		{
			server_info info = server_list_->add_server(dialog.GetValue().ToStdString());
			servers_->Append(info.to_string());
		}
		catch (boost::system::system_error& ec)
		{
			wxMessageBox("Not a valid ip and port", "Error", wxOK | wxICON_INFORMATION);
		}
	}
}

void MyFrame::OnDeleteServer(wxCommandEvent& event) {
	if (servers_->GetSelection() == wxNOT_FOUND)
		wxMessageBox("No server selected", "Doh", wxOK | wxICON_INFORMATION);
	else
		servers_->Delete(servers_->GetSelection());
}

void MyFrame::OnConnect(wxCommandEvent& event) {
	wxString selection = wxT("Nothing selected");
	if (servers_->GetSelection() != wxNOT_FOUND)
		selection = servers_->GetString(servers_->GetSelection());
	wxMessageBox(selection, "Lets pretend I'm connecting", wxOK | wxICON_INFORMATION);
}

void MyFrame::UpdateServer(const std::string ip, std::string server_info)
{
	wxMessageBox("updating " + ip, server_info, wxOK | wxICON_INFORMATION);
}