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


#ifndef __CWPSTUFF
#define __CWPSTUFF

#include <limits.h>
#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

//=========================================================
// 2DVector - used for many pathfinding and many other 
// operations that are treated as planar rather than 3d.
//=========================================================
class Vector2D
{
public:
	inline Vector2D(void)									{ }
	inline Vector2D(float X, float Y)						{ x = X; y = Y; }
	inline Vector2D operator+(const Vector2D& v)	const	{ return Vector2D(x+v.x, y+v.y);	}
	inline Vector2D operator-(const Vector2D& v)	const	{ return Vector2D(x-v.x, y-v.y);	}
	inline Vector2D operator*(float fl)				const	{ return Vector2D(x*fl, y*fl);	}
	inline Vector2D operator/(float fl)				const	{ return Vector2D(x/fl, y/fl);	}
	
	inline float Length(void)						const	{ return sqrt(x*x + y*y );		}
	
	inline Vector2D Normalize ( void ) const
	{
		Vector2D vec2;
		
		float flLen = Length();
		if ( flLen == 0 )
		{
			return Vector2D( 0, 0 );
		}
		else
		{
			flLen = 1 / flLen;
			return Vector2D( x * flLen, y * flLen );
		}
	}
	
	float	x, y;
};

inline float DotProduct(const Vector2D& a, const Vector2D& b) { return( a.x*b.x + a.y*b.y ); }
inline Vector2D operator*(float fl, const Vector2D& v)	{ return v * fl; }

//=========================================================
// 3D Vector
//=========================================================
class Vector						// same data-layout as engine's vec3_t,
{								//		which is a vec_t[3]
public:
	// Construction/destruction
	inline Vector(void)								{ }
	inline Vector(float X, float Y, float Z)		{ x = X; y = Y; z = Z;						}
	//inline Vector(double X, double Y, double Z)		{ x = (float)X; y = (float)Y; z = (float)Z;	}
	//inline Vector(int X, int Y, int Z)				{ x = (float)X; y = (float)Y; z = (float)Z;	}
	inline Vector(const Vector& v)					{ x = v.x; y = v.y; z = v.z;				} 
	inline Vector(float rgfl[3])					{ x = rgfl[0]; y = rgfl[1]; z = rgfl[2];	}
	
	// Operators
	inline Vector operator-(void) const				{ return Vector(-x,-y,-z);				}
	inline int operator==(const Vector& v) const	{ return x==v.x && y==v.y && z==v.z;	}
	inline int operator!=(const Vector& v) const	{ return !(*this==v);					}
	inline Vector operator+(const Vector& v) const	{ return Vector(x+v.x, y+v.y, z+v.z);	}
	inline Vector operator-(const Vector& v) const	{ return Vector(x-v.x, y-v.y, z-v.z);	}
	inline Vector operator*(float fl) const			{ return Vector(x*fl, y*fl, z*fl);		}
	inline Vector operator/(float fl) const			{ return Vector(x/fl, y/fl, z/fl);		}
	
	// Methods
	inline void CopyToArray(float* rgfl) const		{ rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; }
	inline float Length(void) const					{ return sqrt(x*x + y*y + z*z); }
	operator float *()								{ return &x; } // Vectors will now automatically convert to float * when needed
	operator const float *() const					{ return &x; } // Vectors will now automatically convert to float * when needed
	inline Vector Normalize(void) const
	{
		float flLen = Length();
		if (flLen == 0) return Vector(0,0,1); // ????
		flLen = 1 / flLen;
		return Vector(x * flLen, y * flLen, z * flLen);
	}
	
	inline Vector2D Make2D ( void ) const
	{
		Vector2D	Vec2;
		
		Vec2.x = x;
		Vec2.y = y;
		
		return Vec2;
	}
	inline float Length2D(void) const					{ return sqrt(x*x + y*y); }
	
	// Members
	float x, y, z;
};
inline Vector operator*(float fl, const Vector& v)	{ return v * fl; }
inline float DotProduct(const Vector& a, const Vector& b) { return(a.x*b.x+a.y*b.y+a.z*b.z); }
inline Vector CrossProduct(const Vector& a, const Vector& b) { return Vector( a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x ); }


#define _MAXTEMPDIV 250

#define MAX_WAYPOINTS 4096

#define REACHABLE_RANGE 400													// ???? to change some time

#define WAYPOINT_UNREACHABLE   USHRT_MAX
#define WAYPOINT_MAX_DISTANCE (USHRT_MAX-1)

