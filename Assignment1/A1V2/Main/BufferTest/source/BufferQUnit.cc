
#ifndef CATALOG_UNIT_H
#define CATALOG_UNIT_H

#include "MyDB_BufferManager.h"
#include "MyDB_PageHandle.h"
#include "MyDB_Table.h"
#include "MyDB_page.h"
#include "QUnit.h"
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

// these functions write a bunch of characters to a string... used to produce data
void writeNums (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = '0' + (i % 10);
		//cout<<bytes[j]<<" ";
	}	
	bytes[len - 1] = 0;
}

void writeLetters (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = 'i' + (i % 10);
	}
	bytes[len - 1] = 0;
}

void writeSymbols (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = '!' + (i % 10);
	}
	bytes[len - 1] = 0;
}

int main () {

	QUnit::UnitTest qunit(cerr, QUnit::verbose);

	// UNIT TEST 1: A BIG ONE!!
	{

		// create a buffer manager 
		cout << "Buffer manager test starts:"<<endl;
        MyDB_BufferManager myMgr (8, 16, "tempDSFSD");
        
        // create a table whose name is "tempTable" and location is "foobar"
		MyDB_TablePtr table1 = make_shared <MyDB_Table> ("tempTable", "foobar");
		cout<<table1->getStorageLoc();
		// allocate a pinned page
		cout << "allocating pinned page\n";
		MyDB_PageHandle pinnedPage = myMgr.getPinnedPage (table1, 0);
		pinnedPage = myMgr.getPinnedPage ();
	
        cout << "I got the handle of page "<<endl;
		
        //write a few bytes to the page
		char *bytes = (char *) pinnedPage->getBytes();
		cout<<"i can access the content of the page: "<<endl;
		pinnedPage->show_content();

		writeNums (bytes, 8, 0);
		cout<<"i can write to the page's content!"<<endl;
		pinnedPage->show_content();
		cout<<""<<endl;

        //mark the page dirty
		pinnedPage->wroteBytes();
	
		// create a bunch of pinned pages and remember them
		MyDB_PageHandle test_17;

		vector <MyDB_PageHandle> myHandles;
		for (int i = 1; i <20; i++) {
			cout<<""<<endl;
			cout << "allocating pinned page\n";
			MyDB_PageHandle temp = myMgr.getPage (table1, i);
			char *bytes = (char *) temp->getBytes();

			writeNums (bytes, 8, i);
			temp->show_content();
			cout<<""<<endl;
			temp->wroteBytes ();
			myHandles.push_back (temp);

			// testing writing to unbuffered content
			if( i == 17)
			{
				test_17 = temp;
			}
			// testing writing to unbuffered content
		}

// testing writing to unbuffered content
cout<<""<<endl;
char* entrance_17 = (char*)test_17->getBytes();
writeNums (entrance_17, 8, 1);
test_17->wroteBytes ();
// testing writing to unbuffered content

		

		cout<<"about to quit"<<endl;
		cout<<""<<endl;
		return 0;
	}
	/*
		// now forget the pages we created
		vector <MyDB_PageHandle> temp;
		myHandles = temp;

		// now remember 8 more pages
		for (int i = 0; i < 8; i++) {
			cout << "allocating pinned page\n";
			MyDB_PageHandle temp = myMgr.getPinnedPage (table1, i);
			char *bytes = (char *) temp->getBytes ();

			// write numbers at the 0th position
			if (i == 0)
				writeNums (bytes, 64, i);
			else
				writeSymbols (bytes, 64, i);
			temp->wroteBytes ();
			myHandles.push_back (temp);
		}

		// now correctly write nums at the 0th position
		cout << "allocating unpinned page\n";
		MyDB_PageHandle anotherDude = myMgr.getPage (table1, 0);
		bytes = (char *) anotherDude->getBytes ();
		writeSymbols (bytes, 64, 0);
		anotherDude->wroteBytes ();
		
		// now do 90 more pages, for which we forget the handle immediately
		for (int i = 10; i < 100; i++) {
			cout << "allocating unpinned page\n";
			MyDB_PageHandle temp = myMgr.getPage (table1, i);
			char *bytes = (char *) temp->getBytes ();
			writeNums (bytes, 64, i);
			temp->wroteBytes ();
		}

		// now forget all of the pinned pages we were remembering
		vector <MyDB_PageHandle> temp2;
		myHandles = temp2;

		// now get a pair of pages and write them
		for (int i = 0; i < 100; i++) {
			cout << "allocating pinned page\n";
			MyDB_PageHandle oneHandle = myMgr.getPinnedPage ();
			char *bytes = (char *) oneHandle->getBytes ();
			writeNums (bytes, 64, i);
			oneHandle->wroteBytes ();
			cout << "allocating pinned page\n";
			MyDB_PageHandle twoHandle = myMgr.getPinnedPage ();
			writeNums (bytes, 64, i);
			twoHandle->wroteBytes ();
		}

		// make a second table
		MyDB_TablePtr table2 = make_shared <MyDB_Table> ("tempTable2", "barfoo");
		for (int i = 0; i < 100; i++) {
			cout << "allocating unpinned page\n";
			MyDB_PageHandle temp = myMgr.getPage (table2, i);
			char *bytes = (char *) temp->getBytes ();
			writeLetters (bytes, 64, i);
			temp->wroteBytes ();
		}
		
	}

	// UNIT TEST 2
	{
		MyDB_BufferManager myMgr (64, 16, "tempDSFSD");
		MyDB_TablePtr table1 = make_shared <MyDB_Table> ("tempTable", "foobar");

		// look up all of the pages, and make sure they have the correct numbers
		for (int i = 0; i < 100; i++) {
			MyDB_PageHandle temp = myMgr.getPage (table1, i);
			char answer[64];
			if (i < 8)
				writeSymbols (answer, 64, i);
			else
				writeNums (answer, 64, i);
			char *bytes = (char *) temp->getBytes ();
			QUNIT_IS_EQUAL (string (answer), string (bytes));
		}
	}
*/
}

#endif
