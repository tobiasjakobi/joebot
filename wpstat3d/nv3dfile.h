/******************************************************************************

    JoeBOT - a bot for Counter-Strike
    Copyright (C) 2000-2002  Johannes Lampel

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

******************************************************************************/
#ifndef __NVFILE_H
#define __NVFILE_H

#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>

extern OPENFILENAME ofn;

void FileInitialize (HWND hwnd)				// Intialisiert Inf. in ofn (Def. in simulgrav.cpp)
{
	static char szFilter[] =	"Waypoints (*.wpj)\0*.wpj\0"  \
								"Stats (*.wst)\0*.wst\0" \
								"Overviews (*.bmp)\0*.bmp\0\0" ;
	
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hwndOwner         = hwnd ;
	ofn.hInstance         = NULL ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter    = 0 ;
	ofn.nFilterIndex      = 0 ;
	ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
	ofn.nMaxFile          = _MAX_PATH ;
	ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
	ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
	ofn.lpstrInitialDir   = NULL ;
	ofn.lpstrTitle        = NULL ;
	ofn.Flags             = 0 ;             // Set in Open and Close functions
	ofn.nFileOffset       = 0 ;
	ofn.nFileExtension    = 0 ;
	ofn.lpstrDefExt       = "txt" ;
	ofn.lCustData         = 0L ;
	ofn.lpfnHook          = NULL ;
	ofn.lpTemplateName    = NULL ;
}

BOOL FileOpenDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
{
	ofn.hwndOwner			=	hwnd ;
	ofn.lpstrFile			=	pstrFileName ;
	ofn.lpstrFileTitle		=	pstrTitleName ;
	ofn.Flags				=	OFN_HIDEREADONLY | OFN_CREATEPROMPT ;
	
	return GetOpenFileName (&ofn) ;
}

BOOL FileSaveDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
{
	ofn.hwndOwner			=	hwnd ;
	ofn.lpstrFile			=	pstrFileName ;
	ofn.lpstrFileTitle		=	pstrTitleName ;
	ofn.Flags				=	OFN_OVERWRITEPROMPT ;
	
	return GetSaveFileName (&ofn) ;
}

#endif // __NVFILE_H