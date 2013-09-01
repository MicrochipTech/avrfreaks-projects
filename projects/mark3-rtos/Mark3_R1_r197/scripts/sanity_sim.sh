#!/bin/sh

# Open the terminal, run profiling app for 30 seconds
flavr --silent --hexfile ./stage/app/avr/atmega328p/gcc/kernel_sanity.hex > ./sanity.txt &
pid=$!
sleep 60
kill ${pid}

metric="none"
metric_name="none"
metric_time=0

pass_count=0
event_sum=0

outfile="./sanity_report.txt"
#============================================================================
check_sanity()
{
    pass_count=0
	event_sum=0
	
	for line in `cat ./sanity.txt | grep ${metric} --text | sed -e "s/${metric}//"`;
	do
		if [ "${line}" = "PASS" ]; then 
		    pass_count=`expr $pass_count + 1`
		fi  
		event_sum=`expr $event_sum + 1`
	done;
	
	if [ $event_sum -eq $pass_count ]; then
		echo "${metric_name} : PASS -  ${event_sum} iterations" 
		echo "${metric_name} : PASS -  ${event_sum} iterations" >> ${outfile}
	else	
		echo "${metric_name} : FAILED - ${pass_count} of ${event_sum} iterations passed"
		echo "${metric_name} : FAILED - ${pass_count} of ${event_sum} iterations passed" >> ${outfile}
	fi
}
echo "Sanity Report:" > ${outfile}
date >> ${outfile}

metric="Timer: "
metric_name="Basic Timer Sanity"
check_sanity

metric="RR: "
metric_name="Round-Robin Scheduling - Basic Functionality"
check_sanity

metric="Quantum: "
metric_name="Round-Robin Scheduling - Variable Thread Quantums"
check_sanity

metric="TimedSem: "
metric_name="Semaphore - Timed"
check_sanity

metric="Sem: "
metric_name="Semaphore - Binary and Counting Semaphores"
check_sanity

metric="Mutex: "
metric_name="Mutual Exclusion Objects"
check_sanity

metric="Sleep: "
metric_name="Thread Sleep Functionality"
check_sanity

metric="Message: "
metric_name="Message Passing"
check_sanity
