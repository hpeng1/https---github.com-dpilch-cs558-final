
#include "Lexer.h"
#include "Parser.h"
#include "ParserTypes.h"
#include <string>      
#include <iostream>   
#include <sstream>

int main (int numArgs, char **args) {

	// make sure we have the correct arguments
	if (numArgs != 3) {
		cout << "args: catalog_file directory_for_tables\n";
		return 0;
	}

	// open up the catalog file
	MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> (args [1]);

	// print out the intro notification
	cout << "\n          Welcome to MyDB v0.1\n\n";
	cout << "\"Not the worst database in the world\" (tm) \n\n";

	// and repeatedly accept queries
	while (true) {
		
		cout << "MyDB> ";

		// this will be used to collect the query
		stringstream ss;

		// get a line
		for (string line; getline (cin, line);) {
			
			// see if it has a ";" at the end
			size_t pos = line.find (';');

			// it does!!  so we are ready yo parse
			if (pos != string :: npos) {

				// append the last line
				line.resize (pos);
				ss << line;

				// get the string to parse
				string parseMe = ss.str ();

				// see if we got a "quit" or "exit"
				if (parseMe.find ("quit") != string :: npos || parseMe.find ("exit") != string :: npos) {
					cout << "OK, goodbye.\n";
					return 0;
				}

				// add an extra zero at the end; needed by lexer
				parseMe.push_back ('\0');

				// now parse it
				yyscan_t scanner;
				LexerExtra extra { "" };
				yylex_init_extra (&extra, &scanner);
				const YY_BUFFER_STATE buffer { yy_scan_string (parseMe.data(), scanner) };
				SQLStatement *final = nullptr;
				const int parseFailed { yyparse (scanner, &final) };
				yy_delete_buffer (buffer, scanner);
				yylex_destroy (scanner);

				// if we did not parse correctly
				if (parseFailed) {

					// print out the parse error
					cout << "Parse error: " << extra.errorMessage;

					// get outta here
					if (final != nullptr)
						delete final;
					break;

				// if we did parse correctly, just print out the query
				} else {

					// see if we got a create table
					if (final->isCreateTable ()) {

						cout << "Added table " << final->addToCatalog (args[2], myCatalog) << "\n";
						
					} else if (final->isSFWQuery ()) {

						// print it out
						final->printSFWQuery ();
					}

					// get outta here
					if (final != nullptr)
						delete final;
					break;
				}

			} else {
				ss << line << "\n";
				cout << "    > ";
			}
		}
	}
}
