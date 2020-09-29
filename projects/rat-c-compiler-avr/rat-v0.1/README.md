# The Rat AVR compiler

Version: v0.1

## Language description
See here: https://trolsoft.ru/soft/the-rat-avr

## Samples
* simple
  A simple example of a flashing LED in assembler
* led-blink
  A simple example of a flashing LED in Rat
* interrupts
  A simple example of declaring an interrupt table
* f-counter
  Frequency counter firmware on Rat for this project: https://trolsoft.ru/sch/f-meter

## Usage
Usage: ratc [options] <source_file> [<output_file>]

OPTIONS:
-D<macro>=<value>      Define <macro> to <value> (or 1 if <value> omitted)
-I<dir>                Add directory to include search path
-dev=<path>            Set path to dev-files
-gcc                   Produce GCC Assembler file
-help                  Show this usage screen



To Install the Netbeans plugin (for Netbeans 8)
* Start NetBeans IDE.
* Click Tools - Plugins.
* Click on the Downloaded tab.
* Click Add Plugins to browse for misc/netbeans/ru-trolsoft-therat-avr.nbm
* Click on Install and follow the prompts.