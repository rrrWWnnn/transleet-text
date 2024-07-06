#include <windows.h>
#include <string>
#include <unordered_map>
#include <CommCtrl.h>

#pragma comment(lib, "Comctl32.lib")

HINSTANCE hInst;
HWND hInputBox, hOutputBox;

std::unordered_map<wchar_t, std::wstring> translit_map = {
    {L'А', L"A"}, {L'а', L"а"}, {L'б', L"6"}, {L'Б', L"6"}, {L'в', L"B"}, {L'В', L"B"},
    {L'г', L"r"}, {L'Г', L"r"}, {L'д', L"g"}, {L'Д', L"g"}, {L'е', L"е"}, {L'Е', L"E"},
    {L'ё', L"e"}, {L'Ё', L"E"}, {L'ж', L">|<"}, {L'Ж', L">|<"}, {L'з', L"3"}, {L'З', L"3"},
    {L'и', L"u"}, {L'И', L"u"}, {L'й', L"u"}, {L'Й', L"u"}, {L'к', L"k"}, {L'К', L"K"},
    {L'л', L"JI"}, {L'Л', L"JI"}, {L'М', L"M"}, {L'м', L"M"}, {L'н', L"H"}, {L'Н', L"H"},
    {L'О', L"0"}, {L'о', L"o"}, {L'п', L"II"}, {L'П', L"II"}, {L'р', L"p"}, {L'Р', L"P"},
    {L'с', L"c"}, {L'С', L"C"}, {L'т', L"T"}, {L'Т', L"T"}, {L'У', L"Y"}, {L'у', L"y"},
    {L'ф', L"qp"}, {L'Ф', L"qp"}, {L'х', L"X"}, {L'Х', L"x"}, {L'ц', L"LL"}, {L'Ц', L"LL"},
    {L'ч', L"4"}, {L'Ч', L"4"}, {L'Ш', L"W"}, {L'ш', L"w"}, {L'щ', L"LLI"}, {L'Щ', L"LLI"},
    {L'ъ', L"'b"}, {L'Ъ', L"'b"}, {L'ы', L"bl"}, {L'Ы', L"bl"}, {L'ь', L"b"}, {L'Ь', L"b"},
    {L'э', L"€"}, {L'Э', L"€"}, {L'ю', L"I-0"}, {L'Ю', L"I-0"}, {L'я', L"9"}, {L'Я', L"9"}
};

std::wstring Transliterate(const std::wstring& input) {
    std::wstring output;
    for (const auto& ch : input) {
        auto it = translit_map.find(ch);
        if (it != translit_map.end()) {
            output += it->second;
        }
        else {
            output += ch;
        }
    }
    return output;
}

void OnTextChanged() {
    int length = GetWindowTextLength(hInputBox);
    std::wstring input(length, L'\0');
    GetWindowText(hInputBox, &input[0], length + 1);

    std::wstring output = Transliterate(input);
    SetWindowText(hOutputBox, output.c_str());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        hInputBox = CreateWindowEx(0, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
            10, 10, 520, 180, hWnd, NULL, hInst, NULL);
        hOutputBox = CreateWindowEx(0, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
            10, 200, 520, 180, hWnd, NULL, hInst, NULL);
        SetWindowSubclass(hInputBox, [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) -> LRESULT {
            if (uMsg == WM_CHAR || uMsg == WM_KEYUP) {
                OnTextChanged();
            }
            if (uMsg == WM_KEYDOWN && (wParam == 'A' || wParam == 'a') && GetKeyState(VK_CONTROL) < 0) {
                SendMessage(hWnd, EM_SETSEL, 0, -1);
                return 0;
            }
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }, 1, 0);
        SetWindowSubclass(hOutputBox, [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) -> LRESULT {
            if (uMsg == WM_KEYDOWN && (wParam == 'A' || wParam == 'a') && GetKeyState(VK_CONTROL) < 0) {
                SendMessage(hWnd, EM_SETSEL, 0, -1);
                return 0;
            }
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }, 1, 0);
        break;
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT: {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0, 0, 0));
        SetBkColor(hdcStatic, RGB(240, 248, 255));
        return (INT_PTR)CreateSolidBrush(RGB(240, 248, 255));
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icex);

    WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW),
                        (HBRUSH)(COLOR_WINDOW + 1), NULL, L"TranslitClass", NULL };

    RegisterClassEx(&wcex);

    HWND hWnd = CreateWindow(L"TranslitClass", L"TP4HCJIuT Hack v34", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 560, 420, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
