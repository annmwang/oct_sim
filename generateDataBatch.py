import os
import subprocess
import argparse
from datetime import datetime 
import threading 

# initialize date and time
now = datetime.now()

# Construct the parser to take in the inputs 
parser = argparse.ArgumentParser()
parser.add_argument("--outDir", '-o', type=str, help="output directory name", default="simResults_{}".format(now.strftime("%m%d%y_%H%M%S")))
args = parser.parse_args()

outDir = args.outDir

# Parameters constant from run to run
chamber = "large"
xCoincidenceThreshold = 3
uvCoincidenceThreshold = 3
outFilePrefix = 'outFile'

# Parameters that will change from run to run. Others are left default
nEvents = [10**3]
bkgRates = [0]
#bkgRates = [i*(10**j) for i in range(1,10) for j in range(2,5)] 
bkgRates.sort()

# setup the output directory
if os.path.isdir(outDir) == False:
	os.system("mkdir {}".format(outDir))

#################### Begin the production of events ####################

os.system("make")

nNEvents = len(nEvents)
nBkgRates = len(bkgRates)

counter = 0
for nE in range(0,nNEvents):
	for nB in range(0,nBkgRates):
		outFileName = outDir + '/' + outFilePrefix + '_nEvents{}_bkgRate{}'.format(nEvents[nE],bkgRates[nB])
		with open("script_%d.sh"%counter,"w") as text_file:
                        text_file.write(
                                """ #!/bin/bash

                                printf "Start time: "; /bin/date
                                printf "Job is running on node: "; /bin/hostname
                                printf "Job running as user: "; /usr/bin/id
                                printf "Job is running in directory: "; /bin/pwd
                                echo
                                echo "Working hard..."

                                printf "###########"
                                
                                ./sim -n {0} -ch {1} -b {2} -o {3}.root -uvr -thrx {4} -thruv {5} -tree

                                """.format(nEvents[nE],                                                                                                    
                                        chamber,                                                                                                              
                                        bkgRates[nB],                                                                                                   
                                        outFileName,                                                                                              
                                        xCoincidenceThreshold,                                                                                            
                                        uvCoincidenceThreshold)
                                )
                
                os.system("chmod +x script_%d.sh"%counter)
                # os.system("source script_%d.sh"%counter); break;
                os.system("qsub -V -cwd -q tier3 script_%d.sh"%counter)
                counter += 1

