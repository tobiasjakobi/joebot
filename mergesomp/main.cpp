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
#include "SOM/SOM.h"

long lNum=0;
char files[10][100]={
	"combat cs_delta_assault 20011208.spt",
	"combat mixed mean 20011209.spt",
	"\0"
};
SOMPattern SPE(6),SPT(6);

void main(void){
	SPE.SetMaxPatternNum(1000000);
	while(*files[lNum]){
		cout << "processing ... " << files[lNum] << endl;
		SPT.Load(files[lNum]);
		SPE.AddPattern(SPT);
		lNum++;
	}
	cout << "Saving merged data" << endl;
	SPE.Save("combat.spt");
	cout << SPE.GetNumP()<<endl;
}