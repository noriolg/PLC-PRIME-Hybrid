#!/usr/bin/python3
import os

# nedLibraries = '-n ../src -n ./ -n ../../../inet4.3/src -n ../../../inet4.3/examples -n ../../../inet4.3/tutorials -n ../../../inet4.3/showcases -n ../../../simPRIME2/src'

nedLibraries = "-n .:../src:../../../../inet4.3/src:../../../../inet4.3/examples:../../../../inet4.3/tutorials:../../../../inet4.3/showcases:../../../../simprime2/omnet/simPRIME2/simulations:../../../../simprime2/omnet/simPRIME2/src -x inet.emulation;inet.showcases.visualizer.osg;inet.showcases.emulation;inet.clock.common;inet.clock.model;inet.visualizer.osg;inet.examples.voipstream;inet.clock.oscillator;inet.examples.emulation;inet.transportlayer.tcp_lwip;inet.applications.voipstream;inet.clock.base;inet.examples.clock;inet.transportlayer.tcp_nsc --image-path=../../../../inet4.3/images -l ../../../../inet4.3/src/INET -l ../../../../simprime2/omnet/simPRIME2/src/simPRIME2 --debug-on-errors=true "




os.system('../src/HybridMeter_dbg HybridMeter.ini  -u Cmdenv '+ nedLibraries +' -c General --sim-time-limit=500s')
#os.system('../out/gcc-release/src/HybridMeter omnetpp.ini '+ nedLibraries +' -u Cmdenv -c RSCM2 --sim-time-limit=40s')
#os.system('../out/gcc-release/src/HybridMeter omnetpp.ini '+ nedLibraries +' -u Cmdenv -c RSCM4 --sim-time-limit=40s')
#os.system('../out/gcc-release/src/HybridMeter omnetpp.ini '+ nedLibraries +' -u Cmdenv -c NRNSCM2 --sim-time-limit=40s')
#os.system('../out/gcc-release/src/HybridMeter omnetpp.ini '+ nedLibraries +' -u Cmdenv -c NRNSCM2 --sim-time-limit=40s')

#os.system('valgrind --tool=memcheck --log-file=valgrindlog.txt ../out/gcc-release/src/HybridMeter finalTest.ini -n ../src -n ./ -n ../../../inet4.3/src -n ../../../inet4.3/examples -n ../../../inet4.3/tutorials -n ../../../inet4.3/showcases -u Cmdenv -c num27 --sim-time-limit=28800s')

os.chdir('./results')
os.system('opp_scavetool x *sca -o scalars.csv')
os.system('opp_scavetool x *vec -o vectors.csv')
