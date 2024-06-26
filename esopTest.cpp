#include "include/easy/easy.hpp"
#include <vector> 
#include <iostream>
//#include "synthesis.hpp"
//#include "exact_synthesis.hpp"
using namespace std;
using namespace kitty;
using namespace easy;
using namespace esop;

int main(){


vector<cube> listOfMinterms; 
int numVars;
cout << "num vars?" << "\n";
cin >> numVars;
int numMinterms;
cout << "numMinterms? " << "\n";
cin >> numMinterms;

string minterm;

for(int i = 0; i < numMinterms; i++){
	cin >> minterm;
	cube cuber = *(new cube(minterm));
	listOfMinterms.push_back(cuber);

}
cout <<"\n\n";
print_cubes(listOfMinterms, numMinterms, cout);


cout <<"\n\n";


dynamic_truth_table tTbl = dynamic_truth_table(numVars);

create_from_cubes(tTbl, listOfMinterms, true);
//string stream????
print_binary(tTbl, cout);
cout << "\n\n";


vector<cube> finalAnswer;
vector<cube> finalAnswer2;

finalAnswer = kitty::esop_from_pprm(tTbl);
print_cubes(finalAnswer, numVars, cout);

cout << "\n\n";

finalAnswer2 = kitty::esop_from_optimum_pkrm(tTbl);
print_cubes(finalAnswer2, numVars, cout);



return 0;

}
