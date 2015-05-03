#pragma once

#include "server_list.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void UpdateServer(const std::string ip, std::string server_info);

private:
    std::unique_ptr<wxListBox> servers_;
    std::unique_ptr<server_list> server_list_;

    void OnExit(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);

    void OnAddServer(wxCommandEvent& event);

    void OnDeleteServer(wxCommandEvent& event);

    void OnConnect(wxCommandEvent& event); //Used for button and double click on server hopefully...
wxDECLARE_EVENT_TABLE();

};
