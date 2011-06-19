#!/usr/bin/python

# Module: plot_settings.py
# Author: Varun Hiremath <vh63@cornell.edu>
# Created: Thu,  2 Apr 2009 05:06:31 -0400

import pylab
import math

# Symbols 
symbols = ['-','--','-.',':','.',',','o','^','v','<','>','s','+','x','D','d','1','2','3','4','h','H','p']
# Symbols + line
lps = [k+'-' for k in [',','.','o','^','v','<','>','s','+','x','D','d','1','2','3','4','h','H','p']]
# Colors
colors= ['b','g','r','c','m','y','k','w']

def get_figsize(fig_width_pt):
    inches_per_pt = 1.0/72.0                # Convert pt to inch
    golden_mean = (math.sqrt(5)-1.0)/2.0    # Aesthetic ratio
    fig_width = fig_width_pt*inches_per_pt  # width in inches
    fig_height = fig_width*golden_mean      # height in inches
    fig_size =  [fig_width,fig_height]      # exact figsize
    return fig_size

# Publishable quality image settings for 2-column papers
params0 = {'backend': 'eps',
          'axes.labelsize': 6,
          'text.fontsize': 6,
          'xtick.labelsize': 6,
          'ytick.labelsize': 6,
          'legend.pad': 0.1,    # empty space around the legend box
          'legend.fontsize': 5,
          'lines.markersize': 3,
          'font.size': 6,
          'text.usetex': False,
          'figure.figsize': get_figsize(250)}

# Medium sized images
params1 = {'backend': 'eps',
          'axes.labelsize': 9,
          'text.fontsize': 9,
          'xtick.labelsize': 9,
          'ytick.labelsize': 9,
          'legend.pad': 0.1,     # empty space around the legend box
          'legend.fontsize': 9,
          'lines.markersize': 3,
          'font.size': 9,
          'text.usetex': False,
          'figure.figsize': get_figsize(500)}

# Large images (default)
params2 = {'backend': 'eps',
          'axes.labelsize': 10,
          'text.fontsize': 10,
          'xtick.labelsize': 10,
          'ytick.labelsize': 10,
          'legend.pad': 0.2,     # empty space around the legend box
          'legend.fontsize': 10,
           'lines.markersize': 3,
          'font.size': 10,
          'text.usetex': False,
          'figure.figsize': get_figsize(1000)}

def set_mode(mode):
    if mode == "publish":
        pylab.rcParams.update(params0)
    elif mode == "medium":
        pylab.rcParams.update(params1)
    else:
        pylab.rcParams.update(params2)

def set_figsize(fig_width_pt):
    pylab.rcParams['figure.figsize'] = get_figsize(fig_width_pt)

pylab.rcParams.update(params2)
