/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//{{NO_DEPENDENCIES}}
// Microsoft Developer Studio generated include file.
// Used by joebot.rc
//

#ifndef VERS_INFO_H
#define VERS_INFO_H

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1000
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif

#define VVERSION "1.6.5.2"
#define VDATE __DATE__
#define VAUTHOR "@$3.1415rin <as3.1415rin@bots-united.com>"
#define VCOMPANY "joebot.bots-united.com"
#define VNAME "JOEBOT"
#define VURL "http://joebot.bots-united.com/"
#define VLOGTAG "JOEBOT"
#ifdef __linux__
#define VPLATFORM "linux"
#else
#define VPLATFORM "win32"
#endif
#ifdef USE_METAMOD
#define VDLLTYPE "MM"
#else
#define VDLLTYPE "STD"
#endif

#define RC_VERSION 1,6,5,2
#ifdef USE_METAMOD
#define RC_DESC "JOEBOT - Metamod Counter-Strike Bot"
#define RC_INTERNAL "joebot_mm"
#define RC_FILENAME "joebot_mm.dll"
#else /* USE_METAMOD */
#define RC_DESC "JOEBOT - Counter-Strike Bot"
#define RC_INTERNAL "joebot"
#define RC_FILENAME "joebot.dll"
#endif /* not USE_METAMOD */
#define RC_COPYRIGHT "Copyright © 2004"
#define RC_TRADEMARK ""

#endif /* VERS_INFO_H */
