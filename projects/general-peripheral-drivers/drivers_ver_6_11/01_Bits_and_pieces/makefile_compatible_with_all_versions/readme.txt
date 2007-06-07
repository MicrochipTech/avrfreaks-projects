
Remember to edit the AVRGCC_VERSION and the F_CPU lines in order to pass
the needed data to your code files.
You might not need those definitions but i think they are usefull
since the MPU clock frequency (F_CPU) is needed in almost all files
and the gcc version helps finding the included files
as the location of those files is changing constantly and i am tired editing my code.
Ok i know that the winavr (avrgcc 3.3) is build from an experimental version
and i must say that it works fine for me.  
