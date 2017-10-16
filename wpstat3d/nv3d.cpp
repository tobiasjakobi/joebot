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
#include <windows.h>		// Header File For Windows
#include <winuser.h>		// Header File For Windows
#include <math.h>
#include <time.h>
#include <stdio.h>			// Header File For Standard Input/Output
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include "CFrameTimer.h"
#include "CWPStuff.h"

#include "dialog.h"
#include "resource.h"

//#include "NNSim/NeuralNet.h"

#include "Cube.h"
#include "Line.h"

#include "NV3dfile.h"

#define _ZEILENAB -1

OPENFILENAME ofn;

extern WAYPOINT_HDR header;

const char szOverviewPath[80]="c:\\spiele\\half-life\\cstrike\\overviews\\";

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=FALSE;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool	alias = false;
bool	light =true;				// Lighting ON/OFF ( NEW )
bool	fog = true;
bool	lp;					// L Pressed? ( NEW )
bool	lpa;				// A Pressed? ( NEW )
bool	lpo;				// ...
bool	fp;					// F Pressed? ( NEW )
bool bDisplayConnections = true;
bool bDisplayLOS = true;
bool bDisplayOverview = true;
bool bDisplayNeurons = true;		// here the neurons are the waypoints :)
bool bDisplayOrigin = false;
bool bDisplayText = true;			// no aequivalent here
bool bDisplayDesc = true;
bool bDisplayFPS = true;
CFrameTimer CFTimer;

int iTOD = ID_DISPLAY_DEFAULT;

#define _LD 2
#define _SENSIMOUSE .2f

//CNeuralNetBProp NN;

CObject **Objects = 0;
long lObjects = 0;

float tX=0,tY=0,tZ=0;
float tXS=0,tYS=0,tZS=5;

GLfloat rD = .02f;

GLfloat	xrot =45;				// X Rotation
GLfloat	yrot  =-45;				// Y Rotation
GLfloat	xrots;				// X Rotation
GLfloat	yrots;				// Y Rotation
GLfloat xspeed;				// X Rotation Speed
GLfloat yspeed;				// Y Rotation Speed
GLfloat fOverviewZ = -.01f;
GLfloat fOverviewR = 0;
GLfloat fOZoom = 1.12f*2;
GLfloat fOOffset[3]={-.103f,1.011f,-.193f};
//GLfloat	z=-15.0f;			// Depth Into The Screen

