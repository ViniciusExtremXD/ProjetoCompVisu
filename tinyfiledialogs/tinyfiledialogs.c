/*_________
 /         \ tinyfiledialogs.c v3.8.8 [Apr 22, 2021] zlib licence
 |tiny file| Unique code file created [November 9, 2014]
 | dialogs | Copyright (c) 2014 - 2021 Guillaume Vareille http://ysengrin.com
 \____  ___/ http://tinyfiledialogs.sourceforge.net
      \|     git clone http://git.code.sf.net/p/tinyfiledialogs/code tinyfd
             ____________________________________________
              ‖ ‖  ‖  ‖
              ‖ ‖  ‖  ‖
              ‖ ‖  ‖  ‖
              ‖_‖__‖__‖__________________________________________
              |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	          | Use project functions as-is and watch the console |
              |_______|_______|_______|_______|_______|_______|*/

#include "tinyfiledialogs.h"

#ifndef __GNUC__
#pragma warning(disable:4996) /* allows usage of strncpy, strcpy, strcat, sprintf, fopen */
#pragma warning(disable:4100) /* allows usage of strncpy, strcpy, strcat, sprintf, fopen */
#pragma warning(disable:4706) /* allows usage of strncpy, strcpy, strcat, sprintf, fopen */
#endif

int tinyfd_winUtf8 = 0 ; /* on windows string char can be 0:MBCS or 1:UTF-8 */
int tinyfd_forceConsole = 0 ; /* 0 (default) or 1 */

/* Here is the API string and file selector code */

static int osascriptPresent(void)
{
        static int lOsascriptPresent = -1 ;
        if ( lOsascriptPresent < 0 )
        {
                lOsascriptPresent = 0 ;
                if (! tinyfd_forceConsole)
                {
                        char lBuff[256] ;
                        char const* lDialogFile = "/tmp/tinyfd.txt";
                        FILE * lIn ;
                        char const * lString = "osascript -e 'display notification \"Hello\" with title \"tinyfiledialogs\"'" ;

                        system( lString ) ;
                        lIn = fopen( lDialogFile , "r" ) ;
                        if ( lIn != NULL )
                        {
                                if ( fgets( lBuff , sizeof( lBuff ) , lIn ) != NULL )
                                {
                                        if ( ! strchr( lBuff , '=' ) )
                                        {
                                                lOsascriptPresent = 1 ;
                                        }
                                }
                                fclose( lIn ) ;
                                if ( remove( lDialogFile ) != 0 )
                                {
                                        return 0 ;
                                }
                        }
                }
        }
        return lOsascriptPresent;
}


static int zenityPresent(void)
{
        static int lZenityPresent = -1 ;
        if ( lZenityPresent < 0 )
        {
                lZenityPresent = 0 ;
                if (! tinyfd_forceConsole)
                {
                        char lBuff[256] ;
                        char const* lDialogFile = "/tmp/tinyfd.txt";
                        FILE * lIn ;
                        char const * lString = "zenity --version" ;

                        system( lString ) ;
                        lIn = fopen( lDialogFile , "r" ) ;
                        if ( lIn != NULL )
                        {
                                if ( fgets( lBuff , sizeof( lBuff ) , lIn ) != NULL )
                                {
                                        if ( atoi(lBuff) >= 3 )
                                        {
                                                lZenityPresent = 1 ;
                                        }
                                }
                                fclose( lIn ) ;
                                if ( remove( lDialogFile ) != 0 )
                                {
                                        return 0 ;
                                }
                        }
                }
        }
        return lZenityPresent;
}


static int kdialogPresent(void)
{
        static int lKdialogPresent = -1 ;
        char lBuff[256] ;
        FILE * lIn ;
        char const * lString = "kdialog --version" ;
        
        if ( lKdialogPresent < 0 )
        {
                lKdialogPresent = 0 ;
                if (! tinyfd_forceConsole)
                {
                        system( lString ) ;
                        lIn = popen( lString , "r" ) ;
                        if ( lIn != NULL )
                        {
                                if ( fgets( lBuff , sizeof( lBuff ) , lIn ) != NULL )
                                {
                                        if ( atoi(lBuff) >= 4 )
                                        {
                                                lKdialogPresent = 1 ;
                                        }
                                }
                                pclose( lIn ) ;
                        }
                }
        }
        return lKdialogPresent;
}


