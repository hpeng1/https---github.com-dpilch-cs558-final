

/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A1.  You should not be looking at this file     **
** unless you have completed A1!                   **
****************************************************/


#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include <fcntl.h>
#include <iostream>
#include "MyDB_BufferManager.h"
#include "MyDB_Page.h"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <utility>

using namespace std;

size_t MyDB_BufferManager :: getPageSize () {
	return pageSize;
}

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
		
	// open the file, if it is not open
	if (fds.count (whichTable) == 0) {
		int fd = open (whichTable->getStorageLoc ().c_str (), O_CREAT | O_RDWR, 0666);
		fds[whichTable] = fd;
	}

	// make sure we don't have a null table
	if (whichTable == nullptr) {
		cout << "Can't allocate a page with a null table!!\n";
		exit (1);
	}
	
	// next, see if the page is already in existence
	pair <MyDB_TablePtr, long> whichPage = make_pair (whichTable, i);
	if (allPages.count (whichPage) == 0) {

		// it is not there, so create a page
		MyDB_PagePtr returnVal = make_shared <MyDB_Page> (whichTable, i, *this);
		allPages [whichPage] = returnVal;
		return make_shared <MyDB_PageHandleBase> (returnVal);
	}

	// it is there, so return it
	return make_shared <MyDB_PageHandleBase> (allPages [whichPage]);
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {

	// open the file, if it is not open
	if (fds.count (nullptr) == 0) {
		int fd = open (tempFile.c_str (), O_TRUNC | O_CREAT | O_RDWR, 0666);
		fds[nullptr] = fd;
	}

	// check if we are extending the size of the temp file
	size_t pos;
	if (availablePositions.size () == 0) {
		pos = lastTempPos++;
	} else {
		pos = availablePositions.top ();
		availablePositions.pop ();
	}

	MyDB_PagePtr returnVal = make_shared <MyDB_Page> (nullptr, pos, *this);
	pair <MyDB_TablePtr, long> whichPage = make_pair (nullptr, pos);
	allPages [whichPage] = returnVal;
	return make_shared <MyDB_PageHandleBase> (returnVal);
}

void MyDB_BufferManager :: kickOutPage () {
	
	// find the oldest page
	auto it = lastUsed.begin();
	auto page = *it;

	// write it back if necessary
	if (page->page->isDirty) {
		lseek (fds[page->page->myTable], page->page->pos * pageSize, SEEK_SET);
		write (fds[page->page->myTable], page->page->bytes, pageSize);
		page->page->isDirty = false;
	}

	// remove it
	lastUsed.erase (page);

	// and remember its RAM
	availableRam.push_back (page->page->bytes);
	page->page->bytes = nullptr;
}

void MyDB_BufferManager :: killPage (MyDB_Page &killMe) {
	
	// we have no refreences left to him
	killMe.refCount = -1;

	// if this is a temp page, recycle his slot
	if (killMe.myTable == nullptr) {
		availablePositions.push (killMe.pos);
	}

	// find the page
	pair <MyDB_TablePtr, long> whichPage = make_pair (killMe.myTable, killMe.pos);
	if (allPages.count (whichPage) != 0) {
		auto myPtr = allPages[whichPage];

		// special case is when there are no refs left to this page, but he is pinned
		// in this case... we just unpin him
		MyDB_PageHandle temp = make_shared <MyDB_PageHandleBase> (myPtr);
		if (killMe.bytes != nullptr && lastUsed.count (temp) == 0) {
			temp->page->incRefCount ();
			unpin (temp);	
			return;
		}

		// kill from the LRU list, if needed
		if (lastUsed.count (temp) != 0) {
			auto page = *(lastUsed.find (temp));
		       	lastUsed.erase (page);
		}

		// kill from the list of all pages
		auto page = allPages.find (whichPage);
		allPages.erase (page); 
	}

	// if it is dirty, write it
	if (killMe.isDirty) {
		lseek (fds[killMe.myTable], killMe.pos * pageSize, SEEK_SET);
		write (fds[killMe.myTable], killMe.bytes, pageSize);
		killMe.isDirty = false;
	}

	// if there is some RAM, remember it
	if (killMe.bytes != nullptr) {
		availableRam.push_back (killMe.bytes);
		killMe.bytes = nullptr;
	}
}

