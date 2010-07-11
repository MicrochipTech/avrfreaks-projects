#!/usr/bin/env python

"""Modulo para ejemplificar el uso de *epydoc*.
:author: Raul Gonzalez
:version: 0.1"""

__docformat__ = "restructuredtext"


import serial
import time
import usb
import glob
import sys
import calendar
import string
from ctes_ds1631 import *

class comunicaciones:
    def __init__(self):
        """Clase para el manejo y comunicaciones con el dispositivo externo."""
        return None
    
    def ftdi( self):
        """Busca la placa Arduino Nano en el bus usb
        Devuelve la especificacion del dispositivo.
        """
        return any(dev.idVendor == 1027 and dev.idProduct == 24577 
            for bus in usb.busses() for dev in bus.devices)

    def puertos( self, id):
        """Busca el puerto serie en donde esta conectada la placa Arduino Nano.
        
        :param id: identificador del dispositivo buscado.
        :param s:puerto en el que esta conectado. En caso de no encontralo, devuelve False"""
        for i in glob.glob('/dev/ttyUSB*'):
            try:
                Puerto= serial.Serial( i, 19200, timeout= 1)
            except:
                Existe= 0
            else:
                Puerto.write("?")
                idPort= Puerto.readline(None, "\r")
                idP= "%s\r" % id
                if idPort== idP:
                    Puerto.close()
                    self.puerto= i
                    return i
                else:
                    return False


    def device_command( self, id, Comando, Valor):
        """Controla las comunicaciones con el dispositivo.
        *Es responsabilidad del usuario enviar la informacion correctamente tipeada.*
        :param id: Identificador unico de cada dispositivo.
        :param Comando: Comando a ejecutar en el dispositivo puede ser de lectura o escritura.
        :param Valor: Valor que se desea enviar al dispositivo
        """
        comandos_lectura=['?', 'T', 't', 'H', 'L', 'C', 'P']
        comandos_escritura=['h', 'l', 'c', 'r']
        idFound= False
        
        if self.ftdi():
            puerto= self.puertos( id)
            if puerto != 0:
                try:
                    PuertoSerie= serial.Serial(puerto, 19200, timeout= 1)
                    idFound= True
                   
                except:
                   
                    idFound= False
            else:
                
                idFound= False
        else:
            
            idFound= False
        
        
        if Comando in comandos_lectura:
            
            if Comando== 'T':
                """Devuelve el valor de la temperatura para ser procesada como cadena"""
                if idFound:
                    PuertoSerie.write("T")
                    Temp= PuertoSerie.readline( None, "\r")
                    PuertoSerie.close()
                    return Temp
                else:
                    return False
                
            if Comando== 't':
                """Devuelve el valor de la temperatura para ser procesada como float"""
                if idFound:
                    PuertoSerie.write("t")
                    Temp= PuertoSerie.readline( None, "\r")
                    PuertoSerie.close()
                    return Temp
                else:
                    return False
            
            if Comando== 'H':
                if idFound:
                    PuertoSerie.write("H")
                    TH= PuertoSerie.readline( None, "\r")
                    PuertoSerie.close()
                    return TH
                else:
                    print "Error, dispositivo no encontrado"
                    return False
            
            if Comando== 'L':
                if idFound:				
                    PuertoSerie.write("L")
                    TL= PuertoSerie.readline( None, "\r")
                    PuertoSerie.close()
                    return TL
                else:
                    print "Error, dispositivo no encontrado"
                    return False
            
            if Comando== 'C':
                if idFound:
                    PuertoSerie.write("C")
                    Config= PuertoSerie.readline( None, "\r")
                    PuertoSerie.close()
                    return Config
                else:
                    print "Error, dispositivo no encontrado"
                    return False
                
            if Comando== 'P':
                if idFound:
                    PuertoSerie.write("P")
                    Tupla= PuertoSerie.readline( None, "\r")
                    PuertoSerie.close()
                    return Tupla
                else:
                    print "Error, dispositivo no encontrado"
                    return False
            
        if Comando in comandos_escritura:
            
            if Comando== 'h':
                if idFound:
                    PuertoSerie.write("h")
                    PuertoSerie.write( Valor)
                    PuertoSerie.close()
                    return True
                else:
                    print "Error, dispositivo no encontrado"
                    return False
                    
            if Comando== 'l':
                if idFound:
                    PuertoSerie.write("l")
                    PuertoSerie.write( Valor)
                    PuertoSerie.close()
                    return True
                else:
                    print "Error, dispositivo no encontrado"
                    return False
                
            if Comando== 'c':
                if idFound:
                    PuertoSerie.write("c")
                    PuertoSerie.write( Valor)
                    PuertoSerie.close()
                    return True
                else:
                    print "Error, dispositivo no encontrado"
                    return False
            
            if Comando== 'r':
                if idFound:
                    PuertoSerie.write("r")
                    PuertoSerie.close()
                    return True
                else:
                    print "Error, dispositivo no encontrado"
                    return False
                

    def Get_Tupla( self, id):
        
        Valor= self.device_command( id, 'P', 0)
        Valor= Valor.replace( "\r", "")
        Tupla= eval( Valor, dict(__builtins__=None))
        tiempo= time.time()
        otros= Tupla+ ( self.puerto,)
        returns= otros+  ( tiempo,)
        #print returns
        return returns
    
    
