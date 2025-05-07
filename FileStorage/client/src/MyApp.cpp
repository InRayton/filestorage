#include "MyApp.h"
#include "MainFrame.h"

using namespace std;

wxIMPLEMENT_APP(MyApp);//макрос запуска приложения

bool MyApp::OnInit() {
    MainFrame* frame=new MainFrame();//создаём главное окно
    frame->Show(true);//отображаем окно пользователю
    return true;//инициализация прошла успешно
}
