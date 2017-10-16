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
//#include "pch.hpp"
#include "cframetimer.h"

//////////////////////////////////////////////////////////////////////////////
// CFrameTimer implementation //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CFrameTimer::CFrameTimer()
{
    if(!QueryPerformanceFrequency((LARGE_INTEGER *) &m_qwTicksPerSec))
        m_qwTicksPerSec = 1000;

    m_fTicksPerSec  = (float) (__int64) m_qwTicksPerSec;
}


void 
CFrameTimer::Start(float fFramesPerSec)
{
    m_fFramesPerSec = fFramesPerSec;
    m_fSecsPerFrame = 1.0f / fFramesPerSec;
    m_qwTicksPerFrame = m_qwTicksPerSec / (__int64) fFramesPerSec;

    m_qwTicks = GetTicks();
}


void 
CFrameTimer::Frame()
{
    UINT64 qw;
    qw = GetTicks();


    if(qw != m_qwTicks)
    {
        m_qwTicksPerFrame = qw - m_qwTicks;
        m_qwTicks = qw;

        m_fFramesPerSec = 0.75f * m_fFramesPerSec +
            0.25f * (m_fTicksPerSec / (float) (__int64) m_qwTicksPerFrame);

        if(m_fFramesPerSec < 1.0f)
            m_fFramesPerSec = 1.0f;

        m_fSecsPerFrame = 1.0f / m_fFramesPerSec;
    }
}


UINT64
CFrameTimer::GetTicks()
{
    UINT64 qw;

    if(QueryPerformanceCounter((LARGE_INTEGER *) &qw))
    {
        while(!qw)
          QueryPerformanceCounter((LARGE_INTEGER *) &qw);
    }
    else
    {
        qw = (UINT64) timeGetTime();
    }

    return qw;
}