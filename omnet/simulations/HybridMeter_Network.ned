package hybridmeter.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.INetworkNode;
import inet.node.inet.WirelessHost;
import inet.physicallayer.wireless.common.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
import simprime2.PLCBaseNodeDLMSCOSEM;
import simprime2.PLCBus;
import simprime2.PLCServiceNodeDLMSCOSEM;


network HybridMeter_Net
{
    parameters:
        @display("bgb=650,500;bgg=100,1,grey95");
        @figure[title](type=label; pos=0,-1; anchor=sw; color=darkblue);

        @figure[rcvdPkText](type=indicatorText; pos=380,20; anchor=w; font=,18; textFormat="packets received: %g"; initialValue=0);
        @statistic[packetReceived](source=hostB.app[0].packetReceived; record=figure(count); targetFigure=rcvdPkText);

        int nHostsPerBranch = default(3);
        int nBranches = default(2);
        @display("bgi=background/streetmap,s;bgb=1600,1000");

    submodules:
        // ------------------------------------ RADIO ---------------------------------------------------------------
        visualizer: <default(firstAvailableOrEmpty("IntegratedCanvasVisualizer"))> like IIntegratedVisualizer if typename != "" {
            @display("p=580,125");
        }
         // This configurator assigns addresses to the hosts of the network
        configurator: Ipv4NetworkConfigurator {
            @display("p=580,200");
        }

        radioMedium: <default("UnitDiskRadioMedium")> like IRadioMedium {
            @display("p=580,275");
        }
        hostA: WirelessHost {
            @display("p=324.57602,445.536");
        }
        hostB: <default("WirelessHost")> like INetworkNode {
            @display("p=466.704,445.536");
        }

        // ------------------------------------ PLC ---------------------------------------------------------------
        plcBus[nBranches]: PLCBus {
            parameters:
                propagationSpeed = 2e8 mps;
                @display("p=750,100;b=1300,6,rect;o=#408060");
            gates:
                ethg[nHostsPerBranch+1];
        }

        plcBusSwitch: PLCBus {
            parameters:
                //positions = default("0 100 200 300");
                propagationSpeed = 2e8 mps;
                @display("p=100,500;b=6,900,rect;o=#408060");
            gates:
                ethg[nBranches+1];
        }

        PLC_BN: PLCBaseNodeDLMSCOSEM {
            parameters:
                @display("p=20,100;q=txQueue");
        }
        PLC_SN[nHostsPerBranch*nBranches]: PLCServiceNodeDLMSCOSEM {
            parameters:
                @display("q=txQueue;p=376,318");
        }


    connections:

        for j=0..(nBranches*nHostsPerBranch)-1 {
            plcBus[int(j/nHostsPerBranch)].ethg[(j%nHostsPerBranch)+1] <--> PLC_SN[j].ethg;
        }

         //add all branches together in the plcBusSwitch
        for j=0..nBranches-1 {
            plcBus[j].ethg[0] <--> plcBusSwitch.ethg[j];
        }

        PLC_BN.ethg <--> plcBusSwitch.ethg[nBranches];

}