static int qarmaPresent(void)
{
        static int lQarmaPresent = -1 ;
        char lBuff[256] ;
        FILE * lIn ;
        char const * lString = "qarma --version" ;
        
        if ( lQarmaPresent < 0 )
        {
                lQarmaPresent = 0 ;
                if (! tinyfd_forceConsole)
                {
                        lIn = popen( lString , "r" ) ;
                        if ( lIn != NULL )
                        {
                                if ( fgets( lBuff , sizeof( lBuff ) , lIn ) != NULL )
                                {
                                        if ( strchr(lBuff,'3') )
                                        {
                                                lQarmaPresent = 1 ;
                                        }
                                }
                                pclose( lIn ) ;
                        }
                }
        }
        return lQarmaPresent;
}


static int gxmessagePresent(
        char const * aThe_gxmessageCommand )
{
        static int lGxMessagePresent = -1 ;
        char lBuff[256] ;
        FILE * lIn ;
        char const * lString = aThe_gxmessageCommand ;

        if ( lGxMessagePresent < 0 )
        {
                lGxMessagePresent = 0 ;
                if (! tinyfd_forceConsole)
                {
                        lIn = popen( lString , "r" ) ;
                        if ( lIn != NULL )
                        {
                                if ( fgets( lBuff , sizeof( lBuff ) , lIn ) != NULL )
                                {
                                        lGxMessagePresent = 1 ;
                                }
                                pclose( lIn ) ;
                        }
                }
        }
        return lGxMessagePresent;
}


static int xdialogPresent(void)
{
        static int lXdialogPresent = -1 ;
        char lBuff[256] ;
        FILE * lIn ;
        char const * lString = "Xdialog --version" ;

        if ( lXdialogPresent < 0 )
        {
                lXdialogPresent = 0 ;
                if (! tinyfd_forceConsole)
                {
                        lIn = popen( lString , "r" ) ;
                        if ( lIn != NULL )
                        {
                                if ( fgets( lBuff , sizeof( lBuff ) , lIn ) != NULL )
                                {
                                        lXdialogPresent = 1 ;
                                }
                                pclose( lIn ) ;
                        }
                }
        }
        return lXdialogPresent;
}


static int dialogPresent(void)
{
        static int lDialogPresent = -1 ;
        char lBuff[256] ;
        FILE * lIn ;
        char const * lString = "dialog --version" ;

        if ( lDialogPresent < 0 )
        {
                lDialogPresent = 0 ;
                if (! tinyfd_forceConsole)
                {
                        lIn = popen( lString , "r" ) ;
                        if ( lIn != NULL )
                        {
                                if ( fgets( lBuff , sizeof( lBuff ) , lIn ) != NULL )
                                {
                                        lDialogPresent = 1 ;
                                }
                                pclose( lIn ) ;
                        }
                }
        }
        return lDialogPresent;
}


static int whiptailPresent(void)
{
        static int lWhiptailPresent = -1 ;
        char lBuff[256] ;
        FILE * lIn ;
        char const * lString = "whiptail --version" ;

        if ( lWhiptailPresent < 0 )
        {
                lWhiptailPresent = 0 ;
                if (! tinyfd_forceConsole)
                {
                        lIn = popen( lString , "r" ) ;
                        if ( lIn != NULL )
                        {
                                if ( fgets( lBuff , sizeof( lBuff ) , lIn ) != NULL )
                                {
                                        lWhiptailPresent = 1 ;
                                }
                                pclose( lIn ) ;
                        }
                }
        }
        return lWhiptailPresent;
}


