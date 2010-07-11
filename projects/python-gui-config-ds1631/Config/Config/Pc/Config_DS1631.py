#!/usr/bin/env python

import serial #pyserial
import time
import usb    #pyusb
import glob
import sys
import gobject
import pango
import string
from ctes_ds1631 import *


try:
	import pygtk
	pygtk.require("2.0")
except:
	pass
try:
	import gtk
	import gtk.glade
except:
	sys.exit(1)



def ftdi():
	"""Busca la placa Arduino Nano en el bus usb
	Devuelve la especificacion del dispositivo.
	"""
	return any(dev.idVendor == 1027 and dev.idProduct == 24577 
		for bus in usb.busses() for dev in bus.devices)

def puertos( id):
	"""Busca el puerto serie en donde esta conectada la placa
	Arduino Nano.
	
	Entrada-> identificador del dispositivo buscado.
	Salida-> puerto en el que esta conectado. En caso de
	no encontralo, devuelve False.
	"""
	for i in glob.glob('/dev/ttyUSB*'):
		try:
			Puerto= serial.Serial( i, bauds, timeout= 1)
		except:
			Existe= 0
		else:
			Puerto.write("?")
			idPort= Puerto.readline(None, "\r")
			idP= "%s\r" % id
			if idPort== idP:
				Puerto.close()
				print "Dispositivo encontrado %s" % idPort
				return i
			else:
				print 'Dispositivo no encontrado %s' % idPort
	return False

		
def device_command( id, Comando, Valor):
	"""Controla las comunicaciones con el dispositivo.
	
	Valores de entrada:
	
	id------- Identificador unico de cada dispositivo
	Comando-- Comando a ejecutar en el dispositivo puede ser de lectura o escritura
	Valor---- valor que se desea enviar al dispositivo.
	
	Es responsabilidad del usuario enviar la informacion correctamente tipeada.
	"""
	comandos_lectura=['?', 'T', 'H', 'L', 'C']
	comandos_escritura=['h', 'l', 'c', 'r']
	idFound= False
	
	if ftdi():
		puerto= puertos( id)
		if puerto != 0:
			try:
				PuertoSerie= serial.Serial(puerto, 19200, timeout= 1)
				idFound= True
				print "Canal de comunicacion %s, abierto" % puerto
			except:
				print "Error de apertura del canal serie %s" % puerto
				idFound= False
		else:
			print "Error, dispositivo %s no encontrado en el puerto." % id
			idFound= False
	else:
		print "Error, no se encuentra dispositivo usb a serie."
		idFound= False
	
	
	if Comando in comandos_lectura:
		
		if Comando== 'T':
			"""Prueba de comentario"""
			if idFound:
				PuertoSerie.write("T")
				Temp= PuertoSerie.readline( None, "\r")
				PuertoSerie.close()
				return Temp
			else:
				print "Error, dispositivo no encontrado"
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

class Aviso:
	
	
		
	def dialogo_error_ajuste_termostato(self):
		"""Dialogo para indicar que la configuracion de las temperaturas del termostato, son incorrectas."""
		print "Abro ventana de dialogo."
		builder = gtk.Builder()
		builder.add_from_file("DS1631_error_ajuste_termostato.xml") 
		self.widgets= dict(
			Ventana= builder.get_object("Dialogo_Error_Ajuste_Termostato")
			)
		builder.connect_signals(self)
		
	def error_termostato_close(self, widget):
		"""Cierra ventana de aviso por fallo en datos termostato"""
		Ventana= self.widgets["Ventana"]
		
		Ventana.destroy()
		
