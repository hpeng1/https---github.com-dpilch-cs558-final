
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C


#include <memory>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "MyDB_PageHandle.h"
using namespace std;

void * MyDB_PageHandleBase :: getBytes () {
    return content;
}

void MyDB_PageHandleBase :: wroteBytes () {
    // set this page dirty
    set_dirty();
    return;
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
    // make the count decrease by 1
    set_count(see_count()-1);
    cout<<"handle lost!!!!!"<<endl;
    // if count is 0, unpin the page
    if(see_count()==0)
        set_unpin();
}

MyDB_PageHandleBase :: MyDB_PageHandleBase (int i, int p_size, string file_location) {

    //read contents from the file location
    page_id = i;
    page_size = p_size;
    content = (char*)malloc(page_size*sizeof(char));
    location = file_location;

    //  to write malloc lseek slot
    if(page_id != -1)
    {
        FILE* fp = fopen(location.c_str(),"r"); 
        if(fp == nullptr)
            cout<<"can't find the file"<<endl;
        else
            cout<<"file bytes is loaded from "<<location<<" page "<<page_id<<endl;

        fseek(fp, page_id*page_size,SEEK_SET);  
        for(int num=0;num<page_size;num++)
        {
            content[num]=fgetc(fp);
        } 
        fclose(fp); 
    }
    else
    {
        cout<<"Need to find a slot in "<<location<<endl;
        // use lseek to find a slot in that location
        
        // page_id = slot_id
    }

}

void MyDB_PageHandleBase :: set_pin(){
    // 1 means that this page is pinned
    pin_status = 1;
}

void MyDB_PageHandleBase :: set_unpin(){
    // 0 means that this page is unpinned
    pin_status = 0;
}

int MyDB_PageHandleBase :: see_pin_status(){
    // just return the private variable
    return pin_status;
}

void MyDB_PageHandleBase :: set_dirty(){
    // 1 means that this page is pinned
    dirty_status = 1;
}

void MyDB_PageHandleBase :: set_clean(){
    // 1 means that this page is pinned
    dirty_status = 0;
}

int MyDB_PageHandleBase :: see_dirty_status(){
    // just return the private variable
    return dirty_status;
}

string MyDB_PageHandleBase :: show_location(){
    // 1 means that this page is pinned
    return location;
}

int MyDB_PageHandleBase :: see_count(){
    // see the count number of this handle
    return count;
}

void MyDB_PageHandleBase :: set_count(int new_value){
    // set the count to a new value
    count = new_value;
    return;
}

void MyDB_PageHandleBase :: show_content(int page_size){
    // just return the private variable
    cout<<"show content, page size is: "<<page_size<<endl;
    for(int i=0; i<page_size; i++)
        cout<<content[i]<<" ";
    cout<<""<<endl;
    return;
}


#endif

