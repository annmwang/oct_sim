import os
import sys
import subprocess
import argparse
from datetime import datetime 
import threading 

def formatMMEffString(mm_eff):
    s = '\''
    for i in mm_eff:
            s+='{},'.format(i)
    s = s[:-1]+'\''
    return s

def formatMMEffStringFileName(mm_eff):
	s = ''
	for i in mm_eff:
	        s+='{}'.format(i*10)
	s = s[:-1]
	return s

# construct output log file
sys.stdout = open("generateDataBatch.log", "w")

# initialize date and time
now = datetime.now()

# Construct the parser to take in the inputs 
parser = argparse.ArgumentParser()
parser.add_argument("--outDir", '-o', type=str, help="output directory name", default=os.getcwd())
args = parser.parse_args()

outDir = args.outDir
print(outDir)
if os.path.isdir(outDir) is False:
	os.mkdir(outDir)
# Parameters constant from run to run
chamber = "large"
xCoincidenceThreshold = 3
uvCoincidenceThreshold = 3
chamber_feature = ['']

outFilePrefix = 'outFile'

# Parameters that will change from run to run. Others are left default
print("Listing the parameters scanned over")


print("----------------------------------------------------------")
print("Number of Events")
nEvents = [10**3]
for i in range(0,len(nEvents)):
	print("{} | {}".format(i,nEvents[i]))
print("----------------------------------------------------------")
print("Background Rates (Hz)")
#bkgRates = [0,1] 
bkgRates = [i*(10**j) for i in [1] for j in range(0,6)] 
for i in range(0,len(bkgRates)):
	print("{} | {}".format(i,bkgRates[i]))
print("Number of background rates: {}".format(len(bkgRates)))
print("----------------------------------------------------------") 
bkgRates.sort()
# first check is with all detectors run at 1.0 efficiency and 0.9 efficiency 
mmEffs = [[1.0]*8,[0.9]*8]
# check behavior if one or two of the mm are low efficiency
lowEff = 0.3 #arbitrary definition of low efficiency as 0.3 
print("Listing the MM Efficiency configurations [mm1,mm2,...,mm8]")
print("----------------------------------------------------------")
print("Initial configurations")
counter = 0
for i in mmEffs:
	print("{} | {}".format(counter,i))
	counter+=1
print("----------------------------------------------------------")
print("Permutation configurations")

DoubleEffLoss = False
for i in range(0,8):
	temp1 = [0.9]*8 
	temp2 = [0.9]*8
	temp1[i] = lowEff
	print("-----------")
	print("{} | {}".format(counter,temp1))
	mmEffs.append(temp1)
	temp2[i] = lowEff
	print("-- Double Efficiency Loss --")
	if DoubleEffLoss: print("INCLUDING")
	for j in range(i+1,8):
		temp2[j] = lowEff
		print("{} | {}".format(counter,temp2))
		if DoubleEffLoss: mmEffs.append(temp2)
		temp2[j] = 0.9	
print("Number of MM efficiency configurations: {}".format(len(mmEffs)))
print("----------------------------------------------------------")

debug = True
if debug:
	print("EMERGENCY TEST SETTINGS")
	nEvents = [10]
	bkgRates = [0]
	mmEffs = [[1.0]*8]
	chamber_feature = ['-uvr', '-uvr -ideal-vmm -ideal-addc -ideal-tp', '-uvr -ideal-vmm', '-uvr -ideal-addc', '-uvr -ideal-tp']
	print("Number of events: {}".format(nEvents))
	print("Background rates: {}".format(bkgRates))
	print("MM Efficiencies: {}".format(mmEffs))
	print("Chamber features: {}".format(chamber_feature))
	print("----------------------------------------------------------")

print("Setting up job launches")

# setup the output directory
workDir = outDir + "/simResults_{}".format(now.strftime("%m%d%y_%H%M%S"))
if os.path.isdir(workDir) == False:
	os.system("mkdir {}".format(workDir))
print("The working directory is: {}".format(workDir))

#################### Begin the production of events ####################

os.system("make")

nNEvents = len(nEvents)
nBkgRates = len(bkgRates)
nMMEffs = len(mmEffs)
nChambFeats = len(chamber_feature)
counter = 0

print("The number of jobs launched is: {}".format(nNEvents*nBkgRates*nMMEffs))
for nE in range(0,nNEvents):
	for nB in range(0,nBkgRates):
		for nM in range(0,nMMEffs):
			for cF in range(0,nChambFeats):
				tempDir = workDir + '/job_{}_{}_{}_{}'.format(nEvents[nE],bkgRates[nB],nM,cF)
				if os.path.isdir(tempDir) == False:
					os.system("mkdir {}".format(tempDir))
				outFileName = tempDir + '/' + outFilePrefix + '_nEvents{}_bkgRate{}_mmEffsIndx{}'.format(nEvents[nE],bkgRates[nB],nM)
			       	with open("{}/script_{}.sh".format(tempDir,counter),"w") as text_file:
					text_file.write(
                                """ #!/bin/bash
#$ -o /scratch/stdout_{0}_{2}_{7}.txt 
#$ -e /scratch/stderr_{0}_{2}_{7}.txt

printf "Start time: "; /bin/date
printf "Job is running on node: "; /bin/hostname
printf "Job running as user: "; /usr/bin/id
printf "Job is running in directory: "; /bin/pwd
echo
echo "Working hard..."

printf "###########"
mkdir -p /scratch/testBadea/job_{0}_{2}_{7}
./sim -n {0} -ch {1} -b {2} -o /scratch/testBadea/job_{0}_{2}_{7}/outFile_nEvents{0}_bkgRate{2}_mmEffIndx{7}.root -uvr -e {3} -thrx {4} -thruv {5} -tree {8}
scp /scratch/testBadea/job_{0}_{2}_{7}/* abc-at12:{6}
scp /scratch/std*_{0}_{2}_{7}.txt abc-at12:{6}
rm -rf /scratch/testBadea/job_{0}_{2}_{7}
rm -rf /scratch/std*_{0}_{2}_{7}.txt
                                """.format(nEvents[nE],                                                                                                    
                                        chamber,                                                                                                              
                                        bkgRates[nB],                                                                                                   
					formatMMEffString(mmEffs[nM]),                                                                                         
                                        xCoincidenceThreshold,                                                                                            
                                        uvCoincidenceThreshold,
					tempDir,
					nM,
					chamber_feature[cF]   ))
				os.system("chmod +x {}/script_{}.sh".format(tempDir,counter))
				print("qsub -V -cwd -q tier3 {}/script_{}.sh".format(tempDir,counter))
			       	os.system("qsub -V -cwd -q tier3 {}/script_{}.sh".format(tempDir,counter))
		       		counter += 1
print("----------------------------------------------------------")
print("All jobs launched!")
sys.stdout.close()
