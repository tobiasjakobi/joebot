Compile Requirements:

	- Linux: GCC 2.95
	- Windows: Borland C++ 5.5.1, MinGW 3.1.0 or MSVC6
	- Half-Life SDK 2.3 patched
	- Half-Life SDK 2.3 unofficial update
	- Metmod SDK 1.17

	The patched half-life and metamod sdk can be found at metamod.org. Browse
	the "all files" area (http://metamod.org/files/). The unofficial update to
	the half-life sdk can be found at realbot.bots-united.com in the downloads
	area.

	
Compiling with GCC or MinGW:

	Edit Config.mk and change the paths at the top of the file to point to
	your half-life and metamod sdk, and the path you installed the jobebot
	source. Now just type:
	
		make
		- or -
		make release
		
	By default, this will make a standard and metamod plugin DLL.


Compiling with Borland C++ 5.5.1:

	Edit Makefile.bcc and change the paths at the top of the file to point to
	your half-life and metamod sdk, and the path you installed the joebot
	source. You also need to change the path to the Borland compiler. Now just
	type:

		c:\path\to\borland\make.exe -f Makefile.bcc
		- or -
		c:\path\to\borland\make.exe -f Makefile.bcc release

	By default, this will make a standard and metamod plugin DLL.

Compiling with MSVC6

	To configure compilation as a metamod DLL, you must #define USE_METAMOD.
	Otherwise a standard DLL will be compiled.