static char const * selectFileDialogWinGui(
        char * const aoBuff ,
        char const * const aTitle , /* NULL or "" */
        char const * const aDefaultPathAndFile , /* NULL or "" */
        int const aNumOfFilterPatterns , /* 0  */
        char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
        char const * const aSingleFilterDescription , /* NULL or "image files" */
        int const aAllowMultipleSelects ) /* 0 or 1 */
{
        char lDirname[MAX_PATH];
        char lDialogString[MAX_PATH];
        char lFilterPatterns[MAX_PATH] = "";
        int i;
        char * p;
        char * lRetval;
        HRESULT lHResult;
        OPENFILENAME ofn = {0};

        lHResult = CoInitializeEx(NULL, 0);

        getPathWithoutFinalSlash(lDirname, aDefaultPathAndFile);
        getLastName(aoBuff, aDefaultPathAndFile);

        if (aNumOfFilterPatterns > 0)
        {
                if ( aSingleFilterDescription && strlen(aSingleFilterDescription) )
                {
                        strcpy(lFilterPatterns, aSingleFilterDescription);
                        strcat(lFilterPatterns, "\n");
                }
                strcat(lFilterPatterns, aFilterPatterns[0]);
                for (i = 1; i < aNumOfFilterPatterns; i++)
                {
                        strcat(lFilterPatterns, ";");
                        strcat(lFilterPatterns, aFilterPatterns[i]);
                }
                strcat(lFilterPatterns, "\n");
                if ( ! (aSingleFilterDescription && strlen(aSingleFilterDescription) ) )
                {
                        strcpy(lDialogString, lFilterPatterns);
                        strcat(lFilterPatterns, lDialogString);
                }
                strcat(lFilterPatterns, "All Files\n*.*\n");
                p = lFilterPatterns;
                while ((p = strchr(p, '\n')) != NULL)
                {
                        *p = '\0';
                        p ++ ;
                }
        }

        ofn.lStructSize = sizeof(OPENFILENAME) ;
        ofn.hwndOwner = GetForegroundWindow();
        ofn.hInstance = 0;
        ofn.lpstrFilter = strlen(lFilterPatterns) ? lFilterPatterns : NULL;
        ofn.lpstrCustomFilter = NULL;
        ofn.nMaxCustFilter = 0;
        ofn.nFilterIndex = 1;
        ofn.lpstrFile = aoBuff;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = MAX_PATH/2;
        ofn.lpstrInitialDir = strlen(lDirname) ? lDirname : NULL;
        ofn.lpstrTitle = aTitle && strlen(aTitle) ? aTitle : NULL;
        ofn.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR ;
        if ( ! aAllowMultipleSelects )
        {
                ofn.Flags = ofn.Flags | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST ;
        }
        else
        {
                ofn.Flags = ofn.Flags | OFN_ALLOWMULTISELECT;
        }
        ofn.nFileOffset = 0;
        ofn.nFileExtension = 0;
        ofn.lpstrDefExt = NULL;
        ofn.lCustData = 0L;
        ofn.lpfnHook = NULL;
        ofn.lpTemplateName = NULL;

        if (GetOpenFileName(&ofn))
        {
                lRetval = aoBuff;
        }
        else
        {
                lRetval = NULL ;
        }

        if (lHResult == S_OK || lHResult == S_FALSE)
        {
                CoUninitialize();
        }
        return lRetval;
}


