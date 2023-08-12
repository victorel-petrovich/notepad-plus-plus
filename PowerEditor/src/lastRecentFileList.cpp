// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.



#include "lastRecentFileList.h"
#include "menuCmdID.h"
#include "localization.h"

/*
  initMenu(..), of class LastRecentFileList, is only called once in src folder: in Notepad_plus.cpp, inside init(...), line 580:
 _lastRecentFileList.initMenu(hFileMenu, IDM_FILEMENU_LASTONE + 1, IDM_FILEMENU_EXISTCMDPOSITION, &_accelerator, nppParam.putRecentFileInSubMenu());
	and is followed by calls to add() (from this file) to add RFH file items to menu 
	
 see  menuCmdID.h  
 IDM_FILE_EXIT=41011 (the id) 
 IDM_FILEMENU_LASTONE + 1 = 41026 (first id to be given for a recent filename)
 * at line: 
	// 0 based position of command "Exit"
 IDM_FILEMENU_EXISTCMDPOSITION = 22
 also see what what positions, relative to 0 at top, should be when _lrfl is empty (_lrfl is the internal list of recent files)
*/
void LastRecentFileList::initMenu(HMENU hMenu, int idBase, int posBase, Accelerator *pAccelerator, bool doSubMenu)
{
	//me added for debug prints
	AllocConsole();
	freopen("conin$","r",stdin);
	freopen("conout$","w",stdout);
	freopen("conout$","w",stderr);
	//-----------------------------

	printf("initMenu()\n\n" );
	
	if (doSubMenu)
	{
		_hParentMenu = hMenu;
		_hMenu = ::CreatePopupMenu();
	}
	else
	{
		_hParentMenu = NULL;
		_hMenu = hMenu;
	}

	_idBase = idBase;
	_posBase = posBase;
	_pAccelerator = pAccelerator;
	_nativeLangEncoding = NPP_CP_WIN_1252;

	for (size_t i = 0 ; i < sizeof(_idFreeArray) ; ++i)
		_idFreeArray[i] = true;
	
	/*debugs
	*/
	printf("\t _size=%d\n", _size );
	printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
	printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );	
	printf("\n");
}

