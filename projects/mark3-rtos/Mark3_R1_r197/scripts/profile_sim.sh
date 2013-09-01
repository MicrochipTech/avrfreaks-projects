#!/bin/sh

# Open the terminal, run profiling app for 30 seconds
echo "--[Running Sanity Tests]--"
flavr --silent --hexfile ./stage/app/avr/atmega328p/gcc/kernel_profile.hex > ./profile.txt &
pid=$!
sleep 60
kill ${pid}

metric="none"
metric_name="none"
metric_time=0

profile_count=0
profile_sum=0

outfile=./kernel/public/profiling_results.h

#============================================================================
compute_profile()
{
    profile_count=0
	profile_sum=0
	
	for line in `cat ./profile.txt | grep -a ${metric} | sed -e "s/${metric}//"`;
	do
    	profile_count=`expr $profile_count + 1`	
		profile_sum=`expr $profile_sum + $line`

	done;
	metric_time=`expr $profile_sum / $profile_count`
	echo "${metric_name}: ${metric_time} cycles (averaged over ${profile_count} iterations)"
	echo "    - ${metric_name}: ${metric_time} cycles (averaged over ${profile_count} iterations)" >> ${outfile}
}

#============================================================================
echo "/*!" > ${outfile}
echo "\page PROFILERES Profiling Results" >> ${outfile}
echo "The following profiling results were obtained using an ATMega328p @ 16MHz.">> ${outfile}
echo "The test cases are designed to make use of the kernel profiler, which accurately">> ${outfile}
echo "measures the performance of the fundamental system APIs, in order to provide ">> ${outfile}
echo "information for user comparison, as well as to ensure that regressions are not" >> ${outfile}
echo "being introduced into the system." >> ${outfile}
echo "" >> ${outfile}
echo "\section PROFILEATE Date Performed " >> ${outfile}
date >> ${outfile}
echo "\section COMPVER Compiler Information " >> ${outfile}
echo "The kernel and test code used in these results were built using the following compiler:" >> ${outfile}
echo "\code" >> ${outfile}
avr-gcc -v >> ${outfile} 2>> ${outfile}
echo "\endcode" >> ${outfile}
echo "\section PROFRES Profiling Results" >> ${outfile}

metric="SI:"
metric_name="Semaphore Initialization"
compute_profile

metric="SPo:"
metric_name="Semaphore Post (uncontested)"
compute_profile

metric="SPe:"
metric_name="Semaphore Pend (uncontested)"
compute_profile

metric="SF:"
metric_name="Semaphore Flyback Time (Contested Pend)"
compute_profile

metric="MI:"
metric_name="Mutex Init"
compute_profile

metric="MC:"
metric_name="Mutex Claim"
compute_profile

metric="MR:"
metric_name="Mutex Release"
compute_profile

metric="TI:"
metric_name="Thread Initialize"
compute_profile

metric="TS:"
metric_name="Thread Start"
compute_profile

metric="CS:"
metric_name="Context Switch"
compute_profile

metric="SC:"
metric_name="Thread Schedule"
compute_profile

echo "    . " >> ${outfile}
echo "*/" >> ${outfile}

