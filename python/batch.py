"""
batch.py :: a script for launching sim jobs

Run like:
> python python/batch.py -j 10 -a "-n 1000 -ch large -b 1"
"""

import argparse
import os
import subprocess
import sys
import time

def main():

    ops = options()
    if not ops.j:
        fatal("Please provide a number of jobs to launch with -j")
    if not ops.a:
        fatal("Please provide a string of arguments for the sim with -a")

    # config dict steers everything
    config = {}
    config["jobs"]      = int(ops.j)
    config["time"]      = time.ctime()
    config["exe"]       = os.path.join(os.path.abspath(os.curdir), "sim")
    config["argparse"]  = ops.a
    config["timestamp"] = time.strftime("%Y-%m-%d-%Hh%Mm%Ss")
    config["outdir"]    = ops.o or "/n/atlasfs/atlascode/oct_sim/batch-%s" % (config["timestamp"])
    config["user"]      = os.environ["USER"]
    config["submit"]    = not ops.d
    config["sleep"]     = ops.s or 1

    # checks
    if not os.path.isfile(config["exe"]):
        fatal("Couldnt find executable: %s" % (config["exe"]))

    # announce
    print
    print "Output directory :: %s" % (config["outdir"])
    print "N(jobs)          :: %s" % (config["jobs"])
    print "User args        :: %s" % (config["argparse"])
    print

    for job in xrange(config["jobs"]):

        config["jobdir"]  = os.path.join(config["outdir"], "job_%04i" % (job))
        config["jobname"] = os.path.join(config["jobdir"], "job.sh")
        config["output"]  = os.path.join(config["jobdir"], "output.root")
        config["args"]    = "%s -o %s" % (config["argparse"], config["output"])

        # create the output directory
        if os.path.isdir(config["jobdir"]): 
            fatal(" Output directory already exists: %s" % (config["jobdir"]))
        os.makedirs(config["jobdir"])

        # write the job file
        jobfile = open(config["jobname"], "w")
        jobfile.write(template() % config)
        jobfile.close()

        # run the job
        if config["submit"]:
            cmd = "sbatch %s" % config["jobname"]
            proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
            time.sleep(int(config["sleep"]))
            stdout, stderr = proc.communicate()
            double_check(stdout, stderr, cmd)

            # append the job cluster to the .job file
            jobfile = open(config["jobname"], "a")
            jobfile.write("# stdout: %s\n" % stdout.replace("\n", " "))
            jobfile.close()
            print "Submitted %4i / %4i" % (job+1, config["jobs"])

    print

def template():
    return """#!/bin/bash
#
#SBATCH -p pleiades
#SBATCH -t 1-0:0:0
#SBATCH --mem-per-cpu 2048
#SBATCH -o %(jobdir)s/stdout_%%j.txt
#SBATCH -e %(jobdir)s/stderr_%%j.txt
#SBATCH --mail-type=END
#

# jobname    :: %(jobname)s
# time       :: %(time)s
# user       :: %(user)s
# executable :: %(exe)s
# jobdir     :: %(jobdir)s

%(exe)s %(args)s
"""

def double_check(stdout, stderr, cmd):

    # parse stdout for job cluster (just 1 line)
    if stdout:
        if "Submitted batch job" not in stdout:
            fatal("Could not confirm job when parsing stdout: %s" % (stdout))
    else:
        tries = 1
        while not stdout:
            print "Trying to resubmit because stdout is blank. Try: %s" % (tries)
            proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
            stdout, stderr = proc.communicate()
            tries += 1
            time.sleep(30)
            # fatal("no Popen.stdout for sbatch")
    if stderr:
        print "--------------- Popen.stderr --------------------"
        print stderr

def fatal(msg):
    sys.exit("Fatal error: %s" % (msg))

def options():
    parser = argparse.ArgumentParser(usage=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-a", default=None,        help="String of arguments to pass to ./sim (excluding -o)")
    parser.add_argument("-j", default=None,        help="Number of jobs to launch")
    parser.add_argument("-o", default=None,        help="Output file name")
    parser.add_argument("-s", default=None,        help="Seconds to sleep between launched jobs")
    parser.add_argument("-d", action="store_true", help="Dry run: do everything except submit")
    return parser.parse_args()


if __name__ == "__main__":
    main()
