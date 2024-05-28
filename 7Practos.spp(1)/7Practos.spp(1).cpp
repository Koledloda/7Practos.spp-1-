#define win32_lean_and_mean

#include <windows.h>
#include <iostream>
#include <winsock2.h>  // Библиотеки для работы с сокетами в Windows
#include <ws2tcpip.h>  // Библиотека для работы с TCP/IP

using namespace std;

int main() 
{

    WSADATA wsadata;  // Объявление структуры WSADATA
    addrinfo hints;  // Объявление структуры addrinfo
    addrinfo* addrresult;  // Объявление указателя на структуру addrinfo
    SOCKET connectsocket = INVALID_SOCKET;  // Объявление переменной connectsocket и присвоение ей INVALID_SOCKET
    char recvbuffer[512];  // Объявление массива символов recvbuffer размером 512

    const char* sendbuffer1 = "hello from client 1";  // Объявление указателя sendbuffer1 на строку "hello from client 1"
    const char* sendbuffer2 = "hello from client 2";  // Объявление указателя sendbuffer2 на строку "hello from client 2"

    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);  // Инициализация библиотеки Winsock и сохранение результата в переменной result
    if (result != 0) {  // Проверка
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));  // Заполнение блока памяти нулями
    hints.ai_family = AF_INET;  // Задание семейства адресов
    hints.ai_socktype = SOCK_STREAM;  // Задание типа сокета
    hints.ai_protocol = IPPROTO_TCP;  // Задание протокола

    result = getaddrinfo("localhost", "666", &hints, &addrresult);  // Получение адресной информации о хосте
    if (result != 0) {  // Проверка
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrresult);  // Освобождение памяти
        WSACleanup();  // Завершение использования библиотеки
        return 1;
    }

    connectsocket = socket(addrresult->ai_family, addrresult->ai_socktype, addrresult->ai_protocol);  // Создание сокета
    if (connectsocket == INVALID_SOCKET) {  // Проверка создания сокета
        cout << "socket creation failed" << endl;
        freeaddrinfo(addrresult);  // Освобождение памяти
        WSACleanup();  // Завершение использования библиотеки
        return 1;
    }

    result = connect(connectsocket, addrresult->ai_addr, (int)addrresult->ai_addrlen);  // Соединение с сервером
    if (result == SOCKET_ERROR) {  // Проверка результата
        cout << "unable to connect to server" << endl;
        closesocket(connectsocket);  // Закрытие сокета
        connectsocket = INVALID_SOCKET;
        freeaddrinfo(addrresult);  // Освобождение памяти
        WSACleanup();  // Завершение использования библиотеки
        return 1;
    }

    result = send(connectsocket, sendbuffer1, (int)strlen(sendbuffer1), 0);  // Отправка данных
    if (result == SOCKET_ERROR) {  // проверка результата
        cout << "send failed, error: " << result << endl;
        closesocket(connectsocket);  // Закрытие сокета
        freeaddrinfo(addrresult);  // Освобождение памяти
        WSACleanup();  // Завершение использования библиотеки
        return 1;
    }
    cout << "sent: " << result << " bytes" << endl;  // Вывод количества отправленных байт

    result = send(connectsocket, sendbuffer2, (int)strlen(sendbuffer2), 0);  // Отправка данных
    if (result == SOCKET_ERROR) {  // проверка результата
        cout << "send failed, error: " << result << endl;
        closesocket(connectsocket);  // Закрытие сокета
        freeaddrinfo(addrresult);  // Освобождение памяти
        WSACleanup();  // Завершение использования библиотеки
        return 1;
    }
    cout << "sent: " << result << " bytes" << endl;  // Вывод количества отправленных байт

    result = shutdown(connectsocket, SD_SEND);  // Закрытие отправки данных
    if (result == SOCKET_ERROR) {  // проверка результата
        cout << "shutdown failed, error: " << result << endl;
        closesocket(connectsocket);  // Закрытие сокета
        freeaddrinfo(addrresult);  // Освобождение памяти
        WSACleanup();  // Завершение использования библиотеки
        return 1;
    }

    do {
        ZeroMemory(recvbuffer, 512);
        result = recv(connectsocket, recvbuffer, 512, 0);  // Прием данных
        if (result > 0) {  // Проверка результата
            cout << "received " << result << " bytes" << endl;
            cout << "received data: " << recvbuffer << endl;
        }
        else if (result == 0) {  // Вывод сообщения о закрытии соединения
            cout << "connection closed" << endl;
        }
        else {  // Вывод сообщения об ошибке приема данных
            cout << "recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    closesocket(connectsocket);  // Закрытие сокета
    freeaddrinfo(addrresult);  // Освобождение памяти
    WSACleanup();  // Завершение использования библиотеки
    return 0;
}