#include "resource.h"
#include "win_main.h"
static OPENFILENAME ofn;

/*---------------------------------------------------------------------------------
-- FUNCTION: sendFileList
--
-- REVISIONS:
--
--	DESIGNER:	Jaymz Boilard
--	PROGRAMMER:	Jaymz Boilard
--
-- INTERFACE:   void receiveFileList(
--						WPARAM wParam)   //the socket to send to
--
-- RETURNS: void
--
-- NOTES:   Called on a download or single stream request.  It gets all the song
--			names from the current directory of the executable file and puts
--			them into a buffer, delimiting each song name with a colon, and
--			sends it to the 
------------------------------------------------------------------------------------*/
void sendFileList(WPARAM wParam)
{
	char buf[FILE_BUFF_SIZE];
	WIN32_FIND_DATA wfd;
	HANDLE FF;
	memset(buf, '\0', FILE_BUFF_SIZE);
	strcpy_s(buf, sizeof(buf), "FILES:");
	
	/* Server Stuff */
	if ((FF = FindFirstFile("*.wav",&wfd)) == INVALID_HANDLE_VALUE) 
	{
		MessageBox(ghWndMain, (LPCSTR)"Unable to find any .wav files in current directory.",
			(LPCSTR)"Error!", MB_OK | MB_ICONSTOP);
		return;
	}

	do
	{
		strcat_s(buf, sizeof(buf), wfd.cFileName);
		strcat_s(buf, sizeof(buf), ":");
	}
	while(FindNextFile(FF,&wfd));

	if(send(wParam, buf, strlen(buf), 0) == -1)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			MessageBox(ghWndMain, (LPCSTR)"send() failed.", (LPCSTR)"Error!", MB_OK | MB_ICONSTOP);
			closesocket(wParam);
		}
	}
}

/*---------------------------------------------------------------------------------
-- FUNCTION: receiveFileList
--
-- REVISIONS:
--
--	DESIGNER:	Jaymz Boilard
--	PROGRAMMER:	Jaymz Boilard
--
-- INTERFACE:   void receiveFileList(
--						WPARAM wParam)   //the socket to receive from
--
-- RETURNS: void
--
-- NOTES:   Called on a download or single stream operation.  It receives a list
--			of file names as a string, with each song delimited by a colon.
--			This will cycle through the string, sending each song name to
--			the list in the GUI.
------------------------------------------------------------------------------------*/
void receiveFileList(WPARAM wParam, char buf[])
{
	char * pch, * nextToken;

	/* Client Stuff */
	pch = strtok_s(buf, ":", &nextToken);
	pch = strtok_s(NULL, ":", &nextToken);
	while(pch != NULL)
	{
		AppendList(pch);
		pch = strtok_s(NULL, ":", &nextToken);
	}
}

void AppendList(char *str)
{
	HWND list = GetDlgItem(ghDlgMain, IDC_LST_PLAY);

	ListBox_InsertString(list, -1, str);
	ListBox_SetCurSel(list, ListBox_GetCount(list) -1);

	SetFocus(list);
}

void GetSelList(char * selItem) {
	HWND list = GetDlgItem(ghDlgMain, IDC_LST_PLAY);

	ListBox_GetText(list, ListBox_GetCurSel(list), selItem);
}

/*---------------------------------------------------------------------------------
-- FUNCTION:	menu_up - Moves the current selection down within the list box.
--
-- REVISIONS:
--
--	DESIGNER:	Brendan Neva
--	PROGRAMMER:	Brendan Neva
--
-- INTERFACE:   void menu_down()
--
-- RETURNS: void
--
-- NOTES:   Does nothing if we are at the end.
------------------------------------------------------------------------------------*/
void menu_up()
{
	int currentIndex = 0;

	HWND list = GetDlgItem(ghDlgMain, IDC_LST_PLAY);
	currentIndex = ListBox_GetCurSel(list);

	if(currentIndex-1 < 0)
		return;
	else
		ListBox_SetCurSel(list,currentIndex-1);
}

/*---------------------------------------------------------------------------------
-- FUNCTION:	menu_down - Moves the current selection down within the list box.
--
-- REVISIONS:
--
--	DESIGNER:	Brendan Neva
--	PROGRAMMER:	Brendan Neva
--
-- INTERFACE:   void menu_down()
--
-- RETURNS: void
--
-- NOTES:   Does nothing if we are at the end.
------------------------------------------------------------------------------------*/
void menu_down()
{
	int currentIndex = 0;
	int fileCount = 0;

	HWND list = GetDlgItem(ghDlgMain, IDC_LST_PLAY);
	currentIndex = ListBox_GetCurSel(list);
	fileCount = ListBox_GetCount(list);

	if(currentIndex+1 > fileCount)
		return;
	else
		ListBox_SetCurSel(list,currentIndex+1);
}

/*---------------------------------------------------------------------------------
-- FUNCTION: browseFiles
--
-- DATE:	April 10
--
--	DESIGNER:	Jaymz Boilard
--	PROGRAMMER:	Jaymz Boilard
--
-- INTERFACE: BOOL browseFiles(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
--
-- RETURNS: The status of GetOpenFileName(), as a boolean
--
-- NOTES: Pops open a file browser to let a user choose a song.
------------------------------------------------------------------------------------*/
BOOL browseFiles(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	return GetOpenFileName(&ofn);
}

/*---------------------------------------------------------------------------------
-- FUNCTION: fileInit
--
-- DATE:	April 10
--
-- DESIGNER:	Jaymz Boilard
-- PROGRAMMER:	Jaymz Boilard
--
-- INTERFACE: void fileInit(HWND hwnd)
--
-- RETURNS: void
--
-- NOTES: Initializes the OPENFILENAME structure for later use.
------------------------------------------------------------------------------------*/
void fileInit(HWND hwnd)
{
	static TCHAR szFilter[] = TEXT("All Files (*.*)\0*.*\0\0");
	
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hwnd;
	ofn.hInstance         = NULL;
	ofn.lpstrFilter       = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0;
	ofn.nFilterIndex      = 0;
	ofn.lpstrFile         = NULL;   
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrFileTitle    = NULL;       
	ofn.nMaxFileTitle     = MAX_PATH;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = NULL;
	ofn.Flags             = 0;  
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = TEXT("wav");
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;
}
