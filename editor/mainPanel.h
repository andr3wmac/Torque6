#ifndef MAINPANEL_H
#define MAINPANEL_H
 
class MainPanel : public wxPanel
{

public:
    MainPanel(wxWindow* parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0);

    ~MainPanel();

    virtual void OnIdle(wxIdleEvent& evt);
    virtual void OnSize(wxSizeEvent& evt);
};
 
#endif // MAINPANEL_H