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

void LastRecentFileList::initMenu(HMENU hMenu, int idBase, int posBase, Accelerator *pAccelerator, bool doSubMenu)
{
	printStr(TEXT("THE ANSWER..."));
	printInt(42);
	printStr(TEXT("let's try several lines \n second line \n third line \n and a longher line asdf asdfl jkas;jasdjkasljk asljkdf a end of the line \n") );
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
}

void LastRecentFileList::switchMode()
{	
	if (_size > 0) // remove all RFH (recent file history) menu items
	{
		// remove recent files ; must be done before removing the other RFH items below, b/c the latter - by position as chosen
		for (int i = 0; i < _size; ++i) 
		{
			::RemoveMenu(_hMenu, _lrfl.at(i)._id, MF_BYCOMMAND);
		}
		HMENU hMainMenu = isSubMenuMode() ? _hParentMenu : _hMenu;

		::RemoveMenu(hMainMenu, _posBase + 4, MF_BYPOSITION); // the last extra separator, before Exit item
		::RemoveMenu(hMainMenu, _posBase + 3, MF_BYPOSITION); // IDM_CLEAN_RECENT_FILE_LIST
		::RemoveMenu(hMainMenu, _posBase + 2, MF_BYPOSITION); // IDM_OPEN_ALL_RECENT_FILE
		::RemoveMenu(hMainMenu, _posBase + 1, MF_BYPOSITION); // IDM_FILE_RESTORELASTCLOSEDFILE
		::RemoveMenu(hMainMenu, _posBase + 0, MF_BYPOSITION); // separator or "RecentFiles->" entry

		/*
		 in main-menu after "print now", should have:		
		-----------------			_posBase-1
		Exit						_posBase
		*/
	}		

	if (_hParentMenu == NULL) // mode main menu (all Recent File History (RFH) items are in main-menu); thus _hMenu points to main-menu
	{	
		// switch to sub-menu mode
		_hParentMenu = _hMenu;
		_hMenu = ::CreatePopupMenu(); //  in updateMenu(), this _hMenu will be attached to the hParentMenu(main-menu) at _posBase, and populated
	}
	else // mode sub-menu ; _hMenu points to sub-menu; _hParentMenu points to main-menu (file menu)
	{
		// switch to main menu mode
		::DestroyMenu(_hMenu);
		_hMenu = _hParentMenu;
		_hParentMenu = NULL;
	}
	_hasSeparators = false; // by "separators" here it's meant _extra_ separator items between: the separator after Print-now, and Exit.
}

void LastRecentFileList::add(const TCHAR *fn) 
{	
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
void LastRecentFileList::remove(size_t index) 
{	
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

void LastRecentFileList::clear() 
{
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

void LastRecentFileList::updateMenu()
{
	NppParameters& nppParam = NppParameters::getInstance();

	HMENU hMainMenu = isSubMenuMode() ? _hParentMenu : _hMenu;
	
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
			::InsertMenu(hMainMenu, _posBase + 0, MF_BYPOSITION, static_cast<UINT_PTR>(-1), 0); // a separator 					
		}
		else
		{			
			::InsertMenu(hMainMenu, _posBase + 0, MF_BYPOSITION | MF_POPUP, reinterpret_cast<UINT_PTR>(_hMenu), (LPCTSTR)recentFileList.c_str());
		}
		::InsertMenu(hMainMenu, _posBase + 1, MF_BYPOSITION, IDM_FILE_RESTORELASTCLOSEDFILE, openRecentClosedFile.c_str());
		::InsertMenu(hMainMenu, _posBase + 2, MF_BYPOSITION, IDM_OPEN_ALL_RECENT_FILE, openAllFiles.c_str());
		::InsertMenu(hMainMenu, _posBase + 3, MF_BYPOSITION, IDM_CLEAN_RECENT_FILE_LIST, cleanFileList.c_str());
		::InsertMenu(hMainMenu, _posBase + 4, MF_BYPOSITION, static_cast<UINT_PTR>(-1), 0);

		_hasSeparators = true;
	}
	else if (_hasSeparators && _size == 0) 	//remove RFH menu items (files from menu already gone). Ex, after clear(), and possibly after remove(), setUserMaxNbLRF()
	{
		::RemoveMenu(hMainMenu, _posBase + 4, MF_BYPOSITION); // the last extra separator
		::RemoveMenu(hMainMenu, _posBase + 3, MF_BYPOSITION); // IDM_CLEAN_RECENT_FILE_LIST
		::RemoveMenu(hMainMenu, _posBase + 2, MF_BYPOSITION); // IDM_OPEN_ALL_RECENT_FILE
		::RemoveMenu(hMainMenu, _posBase + 1, MF_BYPOSITION); // IDM_FILE_RESTORELASTCLOSEDFILE
		::RemoveMenu(hMainMenu, _posBase + 0, MF_BYPOSITION); // separator or "RecentFiles->" entry
		
		_hasSeparators = false;

	}
	_pAccelerator->updateFullMenu();

	//Remove all menu recentFiles items
	for (int i = 0; i < _size; ++i) 
	{
		::RemoveMenu(_hMenu, _lrfl.at(i)._id, MF_BYCOMMAND);
	}
	//Then read them, so everything stays in sync
	int hmenuRFpos= isSubMenuMode() ? 0 : _posBase; // for _hMenu, start position of recent files items 
	for (int j = 0; j < _size; ++j)
	{
		generic_string strBuffer(BuildMenuFileName(nppParam.getRecentFileCustomLength(), j, _lrfl.at(j)._name) );
		::InsertMenu(_hMenu, hmenuRFpos + j, MF_BYPOSITION, _lrfl.at(j)._id, strBuffer.c_str());
	}
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
