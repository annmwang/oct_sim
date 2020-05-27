import os
import subprocess
import argparse
from datetime import datetime 
import threading 

# initialize date and time
now = datetime.now()

# Construct the parser to take in the inputs 
parser = argparse.ArgumentParser()
parser.add_argument("--outDir", '-o', type=str, help="output directory name", default=os.getcwd())
args = parser.parse_args()

outDir = args.outDir
print(outDir)
# Parameters constant from run to run
chamber = "large"
xCoincidenceThreshold = 3
uvCoincidenceThreshold = 3
outFilePrefix = 'outFile'

# Parameters that will change from run to run. Others are left default
nEvents = [10**4]
#bkgRates = [0,1] 
bkgRates = [i*(10**j) for i in range(1,10) for j in range(2,5)] 
bkgRates.sort()

# setup the output directory
workDir = outDir + "/simResults_{}".format(now.strftime("%m%d%y_%H%M%S"))
if os.path.isdir(workDir) == False:
	os.system("mkdir {}".format(workDir))
print(workDir)
#################### Begin the production of events ####################

os.system("make")

nNEvents = len(nEvents)
nBkgRates = len(bkgRates)

counter = 0
for nE in range(0,nNEvents):
	for nB in range(0,nBkgRates):
		tempDir = workDir + '/job_{}_{}'.format(nEvents[nE],bkgRates[nB])
		if os.path.isdir(tempDir) == False:
			os.system("mkdir {}".format(tempDir))
		outFileName = tempDir + '/' + outFilePrefix + '_nEvents{}_bkgRate{}'.format(nEvents[nE],bkgRates[nB])
		with open("{}/script_{}.sh".format(tempDir,counter),"w") as text_file:
                        text_file.write(
                                """ #!/bin/bash
#$ -o /scratch/stdout_{0}_{2}.txt 
#$ -e /scratch/stderr_{0}_{2}.txt

printf "Start time: "; /bin/date
printf "Job is running on node: "; /bin/hostname
printf "Job running as user: "; /usr/bin/id
printf "Job is running in directory: "; /bin/pwd
echo
echo "Working hard..."

printf "###########"
mkdir -p /scratch/testBadea/job_{0}_{2}
./sim -n {0} -ch {1} -b {2} -o /scratch/testBadea/job_{0}_{2}/outFile_nEvents{0}_bkgRate{2}.root -uvr -thrx {4} -thruv {5} -tree
scp /scratch/testBadea/job_{0}_{2}/* abc-at12:{6}
scp /scratch/std*_{0}_{2}.txt abc-at12:{6}
rm -rf /scratch/testBadea/job_{0}_{2}
rm -rf /scratch/std*_{0}_{2}.txt
                                """.format(nEvents[nE],                                                                                                    
                                        chamber,                                                                                                              
                                        bkgRates[nB],                                                                                                   
                                        outFileName,                                                                                              
                                        xCoincidenceThreshold,                                                                                            
                                        uvCoincidenceThreshold,
					tempDir))
		os.system("chmod +x {}/script_{}.sh".format(tempDir,counter))
		os.system("qsub -V -cwd -q tier3 {}/script_{}.sh".format(tempDir,counter))
                counter += 1

