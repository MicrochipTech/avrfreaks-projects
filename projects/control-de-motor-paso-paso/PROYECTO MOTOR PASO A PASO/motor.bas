
'************CONTROL DE MOTOR PASO A PASO********

'Autores: Ing.- Manuel Quiñones
'                Sr.-Felipe Quiñones

$regfile = "m32def.dat"                                     'microcontrolador atmega32

$crystal = 8000000                                          'Oscilador De 8Mhz

 '/////////////////////////////////////////////////////
 'CONFIGURANDO PUERTOS

Config Portc = &B00001111
Config Portd = Output
Config Porta = Output
Config Portb = Input


Dim Uni As Byte
Dim Dece As Byte
Dim Idece As Byte
Dim Idece1 As Byte
Dim Cambio As Byte
Dim Timemov As Integer
Dim I3 As Byte                                              ' Contador de pasos
Dim Icam As Byte
Dim Icam1 As Byte
Dim I4 As Byte
Dim I5 As Byte
Dim A12 As Byte                                             'Contador para ciclos for

Timemov = 500                                               'Variable, tiempos de paso en el motor
Do

 Uni = &B00001111 And Pinb                                  'extraer valor unidad

   If Uni > 9 Then                                          'Si valor es mayor a 9, variable se hace cero
    Uni = 0
   End If


  Dece = Pinb

  For A12 = 0 To 3                                          'extrae valor de decena

   Dece = Dece / &B10                                       'Desplazar a la izquierda

  Next



  If Dece <= 9 Then
   Idece = Dece                                             'guardando variable parte entera   izquierda
         For A12 = 0 To 3

         Dece = Dece * &B10                                 'Desplazar a la derecha

         Next

  Else
  Dece = 0
  Idece = Dece                                              'guardando variable parte entera   izquierda
  End If

 Porta = Dece + Uni                                         'imprimir en display
 Idece1 = Idece * &B1010

 Cambio = Idece1 + Uni

'//////////////////////////////////////////////////////////////////////////////////////////////

     If Pinc.6 = 0 Then                                     'Iniciar movimento de motor paso a paso
      Portd = 0
      I3 = 0
      Gosub Contador
      Waitms Timemov

      Gosub Contadordec
     End If

If Pinc.4 = 0 Then Gosub Clockinc
If Pinc.5 = 0 Then Gosub Clockdec

Loop
End

'/////////////////////////////////////////////////////////////
'SUBRUTINAS

'/////////////////////////////////////////////////////////////
'Incrementar tiempo en los pasos del motor
Clockinc:
 Waitms 10
  'If Pinc.4 = 1 Then
    Timemov = Timemov + 100
  'End If
Return

'/////////////////////////////////////////////////////////////
'Decrementar tiempo en los pasos del motor
Clockdec:
 Waitms 10
  '  If Pinc.4 = 1 Then
      If Timemov > 100 Then
       Timemov = Timemov - 100
      End If
   ' End If
Return

'///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
'INCREMENTAR PASOS   DE MOTOR

Contador:

 'CLOCK
  '*******
  If Pinc.4 = 0 Then Gosub Clockinc
  If Pinc.5 = 0 Then Gosub Clockdec
  '*******
 'Secuencia de bobinas(hacia la derecha)

        If Cambio > 1 Then
         Portc = &B0001
         I3 = I3 + 1
         Gosub Visualizar
         Waitms Timemov

             If I3 = Cambio Then
               Return
             End If
        End If

        If Cambio > 2 Then
         Portc = &B0011
         I3 = I3 + 1
         Gosub Visualizar
         Waitms Timemov

            If I3 = Cambio Then
               Return
            End If
        End If

        If Cambio > 3 Then
         Portc = &B0010
         I3 = I3 + 1
         Gosub Visualizar
         Waitms Timemov

          If I3 = Cambio Then
            Return
          End If
        End If

        If Cambio > 4 Then
         Portc = &B0110
         I3 = I3 + 1
         Gosub Visualizar
         Waitms Timemov

            If I3 = Cambio Then
              Return
            End If
        End If

        If Cambio > 5 Then
         Portc = &B0100
         I3 = I3 + 1
         Gosub Visualizar
         Waitms Timemov

            If I3 = Cambio Then
              Return
            End If
        End If

        If Cambio > 6 Then
         Portc = &B1100
         I3 = I3 + 1
        Gosub Visualizar
        Waitms Timemov

            If I3 = Cambio Then
               Return
            End If
        End If

        If Cambio > 7 Then
         Portc = &B1000
         I3 = I3 + 1
         Gosub Visualizar
         Waitms Timemov

            If I3 = Cambio Then
               Return
            End If
        End If

        If Cambio > 8 Then
         Portc = &B1001
         I3 = I3 + 1
         Gosub Visualizar
         Waitms Timemov

            If I3 = Cambio Then
               Return
            End If
        End If

        If I3 < Cambio Then Goto Contador                   'continuar secuencia
        'CLOCK
        '*******
        If Pinc.4 = 0 Then Gosub Clockinc
        If Pinc.5 = 0 Then Gosub Clockdec
        '*******
