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
#pragma once
#ifndef __CFRAMETIMER_HPP
#define __CFRAMETIMER_HPP

typedef unsigned __int64 UINT64;

//#include <d3d.h>
//#include <d3dx.h>
#include <windows.h>
#include <mmsystem.h>

class CFrameTimer
{
public:
    CFrameTimer();

    void Start(float fFramesPerSec);
    void Frame();

    float GetFramesPerSec();
    float GetSecsPerFrame();

    UINT64 GetTicks();
    UINT64 GetTicksPerSec();
    UINT64 GetTicksPerFrame();

protected:
    float m_fTicksPerSec;
    float m_fFramesPerSec;
    float m_fSecsPerFrame;

    UINT64 m_qwTicks;
    UINT64 m_qwTicksPerSec;
    UINT64 m_qwTicksPerFrame;
};



//////////////////////////////////////////////////////////////////////////////
// Inline methods ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

inline float
CFrameTimer::GetFramesPerSec()
{
    return m_fFramesPerSec;
}

inline float
CFrameTimer::GetSecsPerFrame()
{
    return m_fSecsPerFrame;
}

inline UINT64
CFrameTimer::GetTicksPerSec()
{
    return m_qwTicksPerSec;
}

inline UINT64
CFrameTimer::GetTicksPerFrame()
{
    return m_qwTicksPerFrame;
}


#endif __CFRAMETIMER_HPP