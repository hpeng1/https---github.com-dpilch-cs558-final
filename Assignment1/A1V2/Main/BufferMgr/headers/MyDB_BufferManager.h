#ifndef BUFFER_MGR_H
#define BUFFER_MGR_H

#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "MyDB_PageHandle.h"
#include "MyDB_Table.h"
#include "MyDB_page.h"
using namespace std;

class MyDB_PageHandleBase;
typedef shared_ptr <MyDB_PageHandleBase> MyDB_PageHandle;

class MyDB_BufferManager {

public:

	// THESE METHODS MUST APPEAR AND THE PROTOTYPES CANNOT CHANGE!

	// gets the i^th page in the table whichTable... note that if the page
	// is currently being used (that is, the page is current buffered) a handle 
	// to that already-buffered page should be returned
	MyDB_PageHandle getPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page that will no longer exist (1) after the buffer manager
	// has been destroyed, or (2) there are no more references to it anywhere in the
	// program.  Typically such a temporary page will be used as buffer memory.
	// since it is just a temp page, it is not associated with any particular 
	// table
	MyDB_PageHandle getPage ();

	// gets the i^th page in the table whichTable... the only difference 
	// between this method and getPage (whicTable, i) is that the page will be 
	// pinned in RAM; it cannot be written out to the file
	MyDB_PageHandle getPinnedPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page, like getPage (), except that this one is pinned
	MyDB_PageHandle getPinnedPage ();

	// un-pins the specified page
	void unpin (MyDB_PageHandle unpinMe);

	// creates an LRU buffer manager... params are as follows:
	// 1) the size of each page is pageSize 
	// 2) the number of pages managed by the buffer manager is numPages;
	// 3) temporary pages are written to the file tempFile
	MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile);
	
	// when the buffer manager is destroyed, all of the dirty pages need to be
	// written back to disk, any necessary data needs to be written to the catalog,
	// and any temporary files need to be deleted
	int find_victim();

	~MyDB_BufferManager ();

	int see_pool_size();

	// FEEL FREE TO ADD ADDITIONAL PUBLIC METHODS 
	int page_size;

	void clean_up(MyDB_pagePtr);

private:

	// YOUR STUFF HERE
    
    // size of each page in this manager
    
    
    // numebr of pages
    int page_num;
    
    // file descriptor
    string temp_file;
    
    // page pool
	MyDB_pagePtr * pages_pool; // vector ?
    
    // pool size
    int pool_size;
    
    //see if the required page is in the memory, return null if it is not
    MyDB_pagePtr fetch_from_memory(long page_id, string page_location);
    
    // traverse the page pool, show their id and pinned status
    void traverse_pages_pool();
    
    void write_back_page(char* content, int position, int page_size, string loc);


    
    
    

};

#endif


