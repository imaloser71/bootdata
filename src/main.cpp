#include <Windows.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <vector>
#include <map>

#pragma warning(disable: 4996)

HANDLE hConsole;
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
    for (int i = 0; i < 512; i++){
        char value = (buffer)[i];
        if(i%0x20==0){
            _tprintf(L"\n");
            SetConsoleTextAttribute(hConsole, 14);
            std::string hex = intToHex(i);
            std::transform(hex.begin(), hex.end(), hex.begin(), ::toupper);
            std::cout << " 0x" << std::string(3-hex.length(),'0') << hex << ": ";
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
int main() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, 6);
    _tprintf(TEXT("Bootloader data in \"\\\\.\\PhysicalDrive0\"\n\n"));
    char DataBuffer[512];
    DWORD dwBytesToRead = 512;
    DWORD dwBytesRead = 0;
    BOOL bErrorFlag = FALSE;
    HANDLE h = CreateFile(L"\\\\.\\PhysicalDrive0", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    bErrorFlag = ReadFile(h, DataBuffer, 512, &dwBytesRead, NULL);
    if (bErrorFlag) {
        SetConsoleTextAttribute(hConsole, 15);
        _tprintf(TEXT("ANSI:\n"));
        visualizeBuffer(((DataBuffer)));
        _tprintf(TEXT("Success! %d bytes read\n"), dwBytesRead);
    }
    else {
        SetConsoleTextAttribute(hConsole, 12);
        _tprintf(TEXT("ReadFile returned 0.\nGetLastError returned %d.\n"),GetLastError());
    }
    CloseHandle(h);
    return 0;
}
