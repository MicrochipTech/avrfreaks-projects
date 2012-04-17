#!/usr/bin/perl

use strict;

sub stringify {
  my $name = shift(@_);
  my $sname=$name;
  $sname =~ s/\-/_/g;
  open(OUTPUTC, "> $name.c");
  open(OUTPUTH, "> $name.h");
  print OUTPUTH "\#ifndef __${sname}_H__\n";
  print OUTPUTH "\#define __${sname}_H__\n";
  print OUTPUTH "\#include <avr/pgmspace.h>\n";
  print OUTPUTC "\#include <avr/pgmspace.h>\n";
  
  open(FILE, "$name");
  
  while(<FILE>) {
    if(/(.+) "(.+)"/) {
      my $var = $1;
      my $data = $2;
      
      my $datan = $data;
      $datan =~ s/\\r/\r/g;
      $datan =~ s/\\n/\n/g;
      $datan =~ s/\\01/\01/g;      
      $datan =~ s/\\0/\0/g;
      
      printf(OUTPUTC "prog_char $var\[%d] = \n", length($datan) + 1);
      printf(OUTPUTC "/* \"$data\" */\n");
      printf(OUTPUTC "{");
      my  $j;
      for($j = 0; $j < length($datan); $j++) {
        printf(OUTPUTC "%#02x, ", unpack("C", substr($datan, $j, 1)));
      }
      printf(OUTPUTC "};\n");
      
      printf(OUTPUTH "extern prog_char $var\[%d];\n", length($datan) + 1);
      
    }
  }
  print OUTPUTH "#endif\n";
  close(OUTPUTC);
  close(OUTPUTH);
}
stringify("http-strings");

exit 0;

