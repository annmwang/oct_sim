# oct_sim
Quick simulation for the MMTP trigger
C++ version working, python version out-of-date

## PYTHON VERSION
WARNING: not up to date! If on herophysics, just to have numpy and pyroot together:
```{r, engine='bash', count_lines}
source hero_setup.sh
```
## C++ VERSION

```{r, engine='bash', count_lines}
make
./sim 
./sim -n 100 -ch <chamber type> -b <bkg rate in Hz/strip> -o output.root
```
Configurable parameters include:
* [-n] nevents generated 
* [-sig] ART resolution (ns) :: default 32 ns
* [-x] road size :: default 8 strips
* [-w] BC window size :: default 8 BC
* [-ch] chamber type
* [-b] bkg rate in Hz/strip
* [-p] plotting :: default OFF
* [-o] output file :: 
* [-uvr] turn on UV roads :: default OFF
* [-hdir] name of histograms TDirectory :: default "histograms"
* [-e] chamber efficiency (0 to 1) :: default 0.9
* [-ideal-vmm] allow the VMM to always pick the signal hit :: default OFF
* [-ideal-tp] allow the TP to always pick the signal hit :: default OFF
* [-seed] set the seed for TRandom3 :: default: seed is not set
