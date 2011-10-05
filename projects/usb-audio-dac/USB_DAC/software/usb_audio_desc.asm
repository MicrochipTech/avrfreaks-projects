LANG OCTASM,0.1
CLUSTER_STEREO=302H

#device_descriptor
db 12H
db 12h
db 1
dw 110h
db 0,0,0
db 40h
dd 'OCTA' ;vendor
db 0,0,0,0,0,1

#config_descriptor
db >2->1
# dw 209h
  dw >1-<1
  db 2 ;num interfaces
  db 1 ;config
  db 0 ;strng
  db 80h
  db 100  ;200 ma
  ;standard Interface  desc
  dw 409h
  db 0  ;interface number
  db 0  ;alternate setting
  db 0  ;total endpoints
  db 1  ;class audio
  db 1  ;subclass audio control
  db 0  ;protocol
  db 0  ;strng

	{
	# ;audio control interface desc
	db 8+1
	db 24h ;CS_INTERFACE
	db 1   ;TYPE HEADER
	dw 100h;bcd version
	dw >1-<1
	db 1 ;n interfaces
	db 1 ;interface number
	;input terminal
	db 12
	db 24h ;CS_INTERFACE
	db 2  ;INPUT_TERMINAL
	db 1  ;id
	dw 101h ;terminal type usb streaming
	db 0    ;associated terminal
	dd CLUSTER_STEREO
	db 0 ;string
	;output terminal
	db 9
	db 24h ;CS_INTERFACE
	db 3  ;OUTPUT_TERMINAL
	db  2 ;id
	dw 301h ;terminal type speaker
	db 0    ;associated terminal
	db 3    ;conected to feature unit
	db 0    ;string
	;feature unit
		{
		#
		db >1-<1   ;size
		db 24h     ;cs_interface
		db 6       ;feature unit
		db 3 ;unit id
		db 1 ;conected to input terminal
		db 2  ;controls size
		dw 010b  ;master controls: volume
		dw 0  ;chanel left controls
		dw 0  ;chanel right
		db 0  ;string
		#
		}
	#
	}
  ;standard Interface  desc
  dw 409h
  db 1  ;interface number
  db 0  ;alternate setting
  db 0  ;total endpoints
  db 1  ;class audio
  db 2  ;subclass audio streaming
  db 0  ;protocol
  db 0  ;strng
  ;standard Interface  desc
  dw 409h
  db 1  ;interface number
  db 1  ;alternate setting
  db 1  ;total endpoints
  db 1  ;class audio
  db 2  ;subclass audio streaming
  db 0  ;protocol
  db 0  ;strng
  ;stream descriptor
  db 7
  db 24h ;CS_INTERFACE
  db 1  ;subtype general
  db 1  ;terminal link
  db 1  ;frame delay
  dw 1  ;audio format pcm
  ;format descriptor
  db 8+3
  db 24h ;CS_INTERFACE
  db 2   ;subtype format
  db 1   ;format type pcm
  db 2   ;chanels
  db 2   ;sample size
  db 16  ;bit resolution
  db 1   ;1 sample rate
  dw 48000 db 0 ;frequency
  ;endpoint
  dw 507h+2
  db 1  ;endp adress
  db 1101b       ;isochronous,synchronous,data
  dw 256         ;endpoint max packet size
  db 1          ;interval

  db 0  ;refresh ?
  db 0  ;syncendpointnumber ?
  ;Audio_StreamEndpoint
  db 7
  db 25h  ;CS_ENDPOINT
  db 1    ;EP_GENERAL
  db 0    ;attributes
  db 0
  dw 0
#