GLfloat LightAmbient[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[]=		{ 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[]=	{ 0.0f, 0.0f, -20.0f, -10.0f };

GLuint	fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };	// Storage For Three Types Of Fog
GLuint	fogfilter = 1;								// Which Fog Mode To Use 
GLfloat	fogColor[4] = {.0f,.0f,.0f,1.0f};		// Fog Color

GLuint	filter;				// Which Filter To Use
//GLuint	texture[3];			// Storage For 3 Textures

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLuint	base;
GLYPHMETRICSFLOAT gmf[256];	// Storage For Information About Our Outline Font Characters

GLuint texture[1];

char szOverview[80];

GLbyte data[1024][1024][4];

AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
{
	FILE *File=NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File=fopen(Filename,"r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;										// If Load Failed Return NULL
}

int LoadGLTextures()									// Load Bitmaps And Convert To Textures
{
	int Status=FALSE;									// Status Indicator

	AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

	memset(TextureImage,0,sizeof(void *)*1);           	// Set The Pointer To NULL

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage[0]=LoadBMP(szOverview))
	{
		Status=TRUE;									// Set The Status To TRUE

		glGenTextures(1, &texture[0]);					// Create The Texture

		memset(data,0,sizeof(GLbyte)*1024*1024*4);
		//memcpy(data,TextureImage[0]->data,sizeof(GLbyte)*TextureImage[0]->sizeX*TextureImage[0]->sizeY*3);
		// remove fuckin' green
		long lx,ly;
		GLbyte *p;
		for(lx=0;lx < 1024;lx++){
			for(ly=0;ly < 1024;ly++){
				p = data[lx][ly];
				if(lx<768){
					p[0] = TextureImage[0]->data[3*1024*lx+3*ly];
					p[1] = TextureImage[0]->data[3*1024*lx+3*ly+1];
					p[2] = TextureImage[0]->data[3*1024*lx+3*ly+2];
				}
				p[3] = 255;
				if(p[0]==0
					&&p[2]==0
					&&p[1]<0){
					p[1] = 0;
					p[3] = 0;
				}
			}
		}

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		char szOverviewInfo[80];		// get the related .txt file
		char *szChange;
		strcpy(szOverviewInfo,szOverview);
		//MessageBox(0,szOverviewInfo,"",MB_OK);
		strlwr(szOverviewInfo);
		szChange=strstr(szOverviewInfo,".bmp");
		if(szChange){
			strcpy(szChange,".txt");
			char szFile[1024];
			FILE *fhd;
			fhd = fopen(szOverviewInfo,"r");
			if(!fhd)
				return false;
			fread(szFile,sizeof(char),1024,fhd);
			fclose(fhd);
			szChange = strstr(szFile,"ZOOM");
			sscanf(szChange,"ZOOM\t%f",&fOZoom);
			szChange = strstr(szFile,"ORIGIN");
			sscanf(szChange,"ORIGIN\t%f\t%f\t%f",&fOOffset[0],&fOOffset[1],&fOOffset[2]);
			fOOffset[0] /= 1000.f;
			fOOffset[1] /= 1000.f;
			fOOffset[2] /= 1000.f;
			szChange = strstr(szFile,"ROTATED");
			sscanf(szChange,"ROTATED\t%f",&fOverviewR);
			fOverviewR *= 90;
			szChange = strstr(szFile,"HEIGHT");
			sscanf(szChange,"HEIGHT\t%f",&fOverviewZ);
			fOverviewZ /= 1000.f;

			sprintf(szFile,"%f - %f %f %f - %f - %f",fOZoom,fOOffset[0],fOOffset[1],fOOffset[2],fOverviewR,fOverviewZ);
			SetWindowText(hWnd,szFile);
		}
	}

	if (TextureImage[0])									// If Texture Exists
	{
		if (TextureImage[0]->data)							// If Texture Image Exists
		{
			free(TextureImage[0]->data);					// Free The Texture Image Memory
		}

		free(TextureImage[0]);								// Free The Image Structure
	}

	return Status;										// Return The Status
}

void CheckOverview(void){
	char szOverviewTest[256];
	strcpy(szOverviewTest,szOverviewPath);
	strcat(szOverviewTest,header.mapname);
	strcat(szOverviewTest,".bmp");

	strcpy(szOverview,szOverviewTest);

	LoadGLTextures();
}

GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	
	base = glGenLists(256);								// Storage For 256 Characters
	
	font = CreateFont(	-12,							// Height Of Font
		0,								// Width Of Font
		0,								// Angle Of Escapement
		0,								// Orientation Angle
		FW_NORMAL,						// Font Weight
		FALSE,							// Italic
		FALSE,							// Underline
		FALSE,							// Strikeout
		ANSI_CHARSET,					// Character Set Identifier
		OUT_TT_PRECIS,					// Output Precision
		CLIP_DEFAULT_PRECIS,			// Clipping Precision
		ANTIALIASED_QUALITY,			// Output Quality
		FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
		//"Comic Sans MS");				// Font Name
		//"Times New Roman");				// Font Name
		"Tahoma");				// Font Name
	
	SelectObject(hDC, font);							// Selects The Font We Created
	
	wglUseFontOutlines(	hDC,							// Select The Current DC
		0,								// Starting Character
		255,							// Number Of Display Lists To Build
		base,							// Starting Display Lists
		1.f,							// Deviation From The True Outlines
		0.05f,							// Font Thickness In The Z Direction
		WGL_FONT_POLYGONS,				// Use Polygons, Not Lines
		gmf);							// Address Of Buffer To Recieve Data
}

GLvoid KillFont(GLvoid)									// Delete The Font
{
	glDeleteLists(base, 256);								// Delete All 256 Characters
}

GLvoid glPrint(bool bMid,const char *fmt, ...)					// Custom GL "Print" Routine
{
	glPushMatrix();
	float		length=0;								// Used To Find The Length Of The Text
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments
	
	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing
	
	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text
	
	for (unsigned int loop=0;loop<(strlen(text));loop++)	// Loop To Find Text Length
	{
		length+=gmf[text[loop]].gmfCellIncX;			// Increase Length By Each Characters Width
	}
	
	if(bMid)glTranslatef(-length/2,0.0f,0.0f);					// Center Our Text On The Screen
	
	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base);									// Sets The Base Character to 0
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
	glPopMatrix();
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}
	
	glViewport(0,0,width,height);						// Reset The Current Viewport
	
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	
	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