/*
switchMode is only called once in scr folder: in NppBigSwitch.cpp
call to switchMode() is followed by call to updateMenu(), defined lower ; 
calls done at: case NPPM_INTERNAL_RECENTFILELIST_SWITCH

TODO: call updateMenu() inside switchMode(), at end, and remove call in NppBigSwitch.cpp, 
	to be same as with the other 4 functions in current file. 
*/
void LastRecentFileList::switchMode()
{
	printf("switchMode()\n\n");
	
	//Remove all recent file history menu items that are commands (including recent files )
	::RemoveMenu(_hMenu, IDM_FILE_RESTORELASTCLOSEDFILE, MF_BYCOMMAND);
	::RemoveMenu(_hMenu, IDM_OPEN_ALL_RECENT_FILE, MF_BYCOMMAND);
	::RemoveMenu(_hMenu, IDM_CLEAN_RECENT_FILE_LIST, MF_BYCOMMAND);

	for (int i = 0; i < _size; ++i)
	{
		::RemoveMenu(_hMenu, _lrfl.at(i)._id, MF_BYCOMMAND);
	}
	/*debugs
	printf("\t before if-else, after removing commands (including filenames)\n" );	
	printf("\t _size=%d\n", _size );
	printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
	printf("\t GetMenuItemID(_hMenu, _posBase-1)=%d\n", GetMenuItemID(_hMenu, _posBase-1) );
	printf("\t GetMenuItemID(_hMenu, _posBase+0)=%d\n", GetMenuItemID(_hMenu, _posBase+0) );
	printf("\t GetMenuItemID(_hMenu, _posBase+1)=%d\n", GetMenuItemID(_hMenu, _posBase+1) );
	printf("\t GetMenuItemID(_hMenu, _posBase+2)=%d\n", GetMenuItemID(_hMenu, _posBase+2) );
	printf("\t GetMenuItemID(_hMenu, _posBase+3)=%d\n", GetMenuItemID(_hMenu, _posBase+3) );
	printf("\t GetMenuItemID(_hMenu, _posBase+4)=%d\n", GetMenuItemID(_hMenu, _posBase+4) );
	printf("\t GetMenuItemID(_hMenu, _posBase+5)=%d\n", GetMenuItemID(_hMenu, _posBase+5) );
	printf("\t GetMenuItemID(_hMenu, _posBase+6)=%d\n", GetMenuItemID(_hMenu, _posBase+6) );
	printf("\t GetMenuItemID(_hMenu, _posBase+7)=%d\n", GetMenuItemID(_hMenu, _posBase+7) );
	printf("\t GetMenuItemID(_hMenu, _posBase+8)=%d\n", GetMenuItemID(_hMenu, _posBase+8) );
	printf("\n");	
	printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase-1)=%d\n", GetMenuItemID(_hParentMenu, _posBase-1) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+0)=%d\n", GetMenuItemID(_hParentMenu, _posBase+0) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+1)=%d\n", GetMenuItemID(_hParentMenu, _posBase+1) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+2)=%d\n", GetMenuItemID(_hParentMenu, _posBase+2) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+3)=%d\n", GetMenuItemID(_hParentMenu, _posBase+3) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+4)=%d\n", GetMenuItemID(_hParentMenu, _posBase+4) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+5)=%d\n", GetMenuItemID(_hParentMenu, _posBase+5) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+6)=%d\n", GetMenuItemID(_hParentMenu, _posBase+6) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+7)=%d\n", GetMenuItemID(_hParentMenu, _posBase+7) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+8)=%d\n", GetMenuItemID(_hParentMenu, _posBase+8) );
	printf("\n");
	*/

	if (_hParentMenu == NULL) // mode main menu (recent files - also in main-menu); thus _hMenu points to main-menu
	{	
		/*
		// If _lrfl was empty, then in main-menu after "print now", should have:		
		-----------------			_posBase-1
		Exit						_posBase
		
		// If _lrfl was not empty, then have:
		-----------------			_posBase-1
		-----------------			_posBase
		-----------------			_posBase+1
		Exit						_posBase+2
		
		*/
		if (_size > 0) // remove 2 separators
		{
			// the first removal below makes the next bar take position _posBase
			::RemoveMenu(_hMenu, _posBase, MF_BYPOSITION);
			::RemoveMenu(_hMenu, _posBase, MF_BYPOSITION);
		}		
		// switch to sub-menu mode
		_hParentMenu = _hMenu;
		_hMenu = ::CreatePopupMenu(); //  in updateMenu(), this _hMenu will be attached to the hParentMenu(main-menu) at _posBase, and populated
		
		// ::RemoveMenu(_hMenu, _posBase+1, MF_BYPOSITION);  //  redundant line,The new menu created by CreatePopupMenu() is already empty. . 
			// Tested both w/ prints of ID and count sub-menu items before and after,as well as tried the app many ways . 
	}
	else // mode sub-menu ; _hMenu points to sub-menu; _hParentMenu points to main-menu (file menu)
	{
		/*
		// If _lrfl was empty, then in main-menu after "print now", should have:		
		-----------------			_posBase-1
		Exit						_posBase
		
		// If _lrfl was not empty, then should have:
		-----------------			_posBase-1
		RecentFiles ->				_posBase
		-----------------			_posBase+1
		Exit						_posBase+2
		*/
		
		if (_size > 0)//remove "RecentFiles ->" and 1 separator
		{
			::RemoveMenu(_hParentMenu, _posBase, MF_BYPOSITION);
			::RemoveMenu(_hParentMenu, _posBase, MF_BYPOSITION);
		}
		// switch to main menu mode
		::DestroyMenu(_hMenu);
		_hMenu = _hParentMenu;
		_hParentMenu = NULL;

	}

	_hasSeparators = false; // by "separators" here it's meant _extra_ separator items between: the separator after Print-now, and Exit.
	/*
	Now in main-menu after "Print now", have:		
	-----------------			_posBase-1
	Exit						_posBase
	*/
	/*debugs
	printf("\t at end: \n" );	
	printf("\t _size=%d\n", _size );
	printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
	printf("\t GetMenuItemID(_hMenu, _posBase-1)=%d\n", GetMenuItemID(_hMenu, _posBase-1) );
	printf("\t GetMenuItemID(_hMenu, _posBase+0)=%d\n", GetMenuItemID(_hMenu, _posBase+0) );
	printf("\t GetMenuItemID(_hMenu, _posBase+1)=%d\n", GetMenuItemID(_hMenu, _posBase+1) );
	printf("\t GetMenuItemID(_hMenu, _posBase+2)=%d\n", GetMenuItemID(_hMenu, _posBase+2) );
	printf("\t GetMenuItemID(_hMenu, _posBase+3)=%d\n", GetMenuItemID(_hMenu, _posBase+3) );
	printf("\t GetMenuItemID(_hMenu, _posBase+4)=%d\n", GetMenuItemID(_hMenu, _posBase+4) );
	printf("\t GetMenuItemID(_hMenu, _posBase+5)=%d\n", GetMenuItemID(_hMenu, _posBase+5) );
	printf("\t GetMenuItemID(_hMenu, _posBase+6)=%d\n", GetMenuItemID(_hMenu, _posBase+6) );
	printf("\t GetMenuItemID(_hMenu, _posBase+7)=%d\n", GetMenuItemID(_hMenu, _posBase+7) );
	printf("\t GetMenuItemID(_hMenu, _posBase+8)=%d\n", GetMenuItemID(_hMenu, _posBase+8) );
	printf("\n");	
	printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase-1)=%d\n", GetMenuItemID(_hParentMenu, _posBase-1) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+0)=%d\n", GetMenuItemID(_hParentMenu, _posBase+0) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+1)=%d\n", GetMenuItemID(_hParentMenu, _posBase+1) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+2)=%d\n", GetMenuItemID(_hParentMenu, _posBase+2) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+3)=%d\n", GetMenuItemID(_hParentMenu, _posBase+3) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+4)=%d\n", GetMenuItemID(_hParentMenu, _posBase+4) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+5)=%d\n", GetMenuItemID(_hParentMenu, _posBase+5) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+6)=%d\n", GetMenuItemID(_hParentMenu, _posBase+6) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+7)=%d\n", GetMenuItemID(_hParentMenu, _posBase+7) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+8)=%d\n", GetMenuItemID(_hParentMenu, _posBase+8) );
	printf("\n");
	*/
	
}

