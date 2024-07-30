// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "File.h"
#define _AFXDLL

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const string ServerSaveFile = "text.txt";
// The one and only application object

CWinApp theApp;

int main(int argc, TCHAR* argv[], TCHAR* envp[]) {
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
        _tprintf(_T("Fatal Error: MFC initialization failed\n"));
        return 1;
    }

    // lay thong tin cac file co san tren server
    vector<fileZip> file;
    readInputFile(ServerSaveFile, file);

    int num_file = file.size();
    if (!AfxSocketInit(NULL)) {
        cerr << "Socket initialization failed" << endl;
        return 1;
    }

    CSocket server, Client;
    server.Create(45673, SOCK_STREAM, NULL);

    while (true) {
        cout << "Server listening for clients..." << endl;
        server.Listen(10);
        server.Accept(Client);
        cout << "New client connected!" << endl;

        Client.Send(&num_file, sizeof(num_file), 0);
        cout << "Sending file list..." << endl;

        for (const auto& f : file) {
            bool check = false;
            if (Client.Send(&f, sizeof(fileZip), 0) < 0) {
                cerr << "Error sending file info to client" << endl;
                break;
            }
            Client.Receive(&check, sizeof(check), 0);
            if (!check) {
                cerr << "Client did not confirm file info" << endl;
                break;
            }
        }

        string dFile;
        while (true) {
            if (Client.Receive(&dFile, sizeof(dFile), 0) < 0) {
                cerr << "Error receiving file request from client" << endl;
                break;
            }

            cout << "Sending file: " << dFile << endl;
            if (!sendFile(Client, dFile)) {
                cerr << "Error sending file" << endl;
                break;
            }
        }

        Client.Close();
        cout << "Client disconnected." << endl;
        cout << "Server are looking for other clients" << endl;

    }

    server.Close();
    return 0;
}
