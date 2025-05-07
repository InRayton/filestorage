#pragma once
#include <string>
#include <vector>
using namespace std;

class FileServer {
public:
    FileServer(int port);//инициализирует сервер с указанным портом
    void Run();//запускает цикл прослушивания и обработки клиентов
private:
    int listen_port_;//порт для прослушивания входящих соединений
    void HandleClient(int client);//обрабатывает один клиентский запрос
    vector<string> ListFiles();//возвращает список файлов в текущей директории
    void SendFile(int client,const string& filename);//отправляет запрошенный файл клиенту
    void ReceiveFile(int client,const string& filename,size_t size);//принимает и сохраняет файл от клиента
};