// defines for waypoint flags field (32 bits are available)
#define W_FL_TEAM        ((1<<0) + (1<<1))  /* allow for 4 teams (0-3) */
#define W_FL_TEAM_SPECIFIC (1<<2)  /* waypoint only for specified team */
#define W_FL_CROUCH      (1<<3)  /* must crouch to reach this waypoint */
#define W_FL_LADDER      (1<<4)  /* waypoint on a ladder */
#define W_FL_LIFT        (1<<5)  /* wait for lift to be down before approaching this waypoint */
#define W_FL_PAUSE       (1<<6)  /* pause here (wait for door to open, etc.) */
#define W_FL_HEALTH      (1<<7)  /* health kit (or wall mounted) location */
#define W_FL_ARMOR       (1<<8)  /* armor (or HEV) location */
#define W_FL_AMMO        (1<<9) /* ammo location */
#define W_FL_SNIPER      (1<<10) /* sniper waypoint (a good sniper spot) */
#define W_FL_FLAG        (1<<11) /* flag position (or hostage or president) */
#define W_FL_FLAG_GOAL   (1<<12) /* flag return position (or rescue zone) */
#define W_FL_VISIT       (1<<13) /* locations to visit every once in a while */
#define W_FL_AIMING      (1<<14) /* locations to look at while camping (additional to others)*/
#define W_FL_DONTAVOID   (1<<15) /**/
#define W_FL_RESET		 (1<<16) /**/

#define W_FL_DELETED     (1<<31) /* used by waypoint allocation code */


#define WAYPOINT_VERSION 1

// define the waypoint file header structure...
typedef struct {
	char filetype[8];  // should be "Joe_bot\0"
	int  waypoint_file_version;
	int  waypoint_file_flags;  // not currently used
	int  number_of_waypoints;
	char mapname[32];  // name of map for these waypoints
} WAYPOINT_HDR;


// define the structure for waypoints...
typedef struct {
	int    flags;    // button, lift, flag, health, ammo, etc.
	Vector origin;   // location
} WAYPOINT;

#define MAX_PATH_INDEX 4

// define the structure for waypoint paths (paths are connections between
// two waypoint nodes that indicates the bot can get from point A to point B.
// note that paths DON'T have to be two-way.  sometimes they are just one-way
// connections between two points.  There is an array called "paths" that
// contains head pointers to these structures for each waypoint index.
typedef struct path {
	short int index[MAX_PATH_INDEX];  // indexes of waypoints (index -1 means not used)
	struct path *next;   // link to next structure
} PATH;

typedef struct way{
	short int iIndices[MAX_WAYPOINTS];
	int iNum;
}WAY;

#define MAX_TIMES_PWP 4
// structure to contain statistical information
typedef struct WPStatitem{
	long lKill;							// number of kills
	long lKilled;						// number of bots being killed
	float fpKilledTime[MAX_TIMES_PWP];	// time of kills
	int lKilledT;
	float fpKillTime[MAX_TIMES_PWP];	// time of kills
	int lKillT;
	float fADEn;						// average distance to enemy (if 0 set to distance, else (nx+cdist)/(n+1))

	int iNVWp;							// number of visible waypoints
	float fADVWp;						// average distance to visible waypoints
	float fMaxDVWp;
	float fMinDVWp;
	float fAHDVWp;						// average height difference of visible waypoints to this waypoint

	long lTraffic[2];					// for the two teams each one variable
}WPStatITEM;

#define _FTNAME "JoeBOT StatFile\0"
#define _WPSVERSION 14

typedef struct wpsheader{
	char szFTName[20];
	char szMapname[40];		// store map name here. because at every global init of this structure the gpGlobals->mapname is the right stuff, this can be set while loading. because it's not clear which content this variable has when a new map has started, or let's say the var has the name of the new map, the save function just stores the data to the name defined in here
	long lVersion;
}WPSHEADER;

typedef struct WPStatdata{
	long lKill;
	long lKilled;
	WPStatITEM FItem[MAX_WAYPOINTS];
}WPStatDATA;

class CWPStat{
public:
	CWPStat();
	~CWPStat();

	int Load(char *);
	int GetVisible(long,long);
	//int Save(void);
	//int Search(int,int,int,float,float);
	//int SearchNearest(int,int,int,float);

	WPSHEADER wpsHeader;
	WPStatDATA d;

	long *pWPV;
	bool bRecalcWPV;
	long lNWPfWPV;	// number of waypoints for table pWPV
	long lWPSlice;
	long lPercent;		// just for messages

	WPStatITEM &operator [] (long);

	long lKillMax;
	long lKilledMax;
protected:
private:
};

extern bool g_waypoint_paths;		// have any paths been allocated?
extern bool g_waypoint_on;
extern bool g_auto_waypoint;
extern bool g_auto_addpath;
extern bool g_path_waypoint;

extern int num_waypoints;
extern WAYPOINT waypoints[MAX_WAYPOINTS];
extern PATH *paths[MAX_WAYPOINTS];
extern CWPStat WPStat;

bool WaypointLoad(const char *);

#endif