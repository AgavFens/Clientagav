#define WIN32_LEAN_AND_MEAN // Исключает редко используемые компоненты из заголовков Windows

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
    WSADATA wsaData;  // Структура для хранения данных о реализации Windows Sockets
    ADDRINFO hints;   // Подсказки для типа адреса сокета, который мы ищем
    ADDRINFO* addrResult;  // Указатель для хранения результата функции getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET;  // Сокет для подключения к серверу
    char recvBuffer[512];  // Буфер для получения данных

    const char* sendBuffer = "Hello from client";  // Сообщение для отправки серверу

    // Инициализация библиотеки Windows Sockets
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup не удалось, результат: " << result << endl;
        return 1;
    }

    // Обнуление структуры hints
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // Семейство адресов IPv4
    hints.ai_socktype = SOCK_STREAM; // Тип сокета - потоковый (TCP)
    hints.ai_protocol = IPPROTO_TCP; // Протокол - TCP

    // Получение информации о сетевом адресе и порте
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo не удалось, ошибка: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Создание сокета для подключения
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Ошибка создания сокета" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Не удалось подключиться к серверу" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Отправка данных серверу
    result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
    if (result == SOCKET_ERROR) {
        cout << "Ошибка отправки данных: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Отправлено: " << result << " байт" << endl;

    // Завершение отправки данных
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Ошибка завершения отправки: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Цикл для приема данных от сервера
    do {
        ZeroMemory(recvBuffer, 512);  // Обнуление буфера для приема данных
        result = recv(ConnectSocket, recvBuffer, 512, 0);  // Получение данных от сервера

        if (result > 0) {
            cout << "\nПолучено: " << result << " байт" << endl;
            cout << "\nПолученные данные: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "\nСоединение закрыто" << endl;
        }
        else {
            cout << "Ошибка получения данных" << endl;
        }
    } while (result > 0);

    freeaddrinfo(addrResult);  // Освобождение памяти, выделенной для addrResult
    WSACleanup();  // Освобождение ресурсов, используемых Winsock
    return 0;
}
