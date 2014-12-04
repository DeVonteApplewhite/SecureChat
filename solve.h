//c4col.h
//main header file used to hold the columns class of connect 4

#ifndef C4COL_H
#define C4COL_H

// note preprocessing directive wrapper above

#include <iostream>
using namespace std;

class human
{
	public:
		human();	//constructor
		unsigned int choosePaint();
	
		unsigned int chosenPaint;
		unsigned int partnersPaint;
		unsigned int startingPaint;
		unsigned int sharedPaint;
};

#endif
