#include "FileServer.h"
#include <cstdlib>

using namespace std;

int main(int argc,char* argv[]) {
    int port=12345;//порт по умолчанию
    FileServer server(port);//создаём экземпляр сервера
    server.Run();//запускаем серверный цикл
    return 0;//выход из программы после завершения работы сервера
}