/*
 add(..) called in:
- Notepad_plus.cpp , right after initMenu(..), to add RFH files to menu 
	Line  587
- NppIO.cpp:
	- at closing a doc(tab): Line  830
	- in fileSaveAs(..) Line 1812
	
*/
void LastRecentFileList::add(const TCHAR *fn) 
{
	printf("add()\n\n" );
	
	if (_userMax == 0 || _locked)
		return;

	RecentItem itemToAdd(fn);

	int index = find(fn);
	if (index != -1)
	{
		//already in list, bump upwards
		remove(index);
	}

	if (_size == _userMax)
	{
		itemToAdd._id = _lrfl.back()._id;
		_lrfl.pop_back();	//remove oldest
	}
	else
	{
		itemToAdd._id = popFirstAvailableID();
		++_size;
	}
	_lrfl.push_front(itemToAdd);
	
	updateMenu();
}

void LastRecentFileList::remove(const TCHAR *fn) 
{ 
	int index = find(fn);
	if (index != -1)
		remove(index);
}
/*
called by remove(fn) above, and by add(fn) above
*/
void LastRecentFileList::remove(size_t index) 
{
	printf("remove(size_t index)\n\n" );
	
	if (_size == 0 || _locked)
		return;
	if (index < _lrfl.size())
	{
		::RemoveMenu(_hMenu, _lrfl.at(index)._id, MF_BYCOMMAND);
		setAvailable(_lrfl.at(index)._id);
		_lrfl.erase(_lrfl.begin() + index);
		--_size;
	
		updateMenu();
	}
}

/*
called in NppCommands.cpp at:
case IDM_CLEAN_RECENT_FILE_LIST :
*/
void LastRecentFileList::clear() 
{
	printf("clear()\n\n" );
	
	if (_size == 0)
		return;

	for (int i = (_size-1); i >= 0; i--) 
	{
		::RemoveMenu(_hMenu, _lrfl.at(i)._id, MF_BYCOMMAND);
		setAvailable(_lrfl.at(i)._id);
		_lrfl.erase(_lrfl.begin() + i);
	}
	_size = 0;

	updateMenu();
}

/* simplified!
called only in: NppBigSwitch.cpp, at: 
case NPPM_INTERNAL_SETTING_HISTORY_SIZE:
*/
void LastRecentFileList::setUserMaxNbLRF(int size)
{
	_userMax = size;
	if (_size > _userMax) 
	{	//start popping items
		while (_size > _userMax) 
		{
			::RemoveMenu(_hMenu, _lrfl.back()._id, MF_BYCOMMAND);
			setAvailable(_lrfl.back()._id);
			_lrfl.pop_back();
			_size--;
		}
		updateMenu();
	}
}

