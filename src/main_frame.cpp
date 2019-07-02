#include "main_frame.h"

#include <wx/sizer.h>

namespace gh
{
	MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "HubballBot"),
		logtext(new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(100, 60), wxTE_MULTILINE)),
		stream(logtext)
	{
		wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
		// create text ctrl with minimal size 100x60
		topsizer->Add(
			logtext,
			1,            // make vertically stretchable
			wxEXPAND |    // make horizontally stretchable
			wxALL,        //   and make border all around
			10);         // set border width to 10
	}

	void MainFrame::log(std::string const& message) {
		stream << message << '\n';
	}
}