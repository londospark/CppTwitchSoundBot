#pragma once

#include <wx/frame.h>
#include <wx/textctrl.h>

namespace gh
{
	class MainFrame : public wxFrame
	{
	public:
		MainFrame();
		void log(std::string const& message);

	private:
		wxTextCtrl* logtext;
		std::ostream stream;
	};
}