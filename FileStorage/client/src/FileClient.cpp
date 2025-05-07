#include "FileClient.h"
#include <wx/socket.h>
#include <wx/wfstream.h>
#include <wx/sckstrm.h>
#include <wx/txtstrm.h>
#include <wx/msgdlg.h>
#include <filesystem>

using namespace std;

static constexpr int CONNECT_TIMEOUT_MS = 5000;//время ожидания соединения в миллисекундах

wxSocketClient* ConnectToServer(const wxIPV4address& addr) {
    auto sock = new wxSocketClient();//создаём клиентский сокет
    sock->SetFlags(wxSOCKET_BLOCK);//режим блокирующего сокета
    sock->Connect(addr, false);//инициируем подключение
    if (!sock->WaitOnConnect(CONNECT_TIMEOUT_MS)) return nullptr;//если не удалось — возвращаем nullptr
    return sock;//возвращаем готовый сокет
}

FileClient::FileClient(const wxString& host, int port) {
    addr_.Hostname(host);//устанавливаем адрес сервера
    addr_.Service(port);//устанавливаем порт сервера
}

vector<string> FileClient::ListFiles() {
    auto sock = ConnectToServer(addr_);//подключаемся к серверу
    if (!sock) {wxMessageBox("Cannot connect to server for LIST","Error",wxICON_ERROR);return {};}
    sock->Write("LIST\n", 5);//отправляем команду запроса списка
    sock->Shutdown();//завершаем запись в сокет
    wxSocketInputStream sis(*sock);
    wxTextInputStream tis(sis);
    vector<string> files;
    while (!sis.Eof()) {
        wxString line = tis.ReadLine();//читаем по строкам
        if (line.IsEmpty()) continue;
        files.push_back(string(line.mb_str()));//сохраняем имя файла
    }
    sock->Close();delete sock;//закрываем и удаляем сокет
    return files;//возвращаем список
}

bool FileClient::Download(const string& filename) {
    auto sock = ConnectToServer(addr_);//подключаемся к серверу
    if (!sock) {wxMessageBox("Cannot connect to server for DOWNLOAD","Error",wxICON_ERROR);return false;}
    string cmd = "GET " + filename + "\n";//формируем команду загрузки
    sock->Write(cmd.c_str(), cmd.size());
    sock->Shutdown();//закрываем запись после отправки команды
    wxSocketInputStream sis(*sock);
    wxTextInputStream tis(sis);
    wxString sizeLine = tis.ReadLine();//читаем размер файла
    size_t sz = stoul(string(sizeLine.mb_str()));
    wxFileOutputStream fout(filename);//открываем поток для записи на диск
    char buf[4096];
    size_t received = 0;
    while (received < sz && !sis.Eof()) {
        size_t toread = min(sz - received, sizeof(buf));
        sis.Read(buf, toread);
        size_t count = sis.LastRead();
        if (count == 0) break;
        fout.Write(buf, count);//записываем принятые байты
        received += count;
    }
    sock->Close();delete sock;//закрываем сокет
    return received == sz;//вернём true, если всё скачалось
}

bool FileClient::Upload(const string& filename) {
    wxFileInputStream fin(filename);//открываем файл для чтения
    if (!fin.IsOk()) {wxMessageBox("Cannot open file for upload","Error",wxICON_ERROR);return false;}
    size_t sz = fin.GetSize();//узнаём размер файла
    auto sock = ConnectToServer(addr_);//подключаемся к серверу
    if (!sock) {wxMessageBox("Cannot connect to server for UPLOAD","Error",wxICON_ERROR);return false;}
    namespace fs = std::filesystem;
    string nameOnly = fs::path(filename).filename().string();//берём только имя без пути
    string header = "PUT " + nameOnly + "\n" + to_string(sz) + "\n";//формируем заголовок команды
    sock->Write(header.c_str(), header.size());
    sock->Shutdown();//закрываем запись после заголовка
    char buf[4096];
    while (!fin.Eof()) {
        fin.Read(buf, sizeof(buf));
        size_t count = fin.LastRead();
        if (count == 0) break;
        sock->Write(buf, count);//отправляем содержимое файла
    }
    sock->Close();delete sock;//закрываем сокет
    return true;//успешно
}
