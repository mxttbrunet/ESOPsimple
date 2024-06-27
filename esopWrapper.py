import subprocess

numVars = 3 #######change this 

feasibleStates = ["111","110","101","011"]   ###change this 
numMinterms = len(feasibleStates)

input_data = f"{numVars}\n{numMinterms}\n"          #format

with open("between.txt", "a") as file:
     file.write(input_data)
     for i in range(numMinterms):        
        file.write(str(feasibleStates[i]))
        file.write("\n")

file.close()

compileCommand = "g++ esopTest.cpp -o esopTest"

subprocess.run(compileCommand, shell = True, check = True)
run_command = "esopTest"
result = subprocess.run(run_command, shell = True, capture_output = True, text = True, )

print(result.stderr)
print(result.stdout)