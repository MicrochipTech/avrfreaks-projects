#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#       
#       
#       Copyright 2010 Jarik <jarik@Gentoo>
#       
#       This program is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 2 of the License, or
#       (at your option) any later version.
#       
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#       
#       You should have received a copy of the GNU General Public License
#       along with this program; if not, write to the Free Software
#       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#       MA 02110-1301, USA.

import sys,serial,re

def connection_init():
	global ser
	ser=serial.Serial(port="/dev/ttyS0",baudrate=115200,timeout=10)
	ser.write("Hello Flasher\n")
	resp=ser.readline()
	if resp!="Hello Computer\n":
		print "Connection error."
		ser.close()
		quit()
	
def connection_done():
	ser.write("Bye Flasher\n")
	resp=ser.readline()
	if resp!="Bye Computer\n":
		print "Disconnection error."
	ser.close()


def read_flash(filename):
	global data
	try:
		f=open(filename,"wb")
	except IOError,err:
		print "Can't create file %s ." % (filename)
		quit()
	data=""
	connection_init()
	ser.write("read\n")
	req=ser.readline()
	req_l=req.split()
	if req_l[0]=="send":
		length=int(req_l[1])
		data=ser.read(length)
		print "%d bites." % (len(data))
		f.write(data)
		f.close();
	else:
		print "error!"
	connection_done()

def write_flash(filename):
	global data
	try:
		f=open(filename,"rb")
	except IOError,err:
		print "file %s not found." % (filename)
		quit()
	data=f.read()
	f.close()
	connection_init()
	ser.write("write\n")
	writing=1
	while writing:
		req=ser.readline()
#		print req
		req_l=req.split()
		if req_l[0]=="req":
			offset=int(req_l[1])
			length=int(req_l[2])
			send_block(offset,length)
			s="%d bytes writen                  \r" % (offset)
			print s,
		else:
			if req_l[0]=="done":
				writing=0
				print "OK                                            "
				break
			else:
				print "Error"
				quit()
	connection_done()

def send_block(ofs,leng):
	b=data[ofs:(ofs+leng)]
	crc=0;
	for i in b:
		crc=crc ^ ord(i)
	b+=""+chr(crc)
	ser.write(b)
#	print "%0X %i" % (ofs,crc)
def add_block(bl):
	global data
	crc=0
	print bl[:-1]
	for i in bl[:-1]:
		crc=crc^ord(i)
	if crc!=bl[-1]:
		print "error"
	data=data+bl[:-1]
	
def verify_flash(filename):
	try:
		f=open(filename,"rb")
	except IOError,err:
		print "file %s not found." % (filename)
		quit()
	data=f.read()
	


def main():
	args=sys.argv
	if (len(args)<3 or len(args)>3 ):
		print "Usage: flasher.py < --read | --write | --verify > <filename>"
		return 0
	options={ "--read" : read_flash, "--write" : write_flash, "--verify" : verify_flash }
	try:
		options[args[1]](args[2])
	except KeyError,err:
		print "Usage: flasher.py < --read | --write n> <filename>"
		return 0
	return 0

if __name__ == '__main__':
	main()

