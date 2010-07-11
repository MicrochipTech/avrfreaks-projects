#! /usr/bin/env python
#latin-1

"""Modulo para ejemplificar el uso de *gauges*.

:author: Jesus Genicio
:version: 0.1
"""

__docformat__ = "restructuredtext"
import gtk
import gtk.gdk
import medidor
from ctes_ds1631 import *
import profile

def main():
    
    x= 800
    y= 800
    
    Ventana= gtk.Window()
    Ventana.set_title('Pruebas de uso de pycairo y pygtk')
    Ventana.set_default_size( x, y)
    Ventana.set_position(gtk.WIN_POS_CENTER)
    Lineas= gtk.VBox(homogeneous=False, spacing=0)
    
    Imagen= gtk.Image()
    
    Ventana.add( Lineas)
    Lineas.add( Imagen)
    
    Prueba= medidor.gauge( Imagen, x, y, idDevice, "Sala de estudio")
    
    Ventana.connect("destroy", gtk.main_quit)
    
    Ventana.show_all()
    gtk.main()
    
if __name__ == "__main__" :
    main()