/*
updateMenu() has 3 main functions:
- add missing LRFL menu items (when _size>0) 
- remove unnecessary LRFL menu items (when _size==0)
- update updates the menu filenames based on current _lrfl

updateMenu() is called in 2 files only in src:
- in NppBigSwitch.cpp in process(...), switch(message):
	case NPPM_INTERNAL_RECENTFILELIST_UPDATE:
		call to updateMenu() alone
	case NPPM_INTERNAL_RECENTFILELIST_SWITCH:
		call to switchMode() is followed by call to updateMenu();  
- in current file, in 4 places:
	LastRecentFileList::add(const TCHAR *fn) 
	LastRecentFileList::remove(size_t index) 
	LastRecentFileList::clear()
	LastRecentFileList::setUserMaxNbLRF(int size)
*/
void LastRecentFileList::updateMenu()
{
	printf("updateMenu()\n\n" );

	NppParameters& nppParam = NppParameters::getInstance();
	
	if (!_hasSeparators && _size > 0)  // add missing RFH menu items: in main-menu, and, if submenu mode, also in sub-menu
	{	
		// preparation of the names for 4 separators items 
		NativeLangSpeaker *pNativeLangSpeaker = nppParam.getNativeLangSpeaker();

		generic_string recentFileList = pNativeLangSpeaker->getSubMenuEntryName("file-recentFiles");
		generic_string openRecentClosedFile = pNativeLangSpeaker->getNativeLangMenuString(IDM_FILE_RESTORELASTCLOSEDFILE);
		generic_string openAllFiles = pNativeLangSpeaker->getNativeLangMenuString(IDM_OPEN_ALL_RECENT_FILE);
		generic_string cleanFileList = pNativeLangSpeaker->getNativeLangMenuString(IDM_CLEAN_RECENT_FILE_LIST);

		if (recentFileList == TEXT(""))
			recentFileList = TEXT("&Recent Files");
		if (openRecentClosedFile == TEXT(""))
			openRecentClosedFile = TEXT("Restore Recent Closed File");
		if (openAllFiles == TEXT(""))
			openAllFiles = TEXT("Open All Recent Files");
		if (cleanFileList == TEXT(""))
			cleanFileList = TEXT("Empty Recent Files List");
		
		if (!isSubMenuMode())
		{
			::InsertMenu(_hMenu, _posBase + 0, MF_BYPOSITION, static_cast<UINT_PTR>(-1), 0); // a separator 					
			::InsertMenu(_hMenu, _posBase + 1, MF_BYPOSITION, IDM_FILE_RESTORELASTCLOSEDFILE, openRecentClosedFile.c_str());
			::InsertMenu(_hMenu, _posBase + 2, MF_BYPOSITION, IDM_OPEN_ALL_RECENT_FILE, openAllFiles.c_str());
			::InsertMenu(_hMenu, _posBase + 3, MF_BYPOSITION, IDM_CLEAN_RECENT_FILE_LIST, cleanFileList.c_str());
			::InsertMenu(_hMenu, _posBase + 4, MF_BYPOSITION, static_cast<UINT_PTR>(-1), 0);
		}
		else
		{			
			::InsertMenu(_hParentMenu, _posBase + 0, MF_BYPOSITION | MF_POPUP, reinterpret_cast<UINT_PTR>(_hMenu), (LPCTSTR)recentFileList.c_str());
			::InsertMenu(_hParentMenu, _posBase + 1, MF_BYPOSITION, static_cast<UINT_PTR>(-1), 0);
		}
		/*debugs
		*/
		printf("\t if(!_hasSeparators && _size > 0), after: if-else on (!isSubMenuMode()):  \n" );	
		printf("\t _size=%d\n", _size );
		printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos-1)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos-1) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+0)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+0) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+1)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+1) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+2)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+2) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+3)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+3) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+4)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+4) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+5)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+5) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+6)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+6) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+7)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+7) );
		printf("\n");	
		printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase-1)=%d\n", GetMenuItemID(_hParentMenu, _posBase-1) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+0)=%d\n", GetMenuItemID(_hParentMenu, _posBase+0) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+1)=%d\n", GetMenuItemID(_hParentMenu, _posBase+1) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+2)=%d\n", GetMenuItemID(_hParentMenu, _posBase+2) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+3)=%d\n", GetMenuItemID(_hParentMenu, _posBase+3) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+4)=%d\n", GetMenuItemID(_hParentMenu, _posBase+4) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+5)=%d\n", GetMenuItemID(_hParentMenu, _posBase+5) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+6)=%d\n", GetMenuItemID(_hParentMenu, _posBase+6) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+7)=%d\n", GetMenuItemID(_hParentMenu, _posBase+7) );
		printf("\n");
				
		_hasSeparators = true;
	}
	else if (_hasSeparators && _size == 0) 	//remove RFH menu items (files from menu already gone). Ex, after clear(), and possibly after remove(), setUserMaxNbLRF()
	{

		if (!isSubMenuMode())
		{
			::RemoveMenu(_hMenu, _posBase + 0, MF_BYPOSITION); // separator
			::RemoveMenu(_hMenu, _posBase + 1, MF_BYPOSITION); // IDM_FILE_RESTORELASTCLOSEDFILE
			::RemoveMenu(_hMenu, _posBase + 2, MF_BYPOSITION); // IDM_OPEN_ALL_RECENT_FILE
			::RemoveMenu(_hMenu, _posBase + 3, MF_BYPOSITION); // IDM_CLEAN_RECENT_FILE_LIST
			::RemoveMenu(_hMenu, _posBase + 4, MF_BYPOSITION); // the last extra separator
		}
		else
		{			
			::RemoveMenu(_hParentMenu, _posBase + 1, MF_BYPOSITION); // a separator 
			::RemoveMenu(_hParentMenu, _posBase + 0, MF_BYPOSITION); // the "Recent Files" Entry 
		}
		
		_hasSeparators = false;

		/*debugs
		*/
		printf("\t else if (_hasSeparators && _size == 0), before if (isSubMenuMode()) : \n" );	
		printf("\t _size=%d\n", _size );
		printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos-1)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos-1) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+0)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+0) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+1)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+1) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+2)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+2) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+3)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+3) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+4)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+4) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+5)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+5) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+6)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+6) );
		printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+7)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+7) );
		printf("\n");	
		printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase-1)=%d\n", GetMenuItemID(_hParentMenu, _posBase-1) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+0)=%d\n", GetMenuItemID(_hParentMenu, _posBase+0) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+1)=%d\n", GetMenuItemID(_hParentMenu, _posBase+1) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+2)=%d\n", GetMenuItemID(_hParentMenu, _posBase+2) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+3)=%d\n", GetMenuItemID(_hParentMenu, _posBase+3) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+4)=%d\n", GetMenuItemID(_hParentMenu, _posBase+4) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+5)=%d\n", GetMenuItemID(_hParentMenu, _posBase+5) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+6)=%d\n", GetMenuItemID(_hParentMenu, _posBase+6) );
		printf("\t GetMenuItemID(_hParentMenu, _posBase+7)=%d\n", GetMenuItemID(_hParentMenu, _posBase+7) );
		printf("\n");

	}

	_pAccelerator->updateFullMenu();

	/*debugs
	*/
	printf("\t before last 2 for-loops: \n" );	
	printf("\t _size=%d\n", _size );
	printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos-1)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos-1) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+0)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+0) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+1)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+1) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+2)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+2) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+3)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+3) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+4)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+4) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+5)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+5) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+6)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+6) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+7)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+7) );
	printf("\n");	
	printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase-1)=%d\n", GetMenuItemID(_hParentMenu, _posBase-1) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+0)=%d\n", GetMenuItemID(_hParentMenu, _posBase+0) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+1)=%d\n", GetMenuItemID(_hParentMenu, _posBase+1) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+2)=%d\n", GetMenuItemID(_hParentMenu, _posBase+2) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+3)=%d\n", GetMenuItemID(_hParentMenu, _posBase+3) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+4)=%d\n", GetMenuItemID(_hParentMenu, _posBase+4) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+5)=%d\n", GetMenuItemID(_hParentMenu, _posBase+5) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+6)=%d\n", GetMenuItemID(_hParentMenu, _posBase+6) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+7)=%d\n", GetMenuItemID(_hParentMenu, _posBase+7) );
	printf("\n");

	//Remove all menu recentFiles items
	for (int i = 0; i < _size; ++i) 
	{
		::RemoveMenu(_hMenu, _lrfl.at(i)._id, MF_BYCOMMAND);
	}
	//Then read them, so everything stays in sync
	int hmenuRFpos= isSubMenuMode() ? 0 : _posBase; // for _hMenu, start position of Recent Files items 
	for (int j = 0; j < _size; ++j)
	{
		generic_string strBuffer(BuildMenuFileName(nppParam.getRecentFileCustomLength(), j, _lrfl.at(j)._name) );
		::InsertMenu(_hMenu, hmenuRFHpos + j, MF_BYPOSITION, _lrfl.at(j)._id, strBuffer.c_str());
	}
	/*debugs
	*/
	printf("\t at end: \n" );	
	printf("\t _size=%d\n", _size );
	printf("\t GetMenuItemCount(_hMenu )=%d\n", GetMenuItemCount(_hMenu ) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos-1)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos-1) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+0)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+0) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+1)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+1) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+2)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+2) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+3)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+3) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+4)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+4) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+5)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+5) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+6)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+6) );
	printf("\t GetMenuItemID(_hMenu, hmenuRFHpos+7)=%d\n", GetMenuItemID(_hMenu, hmenuRFHpos+7) );
	printf("\n");	
	printf("\t GetMenuItemCount(_hParentMenu )=%d\n", GetMenuItemCount(_hParentMenu ) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase-1)=%d\n", GetMenuItemID(_hParentMenu, _posBase-1) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+0)=%d\n", GetMenuItemID(_hParentMenu, _posBase+0) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+1)=%d\n", GetMenuItemID(_hParentMenu, _posBase+1) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+2)=%d\n", GetMenuItemID(_hParentMenu, _posBase+2) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+3)=%d\n", GetMenuItemID(_hParentMenu, _posBase+3) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+4)=%d\n", GetMenuItemID(_hParentMenu, _posBase+4) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+5)=%d\n", GetMenuItemID(_hParentMenu, _posBase+5) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+6)=%d\n", GetMenuItemID(_hParentMenu, _posBase+6) );
	printf("\t GetMenuItemID(_hParentMenu, _posBase+7)=%d\n", GetMenuItemID(_hParentMenu, _posBase+7) );
	printf("\n");
	//I tried get string names, but just like w/ GetMenuItemID, only works in main-menu.
	// char lpString[50]; int nchars;
	// nchars= GetMenuStringA(_hMenu, _posBase-1, lpString,  49,  MF_BYPOSITION);
	// printf("GetMenuStringA(_hMenu, _posBase-1,...)=%s; nchars=%d\n", lpString, nchars);
}


