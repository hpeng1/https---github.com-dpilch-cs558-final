
#ifndef CATALOG_C
#define CATALOG_C

#include "MyDB_Catalog.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "MyDB_AttType.h"
#include "MyDB_AttVal.h"
#include "MyDB_Catalog.h"
#include "MyDB_Record.h"
#include "MyDB_Schema.h"
#include "MyDB_Table.h"

void MyDB_Catalog :: putString (string key, string value) {
	myData [key] = value;
}

void MyDB_Catalog :: putStringList (string key, vector <string> value) {
	string res("");
	for (string s : value) {
		res = res + s + "#";
	}
	myData [key] = res;
}

void MyDB_Catalog :: putInt (string key, int value) {
	ostringstream convert;
	convert << value;
	myData [key] = convert.str ();
}

bool MyDB_Catalog :: getStringList (string key, vector <string> &returnVal) {

	// verify the entry is in the map
	if (myData.count (key) == 0)
		return false;

	// it is, so parse the other side
	string res = myData[key];	
	for (int pos = 0; pos < (int) res.size (); pos = res.find ("#", pos + 1) + 1) {
		string temp = res.substr (pos, res.find ("#", pos + 1) - pos);
		returnVal.push_back (temp);
	}
	return true;
}

bool MyDB_Catalog :: getString (string key, string &res) {
	if (myData.count (key) == 0)
		return false;

	res = myData[key];
	return true;
}

bool MyDB_Catalog :: getInt (string key, int &value) {

	// verify the entry is in the map
	if (myData.count (key) == 0)
		return false;

	// it is, so convert it to an int
	string :: size_type sz;
	try {
		value = std::stoi (myData [key], &sz);

	// exception means that we could not convert
	} catch (...) {
		return false;
	}
	
	return true;
}

MyDB_Catalog :: MyDB_Catalog (string fNameIn) {

	// remember the catalog name
	fName = fNameIn;

	// try to open the file
	string line;
	ifstream myfile (fName);

	// if we opened it, read the contents
	if (myfile.is_open()) {

		// loop through all of the lines
    		while (getline (myfile,line)) {

			// find how to cut apart the string
			int firstPipe, secPipe, lastPipe;
			firstPipe = line.find ("|");
			secPipe = line.find ("|", firstPipe + 1); 
			lastPipe = line.find ("|", secPipe + 1); 

			// if there is an error, don't add anything
			if (firstPipe >= (int) line.size () || secPipe >= (int) line.size () || lastPipe >= (int) line.size ())
				continue;

			// and add the pair
			myData [line.substr (firstPipe + 1, secPipe - firstPipe - 1)] = 
				line.substr (secPipe + 1, lastPipe - secPipe - 1);
		}
		myfile.close();
	}
}

MyDB_Catalog :: ~MyDB_Catalog () {

	// just save the contents
	save ();
}

void MyDB_Catalog :: add_to_list(string s1, string s2)
{

	table_name_list.push_back(make_pair(s1,s2));
	return;
}

int MyDB_Catalog :: on_list(string s)
{
	bool result = -1;
	for(int i=0; i < table_name_list.size(); i++)
	{
		if(table_name_list[i].first.compare(s) == 0 || table_name_list[i].second.compare(s) == 0)
			return i;
	}
	return result;
}

string MyDB_Catalog :: get_abbrev(string s)
{
	for(int i=0; i < table_name_list.size(); i++)
	{
		if(table_name_list[i].first.compare(s) == 0)
			return table_name_list[i].second;
	}
	cout<<"table "<<s<< " doesn't exist"<<endl;
	return nullptr;
}

string MyDB_Catalog :: get_full_name(string s)
{
	for(int i=0; i < table_name_list.size(); i++)
	{
		if(table_name_list[i].second.compare(s) == 0)
			return table_name_list[i].first;
	}
	cout<<"table "<<s<< " doesn't exist"<<endl;
	return nullptr;
}

void MyDB_Catalog :: traverse_name_list()
{
	cout<<"traverse table list: ";
	for(int i=0; i < table_name_list.size(); i++)
	{
		cout<<"<"<<table_name_list[i].first<<","<<table_name_list[i].second<<">";
	}
	cout<<""<<endl;
	return;
}

void MyDB_Catalog :: save () {

	ofstream myFile (fName, ofstream::out | ofstream::trunc);
	if (myFile.is_open()) {
		for (auto const &ent : myData) {
			myFile << "|" << ent.first << "|" << ent.second << "|\n";
		}
	}
}

#endif


