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
./sim -n 100 -ch <chamber type> -b <bkg rate in Hz/strip> -p <make event displays> -o output.root
```
Configurable parameters include:
* [-n] nevents generated
* [-sig] ART resolution (ns)
* [-x] road size
* [-w] BC window size
* [-ch] chamber type
* [-b] bkg rate in Hz/strip
* [-p] plotting
* [-o] output file