/*if (!LoadGLTextures())								// Jump To Texture Loading Routine
{
return FALSE;									// If Texture Didn't Load Return FALSE
}*/
	
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	CFTimer.Start(60);

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Position The Light
	glEnable(GL_LIGHT1);								// Enable Light One
	
	glFogi(GL_FOG_MODE, fogMode[fogfilter]);			// Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);					// Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.06f);						// How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);					// Fog Hint Value
	glFogf(GL_FOG_START, 30.0f);							// Fog Start Depth
	glFogf(GL_FOG_END, 80.0f);							// Fog End Depth
	glEnable(GL_FOG);									// Enables GL_FOG
	
	BuildFont();
	
	return TRUE;										// Initialization Went OK
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	long lschl;
	CFTimer.Frame();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

	float fText[3] = {.9f,.9f,.9f};
	float faText[3] = {.4f,.4f,.4f};
	
	// show texts
	if(bDisplayFPS||bDisplayText){
		glDisable(GL_FOG);
		if(bDisplayFPS){
			static float ffps = CFTimer.GetFramesPerSec(),frx=0,fry=0,fs=0,r,g,b;
			static long ltime = time(NULL);
			if(ltime - time(NULL)){
				ltime = time(NULL);
				ffps = CFTimer.GetFramesPerSec();
			}
			glLoadIdentity();									// Reset The View
			
			glTranslatef(5.0f,3.5f,-10.f);

			r = ffps < 75.f ? 1-ffps/75.f:0;
			g = ffps > 0.f ? (ffps)/75.0f:0;
			g>1?g=1:g=g;
			b =1.f-r-g;
			//glColor3f(r,g,b);

			float f[3] = {r,g,b};
			float fa[3] = {r/2.f,g/2.f,b/2.f};

			glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,f);
			glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,f);
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,fa);
			
			glScalef(.3f,.3f,.3f);
			
			glPrint(true,"%.2f fps (%li)",ffps,lObjects);
		}
		if(fog)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);
	}
	//return true;
	glLoadIdentity();									// Reset The View
	glTranslatef(-tXS,-tYS,-tZS);
	
	glRotatef(xrot,1.0f,0.0f,0.0f);
	glRotatef(yrot,0.0f,1.0f,0.0f);
	
	glTranslatef(-tX,-tY,-tZ);
	
	//	glBindTexture(GL_TEXTURE_2D, texture[filter]);
	
	
	for(lschl =0 ;lschl < lObjects; lschl ++){
		Objects[lschl]->Draw();
	}

	if(bDisplayOverview){	
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		//glTranslatef(fOOffset[0]-2.15f/fOZoom,fOOffset[1],fOOffset[2]);
		glTranslatef(fOOffset[0],fOOffset[1],fOOffset[2]);
		glScalef(1.f/fOZoom,1.f/fOZoom,1.f/fOZoom);
		glRotatef(fOverviewR,0.0f,0.0f,1.0f);
		glTranslatef(256.f/1024.f*4.096f,0,0);
		glBegin(GL_QUADS);
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glColor4f(1,1,1,1);

		float f[3] = {1,1,1};
		float fh[3] = {1,1,1};
		
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,f);
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,fh);
		glTexCoord2f(1,0);	glVertex3f(-4.096f,-4.096f,fOverviewZ);
		glTexCoord2f(0,0);	glVertex3f(-4.096f,4.096f,fOverviewZ);
		glTexCoord2f(0,1);	glVertex3f(4.096f,4.096f,fOverviewZ);
		glTexCoord2f(1,1);	glVertex3f(4.096f,-4.096f,fOverviewZ);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	xrot+=xspeed * CFTimer.GetSecsPerFrame();
	yrot+=yspeed * CFTimer.GetSecsPerFrame();
	return TRUE;										// Keep Going
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}
	
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		
		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}
	
	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}
	
	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}
	
	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
