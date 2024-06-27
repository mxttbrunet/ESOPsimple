#brunet,matt
#06,26,2024
#esopWrapper.py
#this uses python to take in feasible states and run them through the c++ "easy" library 
#as long as the .hpp files are in your directory, and you have a c++ compiler the library runs smoothly for its purposes
#uses an intermediate text file as communication
import sympy as sp
from sympy.abc import a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p
symbolsAvail = [a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p]
import subprocess



numVariables= 5                             ####number of boolean variables      ###edit this 

feasibleStates = ["11111",
                  "11110",
                  "11101",
                  "11011",
                  "10111",
                  "01111",
                  "01011",
                  "10101",
                  "01010",
                  "01110",
                  "11010",
                  "10110",
                  "01101"
                ]                     ###edit this 





numMinterms = len(feasibleStates)

input_data = f"{numVariables}\n{numMinterms}\n"          #format
open("between.txt", "w").close()


with open("between.txt", "a") as file:        
     file.write(input_data)
     for i in range(numMinterms):        
        file.write(str(feasibleStates[i]))      ##write feasible syayes 
        file.write("\n")

file.close()

compileCommand = "g++ esopTest.cpp -o esopTest"          ##compile c++ code
subprocess.run(compileCommand, shell = True, check = True)

run_command = "esopTest"                                       #run c++ code
result = subprocess.run(run_command, shell = True, capture_output = True, text = True )

print(result.stderr)         #print any errors or output
output = result.stdout         #will be changed to sympy output
print(output)


boolVars = []
for i in range(numVariables):
    boolVars.append(symbolsAvail[i])   #populate list of expression variables 







    