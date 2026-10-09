# Localisation Demo

A Visual Studio 2022 C++ solution demonstrating **runtime language switching** using resource-only satellite DLLs. Three separate host applications (Command-Line, MFC SDI, MFC Dialog) each load the same language DLLs and share a single language preference stored in the Windows registry.

---

## Solution Structure

```
LocalisationDemo.sln
│
├── EnglishDLL/          Resource-only DLL — English strings
├── FrenchDLL/           Resource-only DLL — French strings, menu & accelerators
├── GermanDLL/           Resource-only DLL — German strings, menu & accelerators
│
├── CommandLine/         Console app — loads language DLLs explicitly via LoadString
├── SDIApp/              MFC SDI app — switches via AfxSetResourceHandle
└── DialogApp/           MFC Dialog app — switches via AfxSetResourceHandle
```

### Shared Language Preference

All three host apps read and write the same registry value:

```
HKCU\Software\LocalisationDemo\Language   (REG_SZ: "English" | "French" | "German")
```

A language change in any app is honoured by the others on their next launch.

---

## Requirements

| Tool | Version |
|------|---------|
| Visual Studio | 2022 Community (or higher) |
| Workload | **Desktop development with C++** |
| Platform Toolset | v143 |
| Target Platform | Windows 10 / 11, x64 |

---

## Building

1. Open **`LocalisationDemo.sln`** in Visual Studio 2022.
2. Select **Debug \| x64** or **Release \| x64**.
3. Build → **Build Solution** (`Ctrl+Shift+B`).

All outputs land in a shared folder:

```
bin\x64\Debug\      (or Release\)
  CommandLine.exe
  SDIApp.exe
  DialogApp.exe
  EnglishDLL.dll
  FrenchDLL.dll
  GermanDLL.dll
```

The language DLLs are automatically placed beside the executables, so no extra copying is required.

---

## Projects

### EnglishDLL / FrenchDLL / GermanDLL

Resource-only DLLs — no executable code beyond a stub `DllMain`. Each contains:

| Resource | Content |
|----------|---------|
| `STRINGTABLE` | All `IDS_*` strings in the target language |
| `IDR_MAINFRAME MENU` | Translated menu bar (French & German only) |
| `IDR_MAINFRAME ACCELERATORS` | Ctrl+E / Ctrl+F / Ctrl+G shortcuts |

String IDs are identical across all three DLLs, allowing transparent substitution at runtime.

---

### CommandLine

A Win32 console application that demonstrates **explicit** DLL loading.

- Loads `EnglishDLL.dll`, `FrenchDLL.dll`, or `GermanDLL.dll` with `LoadLibrary`.
- Retrieves strings with `LoadStringW` directly from the loaded module handle.
- Saves and restores the language preference via the Win32 registry API.

**Runtime menu:**

```
  [1]  English   <- current
  [2]  French
  [3]  German
  [0]  Exit
```

---

### SDIApp

An MFC Single Document Interface application demonstrating **transparent** DLL switching.

- Calls `AfxSetResourceHandle` to redirect all MFC resource lookups to the active language DLL.
- Replaces the frame menu at runtime using `::SetMenu` and keeps `CFrameWnd::m_hMenuDefault` in sync to avoid MFC assertions.
- Overrides `CMainFrame::GetMessageString` so MFC-internal strings (status-bar prompts, "Ready" indicator) fall back to the app's own module when not present in the satellite DLL.
- Language menu items show a radio-bullet via `ON_UPDATE_COMMAND_UI`.
- Keyboard shortcuts: **Ctrl+E** (English), **Ctrl+F** (French), **Ctrl+G** (German).

---

### DialogApp

An MFC Dialog-based application with radio buttons for language selection.

- Three radio buttons (English / French / German) drive `AfxSetResourceHandle`.
- All visible strings — static labels, the dialog title bar, and the Exit button — are reloaded from the active DLL whenever the language changes.
- An `m_bInitializing` flag prevents `BS_AUTORADIOBUTTON` notifications from firing during `OnInitDialog` and corrupting the saved preference.

---

## Key Techniques

| Technique | Where used |
|-----------|-----------|
| `LoadLibrary` / `FreeLibrary` | All projects |
| `LoadStringW` (explicit handle) | CommandLine |
| `AfxSetResourceHandle` | SDIApp, DialogApp |
| `CFrameWnd::m_hMenuDefault` sync | SDIApp |
| `GetMessageString` fallback override | SDIApp |
| `ON_UPDATE_COMMAND_UI` radio bullets | SDIApp |
| Registry persistence (Win32 API) | CommandLine, SDIApp, DialogApp |
| `#pragma code_page(65001)` (UTF-8 RC) | FrenchDLL, GermanDLL |

---

## Adding a New Language

1. Copy an existing language DLL project folder (e.g. `GermanDLL`) and rename it.
2. Update `resource.h` to add the new `ID_LANGUAGE_*` constant (next available value after 32773).
3. Translate all strings in the new `.rc` file; set the correct `LANGUAGE` statement.
4. Add a menu item for the new language to **every** language DLL's `IDR_MAINFRAME MENU` and `ACCELERATORS`.
5. Add `ID_LANGUAGE_*` to `SDIApp\resource.h` and `SDIApp\SDIApp.rc` (menu, accelerator, prompt string).
6. Add `OnLanguage*` / `OnUpdateLanguage*` handlers to `SDIApp`.
7. Add a radio button to `DialogApp\DialogApp.rc`, a handler in `MainDlg`, and a `SwitchTo*()` method.
8. Add a menu option (`[4]`, etc.) in `CommandLine\main.cpp`.
9. Add the new project to `LocalisationDemo.sln` and declare it as a dependency of the three host apps.

---

## Registry Cleanup

To reset the language preference to the factory default, delete the registry key:

```
HKCU\Software\LocalisationDemo
```

This can be done from an elevated command prompt:

```cmd
reg delete "HKCU\Software\LocalisationDemo" /f
```
