
#include "MyDB_page.h"

MyDB_page :: MyDB_page (int page_id ,  int page_size, string page_location, int anony) {
	size_id = page_id;
	size = page_size;
	location=page_location;
    content = (char*)malloc(size*sizeof(char));
	dirty_status=0;
	count=0;
	pin_status=0;
	anonymous = anony;
    //  to write malloc lseek slot  slot =-1 
    if(size_id != -1)
    {
        FILE* fp = fopen(location.c_str(),"rb+"); 
        if(fp == nullptr)
            cout<<"can't find the file"<<endl;
        else
            cout<<"file bytes is loaded from "<<location<<" page "<<size_id<<endl;

        fseek(fp, size_id*size,SEEK_SET);  
        for(int num=0;num<size;num++)
        {
            content[num]=fgetc(fp);
        } 
        fclose(fp); 
    }
    else
    {
		// tempfile find slot
        cout<<"Need to find a slot in "<<location<<endl;
        long  fileLen;
		FILE* fp = fopen(location.c_str(),"rb+");
		//long fileLen = filelength(fileno(fp));
		fseek (fp, 0, SEEK_END); 
		fileLen=ftell (fp); 
		size_id = ceil((float)fileLen/size);
		fclose(fp);
    }
}

MyDB_page :: ~MyDB_page () {}

string  MyDB_page ::show_location()
{
	return location;
}

int MyDB_page :: getPageSize () {
	return size;
}

int MyDB_page :: getPageId () {
	return size_id;
}


char* MyDB_page :: getPageContent () {
	return content;
}

int MyDB_page ::see_count()
{
	return count;
}

void MyDB_page :: set_count(int new_value)
{
	count=new_value;
}

int MyDB_page ::see_anonymous()
{
	return anonymous;
}

void MyDB_page ::show_content ()
{
	for(int i=0;i<size;i++)
	{
		cout<<content[i]<<" ";
	}
}

int MyDB_page ::see_dirty_status ()
{
	return dirty_status;
}

int MyDB_page ::see_pin_status ()
{
	return pin_status;
}

void MyDB_page :: set_pin () 
{
	pin_status=1;
}

void MyDB_page :: set_unpin ()
{
	pin_status=0;
}

void MyDB_page :: set_dirty () 
{
	dirty_status=1;
}

void MyDB_page :: set_clean () 
{
	dirty_status=0;
}

int MyDB_page :: getPageCount()
{
	return count;
}

void MyDB_page :: put_into_buffer () 
{
	in_buffer = 1;
}

void MyDB_page :: remove_from_buffer () 
{
	in_buffer = 0;
}

int MyDB_page :: is_in_buffer()
{
	return in_buffer;
}


