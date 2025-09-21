#ifndef TINYFILEDIALOGS_H
#define TINYFILEDIALOGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef _WIN32
 #ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x0600
 #endif
 #ifndef NTDDI_VERSION
  #define NTDDI_VERSION 0x06000000
 #endif
 #include <windows.h>
 #include <shlobj.h>
 #include <conio.h>
 #include <tchar.h>
 #define MAX_PATH_LEN 1024
 #define SLASH "\\"
#else
 #include <limits.h>
 #include <unistd.h>
 #include <dirent.h>
 #include <termios.h>
 #include <sys/utsname.h>
 #include <signal.h>
 #define MAX_PATH_LEN PATH_MAX
 #define SLASH "/"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern char tinyfd_version[8]; /* contains tinyfd current version number */

#ifdef _WIN32
/* these vars can be set by calling functions */
extern int tinyfd_winUtf8; /* 0 (default) or 1 */
/* on windows string char can be 0:MBCS or 1:UTF-8 */
extern int tinyfd_forceConsole;  /* 0 (default) or 1 */
/* for unix & windows: 0 (graphic mode) or 1 (console mode) */
#endif

int tinyfd_messageBox(
    char const* aTitle , /* NULL or "" */
    char const* aMessage , /* NULL or "" may contain \n \t */
    char const* aDialogType , /* "ok" "okcancel" "yesno" "yesnocancel" */
    char const* aIconType , /* "info" "warning" "error" "question" */
    int aDefaultButton ) ; /* 0 for cancel/no , 1 for ok/yes , 2 for no in yesnocancel */

char const* tinyfd_inputBox(
    char const* aTitle , /* NULL or "" */
    char const* aMessage , /* NULL or "" may NOT contain \n \t on windows */
    char const* aDefaultInput ) ;  /* NULL = passwordBox, "" = inputbox */

char const* tinyfd_saveFileDialog(
    char const* aTitle , /* NULL or "" */
    char const* aDefaultPathAndFile , /* NULL or "" */
    int aNumOfFilterPatterns , /* 0 */
    char const* const* aFilterPatterns , /* NULL | {"*.jpg","*.png"} */
    char const* aSingleFilterDescription ) ; /* NULL | "text files" */

char const* tinyfd_openFileDialog(
    char const* aTitle , /* NULL or "" */
    char const* aDefaultPathAndFile , /* NULL or "" */
    int aNumOfFilterPatterns , /* 0 */
    char const* const* aFilterPatterns , /* NULL | {"*.jpg","*.png"} */
    char const* aSingleFilterDescription , /* NULL | "image files" */
    int aAllowMultipleSelects ) ; /* 0 or 1 */

char const* tinyfd_selectFolderDialog(
    char const* aTitle , /* NULL or "" */
    char const* aDefaultPath ) ; /* NULL or "" */

#ifdef __cplusplus
}
#endif

#endif /* TINYFILEDIALOGS_H */