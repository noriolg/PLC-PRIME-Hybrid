#!/usr/bin/python3
import os

nedLibraries = '-n ../src -n ./ -n ../../../inet4.3/src -n ../../../inet4.3/examples -n ../../../inet4.3/tutorials -n ../../../inet4.3/showcases'


os.system('../out/gcc-release/src/RESPONSE_Dijon Dijon.ini '+ nedLibraries +' -u Cmdenv -c StarTopology --sim-time-limit=28800s')
os.system('valgrind --tool=memcheck --log-file=valgrindlog.txt ../out/gcc-release/src/RESPONSE_Dijon finalTest.ini -n ../src -n ./ -n ../../../inet4.3/src -n ../../../inet4.3/examples -n ../../../inet4.3/tutorials -n ../../../inet4.3/showcases -u Cmdenv -c num27 --sim-time-limit=28800s')

os.chdir('./results')
os.system('opp_scavetool x *sca -o scalars.csv')
os.system('opp_scavetool x *vec -o vectors.csv')
