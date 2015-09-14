#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

// Retrieve columnWithTitle()
int main()
{
	// loading the .csv file - assumes the file is in the same directory
	ifstream csv_stream("baseline_SiSandra_short.csv");
	string row_values; // Will be used to store each row values of the .csv file
	istringstream row_stream; // Required for getline() to work - first argument must be stream type.
	string col_values; // Will be used to store each col values from row entries stored in row_values

	// In the raw stream of .csv file, each row is deliminated by newline ('\n'), and each column is deliminated by comma (,).
	while (csv_stream.good())
	{
		// First, retrieve entire row by looking for a newline
		getline(csv_stream, row_values, '\n');
		cout << row_values << endl;
		// Convert the string type to stream, in order for getline() to work
		row_stream.str(row_values);
		// Then, within the row, start to retrieve out cell values of each column by looking for a comma
		getline(row_stream, col_values, ',');
		cout << col_values << endl;

		// The current code will print out each row, and first entry of each row will be printed out.
	}
	return 0;
}