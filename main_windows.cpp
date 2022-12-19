#include <windows.h>      // for CreateProcess
#include <iostream>
#include <string>
#include <tchar.h>        // for TCHAR Type
#include <ctime>          // for time

using namespace std;

bool trow_the_coin(){         // trow the coin function
    if (rand()%2+1 == 2)
        return true;          //will increment the counter
    return false;
}

int _tmain(int argc, TCHAR* argv[]){

    srand(time(NULL));      // restarts the seeds for rand

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    int i = 0;          // counter
    string data;          // data which be sent within the pipe

    //Throw the coin until counter reach 1000
    while(i < 1000){

        // Create a pipe to send/receive data
        HANDLE pipe = CreateNamedPipe(
            "\\\\.\\pipe\\pipe", // pipe name
            PIPE_ACCESS_OUTBOUND | PIPE_ACCESS_INBOUND, // read/write
            PIPE_TYPE_BYTE, // send data as a byte stream
            PIPE_UNLIMITED_INSTANCES, // unlimited instances for this pipe
            0, // no outbound buffer
            0, // no inbound buffer
            0, // use default wait time
            NULL // use default security attributes
        );

        if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
            cout << "Failed to create pipe instance.";
            system("pause");
            return 0;
        }
        // This call blocks until a client process connects to the pipe
        BOOL result = ConnectNamedPipe(pipe, NULL);
        if (!result) {
            cout << "Failed to make connection on named pipe." << endl;
            CloseHandle(pipe); // close the pipe
            system("pause");
            return 0;
        }
        if(trow_the_coin()){
            i++;
            data = to_string(i);
            cout << "Parent process. Counter = " << i << endl;
            // Send data within pipe. This call blocks until a client process reads all the data
            DWORD numBytesWritten = 0;
            result = WriteFile(
                pipe, // handle to our outbound pipe
                &data, // data to send
                1*sizeof(data), // length of data to send (bytes)
                &numBytesWritten, // will store actual amount of data sent
                NULL // not using overlapped IO
            );
        }


        // Start the second process
        if (CreateProcess(NULL, TEXT("cmd"), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            // Open the named pipe
            HANDLE pipe = CreateFile("\\\\.\\pipe\\mpipe",
                GENERIC_READ | GENERIC_READ, // read/write acces to pipe
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
            if (pipe == INVALID_HANDLE_VALUE) {
                cout << "Failed to connect to pipe." << endl;
                system("pause");
                return 0;
            }
            if(trow_the_coin()){
                // Read the pipe. This operation will block until there is data to read
                    DWORD numBytesRead = 0;
                    BOOL result = ReadFile(pipe,
                    &data,               // buffer for data received from pipe
                    1 * sizeof(data), // number of bytes allocated
                    &numBytesRead, // this will store number of bytes actually read
                    NULL // not using overlapped IO
                );
                if (!result) {
                    cout << "Failed to read data from the pipe." << endl;
                    system("pause");
                    return 0;
                }

                i = stoi(data);
                i++;
                cout << "Child process. Counter = " << i << endl;

                // Send data within pipe. This call blocks until a client process reads all the data
                DWORD numBytesWritten = 0;
                result = WriteFile(
                    pipe, // handle to our outbound pipe
                    &data, // data to send
                    1*sizeof(data), // length of data to send (bytes)
                    &numBytesWritten, // will store actual amount of data sent
                    NULL // not using overlapped IO
                );
                CloseHandle(pipe); // close the pipe by the child process
            }
        }
        else {
            cout << "Eroare creare proces!" << endl;
            return 0;
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;

}

