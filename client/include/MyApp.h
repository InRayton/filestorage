#pragma once
#include <wx/wx.h>
using namespace std;

class MyApp : public wxApp {
public:
    virtual bool OnInit() override;//переопределяем для создания главного окна
};
