#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "FileClient.h"
using namespace std;

class MainFrame : public wxFrame {
public:
    MainFrame();//конструктор создаёт окно и настраивает интерфейс
private:
    wxListCtrl* list_;//список файлов на сервере
    FileClient client_;//экземпляр клиента для работы с сервером
    void OnList(wxCommandEvent&);//обработчик нажатия обновления
    void OnDownload(wxCommandEvent&);//обработчик нажатия скачивания
    void OnUpload(wxCommandEvent&);//обработчик нажатия загрузки
    wxDECLARE_EVENT_TABLE();//объявление таблицы событий
};
