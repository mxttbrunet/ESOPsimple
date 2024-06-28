#brunet,matt
#06,26,2024
#esopWrapper.py
#this uses python to take in feasible states and run them through the c++ "easy" library 
#as long as the .hpp files are in your directory, and you have a c++ compiler the library runs smoothly for its purposes
#uses an intermediate text file as communication
#uses reed muller expansions for esops
import sympy as sp
from sympy.abc import a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p
symbolsAvail = [a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p]
import subprocess


def runCppCode(numVars, feasStates, pol ):
  numMinterms = len(feasStates)
  input_data = f"{numVars}\n{numMinterms}\n"          #format
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
  #print(output)
  posEsop_tt, mixEsop_tt = output.split("D")
  if(pol == "positive"):
      return posEsop_tt
  else:
      return mixEsop_tt

def produceExpression(truthTable):
      
  ##populate list with needed variables 
  varSymbols = []
  for i in range(numVariables):
      varSymbols.append(symbolsAvail[i])

  toBeAnded = []
  toBeXord = []
  i = 0
  for char in esop_tt:
      if (char == "1"):
          toBeAnded.append(symbolsAvail[i])
          i+=1
      elif (char == "0"):
          toBeAnded.append(~symbolsAvail[i])      ##convert to sympy expression
          i+=1
      elif (char == "-"):
          i+=1
      elif(char == "\n"):
          continue
      if(i == 5):
          i = 0
          toBeXord.append(sp.And(*toBeAnded, evaluate = False, strict = True))
          toBeAnded = []
  return sp.Xor(*toBeXord, evaluate = False)
    


polarity = "mixed"                     #either "mixed" or "positive", ##default is mixed, as it is smaller
numVariables= 5                             ####number of boolean variables  ###edit this     
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




esop_tt = runCppCode(numVariables, feasibleStates, polarity)
finalEsop = produceExpression(esop_tt)
print("ESOP***: b ", finalEsop, "\n")