void MyDB_BufferManager :: access (MyDB_Page &updateMeIn) {
	
	// if this page was just accessed, get outta here
	if (updateMeIn.timeTick > lastTimeTick - (numPages / 2) && updateMeIn.bytes != nullptr) {
		return;
	}

	// get the page 
	pair <MyDB_TablePtr, long> findMe = make_pair (updateMeIn.myTable, updateMeIn.pos);
	auto updateMe = allPages [findMe];

	// first, see if it is currently in the LRU list; if so, just remove it
	MyDB_PageHandle temp = make_shared <MyDB_PageHandleBase> (updateMe);
	if (lastUsed.count (temp) == 1) {
		lastUsed.erase (temp);
		updateMe->timeTick = ++lastTimeTick;
		lastUsed.insert (temp);

	// verify that we are not pinned
	} else if (updateMe->bytes == nullptr) {
		
		// not in the LRU list means that we don't have its contents buffered
		// see if there is space
		if (availableRam.size () == 0)
			kickOutPage ();

		// if there is no space, we cannot do anything
		if (availableRam.size () == 0) {
			cout << "Can't get any RAM to read a page!!\n";
			exit (1);
		}

		// get some RAM for the page
		updateMe->bytes = availableRam[availableRam.size () - 1]; 
		updateMe->numBytes = pageSize;
		availableRam.pop_back ();

		// and read it
		lseek (fds[updateMe->myTable], updateMe->pos * pageSize, SEEK_SET);
		read (fds[updateMe->myTable], updateMe->bytes, pageSize);

		updateMe->timeTick = ++lastTimeTick;
		lastUsed.insert (temp);
	}

}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {

	// open the file, if it is not open
	if (fds.count (whichTable) == 0) {
		int fd = open (whichTable->getStorageLoc ().c_str (), O_CREAT | O_RDWR, 0666);
		fds[whichTable] = fd;
	}

	// make sure we don't have a null table
	if (whichTable == nullptr) {
		cout << "Can't allocate a page with a null table!!\n";
		exit (1);
	}

	// first, see if the page is there in the buffer
	pair <MyDB_TablePtr, long> whichPage = make_pair (whichTable, i);
	MyDB_PagePtr returnVal;

	// see if we already know him
	if (allPages.count (whichPage) == 0) {

		// in this case, we do not
		returnVal = make_shared <MyDB_Page> (whichTable, i, *this);
		allPages [whichPage] = returnVal;

	// in this case, we do
	} else {

		// get him out of the LRU list if he is there
		returnVal = allPages [whichPage];
		auto temp = make_shared <MyDB_PageHandleBase> (returnVal);
		if (lastUsed.count (temp) != 0) {
			auto page = *(lastUsed.find (temp));
	       		lastUsed.erase (page);
		}
	}

	// see if we need to get his data
	if (returnVal->bytes == nullptr) {

		// see if there is space to make a pinned page
		if (availableRam.size () == 0)
			kickOutPage ();

		// if there is no space, we cannot do anything
		if (availableRam.size () == 0) 
			return nullptr;

		// set up the return val
		returnVal->bytes = availableRam[availableRam.size () - 1];
		returnVal->numBytes = pageSize;
		availableRam.pop_back ();
		allPages [whichPage] = returnVal;

		// and read it
		lseek (fds[returnVal->myTable], returnVal->pos * pageSize, SEEK_SET);
		read (fds[returnVal->myTable], returnVal->bytes, pageSize);

	}	

	// get outta here
	return make_shared <MyDB_PageHandleBase> (returnVal);
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {

	// see if there is space to make a pinned page
	if (availableRam.size () == 0)
		kickOutPage ();

	// if there is no space, we cannot do anything
	if (availableRam.size () == 0) 
		return nullptr;

	// get a page to return
	MyDB_PageHandle returnVal = getPage ();
	returnVal->page->bytes = availableRam[availableRam.size () - 1];
	returnVal->page->numBytes = pageSize;
	availableRam.pop_back ();

	// and get outta here
	return returnVal;
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
	if (lastUsed.count (unpinMe) != 1) {
		lastUsed.erase (unpinMe);
		unpinMe->page->timeTick = ++lastTimeTick;
		lastUsed.insert (unpinMe);
	}
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSizeIn, size_t numPagesIn, string tempFileIn) {

	// remember the inputs
	pageSize = pageSizeIn;

	// this is the location where we write temp pages
	tempFile = tempFileIn;

	// start at time tick zero
	lastTimeTick = 0;

	// position in temp file
	lastTempPos = 0;

	// the number of pages
	numPages = numPagesIn;

	// create all of the RAM
	for (size_t i = 0; i < numPages; i++) {
		availableRam.push_back (malloc (pageSizeIn));
	}	
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
	
	// kill the list of all pages
	map <pair <MyDB_TablePtr, size_t>, MyDB_PagePtr, PageCompare> empty;
	allPages = empty;

	// kill the LRU list	
	set <MyDB_PageHandle, CheckLRU> emptyAgain;
	lastUsed = emptyAgain;

	// delete the rest of the RAM
	for (auto ram : availableRam) {
		free (ram);
	}

	// finally, close the files
	for (auto fd : fds) {
		close (fd.second);
	}

	unlink (tempFile.c_str ());
}


#endif


