'''
TITLE: ACCEL_FUNC.PY
PURPOSE: A collection of classes and functions for using and analyzing data from
    the uLog custom accelerometer tag.
CONTENTS:
    1. Accel_start
DATE: 6-16-2011
AUTHOR: Gen Del Raye
'''

import numpy,pylab,datetime,plot_settings

class Accel_start:
    '''
    Overview: Converts Hex input into float and outputs the resulting data as a 
        diagnostic plot, a comma-delimited text file, or prints to the console.
    Example diagnostic protocol: Stationary for 5 sec, followed by 3 slow 
        rotations 180 degrees per 10 sec.
    Target files: capture_x.txt where x=[0:5]
    Usage example:
        diagnostic=Accel_start('capture_05.txt') # Convert to float array, returns NULL
        diagnostic() # Prints table, plots data, and exports .txt
    Date: 6-16-2011
    '''
    def __init__(self,filename,channel=list(['x','y','z','data points'])\
                 ,frequency=5.0,return_array=False):
        self.channel=channel
        self.frequency=frequency
        fid = open(filename,"rU")  # Open connection
        capture = numpy.loadtxt(fid, dtype='string')
        fid.close()  # Close connection
        for i in xrange(capture.shape[0]):
            for j in xrange(capture.shape[1]):
                capture[i,j]=int(capture[i,j],16)
        self.capture=capture.astype('int')
        if return_array:
            return self.capture
    def __call__(self,plot=True,print_table=True,export_table=True,plot_size='large'):
        printstatus=list(['DONE: hex2float.py\nPlot=%s; Export_table=%s' %(plot,export_table)])
        plot_settings.set_mode(plot_size)
        time=numpy.arange(0,self.capture.shape[0])/self.frequency
        if plot:
            pylab.figure()
            pylab.title('Accelerometer Diagnostic: %s (freq = %.1fHz)' %(datetime.date.today(),self.frequency))
            pylab.hold('on')
            pylab.xlabel('Time (sec)')
            pylab.ylabel('Acceleration (arbitrary units)')
            for c in range(self.capture.shape[1]):
                pylab.scatter(time,self.capture[:,c],s=25,marker='o',c='0.7',linewidths=0)
                pylab.plot(time,self.capture[:,c],label=self.channel[c])
            pylab.legend(self.channel,loc=4)
            pylab.grid('on')
            plotfile='accelerometer_%s_%.1fHz.pdf' %(datetime.date.today(),self.frequency)
            pylab.savefig(plotfile)
            printstatus.append('Plot file: %s' %(plotfile))
        if print_table:
            numpy.set_printoptions(edgeitems=100)
            print 'Data matrix:\n',self.capture,'\n'
        if export_table:
            textfile='datacapture_float.txt'
            numpy.savetxt(textfile,self.capture,delimiter=',',fmt='%f')
            printstatus.append('Text file: %s' %(textfile))
        for p in printstatus:
            print p
        pylab.show()