static char const * selectFileDialogWinConsole(
        char * const aoBuff ,
        char const * const aTitle , /* NULL or "" */
        char const * const aDefaultPathAndFile , /* NULL or "" */
        int const aNumOfFilterPatterns , /* 0 */
        char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
        char const * const aSingleFilterDescription , /* NULL or "image files" */
        int const aAllowMultipleSelects ) /* 0 or 1 */
{
        char lDialogString[MAX_PATH];
        char lString[MAX_PATH] ;
        int i ;

        strcpy(lDialogString, "dialog ");
        if ( aTitle && strlen(aTitle) )
        {
                strcat(lDialogString, "--title \"") ;
                strcat(lDialogString, aTitle) ;
                strcat(lDialogString, "\" ") ;
        }

        strcat(lDialogString, "--backtitle \"") ;
        strcat(lDialogString,
                "tab: focus | /: populate | spacebar: fill text field | ok: TEXT FIELD ONLY") ;
        strcat(lDialogString, "\" ") ;

        strcat(lDialogString, "--fselect \"") ;
        if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
        {
                /* dialog.exe uses unix separators even on windows */
                strcpy(lString, aDefaultPathAndFile) ;
                substituteSubStr(lString, "\\", "/", MAX_PATH) ;
                strcat(lDialogString, lString) ;
        }
        else
        {
                getcwd(lString, MAX_PATH);
                strcat(lDialogString, lString) ;
                strcat(lDialogString, "/") ;
        }
        strcat(lDialogString, "\" ") ;

        strcat(lDialogString, "0 60 ") ;
        strcat(lDialogString, "2> \"") ;
        strcat(lDialogString, getenv("TEMP")) ;
        strcat(lDialogString, "\\tinyfd.txt\" ") ;

        system(lDialogString) ;
        if (aAllowMultipleSelects)
        {
                lDialogString[0] = 0 ;
                strcat(lDialogString, "for /F \"delims=\"") ;
                strcat(lDialogString, " %v in (");
                strcat(lDialogString, "\"");
                strcat(lDialogString, getenv("TEMP")) ;
                strcat(lDialogString, "\\tinyfd.txt\"") ;
                strcat(lDialogString, " ) do @echo. %~v >> \"");
                strcat(lDialogString, getenv("TEMP")) ;
                strcat(lDialogString, "\\tinyfd2.txt\"") ;
                system(lDialogString) ;
                strcpy(lDialogString, "\"");
                strcat(lDialogString, getenv("TEMP")) ;
                strcat(lDialogString, "\\tinyfd2.txt\"") ;
                return lDialogString ;
        }
        else
        {
                strcpy(lDialogString, "\"");
                strcat(lDialogString, getenv("TEMP")) ;
                strcat(lDialogString, "\\tinyfd.txt\"") ;
                return lDialogString ;
        }
}


