/********************************************************************
Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/* Scan codes to ASCII for unshifted keys; unused keys are left out */
char unshift[NSCAN] = { 
 0,033,'1','2','3','4','5','6',        '7','8','9','0','-','=','\b','\t',
 'q','w','e','r','t','y','u','i',      'o','p','[',']', '\r', 0,'a','s',
 'd','f','g','h','j','k','l',';',       0, 0, 0, 0,'z','x','c','v',
 'b','n','m',',','.','/', 0,'*',        0, ' '       
};

/* Scan codes to ASCII for shifted keys; unused keys are left out */
char shift[NSCAN] = {
 0,033,'!','@','#','$','%','^',        '&','*','(',')','_','+','\b','\t',
 'Q','W','E','R','T','Y','U','I',      'O','P','{','}', '\r', 0,'A','S',
 'D','F','G','H','J','K','L',':',       0,'~', 0,'|','Z','X','C','V',
 'B','N','M','<','>','?',0,'*',         0, ' '  
};


