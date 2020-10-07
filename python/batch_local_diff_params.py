"""
Written by: Anthony Badea (June 2020)

Runs batch_local.py for a variety of settings

"""



import os, sys
import random
import copy
import subprocess 
import time
import numpy as np
import multiprocessing 

NPCBS = 128 # 16 per layer
RealEff = [0.5695, 0.9597, 0.9868, 0.985, 0.9825, 0.9835, 0.9849, 0.9844, 0.9752, 0.9691, 0.9761, 0.9856, 0.9786, 0.9848, 0.973, 0.9814, 0.984, 0.9825, 0.9592, 0.9509, 0.9617, 0.3264, 0.9737, 0.9723, 0.9714, 0.9674, 0.9762, 0.9822, 0.9803, 0.9814, 0.9784, 0.9867, 0.4486, 0.9371, 0.9774, 0.987, 0.307, 0.349, 0.9684, 0.9687, 0.9762, 0.981, 0.9784, 0.9801, 0.9714, 0.9723, 0.9675, 0.968, 0.9794, 0.9748, 0.3219, 0.3232, 0.8994, 0.3708, 0.9638, 0.9621, 0.3296, 0.9672, 0.9772, 0.9783, 0.9702, 0.9636, 0.9712, 0.9731, 0.9902, 0.9934, 0.9638, 0.9695, 0.9813, 0.9857, 0.967, 0.977, 0.9693, 0.9726, 0.9667, 0.3797, 0.9766, 0.9749, 0.9662, 0.3184, 0.9577, 0.9916, 0.9649, 0.7332, 0.5326, 0.9563, 0.9691, 0.9746, 0.9758, 0.9755, 0.9768, 0.9761, 0.9587, 0.9564, 0.9679, 0.9685, 0.9864, 0.988, 0.972, 0.9801, 0.9771, 0.9762, 0.9688, 0.9731, 0.9665, 0.9529, 0.9832, 0.9835, 0.9797, 0.9777, 0.9766, 0.9758, 0.9894, 0.9911, 0.9786, 0.9797, 0.6756, 0.9774, 0.5363, 0.9539, 0.968, 0.969, 0.97, 0.9717, 0.9688, 0.9652, 0.963, 0.9626]
# Input: list of efficiencies
# Output: string of efficiencies separated by commas
def formatMMEffString(mm_eff):
    s = '\''
    for i in mm_eff:
            s+='{},'.format(i)
    s = s[:-1]+'\''
    return s



# Input: 
#	- List of efficiencies
#	- Number of PCBs to kill
#	- The dead PCB efficiency
# Output: list of efficiencies with killed PCBs
def kill_pcbs(effs, nKillPCBs = 0, dead_PCB_eff = 0):
	killed = []
	for i in range(nKillPCBs):
		pcb = int(round(random.uniform(0,len(effs)-1)))
		while pcb in killed:
			pcb = int(round(random.uniform(0,len(effs)-1)))
		killed.append(pcb)
		effs[pcb] = dead_PCB_eff
	if nKillPCBs == -1:
		effs = []
		for i in range(8):
			pcb = int(round(random.uniform(0,15)))
			x = [0]*16
			x[pcb] = 1
			effs += x
		print("Number of PCBS, total efficiency: {}, {}".format(len(effs), np.sum(effs)))
	return effs



# Input:
#	- Number of different configurations of randomly killed PCBs
#	- Number of PCBs to kill
#	- The dead PCB efficiency
# Output: Dictionary of job args (job_id, job args)
def make_jobs(nKillPCBJobs = 0, # Number of different configurations of randomly killed PCBs
			  nKillPCBs = 0, # Number of PCBs to randomly kill
			  fullEff = 1.0, # Efficiency of full efficienct PCB 
			  dead_PCB_eff = 0.0, # Efficiency for killed PCBs
			  outDir = "/Users/anthonybadea/Documents/ATLAS/oct_sim/work"
			  ):
	jobs = {}
	job_full_eff = {'nJobs':1,
				   'outDir': outDir,
			  	   'nEvents':10**4,
			  	   'chamber':'large',
			  	   'bkgRate':0,	
			  	   'efficiencies':formatMMEffString([fullEff]*NPCBS)} #RealEff)}

	# jobs.setdefault('job_full_eff',{}).update(job_full_eff)

	for j in range(nKillPCBJobs):
		job = copy.deepcopy(job_full_eff)
		effs = [float(i.replace("'","")) for i in job['efficiencies'].split(',')]
		job['efficiencies'] = formatMMEffString(kill_pcbs(effs,nKillPCBs,dead_PCB_eff))
		jobs.setdefault('job_{}_PCBs_killed_id_{}'.format(nKillPCBs,j),{}).update(job)

	return jobs



# Input: job arguments
# Output: None, calls the job operation 
def run_job(nJobs = 1,
			outDir = "/Users/anthonybadea/Documents/ATLAS/oct_sim/work", # Not currently used 
		    nEvents = 100,
		    chamber = 'large',
		    bkgRate = 1,
		    efficiencies = [1.0]*NPCBS):
	os.system( " python python/batch_local.py -j {} -a \"-n {} -ch {} -b {} -tree -uvr -e {}\" -o {}".format(nJobs,
																				  	  		nEvents,
																				  	  		chamber,
																				  	  		bkgRate,
																				  	 		efficiencies,
																				  	 		outDir))



# Input: None
# Output: None
def main(nKillPCBJobs = 1, nKillPCBs = 16, fullEff = 1.0, outDir = "/Users/anthonybadea/Documents/ATLAS/oct_sim/work"):
	jobs = make_jobs(nKillPCBJobs = nKillPCBJobs, nKillPCBs = nKillPCBs, fullEff = fullEff, outDir = outDir)

	# Run in parallel 
	starttime = time.time()
	pool = multiprocessing.Pool(processes=6) #use all available cores, otherwise specify the number you want as an argument
	for key, vals in jobs.items():
		pool.apply_async(run_job, args=tuple(), kwds=vals)
		time.sleep(3)
	pool.close()
	pool.join()

	print('That took {} seconds'.format(time.time() - starttime)) 

def loop():
	for fullEff in [0.1]:#np.linspace(0,1,11):
		outDir = "/Users/anthonybadea/Documents/ATLAS/oct_sim/work" + "/fullEff%.2f"%fullEff
		os.mkdir(outDir)
		main(nKillPCBJobs=10,nKillPCBs=0,fullEff=round(fullEff,2),outDir=outDir)

if __name__ == "__main__":
    loop()