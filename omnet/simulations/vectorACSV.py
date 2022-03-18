#!/usr/bin/env python3

import sys
import os

# Argumento 1  - de dónde coges la info (General). 
# Argumento 2 - dónde la guardas


nedLibraries = "-n .:../src:../../../../inet4.3/src:../../../../inet4.3/examples:../../../../inet4.3/tutorials:../../../../inet4.3/showcases:../../../../simprime2/omnet/simPRIME2/simulations:../../../../simprime2/omnet/simPRIME2/src -x inet.emulation;inet.showcases.visualizer.osg;inet.showcases.emulation;inet.clock.common;inet.clock.model;inet.visualizer.osg;inet.examples.voipstream;inet.clock.oscillator;inet.examples.emulation;inet.transportlayer.tcp_lwip;inet.applications.voipstream;inet.clock.base;inet.examples.clock;inet.transportlayer.tcp_nsc --image-path=../../../../inet4.3/images -l ../../../../inet4.3/src/INET -l ../../../../simprime2/omnet/simPRIME2/src/simPRIME2 --debug-on-errors=true "


os.system('../src/HybridMeter_dbg HybridMeter.ini  -u Cmdenv '+ nedLibraries +' -c General --sim-time-limit=1500s')


#path_scalars = sys.argv[1] + ".sca" # Este no lo solemos coger
path_vectors = sys.argv[1] + ".vec"

os.chdir('./results')
#os.system('opp_scavetool x '+ path_scalars + ' -o '+ sys.argv[2] + '.csv')
os.system('opp_scavetool x '+ path_vectors + ' -o '+ sys.argv[2] + '.csv')


