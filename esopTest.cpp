#include "include/easy/easy.hpp"
#include <vector> 
#include <iostream>
#include <fstream>       //library includes
#include<string>
using namespace std;
using namespace kitty;
using namespace easy;      //namespace for encapsulation
using namespace esop;


int main(){
string vars,mins;
ifstream input;

input.open("between.txt");              //open data file 


vector<cube> listOfMinterms; 

input >> vars;
int numVars = stoi(vars);        //get number of variables from file

input >> mins;
int numMinterms = stoi(mins);


string minterm;

for(int i = 0; i < numMinterms; i++){
	input >> minterm;
	cube cuber = *(new cube(minterm));
	listOfMinterms.push_back(cuber);


}

input.close();


cout <<"\n\n";
print_cubes(listOfMinterms, numMinterms, cout);


cout <<"\n\n";


dynamic_truth_table tTbl = dynamic_truth_table(numVars);

create_from_cubes(tTbl, listOfMinterms, true);

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
