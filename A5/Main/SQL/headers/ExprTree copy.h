
#ifndef SQL_EXPRESSIONS
#define SQL_EXPRESSIONS

#include "MyDB_AttType.h"
#include "MyDb_Catalog.h"
#include <string>
#include <vector>

// create a smart pointer for database tables
using namespace std;
class ExprTree;
typedef shared_ptr <ExprTree> ExprTreePtr;

// this class encapsules a parsed SQL expression (such as "this.that > 34.5 AND 4 = 5")

// class ExprTree is a pure virtual class... the various classes that implement it are below
class ExprTree {

public:
	virtual bool validate_tree(MyDB_CatalogPtr)=0;
	virtual string get_tpye()=0;
	virtual string toString () = 0;
	virtual ~ExprTree () {}
};

class BoolLiteral : public ExprTree {

private:
	bool myVal;
public:
	
	BoolLiteral (bool fromMe) {
		myVal = fromMe;
	}

	string toString () {
		if (myVal) {
			return "bool[true]";
		} else {
			return "bool[false]";
		}
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		cout<<"aaa"<<endl;
		return true;
	}

	string get_tpye(){
		return "BoolLiteral";
	}
};

class DoubleLiteral : public ExprTree {

private:
	double myVal;
public:

	DoubleLiteral (double fromMe) {
		myVal = fromMe;
	}

	string toString () {
		return "double[" + to_string (myVal) + "]";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		cout<<"aaa"<<endl;
		return true;
	}

	string get_tpye(){
		return "DoubleLiteral";
	}

	~DoubleLiteral () {}
};

// this implement class ExprTree
class IntLiteral : public ExprTree {

private:
	int myVal;
public:

	IntLiteral (int fromMe) {
		myVal = fromMe;
	}

	string toString () {
		return "int[" + to_string (myVal) + "]";
	}

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		cout<<"aaa"<<endl;
		return true;
	}

	string get_tpye(){
		return "IntLiteral";
	}

	~IntLiteral () {}
};

class StringLiteral : public ExprTree {

private:
	string myVal;
public:

	StringLiteral (char *fromMe) {
		fromMe[strlen (fromMe) - 1] = 0;
		myVal = string (fromMe + 1);
	}

	string toString () {
		return "string[" + myVal + "]";
	}
	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		cout<<"aaa"<<endl;
		return true;
	}

	string get_tpye(){
		return "StringLiteral";
	}
	~StringLiteral () {}
};

class Identifier : public ExprTree {

private:
	string tableName;
	string attName;
public:

	Identifier (char *tableNameIn, char *attNameIn) {
		tableName = string (tableNameIn);
		attName = string (attNameIn);
	}

	string toString () {
		return "[" + tableName + "_" + attName + "]";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//cout<<"validating Identifier"<<endl;
		bool result = true;
		if(mycatalog->on_list(tableName) == -1 )
		{
			cout<<"table "<<tableName<<" doesn't exist"<<endl;
			//TODO: also need to check if attName exist
			result = false;
		}	
		return result;
	}

	string get_tpye(){
		return "Identifier";
	}

	~Identifier () {}
};

class MinusOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	MinusOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "- (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure two attribute can do math
		return ( lhs->validate_tree(mycatalog) && rhs->validate_tree(mycatalog));
	}

	string get_tpye(){
		return "MinusOp";
	}

	~MinusOp () {}
};

class PlusOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	PlusOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "+ (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure two attribute can do math
		return ( lhs->validate_tree(mycatalog) && rhs->validate_tree(mycatalog));
	}

	string get_tpye(){
		return "PlusOp";
	}

	~PlusOp () {}
};

class TimesOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	TimesOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "* (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure two attribute can do math
		return ( lhs->validate_tree(mycatalog) && rhs->validate_tree(mycatalog));
	}

	string get_tpye(){
		return "TimesOp";
	}
	~TimesOp () {}
};

class DivideOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	DivideOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "/ (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure two attribute can do math
		return ( lhs->validate_tree(mycatalog) && rhs->validate_tree(mycatalog));
	}

	string get_tpye(){
		return "DivideOp";
	}

	~DivideOp () {}
};

class GtOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	GtOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "> (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure two attribute can do math
		return ( lhs->validate_tree(mycatalog) && rhs->validate_tree(mycatalog));
	}

	string get_tpye(){
		return "GtOp";
	}

	~GtOp () {}
};

class LtOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	LtOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "< (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure two attribute can do math
		return ( lhs->validate_tree(mycatalog) && rhs->validate_tree(mycatalog));
	}

	string get_tpye(){
		return "LtOp";
	}

	~LtOp () {}
};

class NeqOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	NeqOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "!= (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure two attribute can do math
		return ( lhs->validate_tree(mycatalog) && rhs->validate_tree(mycatalog));
	}

	string get_tpye(){
		return "NeqOp";
	}

	~NeqOp () {}
};

class OrOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	OrOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "|| (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure two attribute can do math
		return ( lhs->validate_tree(mycatalog) && rhs->validate_tree(mycatalog));
	}

	string get_tpye(){
		return "OrOp";
	}

	~OrOp () {}
};

class EqOp : public ExprTree {

private:

	ExprTreePtr lhs;
	ExprTreePtr rhs;
	
public:

	EqOp (ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
		lhs = lhsIn;
		rhs = rhsIn;
	}

	string toString () {
		return "== (" + lhs->toString () + ", " + rhs->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure two attribute can do math
		return ( lhs->validate_tree(mycatalog) && rhs->validate_tree(mycatalog));
	}

	string get_tpye(){
		return "EqOp";
	}


	~EqOp () {}
};

class NotOp : public ExprTree {

private:

	ExprTreePtr child;
	
public:

	NotOp (ExprTreePtr childIn) {
		child = childIn;
	}

	string toString () {
		return "!(" + child->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure child is valid
		return child->validate_tree(mycatalog);
	}

	string get_tpye(){
		return "NotOp";
	}

	~NotOp () {}
};

class SumOp : public ExprTree {

private:

	ExprTreePtr child;
	
public:

	SumOp (ExprTreePtr childIn) {
		child = childIn;
	}

	string toString () {
		return "sum(" + child->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure child can be sum up
		return child->validate_tree(mycatalog);
	}

	string get_tpye(){
		return "SumOp";
	}

	~SumOp () {}
};

class AvgOp : public ExprTree {

private:

	ExprTreePtr child;
	
public:

	AvgOp (ExprTreePtr childIn) {
		child = childIn;
	}

	string toString () {
		return "avg(" + child->toString () + ")";
	}	

	bool validate_tree(MyDB_CatalogPtr mycatalog)
	{
		//TODO: need to make sure child can to average
		return child->validate_tree(mycatalog);
	}

	string get_tpye(){
		return "AvgOp";
	}

	~AvgOp () {}
};


#endif
