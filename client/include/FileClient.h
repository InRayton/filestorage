#pragma once
#include <wx/socket.h>
#include <wx/wx.h>
#include <wx/txtstrm.h>
#include <vector>
#include <string>
using namespace std;

class FileClient {
public:
    FileClient(const wxString& host, int port);//конструктор задаёт хост и порт сервера
    vector<string> ListFiles();//возвращает список файлов на сервере
    bool Download(const string& filename);//скачивает файл с сервера
    bool Upload(const string& filename);//загружает файл на сервер
private:
    wxIPV4address addr_;//хранит адрес и порт сервера
};
