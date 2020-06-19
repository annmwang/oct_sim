"""
batch.py :: a script for launching sim jobs

Run like:
> python python/batch.py -j 10 -a "-n 1000 -ch large -b 1"

To set the seed based on job number, use "-seed \$SLURM_JOB_ID" in the -a arguments.

Written by: Ann Wang and Alexander Tuna
Modified by: Anthony Badea (June 2020)
"""

import argparse
import os
import subprocess
import sys
import time

def main():

    now = time.strftime("%Y-%m-%d-%Hh%Mm%Ss")

    ops = options()
    print(ops)
    if not ops.j:
        fatal("Please provide a number of jobs to launch with -j")
    if not ops.a:
        fatal("Please provide a string of arguments for the sim with -a")
    if not ops.o:
        dirname = "batch-%s" % (now)
        topdir  = "/Users/anthonybadea/Documents/ATLAS/oct_sim/work" if not ops.NET3 else "/gpfs3/harvard/oct_sim"
        ops.o   = os.path.join(topdir, dirname)

    # config dict steers everything
    config = {}
    config["jobs"]      = int(ops.j)
    config["time"]      = time.ctime()
    config["exe"]       = os.path.join(os.path.abspath(os.curdir), "sim_PCBEff_Modular")
    config["argparse"]  = ops.a
    config["timestamp"] = now
    config["outdir"]    = ops.o
    config["user"]      = os.environ["USER"]
    config["submit"]    = not ops.d

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
            batch = "source" if not ops.NET3 else "qsub -V -q tier3"
            cmd = "%s %s" % (batch, config["jobname"])
            proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            time.sleep(1)
            stdout, stderr = proc.communicate()
            double_check(stdout, stderr, cmd)

            # write the log file
            logfile = open(os.path.join(config['jobdir'],'stdout.txt'),"w")
            logfile.write("# stdout: %s\n" % stdout.replace("\n", " "))
            logfile.close()

            # write the error file
            errfile = open(os.path.join(config['jobdir'],'stderr.txt'),"w")
            errfile.write("# stderr: %s\n" % stderr.replace("\n"," "))
            errfile.close()

            print "Submitted %4i / %4i @ %s" % (job+1, config["jobs"], time.strftime("%Y-%m-%d-%Hh%Mm%Ss"))

    print


def template():
    ops = options()
    if not ops.NET3:
        return """#!/bin/bash
#
# jobname    :: %(jobname)s
# time       :: %(time)s
# user       :: %(user)s
# executable :: %(exe)s
# jobdir     :: %(jobdir)s

%(exe)s %(args)s
"""

    else:
        return """#!/bin/bash
#
#$ -o %(jobdir)s/stdout_$JOB_ID.txt
#$ -e %(jobdir)s/stderr_$JOB_ID.txt
#
# jobname   :: %(jobname)s
# time      :: %(time)s
# user      :: %(user)s
# generated :: %(exe)s
# jobdir    :: %(jobdir)s

%(exe)s %(args)s
"""

def double_check(stdout, stderr, cmd):

    ops = options()

    # parse stdout for job cluster (just 1 line)
    if stdout:
        if (not ops.NET3 and "SIMULATION SUMMARY" not in stdout) or (ops.NET3 and "has been submitted" not in stdout):
            fatal("Could not confirm job when parsing stdout: %s" % (stdout))
    else:
        tries = 1
        print("Printing stdout %s\n"%stdout.replace("\n"," "))
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
    parser.add_argument("-a",     default=None,        help="String of arguments to pass to ./sim (excluding -o)")
    parser.add_argument("-j",     default=None,        help="Number of jobs to launch")
    parser.add_argument("-o",     default=None,        help="Output file name")
    parser.add_argument("-d",     action="store_true", help="Dry run: do everything except submit")
    parser.add_argument("--NET3", action="store_true", help="For use at the NET3")
    return parser.parse_args()


if __name__ == "__main__":
    main()
