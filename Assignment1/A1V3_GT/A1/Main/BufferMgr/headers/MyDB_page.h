
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
	MyDB_page (int page_size, string page_location, int anolymous_id);
	// get the name of the page
	int getPageSize ();

	char* getPageContent () ;

	// see and change the buffer index
	void change_buffer_index(int i);
	int see_buffer_index();

	// get the page id
	int getPageId ();

	// return the associate file location 
	string show_location();

	// see and change the pin status
	int see_pin_status ();
	void set_pin();
	void set_unpin();

	// see and change the dirty status
	int see_dirty_status ();
	void set_dirty();
	void set_clean();

	// see and change the page count
	int getPageCount ();
	void set_count(int new_value);
	int see_count();

	// is anonymous or not
	int see_anonymous();

	// in buffer or not 
	void put_into_buffer();
	void remove_from_buffer();
	int is_in_buffer();

	// kill the dude
	~MyDB_page ();


	// the content bytess
	char * content;

	int timestamp;

private:

	string location;

	// associate buffer id
	int buffer_index;

	// the size of the page
	int size;

	// the location where it is stored
	int size_id;



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