*	title			- Title To Appear At The Top Of The Window				*
*	width			- Width Of The GL Window Or Fullscreen Mode				*
*	height			- Height Of The GL Window Or Fullscreen Mode			*
*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag,char *szCmdLine)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height
	
	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag
	
	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= "IDR_MENU1";									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name
	
	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		
		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","LSNNSIVI3D GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}
	
	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}
	
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size
	
	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								szCmdLine)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			bits,										// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen
	
	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	return TRUE;									// Success
}
static HMENU hMenu;
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
						 UINT	uMsg,			// Message For This Window
						 WPARAM	wParam,			// Additional Message Information
						 LPARAM	lParam)			// Additional Message Information
{
	static bool lbdown = false;
	static bool lbdownctrl = false;
	static bool lbdownshift = false;
	static bool rbdown = false;
	static long lxpos,lypos;
	static long lxposrd,lyposrd;
	static float fxrotrd,fyrotrd,fzrd;
	static long lxposld,lyposld;
	static float tXld,tYld;
	static float tXSld,tYSld;
	static float fxrotld,fyrotld,fzld;
	static char	szFileName[_MAX_PATH]="",
		szTitleName[_MAX_FNAME+_MAX_EXT]="",
		szBuffer[200];
	static char	szFileNameS[_MAX_PATH]="",
		szTitleNameS[_MAX_FNAME+_MAX_EXT]="";
	static char	szFileNameO[_MAX_PATH]="",
		szTitleNameO[_MAX_FNAME+_MAX_EXT]="";
	
	switch (uMsg)									// Check For Windows Messages
	{
	case WM_CREATE:
		hMenu = GetMenu(hWnd);
		FileInitialize(hWnd);
		
		lstrcpy (szFileName, (PSTR)
			(((LPCREATESTRUCT) lParam)->lpCreateParams)) ;
		//MessageBox(hWnd,szFileName,"",MB_OK);
		if (strlen (szFileName) > 0)
		{
			GetFileTitle (szFileName, szTitleName, sizeof (szTitleName)) ;
			
			szFileName[strlen(szFileName)-1]='\0';
		}
		else{
			WaypointLoad("de_dust.wpj");
			CheckOverview();
		}

		SetWindowText(hWnd,szBuffer);
		
		InitObjects();
		break;
	case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}
			
			return 0;								// Return To The Message Loop
		}
	case WM_COMMAND:
		hMenu = GetMenu(hWnd);
		
		switch(LOWORD(wParam))
		{
		case ID_FILE_QUIT:
			PostQuitMessage(0);
			break;
		case ID_FILE_LOAD:
			*szFileName=0;
			FileOpenDlg(hWnd,szFileName,szTitleName);
			
			if(strlen(szFileName))
			{
				//SetWindowText(hwndWait,buffer);
				DelObjects();
				WaypointLoad(szFileName);
				CheckOverview();

				strcpy(szFileNameS,szFileName);

				char *szTmp = szFileNameS;
				while(*szTmp)szTmp++;

				while(szTmp>szFileNameS&&*szTmp!='\\')szTmp--;
				strcpy(szTmp,"\\routes\\");
				strcat(szTmp,header.mapname);
				strcat(szTmp,".wst");
				WPStat.Load(szFileNameS);

				InitObjects();
				
				//DestroyWindow(hwndWait);
			}
			break;

		case ID_FILE_LOADSTATS:
			*szFileName=0;
			FileOpenDlg(hWnd,szFileNameS,szTitleNameS);
			
			if(strlen(szFileNameS))
			{		
				//SetWindowText(hwndWait,buffer);
				DelObjects();
				WPStat.Load(szFileNameS);
				InitObjects();
				
				//DestroyWindow(hwndWait);
			}
			break;

		case ID_FILE_LOADOVERVIEW:
			*szFileNameO=0;
			FileOpenDlg(hWnd,szFileNameO,szTitleNameO);
			
			if(strlen(szFileNameO))
			{		
				//SetWindowText(hwndWait,buffer);
				strcpy(szOverview,szFileNameO);
				if(!LoadGLTextures())
					MessageBeep(1);
				
				//DestroyWindow(hwndWait);
			}
			break;
			
		case ID_FILE_SAVEFILE:
			*szFileName=0;
			*szTitleName=0;
			FileSaveDlg(hWnd,szFileName,szTitleName);
			if(strlen(szFileName)){
				//NN.SaveFile(szFileName);
			}
			break;

		/*case ID__ABOUT:
			DialogBox(hInstance,"ABOUT",hWnd,DLGPROC(AboutDlgProc));
			return 0;*/
			
		case ID_VIEW_SHOWHIDECONNECTIONS:
			bDisplayConnections = !bDisplayConnections;
			if(!bDisplayConnections){
				CheckMenuItem(hMenu,ID_VIEW_SHOWHIDECONNECTIONS,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_VIEW_SHOWHIDECONNECTIONS,MF_CHECKED);
			DelObjects();
			InitObjects();
			return 0;

		case ID_VIEW_LOS:
			bDisplayLOS = !bDisplayLOS;
			if(!bDisplayLOS){
				CheckMenuItem(hMenu,ID_VIEW_LOS,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_VIEW_LOS,MF_CHECKED);
			DelObjects();
			InitObjects();
			return 0;

		case ID_VIEW_OVERVIEW:
			bDisplayOverview = !bDisplayOverview;
			if(!bDisplayOverview){
				CheckMenuItem(hMenu,ID_VIEW_OVERVIEW,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_VIEW_OVERVIEW,MF_CHECKED);
			return 0;

		case ID_VIEW_SHOWHIDENEURONS:
			bDisplayNeurons = !bDisplayNeurons;
			if(!bDisplayNeurons){
				CheckMenuItem(hMenu,ID_VIEW_SHOWHIDENEURONS,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_VIEW_SHOWHIDENEURONS,MF_CHECKED);
			DelObjects();
			InitObjects();
			return 0;
		case ID_VIEW_DESC:
			bDisplayDesc= !bDisplayDesc;
			if(!bDisplayDesc){
				CheckMenuItem(hMenu,ID_VIEW_DESC,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_VIEW_DESC,MF_CHECKED);
			return 0;
		case ID_VIEW_FPS:
			bDisplayFPS= !bDisplayFPS;
			if(!bDisplayFPS){
				CheckMenuItem(hMenu,ID_VIEW_FPS,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_VIEW_FPS,MF_CHECKED);
			return 0;
		case ID_VIEW_ORIGIN:
			bDisplayOrigin = !bDisplayOrigin;
			if(!bDisplayOrigin){
				CheckMenuItem(hMenu,ID_VIEW_ORIGIN,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_VIEW_ORIGIN,MF_CHECKED);
			DelObjects();
			InitObjects();
			return 0;
		case ID_VIEW_TEXT:
			bDisplayText = !bDisplayText;
			if(!bDisplayText){
				CheckMenuItem(hMenu,ID_VIEW_TEXT,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_VIEW_TEXT,MF_CHECKED);
			return 0;
		case ID_VIEW_DEFAULT:
			tX=0;tY=0;tZ=0;
			tXS=0;tYS=0;tZS=15;
			
			xrot=45;
			yrot=-45;
			xspeed=0;
			yspeed=0;
			return 0;
		case ID_RENDER_ANTIALIASING:
			alias=!alias;
			
			if(alias)
				glEnable(GL_LINE_SMOOTH);
			else
				glDisable(GL_LINE_SMOOTH);
			if(!alias){
				CheckMenuItem(hMenu,ID_RENDER_ANTIALIASING,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_RENDER_ANTIALIASING,MF_CHECKED);
			return 0;
			
		case ID_RENDER_FOG:
			fog =! fog;
			
			if(fog)
				glEnable(GL_FOG);
			else
				glDisable(GL_FOG);
			if(!fog){
				CheckMenuItem(hMenu,ID_RENDER_FOG,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_RENDER_FOG,MF_CHECKED);
			return 0;
			
		case ID_RENDER_LIGHT:
			light=!light;
			if (!light)
			{
				glDisable(GL_LIGHTING);
			}
			else
			{
				glEnable(GL_LIGHTING);
			}
			if(!light){
				CheckMenuItem(hMenu,ID_RENDER_LIGHT,MF_UNCHECKED);
			}
			else
				CheckMenuItem(hMenu,ID_RENDER_LIGHT,MF_CHECKED);
			return 0;

		case ID_DISPLAY_DEFAULT:
			CheckMenuItem(hMenu,iTOD,MF_UNCHECKED);
			iTOD = LOWORD(wParam);
			CheckMenuItem(hMenu,iTOD,MF_CHECKED);
			DelObjects();
			InitObjects();
			break;
			
		case ID_DISPLAY_KILL:
			CheckMenuItem(hMenu,iTOD,MF_UNCHECKED);
			iTOD = LOWORD(wParam);
			CheckMenuItem(hMenu,iTOD,MF_CHECKED);
			DelObjects();
			InitObjects();
			break;
			
		case ID_DISPLAY_KILLED:
			CheckMenuItem(hMenu,iTOD,MF_UNCHECKED);
			iTOD = LOWORD(wParam);
			CheckMenuItem(hMenu,iTOD,MF_CHECKED);
			DelObjects();
			InitObjects();
			break;
			
		case ID_DISPLAY_TIME:
			CheckMenuItem(hMenu,iTOD,MF_UNCHECKED);
			iTOD = LOWORD(wParam);
			CheckMenuItem(hMenu,iTOD,MF_CHECKED);
			DelObjects();
			InitObjects();
			break;
			
		case ID_DISPLAY_DISTANCE:
			CheckMenuItem(hMenu,iTOD,MF_UNCHECKED);
			iTOD = LOWORD(wParam);
			CheckMenuItem(hMenu,iTOD,MF_CHECKED);
			DelObjects();
			InitObjects();
			break;

		case ID_DISPLAY_ADVWP:
			CheckMenuItem(hMenu,iTOD,MF_UNCHECKED);
			iTOD = LOWORD(wParam);
			CheckMenuItem(hMenu,iTOD,MF_CHECKED);
			DelObjects();
			InitObjects();
			break;

		case ID_DISPLAY_AHDVWP:
			CheckMenuItem(hMenu,iTOD,MF_UNCHECKED);
			iTOD = LOWORD(wParam);
			CheckMenuItem(hMenu,iTOD,MF_CHECKED);
			DelObjects();
			InitObjects();
			break;
		};
		break;
		
		case WM_SYSCOMMAND:							// Intercept System Commands
			{
				switch (wParam)							// Check System Calls
				{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					return 0;							// Prevent From Happening
				}
				break;									// Exit
			}
		case WM_LBUTTONDOWN:
			xspeed = 0;
			yspeed = 0;
			if(keys[VK_CONTROL]){
				lbdownctrl = true;
			}
			if(keys[VK_SHIFT]){
				lbdownshift = true;
			}
			else
				lbdown = true;
			
			lxposld = lxpos;
			lyposld = lypos;
			fxrotld = xrot;
			fyrotld = yrot;
			tYSld = tYS;
			tXSld = tXS;
			fzld = tZS;
			tYld = tY;
			tXld = tX;
			
			break;
		case WM_LBUTTONUP:
			lbdownshift = false;
			lbdownctrl = false;
			lbdown = false;
			break;
			
		case WM_RBUTTONDOWN:
			//xspeed = 0;
			//yspeed = 0;
			rbdown = true;
			
			lxposrd = lxpos;
			lyposrd = lypos;
			fxrotrd = xrot;
			fyrotrd = yrot;
			fzrd = tZS;
			
			break;
		case WM_RBUTTONUP:
			rbdown = false;
			break;

		case 0x020A:
			if(short(HIWORD(wParam)) > 0){
				tZS += 1;
			}
			else
				tZS -= 1;
			break;
			
		case WM_MOUSEMOVE:
			lxpos = LOWORD(lParam);  // horizontal position of cursor 
			lypos = HIWORD(lParam);  // vertical position of cursor 
			if(lbdown){
				yrot = fyrotld + (lxpos - lxposld) * _SENSIMOUSE;
				xrot = fxrotld + (lypos - lyposld) * _SENSIMOUSE;
			}
			if(lbdownctrl){
				tYS = tYSld + (lypos - lyposld) * _SENSIMOUSE * .1f;
				tXS = tXSld - (lxpos - lxposld) * _SENSIMOUSE * .1f;
			}
			if(lbdownshift){
				tY = tYld + (lypos - lyposld) * _SENSIMOUSE * .1f;
				tX = tXld - (lxpos - lxposld) * _SENSIMOUSE * .1f;
			}
			if(rbdown){
				tZS = fzrd + (lypos - lyposrd) * 1.0f * _SENSIMOUSE;
			}
			break;
			
		case WM_CLOSE:								// Did We Receive A Close Message?
			{
				PostQuitMessage(0);						// Send A Quit Message
				return 0;								// Jump Back
			}
			
		case WM_KEYDOWN:							// Is A Key Being Held Down?
			{
				keys[wParam] = TRUE;					// If So, Mark It As TRUE
				return 0;								// Jump Back
			}
			
		case WM_KEYUP:								// Has A Key Been Released?
			{
				keys[wParam] = FALSE;					// If So, Mark It As FALSE
				return 0;								// Jump Back
			}
			
		case WM_SIZE:								// Resize The OpenGL Window
			{
				ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
				return 0;								// Jump Back
			}
	}
	
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void DelObjects(void){
	long lschl;
	if(Objects){
		for(lschl =0 ;lschl < lObjects;lschl ++){
			delete Objects[lschl];
		}
		delete [] Objects;
	}
	Objects = 0;
	lObjects = 0;
}

void InitObjects(void){
	Objects = new CObject *[1000000];	// just a lot of objects - why do we got virtual mem ? ;)
	lObjects = 0;
	
//	long lN,lL,lNO,lLO,lNumoW=0,lC;
	
	if(bDisplayOrigin){
		Objects[lObjects] = new CLine;
		((CLine *)Objects[lObjects])->x1 = 0;
		((CLine *)Objects[lObjects])->y1 = 0;
		((CLine *)Objects[lObjects])->z1 = 0;
		
		((CLine *)Objects[lObjects])->x2 = 100;
		((CLine *)Objects[lObjects])->y2 = 0;
		((CLine *)Objects[lObjects])->z2 = 0;
		
		((CLine *)Objects[lObjects])->r = 1;
		((CLine *)Objects[lObjects])->g = 0;
		((CLine *)Objects[lObjects])->b = 0;
		
		lObjects ++;
		
		Objects[lObjects] = new CLine;
		((CLine *)Objects[lObjects])->x1 = 0;
		((CLine *)Objects[lObjects])->y1 = 0;
		((CLine *)Objects[lObjects])->z1 = 0;
		
		((CLine *)Objects[lObjects])->x2 = 0;
		((CLine *)Objects[lObjects])->y2 = 100;
		((CLine *)Objects[lObjects])->z2 = 0;
		
		((CLine *)Objects[lObjects])->r = 0;
		((CLine *)Objects[lObjects])->g = 1;
		((CLine *)Objects[lObjects])->b = 0;
		
		lObjects ++;
		Objects[lObjects] = new CLine;
		((CLine *)Objects[lObjects])->x1 = 0;
		((CLine *)Objects[lObjects])->y1 = 0;
		((CLine *)Objects[lObjects])->z1 = 0;
		
		((CLine *)Objects[lObjects])->x2 = 0;
		((CLine *)Objects[lObjects])->y2 = 0;
		((CLine *)Objects[lObjects])->z2 = 100;
		
		((CLine *)Objects[lObjects])->r = 0;
		((CLine *)Objects[lObjects])->g = 0;
		((CLine *)Objects[lObjects])->b = 1;
		
		lObjects ++;
	}
	long lschl,lschl1;
	for(lschl = 0;lschl < num_waypoints; lschl ++){
		if ((waypoints[lschl].flags & W_FL_DELETED) == W_FL_DELETED)
			continue;  // skip any deleted waypoints
		if(bDisplayNeurons){
			Objects[lObjects] = new CCube;
			((CCube *)Objects[lObjects])->x = waypoints[lschl].origin.x/1000;
			((CCube *)Objects[lObjects])->y = waypoints[lschl].origin.y/1000;
			((CCube *)Objects[lObjects])->z = waypoints[lschl].origin.z/1000;
			
			((CCube *)Objects[lObjects])->rx = rD;
			((CCube *)Objects[lObjects])->ry = rD;
			((CCube *)Objects[lObjects])->rz = rD;
			switch(iTOD){
			case ID_DISPLAY_KILL:
				((CCube *)Objects[lObjects])->r = float(WPStat.d.FItem[lschl].lKill)/float(WPStat.lKillMax) ;
				((CCube *)Objects[lObjects])->g = 0;
				((CCube *)Objects[lObjects])->b = 0;
				break;
			case ID_DISPLAY_KILLED:
				((CCube *)Objects[lObjects])->r = float(WPStat.d.FItem[lschl].lKilled)/float(WPStat.lKilledMax) ;
				((CCube *)Objects[lObjects])->g = 0;
				((CCube *)Objects[lObjects])->b = 0;
				break;
			case ID_DISPLAY_DISTANCE:
				((CCube *)Objects[lObjects])->r = float(WPStat.d.FItem[lschl].fADEn)/5000.0f;
				((CCube *)Objects[lObjects])->g = float(WPStat.d.FItem[lschl].fADEn)/2500.0f;
				((CCube *)Objects[lObjects])->b = 0;
				if(((CCube *)Objects[lObjects])->r>1)
					((CCube *)Objects[lObjects])->r = 1;
				if(((CCube *)Objects[lObjects])->g>1)
					((CCube *)Objects[lObjects])->g = 1;
				if(((CCube *)Objects[lObjects])->b>1)
					((CCube *)Objects[lObjects])->b = 1;
				break;
			case ID_DISPLAY_TIME:{
				float d = 0;
				int iCount = 0;
				for(int ischl=0;ischl < 4;ischl ++){
					if(int(WPStat.d.FItem[lschl].fpKillTime[ischl]) != 0){
						d+=WPStat.d.FItem[lschl].fpKillTime[ischl];
						iCount ++;
					}
				}
				if(iCount != 0)
					d/=float(iCount);
				((CCube *)Objects[lObjects])->r = 1.0f-float(d)/200.0f ;
				if(d == 0){
					((CCube *)Objects[lObjects])->g = 1;
				}
				else
					((CCube *)Objects[lObjects])->g = 0;
				((CCube *)Objects[lObjects])->b = 0;
				break;
								 }
			case ID_DISPLAY_ADVWP:{
				((CCube *)Objects[lObjects])->r = WPStat.d.FItem[lschl].fADVWp/2000.f;
				((CCube *)Objects[lObjects])->g = 0;
				((CCube *)Objects[lObjects])->b = 0;
								  }
				break;
			case ID_DISPLAY_AHDVWP:{
				((CCube *)Objects[lObjects])->r = WPStat.d.FItem[lschl].fAHDVWp/200.f;
				((CCube *)Objects[lObjects])->g = 0;
				((CCube *)Objects[lObjects])->b = 0;
								   }
				break;
			default :
				((CCube *)Objects[lObjects])->r = 1;
				((CCube *)Objects[lObjects])->g = 0;
				((CCube *)Objects[lObjects])->b = 0;
			}
			
			lObjects++;
		}
		
		if (paths[lschl] && bDisplayConnections)
		{
			PATH *p = paths[lschl];
			PATH *p_next;
			
			while (p)
			{
				int i;
				for(i = 0; i < MAX_PATH_INDEX; i++){
					if(p->index[i] != -1){
						Objects[lObjects] = new CLine;
						((CLine *)Objects[lObjects])->x1 = waypoints[lschl].origin.x/1000;
						((CLine *)Objects[lObjects])->y1 = waypoints[lschl].origin.y/1000;
						((CLine *)Objects[lObjects])->z1 = waypoints[lschl].origin.z/1000;
						
						((CLine *)Objects[lObjects])->x2 = waypoints[p->index[i]].origin.x/1000;
						((CLine *)Objects[lObjects])->y2 = waypoints[p->index[i]].origin.y/1000;
						((CLine *)Objects[lObjects])->z2 = waypoints[p->index[i]].origin.z/1000;
						
						((CLine *)Objects[lObjects])->r = .5;
						((CLine *)Objects[lObjects])->g = .5;
						((CLine *)Objects[lObjects])->b = .5;
						
						lObjects ++;
					}
				}
				p_next = p->next;  // save the link to next
				p = p_next;
			}
		}
		
		if(bDisplayLOS){
			for(lschl1 = lschl;lschl1 < num_waypoints; lschl1 ++){
				if ((waypoints[lschl1].flags & W_FL_DELETED) == W_FL_DELETED)
					continue;  // skip any deleted waypoints
				if(WPStat.GetVisible(lschl,lschl1)){
					Objects[lObjects] = new CLine;
					((CLine *)Objects[lObjects])->x1 = waypoints[lschl].origin.x/1000;
					((CLine *)Objects[lObjects])->y1 = waypoints[lschl].origin.y/1000;
					((CLine *)Objects[lObjects])->z1 = waypoints[lschl].origin.z/1000;
					
					((CLine *)Objects[lObjects])->x2 = waypoints[lschl1].origin.x/1000;
					((CLine *)Objects[lObjects])->y2 = waypoints[lschl1].origin.y/1000;
					((CLine *)Objects[lObjects])->z2 = waypoints[lschl1].origin.z/1000;
					
					((CLine *)Objects[lObjects])->r = .0;
					((CLine *)Objects[lObjects])->g = .0;
					((CLine *)Objects[lObjects])->b = 1;
					
					lObjects ++;
				}
			}
		}
	}
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
				   HINSTANCE	hPrevInstance,		// Previous Instance
				   LPSTR		lpCmdLine,			// Command Line Parameters
				   int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop
	
	// Ask The User Which Screen Mode They Prefer
	fullscreen=FALSE;							// Windowed Mode
	
	// Create Our OpenGL Window
	if (!CreateGLWindow("Johannes' NNVISU3D",640,480,16,fullscreen,lpCmdLine))
	{
		return 0;									// Quit If Window Was Not Created
	}
	
	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			}
			else									// Not Time To Quit, Update Screen
			{
				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
				if (keys['L'] && !lp)
				{
					if(!light){
						CheckMenuItem(hMenu,ID_RENDER_LIGHT,MF_UNCHECKED);
					}
					else
						CheckMenuItem(hMenu,ID_RENDER_LIGHT,MF_CHECKED);
					lp=TRUE;
					light=!light;
					if (!light)
					{
						glDisable(GL_LIGHTING);
					}
					else
					{
						glEnable(GL_LIGHTING);
					}
				}
				if (keys['O'] && !lpo)
				{
					lpo=TRUE;
					fog =! fog;
					
					if(fog)
						glEnable(GL_FOG);
					else
						glDisable(GL_FOG);
					
					if(!fog){
						CheckMenuItem(hMenu,ID_RENDER_FOG,MF_UNCHECKED);
					}
					else
						CheckMenuItem(hMenu,ID_RENDER_FOG,MF_CHECKED);
				}
				if (!keys['O'])
				{
					lpo=FALSE;
				}
				if (keys['A'] && !lpa)
				{
					lpa=TRUE;
					alias=!alias;
					
					if(alias)
						glEnable(GL_LINE_SMOOTH);
					else
						glDisable(GL_LINE_SMOOTH);
					
					if(!alias){
						CheckMenuItem(hMenu,ID_RENDER_ANTIALIASING,MF_UNCHECKED);
					}
					else
						CheckMenuItem(hMenu,ID_RENDER_ANTIALIASING,MF_CHECKED);
				}
				if (!keys['A'])
				{
					lpa=FALSE;
				}
				if (!keys['L'])
				{
					lp=FALSE;
				}
				if (keys['F'] && !fp)
				{
					fp=TRUE;
					filter+=1;
					if (filter>2)
					{
						filter=0;
					}
				}
				if (!keys['F'])
				{
					fp=FALSE;
				}
				if (keys[VK_PRIOR])
				{
					tZS-=0.2f;
				}
				if (keys[VK_NEXT])
				{
					tZS+=0.2f;
				}
				if (keys['Y'])
				{
					fOverviewZ-=0.02f;
				}
				if (keys['X'])
				{
					fOverviewZ-=0.002f;
				}
				if (keys['C'])
				{
					fOverviewZ+=0.002f;
				}
				if (keys['V'])
				{
					fOverviewZ+=0.02f;
				}
				if (keys[VK_UP])
				{
					xspeed-=1.f;
				}
				if (keys[VK_DOWN])
				{
					xspeed+=1.f;
				}
				if (keys[VK_RIGHT])
				{
					yspeed+=1.f;
				}
				if (keys[VK_LEFT])
				{
					yspeed-=1.f;
				}
				
				if (keys[VK_F1])						// Is F1 Being Pressed?
				{
					keys[VK_F1]=FALSE;					// If So Make Key FALSE
					KillGLWindow();						// Kill Our Current Window
					fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
					// Recreate Our OpenGL Window
					if (!CreateGLWindow("Johannes' LSNNVISU3D",1024,768,32,fullscreen,""))
					{
						return 0;						// Quit If Window Was Not Created
					}
				}
			}
		}
	}
	
	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}
