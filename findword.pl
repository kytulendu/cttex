#!/usr/bin/perl
#
# Use this program to search in case one cannot 
# type Thai in console for grep

while(<>) {
  print if /^ริ$/;
}
