#! /usr/bin/env python
#latin-1

"""clase para el manejo del **indicador**.

:author: Jesus Genicio
:version: 0.1
"""


from math import sin, cos, tan, pi, radians, degrees
import comunicaciones
#from Get_Temp import Get_Temp
from ctes_ds1631 import *
import gtk
import gtk.gdk
import gobject
import cairo
import cStringIO 
import mmap



class gauge:
    def __init__(self, Imagen, ancho, alto, idDispositivo, ubicacion):
        self.Comm= comunicaciones.comunicaciones()
        self.Imagen= Imagen  
        self.w= ancho
        self.h= alto
        self.idDevice= idDispositivo
        self.ubicacion= ubicacion
        self.fondo = cStringIO.StringIO()
        self.ajusta_escala()
        self.crea_lienzo()
        self.dibuja_arco_escala()
        self.logo_bienvenida()
        self.obtener_temperatura()
        self.imprime_puerto_conexion()
        gobject.timeout_add( 1000, self.leer_temperatura)
    
    
    def logo_bienvenida( self):
        
        Lienzo= cairo.ImageSurface.create_from_png( "imagenes/logos/Logo_Tux-Python-200x200.png")
        Dibujo= cairo.Context( Lienzo)
        
        a= Dibujo.get_matrix()
        
        Dibujo.stroke()
        buf = cStringIO.StringIO()
        Lienzo.write_to_png( buf)
        
        buf.seek( 0)
        loader = gtk.gdk.PixbufLoader()
        loader.write( buf.getvalue())
        loader.close()
        self.Imagen.set_from_pixbuf( loader.get_pixbuf())
        buf.close()
    
    def leer_temperatura( self):
        self.obtener_temperatura()
        self.angulo_x_temperatura= (2*pi) -(( self.Temperatura* ( pi/ 2))/ self.Tope_Escala)
        self.angulo= self.angulo_x_temperatura
        self.dibuja_fiel() 
        self.imprime_temperatura()
        self.mezcla_imagenes()
        return True
        
    def ajusta_escala( self):
        """
        Segun el tamano de la imagen, ajusta las variables para que la visualicacion sea correcta.
        
        """
        #Ajuste de gauje, debe ser pasado como opcion

        self.Tope_Escala= 70.0
        self.Salto_Escala= 10.0
        
        # constantes de forma.
        
        cte_forma_circulo_central_aguja= 6.0
        self.aguja_angulo_inicial= radians( 22.5)
        self.aguja_angulo_final= radians( 360.0- 22.5)
        
        # Radio general para trabajar, es el menor de la relacion entre largo y alto de la ventana
        self.radio_exterior=  min( self.w, self.h)/ 2.0
        
        
        # Centro_x , centro_y
        
        self.centro_x= self.w/ 2.0
        self.centro_y= self.h/ 2.0
                
        #Ajuste la la escala de la aguja
        
        self.radio_exterior_aguja= self.radio_exterior- ( self.radio_exterior* 0.2)
        self.radio_circulo_central= self.radio_exterior_aguja/ cte_forma_circulo_central_aguja
        
        # Ajuste para la creacion del lienzo para la aguja
        
        self.aguja_w= self.radio_circulo_central+ self.radio_exterior_aguja
        self.aguja_h= self.radio_circulo_central* 2.0
        
    def crea_lienzo( self):
        """ Inicia la base del gauge a base de:
            1. Crea un lienzo nuevo a base del formato *FORMAT_ARGB32*
            2. Lo crea con las medidas del gtk.Image
        """
        self.Lienzo= cairo.ImageSurface(cairo.FORMAT_ARGB32, self.w, self.h)
        #self.Lienzo= cairo.ImageSurface.create_from_png( "./lienzo.png") 
        self.Dibujo= cairo.Context( self.Lienzo)
        

    def dibuja_arco_escala( self):
        """Dibjua la escala a usar en el medidor."""
        ajuste= min( self.w, self.h)
        
        centro_arco_x= ajuste* 0.15
        centro_arco_y= self.h- centro_arco_x
        
        separador_paso= ajuste* 0.08
        
        radio_arco= ajuste- ( 2* centro_arco_x)
        self.radio_exterior_aguja= radio_arco
        pluma_gruesa= ajuste* 0.01
        pluma_fina= ajuste* 0.0025
        
        self.Dibujo.set_source_rgb(0, 0, 0)
        self.Dibujo.set_line_width( pluma_gruesa)
        self.Dibujo.arc( centro_arco_x, centro_arco_y, radio_arco, 3*( pi/2), 2*pi)
        self.Dibujo.stroke()
        
        self.Dibujo.set_line_width( pluma_fina)
        self.Dibujo.move_to( centro_arco_x, centro_arco_y)
        self.Dibujo.rel_line_to( radio_arco, 0)
        self.Dibujo.stroke()

        self.Dibujo.set_line_width( pluma_fina)
        self.Dibujo.move_to( centro_arco_x, centro_arco_y)
        self.Dibujo.rel_line_to( 0, -1*radio_arco)
        self.Dibujo.stroke()
        
        angulo_iniciado_a= 3*( pi/ 2)
        
        a= 0
        paso= ( self.Tope_Escala- a)/ self.Salto_Escala
        alfa= ( pi/2)/ paso

        separador_paso= radio_arco* 0.08
        radio_a= radio_arco- ( 3* separador_paso/ 5)
        radio_aa=radio_arco+ ( 3* separador_paso/ 5)
        radio_b= radio_arco+ ( 2* separador_paso/ 5)
        
        while( angulo_iniciado_a< (2* pi)+ alfa):
            Texto_Divisiones= str(int((((2*pi)- angulo_iniciado_a)/ alfa)* self.Salto_Escala))

            ax= radio_a* cos ( angulo_iniciado_a)
            ay= radio_a* sin ( angulo_iniciado_a)
            bx= radio_b* cos ( angulo_iniciado_a)
            by= radio_b* sin ( angulo_iniciado_a)
            
            self.Dibujo.set_line_width( pluma_gruesa)
            self.Dibujo.move_to( centro_arco_x+ ax, centro_arco_y+ ay)
            self.Dibujo.line_to( centro_arco_x+ bx, centro_arco_y+ by)
            self.Dibujo.stroke()

            self.Dibujo.set_source_rgb(0, 0, 0)
            self.Dibujo.select_font_face("serif",
                cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_BOLD)
            self.Dibujo.set_font_size( separador_paso/ 1.3)
            tx, ty, tlargo, talto, tx_ext, ty_ext= self.Dibujo.text_extents( "000")
            
            radio_txt= radio_b+ ( 3* separador_paso/ 5)
            txt_x= radio_txt* cos( angulo_iniciado_a)
            txt_y= radio_txt* sin( angulo_iniciado_a)
            
            x= centro_arco_x+ txt_x- ( tlargo/ 2.0)
            y= centro_arco_y+ txt_y+ ( talto/ 2.0)
            
            
            self.Dibujo.move_to( x, y)
            self.Dibujo.show_text( Texto_Divisiones)
            self.Dibujo.stroke()
            
            angulo_iniciado_a += alfa

        self.Lienzo.write_to_png( self.fondo)
                
        self.centro_aguja_x= centro_arco_x
        self.centro_aguja_y= centro_arco_y
        return True
        

    def dibuja_fiel( self):
        """Dibuja la aguja del medidor"""

        self.aguja= cStringIO.StringIO()        
        angulo_inicial= self.angulo+ self.aguja_angulo_inicial
        angulo_final= self.angulo+ self.aguja_angulo_final
        
        cx= self.radio_exterior_aguja* cos(  self.angulo)
        cy= self.radio_exterior_aguja* sin(  self.angulo)        
        
        self.Lienzo_Aguja= cairo.ImageSurface(cairo.FORMAT_ARGB32, self.w, self.h)
        self.Dibujo_Aguja= cairo.Context( self.Lienzo_Aguja)
        
        self.Dibujo_Aguja.set_source_rgb(0, 0, 0)
        self.Dibujo_Aguja.arc( self.centro_aguja_x, self.centro_aguja_y,
                                self.centro_aguja_x/ 3.0,
                                    angulo_inicial, angulo_final)
        self.Dibujo_Aguja.line_to( self.centro_aguja_x+ cx, self.centro_aguja_y+ cy)
        self.Dibujo_Aguja.fill()
        self.Dibujo_Aguja.stroke()
        
        self.Lienzo_Aguja.write_to_png( self.aguja)
        

        self.Lienzo_Aguja.finish()
        
    def imprime_temperatura( self):
        self.txt_Temperatura= cStringIO.StringIO()        
        Lienzo= cairo.ImageSurface(cairo.FORMAT_ARGB32, self.w, self.h)
        Dibujo= cairo.Context( Lienzo)
        Dibujo.set_source_rgb(0, 0, 0)
        Dibujo.select_font_face("fantasy",
                cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_NORMAL)
        Dibujo.set_font_size( self.w* 0.05)
        
        Temp= "Actual: %s C" % self.Temperatura
        tx, ty, tlargo, talto, tx_ext, ty_ext= Dibujo.text_extents( Temp)
        x= self.w- tx_ext
        y= talto+ ty_ext
        Dibujo.move_to( x, y)
        Dibujo.show_text( Temp)
        
        
        
        
        self.Dibujo.stroke()
        Lienzo.write_to_png( self.txt_Temperatura)
        Lienzo.finish()
        
        

    def imprime_puerto_conexion( self):
        self.txt_puerto_conexion= cStringIO.StringIO()        
        Lienzo= cairo.ImageSurface(cairo.FORMAT_ARGB32, self.w, self.h)
        Dibujo= cairo.Context( Lienzo)
        Dibujo.set_source_rgb(0, 0, 0)
        Dibujo.select_font_face("fantasy",
                cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_NORMAL)
        Dibujo.set_font_size( self.w* 0.05)
                
        Temp= "Th: %s" % self.Th
        tx, ty, tlargo, talto, tx_ext, ty_ext= Dibujo.text_extents( Temp)
        x= self.w- tx_ext
        y = 2.5* talto
        Dibujo.move_to( x, y)
        Dibujo.show_text( Temp)
        
        Temp= "Tl: %s" % self.Tl
        tx, ty, tlargo, talto, tx_ext, ty_ext= Dibujo.text_extents( Temp)
        x= self.w- tx_ext
        y += 1.5* talto 
        Dibujo.move_to( x, y)
        Dibujo.show_text( Temp)
        
        Texto= str(self.puerto_conexion)
        tx, ty, tlargo, talto, tx_ext, ty_ext= Dibujo.text_extents( Texto)
        x= self.w- tx_ext
        y += talto+ ty_ext
        Dibujo.move_to( x, y)
        Dibujo.show_text( Texto)
        
        Temp= self.ubicacion
        tx, ty, tlargo, talto, tx_ext, ty_ext= Dibujo.text_extents( Temp)
        x= tx
        y= talto+ ty_ext
        Dibujo.move_to( x, y)
        Dibujo.show_text( Temp)
        
        Temp= "id: %s" % self.idDevice
        Dibujo.set_font_size( self.w* 0.040)
        tx, ty, tlargo, talto, tx_ext, ty_ext= Dibujo.text_extents( Temp)
        x= tx
        y= self.h- talto- ty_ext
        Dibujo.move_to( x, y)
        Dibujo.show_text( Temp)
        self.Dibujo.stroke()
        Lienzo.write_to_png( self.txt_puerto_conexion)
        Lienzo.finish()
        
    def mezcla_imagenes( self):
        """Mezclamos el fondo con la aguja e imprimimos"""
        
        self.fondo.seek( 0)
        self.aguja.seek( 0)
        self.txt_Temperatura.seek( 0)
        self.txt_puerto_conexion.seek( 0)
        Lienzo_base= cairo.ImageSurface.create_from_png( self.fondo)
        Dibujo_base= cairo.Context( Lienzo_base)
        
        Lienzo_aguja= cairo.ImageSurface.create_from_png( self.aguja)
        Dibujo_aguja= cairo.Context( Lienzo_aguja)
        Dibujo_base.set_source_surface( Lienzo_aguja, 0,0)
        Dibujo_base.paint()
        
        Lienzo= cairo.ImageSurface.create_from_png( self.txt_Temperatura)
        Dibujo= cairo.Context( Lienzo)
        Dibujo_base.set_source_surface( Lienzo, 0,0)
        Dibujo_base.paint()
        
        Lienzo= cairo.ImageSurface.create_from_png( self.txt_puerto_conexion)
        Dibujo= cairo.Context( Lienzo)
        Dibujo_base.set_source_surface( Lienzo, 0,0)
        Dibujo_base.paint()
               
        buf = cStringIO.StringIO()
        Lienzo_base.write_to_png( buf)
        
        buf.seek( 0)
        loader = gtk.gdk.PixbufLoader()
        loader.write( buf.getvalue())
        loader.close()
        self.Imagen.set_from_pixbuf( loader.get_pixbuf())
        buf.close()

        return True  
    
    def obtener_temperatura(self):
        """H"""
        Temp, TH, TL, Conf, Port, timestamp= self.Comm.Get_Tupla( idDevice)
        self.Temperatura= float( Temp)
        self.puerto_conexion= Port
        self.Th= float( TH)
        self.Tl= float( TL)
        return True
    

    
