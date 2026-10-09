// CommandLine.cpp
// Demonstrates persistent language switching via resource DLLs.
// Defaults to English on startup; language choice is saved to the registry.

#include <windows.h>
#include <tchar.h>
#include <cstdio>
#include <io.h>
#include <fcntl.h>

// Resource IDs - must match all language DLLs
#define IDS_HELLO           101
#define IDS_GREETING        102
#define IDS_LANGUAGE        103
#define IDS_GOODBYE         104
#define IDS_INSTRUCTION     105
#define IDS_BTN_EXIT        108

// Shared with SDIApp and DialogApp — same key, same value name.
static const wchar_t* k_RegKey = L"Software\\LocalisationDemo";

// ---------------------------------------------------------------------------
// Language state - DLL kept loaded between interactions
// ---------------------------------------------------------------------------
static HMODULE  g_hLangDLL = NULL;
static bool     g_bFrench  = false;

static void Separator(wchar_t ch = L'-', int w = 52)
{
    for (int i = 0; i < w; i++) putwchar(ch);
    putwchar(L'\n');
}

static void GetStr(UINT id, wchar_t* buf, int cch)
{
    buf[0] = L'\0';
    if (g_hLangDLL)
        ::LoadStringW(g_hLangDLL, id, buf, cch);
}

// ---------------------------------------------------------------------------
// Registry persistence
// ---------------------------------------------------------------------------

static int LoadLanguagePref()
{
    HKEY hKey = NULL;
    int lang = 0;
    if (::RegOpenKeyExW(HKEY_CURRENT_USER, k_RegKey,
            0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        wchar_t buf[64] = {};
        DWORD size = sizeof(buf);
        DWORD type = REG_SZ;
        if (::RegQueryValueExW(hKey, L"Language", NULL, &type,
                reinterpret_cast<BYTE*>(buf), &size) == ERROR_SUCCESS)
        {
            if (wcscmp(buf, L"French") == 0) lang = 1;
            else if (wcscmp(buf, L"German") == 0) lang = 2;
        }
        ::RegCloseKey(hKey);
    }
    return lang;
}

// ---------------------------------------------------------------------------
// Language loading
// ---------------------------------------------------------------------------
// 0=English  1=French  2=German
static int g_lang = 0;

static bool SetLanguage(int lang)
{
    const wchar_t* dll = (lang == 1) ? L"FrenchDLL.dll"
                       : (lang == 2) ? L"GermanDLL.dll"
                       :               L"EnglishDLL.dll";
    HMODULE h = ::LoadLibraryW(dll);
    if (!h)
    {
        wprintf(L"\n  ERROR: Cannot load %s  (Win32 error %lu)\n"
                L"  Make sure all language DLLs are in the same folder as CommandLine.exe\n",
                dll, ::GetLastError());
        return false;
    }
    if (g_hLangDLL) ::FreeLibrary(g_hLangDLL);
    g_hLangDLL = h;
    g_lang     = lang;
    g_bFrench  = (lang == 1);

    const wchar_t* pref = (lang == 1) ? L"French"
                        : (lang == 2) ? L"German"
                        :               L"English";
    HKEY hKey = NULL;
    if (::RegCreateKeyExW(HKEY_CURRENT_USER, k_RegKey,
            0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        ::RegSetValueExW(hKey, L"Language", 0, REG_SZ,
            reinterpret_cast<const BYTE*>(pref),
            static_cast<DWORD>((wcslen(pref) + 1) * sizeof(wchar_t)));
        ::RegCloseKey(hKey);
    }
    return true;
}

// ---------------------------------------------------------------------------
// Display helpers
// ---------------------------------------------------------------------------
static void DisplayStrings()
{
    wchar_t hello[256], greeting[256], language[256], goodbye[256], instruction[256];
    GetStr(IDS_HELLO,       hello,       256);
    GetStr(IDS_GREETING,    greeting,    256);
    GetStr(IDS_LANGUAGE,    language,    256);
    GetStr(IDS_GOODBYE,     goodbye,     256);
    GetStr(IDS_INSTRUCTION, instruction, 256);

    wprintf(L"\n");
    Separator();
    wprintf(L"  Active DLL : %s\n",
            g_bFrench ? L"FrenchDLL.dll" : L"EnglishDLL.dll");
    Separator();
    wprintf(L"  %-14s %s\n", L"Hello:",       hello);
    wprintf(L"  %-14s %s\n", L"Greeting:",    greeting);
    wprintf(L"  %-14s %s\n", L"Language:",    language);
    wprintf(L"  %-14s %s\n", L"Goodbye:",     goodbye);
    wprintf(L"  %-14s %s\n", L"Instruction:", instruction);
    Separator();
}

static void ShowMenu()
{
    wchar_t exitLabel[128];
    GetStr(IDS_BTN_EXIT, exitLabel, 128);

    wprintf(L"\n");
    wprintf(L"  [1]  English%s\n", g_lang == 0 ? L"  <- current" : L"");
    wprintf(L"  [2]  French%s\n",  g_lang == 1 ? L"   <- current" : L"");
    wprintf(L"  [3]  German%s\n",  g_lang == 2 ? L"   <- current" : L"");
    wprintf(L"  [0]  %s\n", exitLabel);
    wprintf(L"\n  Choice: ");
}

// ---------------------------------------------------------------------------
int wmain(int, wchar_t*[])
{
    _setmode(_fileno(stdout), _O_U16TEXT);

    wprintf(L"\n");
    Separator(L'=');
    wprintf(L"  Localisation Demo - Command Line Application\n");
    wprintf(L"  Visual Studio 2022  /  Win32  /  x64\n");
    Separator(L'=');

    // Restore saved language preference (default: English)
    int savedLang = LoadLanguagePref();
    if (!SetLanguage(savedLang))
    {
        // Fallback: try English
        if (!SetLanguage(0))
        {
            wprintf(L"\n  No language DLLs found. Exiting.\n");
            return 1;
        }
    }

    for (;;)
    {
        DisplayStrings();
        ShowMenu();

        int choice = -1;
        if (wscanf_s(L"%d", &choice) != 1)
        {
            wint_t c;
            while ((c = getwchar()) != L'\n' && c != WEOF) {}
            wprintf(L"  Invalid input. Enter 1, 2, or 0.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            if (g_lang == 0) wprintf(L"  Already in English.\n");
            else SetLanguage(0);
            break;

        case 2:
            if (g_lang == 1) wprintf(L"  Already in French.\n");
            else SetLanguage(1);
            break;

        case 3:
            if (g_lang == 2) wprintf(L"  Already in German.\n");
            else SetLanguage(2);
            break;

        case 0:
        {
            DisplayStrings();
            wchar_t bye[256];
            GetStr(IDS_GOODBYE, bye, 256);
            wprintf(L"\n  %s\n\n", bye);
            if (g_hLangDLL) { ::FreeLibrary(g_hLangDLL); g_hLangDLL = NULL; }
            return 0;
        }

        default:
            wprintf(L"  Invalid choice. Enter 1, 2, or 0.\n");
            break;
        }
    }
}
