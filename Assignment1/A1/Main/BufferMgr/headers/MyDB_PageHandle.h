
#ifndef PAGE_HANDLE_H
#define PAGE_HANDLE_H

#include <memory>
#include <string>

// page handles are basically smart pointers
using namespace std;
class MyDB_PageHandleBase;
typedef shared_ptr <MyDB_PageHandleBase> MyDB_PageHandle;

class MyDB_PageHandleBase {

public:

	// THESE METHODS MUST BE IMPLEMENTED WITHOUT CHANGING THE DEFINITION

	// access the raw bytes in this page... if the page is not currently
	// in the buffer, then the contents of the page are loaded from 
	// secondary storage. 
	void *getBytes ();

	// let the page know that we have written to the bytes.  Must always
	// be called once the page's bytes have been written.  If this is not
	// called, then the page will never be marked as dirty, and the page
	// will never be written to disk. 
	void wroteBytes ();

	// There are no more references to the handle when this is called...
	// this should decrmeent a reference count to the number of handles
	// to the particular page that it references.  If the number of 
	// references to a pinned page goes down to zero, then the page should
	// become unpinned.  
	~MyDB_PageHandleBase ();

	// FEEL FREE TO ADD ADDITIONAL PUBLIC METHODS

    // create a new page with its unique id
    MyDB_PageHandleBase (int i, int page_size, string file_location);
    
    // pin down this page
    void set_pin();
    
    // unpin this page
    void set_unpin();
    
    // show the pin status
    int see_pin_status();
    
    // show the dirty status
    int see_dirty_status();
    
    // set the page dirty
    void set_dirty();
    
    // set the page clean
    void set_clean();
    
    // show the content
    void show_content(int page_size);

    // show the location of the file
    string show_location();

    // see the count number of this handle
    int see_count();

    // manipulate the count number
    void set_count(int new_value);

    // page id
    int page_id;

private:

	// YOUR CODE HERE
	int page_size;
    int pin_status;
    int dirty_status;
    char* content;
    string location;
    int count;

    PagePtr page_ptr;
};

#endif