static char const * selectFileDialogLinux(
        char * const aoBuff ,
        char const * const aTitle , /* NULL or "" */
        char const * const aDefaultPathAndFile , /* NULL or "" */
        int const aNumOfFilterPatterns , /* 0 */
        char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
        char const * const aSingleFilterDescription , /* NULL or "image files" */
        int const aAllowMultipleSelects ) /* 0 or 1 */
{
        static char lBuff[MAX_PATH] ;
        char lDialogString[1024] ;
        char lString[1024] ;
        int i ;
        FILE * lIn ;
        lBuff[0]='\0';

        if (zenityPresent())
        {
                strcpy(lDialogString, "zenity --file-selection") ;
                if (aTitle && strlen(aTitle))
                {
                        strcat(lDialogString, " --title=\"") ;
                        strcat(lDialogString, aTitle) ;
                        strcat(lDialogString, "\"") ;
                }
                if (aAllowMultipleSelects)
                {
                        strcat(lDialogString, " --multiple") ;
                }
                if (aDefaultPathAndFile && strlen(aDefaultPathAndFile))
                {
                        strcat(lDialogString, " --filename=\"") ;
                        strcat(lDialogString, aDefaultPathAndFile) ;
                        strcat(lDialogString, "\"") ;
                }
                if (aNumOfFilterPatterns > 0)
                {
                        strcat(lDialogString, " --file-filter='") ;
                        if (aSingleFilterDescription &&
                                strlen(aSingleFilterDescription))
                        {
                                strcat(lDialogString, aSingleFilterDescription) ;
                                strcat(lDialogString, " | ") ;
                        }
                        for (i = 0 ; i < aNumOfFilterPatterns ; i++)
                        {
                                strcat(lDialogString, aFilterPatterns[i]) ;
                                strcat(lDialogString, " ") ;
                        }
                        strcat(lDialogString, "' --file-filter='All files | *'") ;
                }
        }
        else if (qarmaPresent())
        {
                strcpy(lDialogString, "qarma --get-file-name") ;
                if (aTitle && strlen(aTitle))
                {
                        strcat(lDialogString, " --title=\"") ;
                        strcat(lDialogString, aTitle) ;
                        strcat(lDialogString, "\"") ;
                }
                if (aAllowMultipleSelects)
                {
                        strcat(lDialogString, " --multiple") ;
                }
                if (aDefaultPathAndFile && strlen(aDefaultPathAndFile))
                {
                        strcat(lDialogString, " --filename=\"") ;
                        strcat(lDialogString, aDefaultPathAndFile) ;
                        strcat(lDialogString, "\"") ;
                }
                if (aNumOfFilterPatterns > 0)
                {
                        strcat(lDialogString, " --file-filter='") ;
                        if (aSingleFilterDescription &&
                                strlen(aSingleFilterDescription))
                        {
                                strcat(lDialogString, aSingleFilterDescription) ;
                                strcat(lDialogString, " | ") ;
                        }
                        for (i = 0 ; i < aNumOfFilterPatterns ; i++)
                        {
                                strcat(lDialogString, aFilterPatterns[i]) ;
                                strcat(lDialogString, " ") ;
                        }
                        strcat(lDialogString, "' --file-filter='All files | *'") ;
                }
        }
        else if (kdialogPresent())
        {
                if (aTitle && strlen(aTitle))
                {
                        strcpy(lDialogString, "kdialog --getopenfilename --title \"") ;
                        strcat(lDialogString, aTitle) ;
                        strcat(lDialogString, "\"") ;
                }
                else
                {
                        strcpy(lDialogString, "kdialog --getopenfilename") ;
                }

                if (aDefaultPathAndFile && strlen(aDefaultPathAndFile))
                {
                        strcat(lDialogString, " \"") ;
                        strcat(lDialogString, aDefaultPathAndFile) ;

                        strcat(lDialogString, "\"") ;
                }
                else
                {
                        strcat(lDialogString, " \"$HOME/\"") ;
                }

                if (aNumOfFilterPatterns > 0)
                {
                        strcat(lDialogString, " \"") ;
                        for (i = 0 ; i < aNumOfFilterPatterns ; i++)
                        {
                                strcat(lDialogString, aFilterPatterns[i]) ;
                                strcat(lDialogString, " ") ;
                        }
                        if (aSingleFilterDescription && strlen(aSingleFilterDescription))
                        {
                                strcat(lDialogString, " | ") ;
                                strcat(lDialogString, aSingleFilterDescription) ;
                        }
                        strcat(lDialogString, "\"") ;
                }

                if (aAllowMultipleSelects)
                {
                        strcat(lDialogString, " --multiple --separate-output") ;
                }
        }
        else if (osascriptPresent())
        {
                if (aTitle && strlen(aTitle))
                {
                        strcpy(lDialogString, "osascript -e 'tell application \"System Events\" to activate' -e 'tell application \"System Events\" to display dialog \"") ;
                        strcat(lDialogString, aTitle) ;
                        strcat(lDialogString, "\" default answer \"\"'") ;
                }
                else
                {
                        strcpy(lDialogString, "osascript -e 'tell application \"System Events\" to activate' -e 'tell application \"System Events\" to display dialog \"Select File\" default answer \"\"'") ;
                }
        }
        else if (xdialogPresent())
        {
                if (aTitle && strlen(aTitle))
                {
                        strcpy(lDialogString, "Xdialog --title \"") ;
                        strcat(lDialogString, aTitle) ;
                        strcat(lDialogString, "\" --fselect \"") ;
                }
                else
                {
                        strcpy(lDialogString, "Xdialog --fselect \"") ;
                }

                if (aDefaultPathAndFile && strlen(aDefaultPathAndFile))
                {
                        strcat(lDialogString, aDefaultPathAndFile) ;
                }
                else
                {
                        strcat(lDialogString, "$HOME/") ;
                }
                strcat(lDialogString, "\" 0 60") ;

                if (aAllowMultipleSelects)
                {
                        strcat(lDialogString, " --multiple") ;
                }
        }
        else
        {
                return NULL ;
        }

        if (tinyfd_verbose) printf("lDialogString: %s\n", lDialogString) ;
        if (!(lIn = popen(lDialogString, "r")))
        {
                return NULL ;
        }
        lBuff[0] = '\0' ;

        while (fgets(lBuff, sizeof(lBuff), lIn) != NULL)
        {} ;

        pclose(lIn) ;

        if (lBuff[strlen(lBuff) - 1] == '\n')
        {
                lBuff[strlen(lBuff) - 1] = '\0' ;
        }

        if (lBuff[0])
        {
                if (aoBuff)
                {
                        strcpy(aoBuff, lBuff) ;
                }
                else
                {
                        return lBuff ;
                }
        }

        return aoBuff ;
}


