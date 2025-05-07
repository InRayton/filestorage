#include "FileServer.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef byte
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <stdexcept>
using namespace std;
namespace fs = std::filesystem;
static string recvLine(SOCKET s) {//считывает строку до '\n' из сокета
    string line;
    char c;
    while (true) {
        int r = recv(s, &c, 1, 0);
        if (r <= 0 || c == '\n') break;
        line += c;
    }
    return line;
}

FileServer::FileServer(int port)
  : listen_port_(port) {}

void FileServer::Run() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(listen_port_);
    bind(listen_sock, (sockaddr*)&addr, sizeof(addr));
    listen(listen_sock, 5);
    cout<<"Server listening on port "<<listen_port_<<endl;

    while (true) {
        SOCKET client = accept(listen_sock, nullptr, nullptr);
        if (client == INVALID_SOCKET) continue;
        HandleClient(client);//обрабатываем подключение
        closesocket(client);//закрываем клиентский сокет
    }
}

void FileServer::HandleClient(int s) {//парсит команду и вызывает соответствующее действие
    string line = recvLine(s);//читаем команду
    if (line=="LIST") {
        auto files = ListFiles();//получаем список файлов
        string out;
        for (auto& f:files) out+=f+"\n";
        send(s, out.c_str(), out.size(), 0);//отправляем список
        shutdown(s, SD_SEND);
    }
    else if (line.rfind("GET ",0)==0) {
        string filename = line.substr(4);
        SendFile(s, filename);//отправляем файл
        shutdown(s, SD_SEND);
    }
    else if (line.rfind("PUT ",0)==0) {
        string filename = line.substr(4);
        string sizeStr = recvLine(s);//читаем размер
        size_t sz=0;
        try { sz=stoul(sizeStr); }
        catch(...) { cerr<<"Invalid size in PUT: '"<<sizeStr<<"'"<<endl; return; }
        ReceiveFile(s, filename, sz);//принимаем файл
        shutdown(s, SD_SEND);
    }
}
vector<string> FileServer::ListFiles() {//возвращает имена всех файлов в текущей папке
    vector<string> result;
    for (auto& p:fs::directory_iterator(".")) {
        if (p.is_regular_file()) result.push_back(p.path().filename().string());
    }
    return result;
}
void FileServer::SendFile(int s, const string& filename) {//читает файл и шлёт клиенту
    ifstream in(filename, ios::binary);
    if (!in) return;
    in.seekg(0, ios::end);
    size_t sz = in.tellg();
    in.seekg(0);
    string header = to_string(sz)+"\n";
    send(s, header.c_str(), header.size(), 0);//отправляем размер
    char buf[4096];
    while (!in.eof()) {
        in.read(buf, sizeof(buf));
        send(s, buf, in.gcount(), 0);//отправляем данные
    }
}

void FileServer::ReceiveFile(int s, const string& filename, size_t size) {//принимает и сохраняет файл
    ofstream out(filename, ios::binary);
    size_t received = 0;
    char buf[4096];
    while (received < size) {
        int toRead = static_cast<int>(min<size_t>(sizeof(buf), size-received));
        int n = recv(s, buf, toRead, 0);
        if (n <= 0) break;
        out.write(buf, n);//записываем данные
        received += n;
    }
}
