#include "stdafx.h"
#include "database.h"
#include <iostream>

using namespace std;

int main()
{
	DB db;
	db.Open("data");
    /*
	db.Store("Alpha", "data1", DB_INSERT);
	db.Store("beta", "Data for beta", DB_INSERT);
	db.Store("gamma", "record3", DB_INSERT);
    */

    cout << db.Fetch("gamma") << endl;

	db.Close();
	return 0;
}
