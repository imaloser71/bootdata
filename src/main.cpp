#include <iostream>
#include <fstream>
#include <Windows.h>
#include <sstream>
#include <string.h>
#include <string>
#include <algorithm>
#include <vector>
#include <map>

#include <tchar.h>
#include <conio.h>

#pragma warning(disable: 4996)

HANDLE hConsole;
bool samestring(char* a, const char* b) {
    if (strlen(a) != strlen(b)) return false;
    for (int i = 0; i < strlen(a); i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}
std::string intToHex(unsigned int number) {
    std::stringstream stream;
    stream << std::hex << number;
    std::string result(stream.str());
    return result;
}
void visualizeBuffer(char* buffer) {
    //USES_CONVERSION;
    SetConsoleOutputCP(437);
    SetConsoleCP(437);
    SetConsoleTextAttribute(hConsole, 8);
    _tprintf(TEXT("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));
    for (int i = 0; i < 512; i++) {
        char value = (buffer)[i];
        if (i % 0x20 == 0) {
            _tprintf(L"\n");
            SetConsoleTextAttribute(hConsole, 14);
            std::string hex = intToHex(i);
            std::transform(hex.begin(), hex.end(), hex.begin(), ::toupper);
            std::cout << " 0x" << std::string(3 - hex.length(), '0') << hex << ": ";
        }
        if ((unsigned char)value < 0x20) {
            SetConsoleTextAttribute(hConsole, 12);
            _tprintf(TEXT("."), (buffer)[i]);
        }/*else if ((unsigned char)value > 0x7f) {
            SetConsoleTextAttribute(hConsole, 15);
            std::string stringv = cmap[(unsigned char)value-0x80];
            std::cout << stringv;
            /*_tprintf(
                A2W(stringv.c_str()), (buffer)[i]);
        }*/
        else {
            SetConsoleTextAttribute(hConsole, 15);
            _tprintf(TEXT("%c"), (buffer)[i]);
        }
    }
    SetConsoleTextAttribute(hConsole, 8);
    _tprintf(TEXT("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));
    SetConsoleTextAttribute(hConsole, 15);
    _tprintf(TEXT("\n\n"));
}
int main(int argc, char** argv) {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    int paramFlag = 0;

    if (argc > 1) {
        if (argc == 1) {
            SetConsoleTextAttribute(hConsole, 12);
            _tprintf(TEXT("Invalid Argument Count.\n"));
        }
        else if (samestring(argv[1], "export")) {
            paramFlag = 1;
        }
        else if (samestring(argv[1], "import")) {
            paramFlag = 2;
        }
        else {
            SetConsoleTextAttribute(hConsole, 12);
            _tprintf(TEXT("Invalid Arguments, Check documentation.\n"));
        }
    }

    SetConsoleTextAttribute(hConsole, 6);
    _tprintf(TEXT("Bootloader data in \"\\\\.\\PhysicalDrive0\"\n\n"));
    char DataBuffer[512];
    DWORD dwBytesToRead = 512;
    DWORD dwBytesRead = 0;
    BOOL bErrorFlag = FALSE;
    HANDLE h;


    if (paramFlag == 2) {
        SetConsoleTextAttribute(hConsole, 4);
        std::string answer;
        std::cout << "\nAre you SURE your filepath is correct? IT HAS TO BE AN ABSOLUTE PATH, NOT A RELATIVE ONE, OR ELSE IT WILL BREAK YOUR BOOTLOADER! (y/n)";
        std::cin >> answer;
        if (answer != "y") {
            SetConsoleTextAttribute(hConsole, 15);
            return -1;
        }
        answer = "";
        SetConsoleTextAttribute(hConsole, 12);
        std::cout << "\nAre you ABSOLUTELY SURE you want to PERMANENTLY OVERWRITE your bootloader? Make SURE you have BACKUPS! (y/n)";
        std::cin >> answer;
        if (answer != "y") {
            SetConsoleTextAttribute(hConsole, 15);
            return -1;
        }
        h = CreateFile(L"\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        std::string istr = std::string(512, '-');
        std::ifstream ifs(argv[2], std::ios_base::binary);
        ifs >> istr;
        for (int i = 0; i < 512; i++) {
            DataBuffer[i] = istr[i];
        }
        bErrorFlag = WriteFile(h, DataBuffer, 512, &dwBytesRead, NULL);
        if (bErrorFlag) {
            SetConsoleTextAttribute(hConsole, 15);
            _tprintf(TEXT("\nSuccess! %d bytes written\n"), dwBytesRead);
        }
        else {
            SetConsoleTextAttribute(hConsole, 12);
            _tprintf(TEXT("ReadFile returned 0.\nGetLastError returned %d.\n"), GetLastError());
        }
    }
    else {
        h = CreateFile(L"\\\\.\\PhysicalDrive0", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    }
    bErrorFlag = ReadFile(h, DataBuffer, 512, &dwBytesRead, NULL);
    if (bErrorFlag) {
        if (paramFlag == 1) {
            std::string ostr = std::string(512, '-');
            for (int i = 0; i < 512; i++) {
                ostr[i] = DataBuffer[i];
            }
            std::ofstream ofs(argv[2], std::ios_base::binary);
            if (!ofs) {
                SetConsoleTextAttribute(hConsole, 12);
                _tprintf(TEXT("Cannot open '%s', %s"), argv[2], strerror(errno));
            }
            else {
                ofs << ostr;
                ofs.close();
                SetConsoleTextAttribute(hConsole, 15);
                std::cout << "Successfully exported bootloader to '" << argv[2] << "'";
            }
            CloseHandle(h);
            SetConsoleTextAttribute(hConsole, 15);
            return 1;
        }
        SetConsoleTextAttribute(hConsole, 15);
        _tprintf(TEXT("ANSI:\n"));
        visualizeBuffer(((DataBuffer)));
        if (paramFlag == 2) {
            _tprintf(TEXT("If the result above is not equivelant to your boot file, it is possible it was not imported correctly, in such case please prepare for unexpected results.\n"), dwBytesRead);
        }
        else {
            _tprintf(TEXT("Success! %d bytes read\n"), dwBytesRead);
        }
    }
    else {
        SetConsoleTextAttribute(hConsole, 12);
        _tprintf(TEXT("ReadFile returned 0.\nGetLastError returned %d.\n"), GetLastError());
    }
    CloseHandle(h);
    SetConsoleTextAttribute(hConsole, 15);
    return -1;
}
