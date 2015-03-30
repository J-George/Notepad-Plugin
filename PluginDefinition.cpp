//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <iostream>
#include <windows.h>
#include "PluginDefinition.h"
#include "menuCmdID.h"
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <locale>
using namespace std;

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];
int rtnFlg = 0;

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );

    ShortcutKey *Compile_key = new ShortcutKey;
    Compile_key->_isAlt      = false;
    Compile_key->_isCtrl     = true;
    Compile_key->_isShift    = true;
    Compile_key->_key        = 0x76; // F7 Key
    
    ShortcutKey *Run_key = new ShortcutKey;
    Run_key->_isAlt      = false;
    Run_key->_isCtrl     = true;
    Run_key->_isShift    = true;
    Run_key->_key        = 0x74; // F5 Key
    
    ShortcutKey *Java_key = new ShortcutKey;
    Java_key->_isAlt      = false;
    Java_key->_isCtrl     = true;
    Java_key->_isShift    = true;
    Java_key->_key        = 0x4A; // J Key

	setCommand(0, TEXT("Compile"), compile, Compile_key, false);
    setCommand(1, TEXT("Compile and Run"), compileAndRun, Run_key, false);
	setCommand(2, TEXT("---"), NULL, NULL, false);
	setCommand(3, TEXT("JAR File Creator"), helloDlg, NULL, false);
	setCommand(4, TEXT("Tab Checker"), helloDlg, NULL, false);
	setCommand(5, TEXT("Compare Files"), helloDlg, NULL, false);
	setCommand(6, TEXT("Connect to Java Documentation"),cnctJvDc, Java_key, false);
	setCommand(7, TEXT("Connect to StackOverflow"),cnctStckOvrflw, NULL, false);
	setCommand(8, TEXT("Connect to GitBash"), cnctgtbsh, NULL, false);
	setCommand(9, TEXT("Beginner's Guide"), bgnnrsGd, NULL, false);
	setCommand(10, TEXT("---"), NULL, NULL, false);
	setCommand(11, TEXT("About"),about, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void hello()
{
    // Open a new document
    ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

    // Get the current scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return;
    HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;

    // Say hello now :
    // Scintilla control has no Unicode mode, so we use (char *) here
    ::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)"Hello, Notepad++!");
}

void helloDlg()
{
    ::MessageBox(NULL, TEXT("Hello, Notepad++!"), TEXT("Notepad++ Plugin Template"), MB_OK);
}

string getPath()
{
    wchar_t *fullPath = new wchar_t[MAX_PATH];
    SendMessageA(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY,0,(LPARAM)fullPath);
    wstring fP = fullPath;
    return string(fP.begin(),fP.end());
}

string getFileName()
{
	wchar_t *name = new wchar_t[MAX_PATH];
    SendMessageA(nppData._nppHandle, NPPM_GETFILENAME, 0, (LPARAM)name);
    wstring ws = name;
    return string(ws.begin(),ws.end());
}

string getNPPDirectory()
{
	wchar_t *directory = new wchar_t[MAX_PATH];
	SendMessageA(nppData._nppHandle, NPPM_GETNPPDIRECTORY, 0, (LPARAM)directory);
	wstring ws = directory;
    return string(ws.begin(),ws.end());
}

void getAllOpenFiles()
{
	int num_files = SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, ALL_OPEN_FILES);

	vector< vector<TCHAR> > file_name_bufs(num_files);
	vector< TCHAR* > file_name_ptrs(num_files);
	ofstream (fileNames);
	fileNames.open("C:/Users/Justin George/Desktop/fileWithAllOpenFileNames.txt");
	for (int i=0; i<num_files; i++) 
	{
		file_name_bufs[i].resize(MAX_PATH);
		file_name_ptrs[i] = &file_name_bufs[i][0];
		fileNames << &file_name_ptrs[i];
		fileNames << "\n";
	}
}

