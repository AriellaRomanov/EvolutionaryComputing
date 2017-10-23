Please refer to http://web.mst.edu/~tauritzd/courses/ec/cs5401fs2017/ for homework assignments.

Direct any grading questions to John Liming at jrl2n4@mst.edu

To Compile:
Place all .h and .cpp files in the same directory (with no other .h or .cpp files), with the makefile
Run 'make' in the command line from the directory with files

To Configure:
Config file follows format:
<key>=<value>
Keys used in this program:
  seed (if not supplied, defaults to random)
  numRuns (if not supplied, defaults to 30)
  numFits (if not supplied, defaults to 1000)
  solFile (if not supplied, solution will not be recorded)
  logFile (if not supplied, log messages will not be recorded)
  probFile (if not supplied, must be in command-line arguments)

To Run:
./run.sh <config-file> <problem-file>
OR
./run.sh <config-file>
NOTE: requires the problem file to be noted in the config file