class App:
	"""Clase para el menejo de un DS1631 via serie, usando el FTDI
	Se usa el id de dispositivo para determinar el sensor a usar.
	
	El id del sensor se define de la manera siguiente:
		md5sum de "DS1631_id_direccion_del_sensor", por ejemplo
		
		jesus@servidor:~$ echo "DS1631_id_0x90" | md5sum
		e303289d37511303620a35309359cdfe  -
	"""
	
	def __init__(self):

		builder = gtk.Builder()
		builder.add_from_file("DS1631.xml") 
		
		self.widgets= dict(
			Principal = builder.get_object("Principal"),
			temp= builder.get_object("label_Temperatura"),
			TH= builder.get_object("label_Termostato_Alta"),
			Deslizante_TH= builder.get_object("hscale_Termostato_Alta"),
			TL= builder.get_object("label_Termostato_Baja"),
			Deslizante_TL= builder.get_object("hscale_Termostato_Baja"),
			Resolucion_9= builder.get_object("Res_9"),
			Resolucion_10= builder.get_object("Res_10"),
			Resolucion_11= builder.get_object("Res_11"),
			Resolucion_12= builder.get_object("Res_12"),
			R0_bit= builder.get_object("R0"),
			R1_bit= builder.get_object("R1"),
			Muestreo_Continuo= builder.get_object("radiobutton_conversion_continua"),
			Muestreo_1Shot= builder.get_object("radiobutton_un_tiro"),
			Muestreo_bit= builder.get_object("Un_Tiro"),
			Polaridad_Tout_High= builder.get_object("radiobutton_Tout_Alta"),
			Polaridad_Tout_Low= builder.get_object("radiobutton_Tout_Baja"),
			Polaridad_bit= builder.get_object("Pol"),
			Control= builder.get_object("THF"),
			Resumen= builder.get_object("Etiqueta_Resumen_Configuracion")
			)
		builder.connect_signals(self)       


	def delete_event(self, widget):
		gtk.main_quit()
	
	
	def cambio_grupo_resolucion(self, widget):
		
		global bit_Conf
		
		R0bit= self.widgets["R0_bit"]
		R1bit= self.widgets["R1_bit"]
		
		Res_9= self.widgets["Resolucion_9"]
		Es_Res_9= Res_9.get_active()
		
		Res_10= self.widgets["Resolucion_10"]
		Es_Res_10= Res_10.get_active()
		
		Res_11= self.widgets["Resolucion_11"]
		Es_Res_11= Res_11.get_active()
		
		Res_12= self.widgets["Resolucion_12"]
		Es_Res_12= Res_12.get_active()
		
		bitControl= self.widgets["Control"]
		Es_Control_Activo= bitControl.get_active()

		if Es_Res_9:
			if Es_Control_Activo:
				bitControl.set_active( False)
			else:
				R0bit.set_active( False)
				R1bit.set_active( False)
				bit_Conf &= ~b_R0
				bit_Conf &= ~b_R1
				bitControl.set_active( True)
		
		if Es_Res_10:
			if Es_Control_Activo:
				bitControl.set_active( False)
			else:
				R0bit.set_active( True)
				R1bit.set_active( False)
				bit_Conf |= b_R0
				bit_Conf &=  ~b_R1
				bitControl.set_active( True)
			
		if Es_Res_11:
			if Es_Control_Activo:
				bitControl.set_active( False)
			else:
				R0bit.set_active( False)
				R1bit.set_active( True)
				bit_Conf &= ~b_R0
				bit_Conf |= b_R1
				bitControl.set_active( True)
				
		if Es_Res_12:
			if Es_Control_Activo:
				bitControl.set_active( False)
			else:
				R0bit.set_active( True)
				R1bit.set_active( True)
				bit_Conf |= b_R0
				bit_Conf |= b_R1
				bitControl.set_active( True)
		
		Etiqueta_Resumen= self.widgets["Resumen"]
		Etiqueta_Resumen.set_text( str(bit_Conf))
		
		
		
	def cambio_grupo_tipo_muestreo(self, widget):
		"""Controla las modificaciones en el tipo de muestreo,
		ajusta el bit de configuracion a este respecto.
		"""
		global bit_Conf
		
		Muestreobit= self.widgets["Muestreo_bit"]
		Muestreo_Cont= self.widgets["Muestreo_Continuo"]
		Muestreo_1Tiro= self.widgets["Muestreo_1Shot"]
		
		Es_Muestreo_Continuo= Muestreo_Cont.get_active()
		Es_Muestreo_1Tiro= Muestreo_1Tiro.get_active()
		
		if Es_Muestreo_Continuo:
			Muestreobit.set_active( False)
			bit_Conf &= ~b_1Shot
			
		if Es_Muestreo_1Tiro:
			Muestreobit.set_active( True)
			bit_Conf |= b_1Shot
			
		Etiqueta_Resumen= self.widgets["Resumen"]
		Etiqueta_Resumen.set_text( str(bit_Conf))
	
	def cambio_polaridad_termostato(self, widget):
		"""Cambia el nivel logico de la salida Tout del termostato."""
		
		global bit_Conf		
		Polaridadbit= self.widgets["Polaridad_bit"]
		Polaridad_High=self.widgets["Polaridad_Tout_High"]
		Polaridad_Low=self.widgets["Polaridad_Tout_Low"]
		
		Es_Pol_H= Polaridad_High.get_active()
		Es_Pol_L= Polaridad_Low.get_active()
		
		if Es_Pol_H:
			Polaridadbit.set_active( True)
			bit_Conf |= b_Pol

		if Es_Pol_L:
			Polaridadbit.set_active( False)
			bit_Conf &= ~b_Pol
			
		Etiqueta_Resumen= self.widgets["Resumen"]
		Etiqueta_Resumen.set_text( str(bit_Conf))
		
		
	def imprime(self, idDevice):
		"""Muestra la temperatura actual en formato %2.2f
		"""
		Temp= device_command( idDevice, 'T', 0)
		if Temp== False:
			print "Lectura erronea."
		else:
			Texto= self.widgets["temp"]
			Texto.modify_font(pango.FontDescription("serif bold 18"))
			Texto.set_text( Temp)
		#gobject.timeout_add( 2000, self.imprime)
		
	def imprime_Termostato_Alto(self, idDevice):
		"""Obtiene la temperatura de ajuste del termostato en la zona alta,
			en formato %2.2f.
		"""
		Temp= device_command( idDevice, 'H', 0)
		if Temp== False:
			print "Lectura erronea."
			Temp= 0
		else:
			Texto= self.widgets["TH"]
			Texto.modify_font(pango.FontDescription("serif bold 12"))
			Texto.set_text( Temp)
		
		Deslizante= self.widgets["Deslizante_TH"]
		ajuste= gtk.Adjustment(value= float( Temp), lower=0.0625, upper=70, step_incr=0.0625, page_incr=0, page_size=0)
		Deslizante.set_adjustment( ajuste)

	def imprime_Termostato_Baja(self, idDevice):
		"""Obtiene la temperatura de ajuste del termostato en la zona baja, en formato %2.2f.
		"""
		Temp= device_command( idDevice, 'L', 0)
		if Temp== False:
			print "Lectura erronea."
			Temp=0
		else:
			Texto= self.widgets["TL"]
			Texto.modify_font(pango.FontDescription("serif bold 12"))
			Texto.set_text( Temp)
		
		Deslizante= self.widgets["Deslizante_TL"]
		ajuste= gtk.Adjustment(value= float( Temp), lower=0.0625, upper=70, step_incr=0.0625, page_incr=0, page_size=0)
		Deslizante.set_adjustment( ajuste)

	def evento_leer_configuracion_dispositivo( self, widget):
		"""Obtiene el byte de configuracion del dispositivo"""
		
		self.leer_configuracion( idDevice)
	
	def leer_configuracion( self, idDevice):
		"""Lee el byte de configuracion desde el dispositivo y la presenta en la pantalla"""
		global bit_Conf
		
		R0bit= self.widgets["R0_bit"]
		R1bit= self.widgets["R1_bit"]
		
		Res_9= self.widgets["Resolucion_9"]
		Es_Res_9= Res_9.get_active()

		Res_10= self.widgets["Resolucion_10"]
		Es_Res_10= Res_10.get_active()

		Res_11= self.widgets["Resolucion_11"]
		Es_Res_11= Res_11.get_active()

		Res_12= self.widgets["Resolucion_12"]
		Es_Res_12= Res_12.get_active()
		
		Conv_1Shot= self.widgets["Muestreo_1Shot"]
		Conv_Continua= self.widgets["Muestreo_Continuo"]
		
		Polaridad_Tout_High= self.widgets["Polaridad_Tout_High"]
		Polaridad_Tout_Low= self.widgets["Polaridad_Tout_Low"]
		Polaridad_bit= self.widgets["Polaridad_bit"]

		bit_Conf= int(device_command( idDevice, 'C', 0))
		Etiqueta_Resumen= self.widgets["Resumen"]
		Etiqueta_Resumen.set_text( str(bit_Conf))
		
		if bit_Conf & b_R0:
			R0bit.set_active( True)
			res= 4
		else:
			R0bit.set_active( False)
			res= 0
			
		if bit_Conf & b_R1:
			R1bit.set_active( True)
			res += 8
		else:
			R1bit.set_active( False)
			res += 0
		
		if res== 0:
			Res_9.set_active( True)
		elif res== 4:
			Res_10.set_active( True)
		elif res== 8:
			Res_11.set_active( True)
		elif res== 12:
			Res_12.set_active( True)

		else:
			print "Error: No concuerda el valor %d." % res
			
		if bit_Conf & b_1Shot:
			Conv_1Shot.set_active( True)
		else:
			Conv_Continua.set_active( True)
		
		if bit_Conf & b_Pol:
			Polaridad_Tout_High.set_active( True)
			Polaridad_bit.set_active( True)
		else:
			Polaridad_Tout_Low.set_active( True)
			Polaridad_bit.set_active( False)
		
		self.imprime_Termostato_Alto( idDevice)
		self.imprime_Termostato_Baja( idDevice)
		
		
	
	
	def modifica_configuracion(self, widget):
		"""Modifica la configuracion del sensor seleccionado."""
		
		global bit_Conf 
		
		Th= self.widgets["Deslizante_TH"]
		Temp_Th= Th.get_value()
		Temp_H= "%2.2f\r" % Temp_Th
				
		Tl= self.widgets["Deslizante_TL"]
		Temp_Tl= Tl.get_value()
		Temp_L= "%2.2f\r" % Temp_Tl
		
		if ( Temp_Th < Temp_Tl ):
			print "Error, la temperatura Tl es mayor que Th"
			self.imprime( idDevice)
			Error.dialogo_error_ajuste_termostato()
			return False
		
		device_command( idDevice, 'h', Temp_H)
		time.sleep( 0.5)

		device_command( idDevice, 'l', Temp_L)
		time.sleep(1)
		
		envio_valor= "%3d\r" % bit_Conf
		print envio_valor
		device_command( idDevice, 'c', envio_valor)
		
		time.sleep(1)
		self.imprime_Termostato_Alto( idDevice)
		self.imprime_Termostato_Baja( idDevice)
		
	def evento_boton_leer_temperatura(self, widget):
		self.imprime( idDevice)
		
	#def refrescar_datos(self):
	
	def evento_power_on_reset(self, widget):
		device_command( idDevice, 'r', 0)
		print "Software POR"
	
	

if __name__ == "__main__":

	try:
		Aplicacion = App()
		Error= Aviso()

		Aplicacion.imprime( idDevice)
		Aplicacion.leer_configuracion( idDevice)
		Aplicacion.imprime_Termostato_Alto(idDevice)
		Aplicacion.imprime_Termostato_Baja(idDevice)
		
		gtk.main()
	except KeyboardInterrupt:
		pass