void compile()
{
	//Save Current Document
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_SAVE);
	
	//get the path of the document
	string fileName, path;
	path = getPath();
	//get the name of the document
	fileName = getFileName();

	//making a log file
	string log = "log_" + fileName + ".txt" ;
	string logPath = path + "//"+ log;

	//combine the strings together to make one command
	string compileAndRedirCommand = "/k cd /d" + path + " & javac " + fileName + " 2> " + log;
	string compileCommand = "/k cd /d" + path + " & javac " + fileName;

	//Open Command Prompt and pipe program to JDK and redirect output to log file
	ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", compileAndRedirCommand.c_str(), NULL, SW_HIDE);
	Sleep(2000);

	//check log file, if empty, compiling success, else fail and show errors
	ifstream file(logPath);
	file.seekg(0, file.end);
	int fileLength = file.tellg();
	if (fileLength == 0) 
    {
        ::MessageBox(NULL, TEXT("Compiling Successful!"), TEXT("PESMU Compiler Message"), MB_OK);
		rtnFlg = 0;
    }
    //otherwise show message
	else if (fileLength > 0)
    {
		::MessageBox(NULL, TEXT("Compiling Not Successful!"), TEXT("PESMU Compiler Message"), MB_OK);
        ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", compileCommand.c_str(), NULL, SW_SHOW);
		rtnFlg = 1;
    }
	else
	{
		::MessageBox(NULL,TEXT("Could not compile. Either JDK is not installed, or the path provided is incorrect."), TEXT("PESMU Compiler Message"), MB_OK);
		rtnFlg = 0;
	}
	file.close();
	remove(logPath.c_str());
}

void compileAndRun()
{
	//Save Current Document
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_SAVE);
	//Use Compile
	compile();
	//get parameters for run
	string fileName,path;
	path = getPath();
	fileName = getFileName();
	//make the string to run the file
	int length = fileName.length();
	length = length - 5;
	fileName.erase(fileName.begin()+length,fileName.end());
	if (rtnFlg != 1)
	{
		string runCommand = "/k cd /d" + path + " & java " + fileName;
		ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", runCommand.c_str(), NULL, SW_SHOW);
	}

}

void tabChecker()
{
	//Get name of document
	string file = getFileName() + "_temp.txt";
	//Create a .txt file
	ifstream tabDcChckr(file);

	
}

void JARfc()
{

}

void compareFiles()
{

}

void cnctJvDc()
{
	ShellExecuteA(NULL,"open","http://docs.oracle.com/javase/tutorial/",NULL,NULL,SW_SHOW);
}

void cnctStckOvrflw()
{
	ShellExecuteA(NULL,"open","www.stackoverflow.com",NULL,NULL,SW_SHOW);
}

void cnctgtbsh()
{
    string bshPath = getNPPDirectory()+ "/plugins/Config/PESMU/config.txt";
	ifstream conFilepath(bshPath);
    if(conFilepath.fail())
	{
		::MessageBox(NULL, TEXT("The config.txt file is not found or corrupt. Please replace the document."), TEXT("PESMU Plugin"), MB_OK);
		return;
	}
	string path;
	while(conFilepath.is_open())
	{
		getline(conFilepath,path);
		conFilepath.close();
	}
	ShellExecuteA(NULL,"open",path.c_str(),NULL,NULL,SW_SHOW);
}

void bgnnrsGd()
{
    string gdPth = getNPPDirectory() + "/plugins/doc/PESMU/BeginnersGuide.mht";
	ShellExecuteA(NULL,"open",gdPth.c_str(),NULL,NULL,SW_SHOW);
}

void about()
{
	vector<wstring> vps;
	vector<TCHAR*> wps;
	int n = SendMessage(nppData._nppHandle,NPPM_GETNBOPENFILES,0,ALL_OPEN_FILES);
	if( n > 0 && n <128 )
	{
		wps.resize(n);
		for( int i = 0;i < n;i++ )
		{
			wstring ws;
			ws.resize(512);
			vps.push_back(ws);
			wps[i] = (TCHAR*)vps[i].c_str();
		}
		SendMessage(nppData._nppHandle,NPPM_GETOPENFILENAMES,(LPARAM)&wps[0],n);
	}
	wofstream (names);
	names.open("C:/Users/Justin George/Desktop/names.txt");
	for ( int i = 0;i < n;i++ )
	{
		names << vps[i].c_str();
		names << "\n";
	}
	names.close();
}