char const * tinyfd_openFileDialog(
        char const * const aTitle , /* NULL or "" */
        char const * const aDefaultPathAndFile , /* NULL or "" */
        int const aNumOfFilterPatterns , /* 0 */
        char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
        char const * const aSingleFilterDescription , /* NULL or "image files" */
        int const aAllowMultipleSelects ) /* 0 or 1 */
{
        static char lBuff[MAX_PATH] ;
        char lString[MAX_PATH] = "" ;

#ifdef _WIN32
        if (tinyfd_winUtf8)
        {
                return selectFileDialogWinGui(
                        lBuff ,
                        aTitle,
                        aDefaultPathAndFile,
                        aNumOfFilterPatterns,
                        aFilterPatterns,
                        aSingleFilterDescription,
                        aAllowMultipleSelects);
        }
        else
        {
                return selectFileDialogWinConsole(
                        lBuff ,
                        aTitle,
                        aDefaultPathAndFile,
                        aNumOfFilterPatterns,
                        aFilterPatterns,
                        aSingleFilterDescription,
                        aAllowMultipleSelects);
        }
#else
        return selectFileDialogLinux(
                lBuff ,
                aTitle,
                aDefaultPathAndFile,
                aNumOfFilterPatterns,
                aFilterPatterns,
                aSingleFilterDescription,
                aAllowMultipleSelects);
#endif        
}


char const * tinyfd_selectFolderDialog(
        char const * const aTitle , /* NULL or "" */
        char const * const aDefaultPath ) /* NULL or "" */
{
        static char lBuff[MAX_PATH];
        char lString[MAX_PATH] = "" ;

#ifdef _WIN32
        if (tinyfd_winUtf8)
        {
                lBuff[0] = '\0' ;
                return selectFolderDialogWinGui(lBuff, aTitle, aDefaultPath) ;
        }
        else
        {
                return selectFolderDialogWinConsole(lBuff, aTitle, aDefaultPath) ;
        }
#else
        return selectFolderDialogLinux(lBuff, aTitle, aDefaultPath) ;
#endif
}


char const * tinyfd_saveFileDialog(
        char const * const aTitle , /* NULL or "" */
        char const * const aDefaultPathAndFile , /* NULL or "" */
        int const aNumOfFilterPatterns , /* 0 */
        char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
        char const * const aSingleFilterDescription ) /* NULL or "image files" */
{
        static char lBuff[MAX_PATH];
        char lString[MAX_PATH] = "" ;

#ifdef _WIN32
        if (tinyfd_winUtf8)
        {
                lBuff[0] = '\0';
                return saveFileDialogWinGui(
                        lBuff,
                        aTitle,
                        aDefaultPathAndFile,
                        aNumOfFilterPatterns,
                        aFilterPatterns,
                        aSingleFilterDescription);
        }
        else
        {
                return saveFileDialogWinConsole(
                        lBuff,
                        aTitle,
                        aDefaultPathAndFile,
                        aNumOfFilterPatterns,
                        aFilterPatterns,
                        aSingleFilterDescription);
        }
#else
        return saveFileDialogLinux(
                lBuff,
                aTitle,
                aDefaultPathAndFile,
                aNumOfFilterPatterns,
                aFilterPatterns,
                aSingleFilterDescription);
#endif
}