Return

'///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
'DECREMENTAR PASOS DE MOTOR

 Contadordec:
 'CLOCK
  '*******
  If Pinc.4 = 0 Then Gosub Clockinc
  If Pinc.5 = 0 Then Gosub Clockdec
  '*******
  'Secuencia de bobinas(hacia la izquierda)

         If Cambio > 1 Then
          Portc = &B1001
          I3 = I3 - 1
          Gosub Visualizar
          Waitms Timemov

            If I3 = 0 Then
              Return
            End If
         End If

         If Cambio > 2 Then
          Portc = &B1000
          I3 = I3 - 1
          Gosub Visualizar
          Waitms Timemov

            If I3 = 0 Then
               Return
            End If
         End If

         If Cambio > 3 Then
          Portc = &B1100
          I3 = I3 - 1
          Gosub Visualizar
          Waitms Timemov

            If I3 = 0 Then
               Return
            End If
         End If

         If Cambio > 4 Then
          Portc = &B0100
          I3 = I3 - 1
          Gosub Visualizar
          Waitms Timemov

            If I3 = 0 Then
               Return
            End If
         End If

         If Cambio > 5 Then
          Portc = &B0110
          I3 = I3 - 1
          Gosub Visualizar
          Waitms Timemov

            If I3 = 0 Then
               Return
            End If

         End If

         If Cambio > 6 Then
          Portc = &B0010
          I3 = I3 - 1
          Gosub Visualizar
          Waitms Timemov

            If I3 = 0 Then
               Return
            End If

         End If

         If Cambio > 7 Then
          Portc = &B0011
          I3 = I3 - 1
          Gosub Visualizar
          Waitms Timemov

            If I3 = 0 Then
               Return
            End If
         End If

         If Cambio > 8 Then
          Portc = &B0001
          I3 = I3 - 1
          Gosub Visualizar
          Waitms Timemov

            If I3 = 0 Then
               Return
            End If

         End If

         If I3 > 0 Then Goto Contadordec

         'CLOCK
         '*******
         If Pinc.4 = 0 Then Gosub Clockinc
         If Pinc.5 = 0 Then Gosub Clockdec
         ' * * * * * * *
Return



'///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
'VISUALIZACIÒN DE PASOS

Visualizar:
    Icam = I3

    If I3 <= 9 Then
         Portd = I3
         Else

            If I3 > 9 And I3 < 20 Then                      'Visualizar entre 10 y 19  el resultado
               Icam = I3 + 6
               Portd = Icam                                 'Visualizar resultado

            End If

            If I3 > 19 And I3 < 99 Then                     'Visualizar entre 20 y 99  el resultado

               I4 = I3 / 10
               I5 = I4

                  For A12 = 0 To 3
                    I4 = I4 * &B10                          'Desplazar a la derecha
                  Next

               I5 = I5 * 10                                 'parte de decenas
               Icam = Icam - I5                             'Extraer Unidades
               Portd = Icam + I4                            'visualizar resultado

             End If

    End If

           'CLOCK
           '*******
           If Pinc.4 = 0 Then Gosub Clockinc
           If Pinc.5 = 0 Then Gosub Clockdec
           '*******

Return

'LOJA- ECUADOR