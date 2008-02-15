#Written by Priyend Somaroo
#Copyright (c) 2008 Vardaan Enterprises, www.vardaan.com

#Used to display an intel hex file in a colour coded format
#Spaces are also shown between fields of the lines

import sys
import WConio

# ------------------------------------------------------------#
# Functions

def KeyPressExit():
  WConio.textattr(WConio.CYAN)
  WConio.cputs("\nPress any key to end\r\n")
  #Wait for a key to be pressed
  WConio.getch()
  #Retore old attribute settings
  WConio.textattr(old_setting)
  
def cprint(text,color):
  WConio.textcolor(color)
  print(text),
  
def DisplayLine(line):
  cprint(line[0],WConio.LIGHTRED) # colon
  cprint(line[1:3],WConio.LIGHTGREEN) # length
  cprint(line[3:7],WConio.LIGHTBLUE)  # address
  cprint(line[7:9],WConio.YELLOW)     # type
  cprint(line[9:-3],WConio.LIGHTGRAY)     # data
  cprint(line[-3:],WConio.LIGHTCYAN)  # checksum


# ------------------------------------------------------------#
# Main program starts here

#Store current attribute settings
old_setting = WConio.gettextinfo()[4] & 0x00FF

cprint("Intel Hex File Display Utility Ver 1.0\r\n",WConio.WHITE)
print
cprint ("Written by Priyend Somaroo\r\n",WConio.LIGHTGRAY)
print "Copyright (c) 2008 Vardaan Enterprises, www.vardaan.com"
print
cprint ("Licence :\r\n",WConio.YELLOW)
cprint ("   This software is free for personal and business use.\r\n",WConio.LIGHTGRAY)
print "   No warranties apply and usage is free from any harm or loss that may occur."
print "   Use at your own risk."
print
cprint ("Description :\r\n",WConio.YELLOW)
cprint ("   This utility displays an Intel Hex text file in a table manner.\r\n",WConio.LIGHTGRAY)
print "   Each field is clearly shown with different colours representing each field."
print
print "If you need help or have comments use either of the contacts : "
cprint("   Vardaan Enterprises website : ",WConio.LIGHTGRAY)
cprint("www.vardaan.com\r\n",WConio.LIGHTBLUE)
cprint("   Vardaan Enterprises email : ",WConio.LIGHTGRAY)
cprint("support@vardaan.com",WConio.LIGHTBLUE)
print
print
cprint("-----------------------------------------------------------------------------\n",WConio.BROWN)


try:
    a1=sys.argv[1]

except:
    thisFile = sys.argv[0]
    temp = thisFile.rsplit("\\")
    thisFile = temp[-1]
    cprint("\nError in usage!\07\r\n\n",WConio.LIGHTRED) #Code 07 is a beep
    cprint ("Usage :   ",WConio.YELLOW)
    cprint (thisFile,WConio.LIGHTGRAY)
    cprint (" <intel hex file>\r\n\n",WConio.WHITE)
    KeyPressExit()
    sys.exit(1)

try:
    f1=open(a1)
except:
    cprint ("\nError in file name!\07\r\n\n",WConio.LIGHTRED)
    WConio.textattr(WConio.WHITE)
    print "Please enclose in quote if there are spaces in the name or path."
    print "Also check that the file exist. Names are also case sensitive."
    print
    KeyPressExit()
    sys.exit(1)

cprint("FORMATTED DISPLAY : \r\n\n",WConio.BROWN)

#Read in the Application hex
l1=f1.readlines()

linecount=0
for line in l1:
  linecount +=1
  if (line[0] != ':') :
    cprint ("\nERROR in hex file!\07",WConio.LIGHTRED)
    s = "At line : %d" % (linecount)
    cprint (s,WConio.WHITE)
    break
  else :
    DisplayLine(line)
    
    #Wait for a keypress every 200 lines
#    if (linecount%200 == 0):
#      c = WConio.getkey()
#      c = ord(c)
#      if (c == 3) :
#        s = "BREAK pressed. Stopped processing at line : %d" % (linecount)
#        cprint(s,WConio.WHITE)
#        break

      

#Close files off
f1.close()

KeyPressExit()

# ------------------------------------------------------------#
# End of file