// The rest don't change the menus

/*
 get filename w/ given id; if no file has such id, return first filename 
*/
generic_string & LastRecentFileList::getItem(int id) 
{
	int i = 0;
	for (; i < _size; ++i)
	{
		if (_lrfl.at(i)._id == id)
			break;
	}
	if (i == _size)
		i = 0;
	return _lrfl.at(i)._name;	//if id not found, return first
}

/*
 get name of file at given index; 
*/
generic_string & LastRecentFileList::getIndex(int index)
{
	return _lrfl.at(index)._name;
}

/* 
called only in: NppBigSwitch.cpp, at: 
	case WM_CLOSE:
*/
void LastRecentFileList::saveLRFL()
{
	NppParameters& nppParams = NppParameters::getInstance();
	if (nppParams.writeRecentFileHistorySettings(_userMax))
	{
		for (int i = _size - 1; i >= 0; i--)	//reverse order: so loading goes in correct order
		{
			nppParams.writeHistory(_lrfl.at(i)._name.c_str());
		}
	}
}

//private methods

/*
get index, in _lrfl, of given filename; if not found => -1
*/
int LastRecentFileList::find(const TCHAR *fn)
{
	for (int i = 0; i < _size; ++i)
	{
		if (OrdinalIgnoreCaseCompareStrings(_lrfl.at(i)._name.c_str(), fn) == 0)
		{
			return i;
		}
	}
	return -1;
}

int LastRecentFileList::popFirstAvailableID() 
{
	for (int i = 0 ; i < NB_MAX_LRF_FILE ; ++i)
	{
		if (_idFreeArray[i])
		{
			_idFreeArray[i] = false;
			return i + _idBase;
		}
	}
	return 0;
}

void LastRecentFileList::setAvailable(int id)
{
	int index = id - _idBase;
	_idFreeArray[index] = true;
}
