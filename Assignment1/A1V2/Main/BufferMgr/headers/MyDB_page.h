
#ifndef PAGE_H
#define PAGE_H

#include <memory>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <math.h>

// create a smart pointer for database pages
using namespace std;
class MyDB_page;
typedef shared_ptr <MyDB_page> MyDB_pagePtr;

class MyDB_page {

public:

	// creates a new page with the given name, at the given storage location
	MyDB_page (int page_id ,  int page_size, string page_location, int anonymous);

	// get the name of the page
	int getPageSize ();

	// get the storage location of the page
	int getPageId ();

	char* getPageContent () ;

	void show_content ();

	string show_location();

	int see_count();

	void set_count(int new_value);

	int see_dirty_status ();

	int see_pin_status ();

	void set_pin();

	void set_unpin();

	void set_dirty();

	void set_clean();

	int getPageCount ();

	int see_anonymous();

	void put_into_buffer();

	void remove_from_buffer();

	int is_in_buffer();

	// kill the dude
	~MyDB_page ();

private:

	string location;

	// the size of the page
	int size;

	// the location where it is stored
	int size_id;

	// the content bytess
	char * content;

	// dirty
	int dirty_status;

	// pin
	int pin_status;

	//count
	int count;

	// anonymous
	int anonymous;

	// in the buffer or not
	int in_buffer;

};
#endif