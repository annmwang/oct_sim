"""
Written by: Anthony Badea (June 2020)

Runs batch_local.py for a variety of settings

"""

import os, sys
import random
import copy
import subprocess 
import time

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
	for i in xrange(nKillPCBs):
			effs[int(round(random.uniform(0,len(effs))))] = dead_PCB_eff
	return effs

# Input:
#	- Number of different configurations of randomly killed PCBs
#	- Number of PCBs to kill
#	- The dead PCB efficiency
# Output: Dictionary of job args (job_id, job args)
def make_jobs(nKillPCBJobs = 0, # Number of different configurations of randomly killed PCBs
			  nKillPCBs = 0, # Number of PCBs to randomly kill
			  dead_PCB_eff = 0.0 # Efficiency for killed PCBs
			  ):
	jobs = {}
	jobs.setdefault('job_full_eff',{}).update({'nJobs':1,
											   'outDir': "/Users/anthonybadea/Documents/ATLAS/oct_sim/work",
										  	   'nEvents':10**4,
										  	   'chamber':'large',
										  	   'bkgRate':1,	
										  	   'efficiencies':formatMMEffString([1.0]*64)})
	for j in xrange(nKillPCBJobs):
		job = copy.deepcopy(jobs['job_full_eff'])
		effs = [float(i.replace("'","")) for i in job['efficiencies'].split(',')]
		job['efficiencies'] = formatMMEffString(kill_pcbs(effs,nKillPCBs,dead_PCB_eff))
		jobs.setdefault('job_{}_PCBs_killed_id_{}'.format(nKillPCBs,j),{}).update(job)

	return jobs

# Input: job arguments
# Output: None, calls the job operation 
def run_job(nJobs = 1,
			outDir = "/Users/anthonybadea/Documents/ATLAS/oct_sim/work",
		    nEvents = 100,
		    chamber = 'large',
		    bkgRate = 1,
		    efficiencies = [1.0]*64):
	os.system( " python python/batch_local.py -j {} -a \"-n {} -ch {} -b {} -e {}\" ".format(nJobs,
																				  	  		nEvents,
																				  	  		chamber,
																				  	  		bkgRate,
																				  	 		efficiencies))

# Input: None
# Output: None
def main():
	jobs = make_jobs(nKillPCBJobs = 1, nKillPCBs = 16)
	for key, vals in jobs.items():
		run_job(**vals)
		time.sleep(1)

if __name__ == "__main__":
    main()