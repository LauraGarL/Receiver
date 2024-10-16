//============================================================================
// Name        : handshakeTX.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include "systemc.h"

#include "rx.h"
using namespace std;

int sc_main(int argc, char* argv[]) {
    sc_signal<bool> 	i_clock;
    sc_signal<bool> 	i_reset;
    sc_signal<bool>		i_Req;		//Output:  Indicates whether the request was granted
    sc_signal<bool> 	i_SoP;		//Output:  Indicates the start of a packet
    sc_signal<bool>		i_ready_to_receive;	//Input: Indicates that there is a pack to receive
    sc_signal<bool>		i_fifo_full;
    sc_signal<sc_uint<9>>		i_nFlit_to_rx;
    sc_signal<bool>		o_OnOff;	//Output: Indicates availability to receive a flit
    sc_signal<bool> 	o_fifo_push;		//Output:  Add an element to  FIFO

    RX receiver("receiver");
    receiver.i_clock(i_clock);
    receiver.i_reset(i_reset);
    receiver.i_Req(i_Req);
    receiver.i_SoP(i_SoP);
    receiver.i_fifo_full(i_fifo_full);
    receiver.i_nFlit_to_rx(i_nFlit_to_rx);
    receiver.i_ready_to_receive(i_ready_to_receive);
    receiver.o_OnOff(o_OnOff);
    receiver.o_fifo_push(o_fifo_push);

    sc_start(10,SC_NS);	//El entero nos dice cuántas unidades de tiempo se va a ejecutar. Es un método necesario para comenzar una simulación.

    		sc_trace_file *wf = sc_create_vcd_trace_file("vcd_myRX");

    		sc_trace(wf, i_clock, "i_clock");
    		sc_trace(wf, i_reset, "i_reset");
    		sc_trace(wf, i_SoP, "i_SoP");
    		sc_trace(wf, i_Req, "i_Req");
    		sc_trace(wf, i_fifo_full, "i_fifo_full");
    		sc_trace(wf, i_nFlit_to_rx, "i_nFlit_to_rx");
    		sc_trace(wf, i_ready_to_receive, "i_ready_to_receive");
    		sc_trace(wf, o_OnOff,"o_OnOff");
    		sc_trace(wf, o_fifo_push,"o_fifo_push");
    		sc_trace(wf, receiver.state, "state");
    		sc_trace(wf, receiver.next_state, "next_state");
    		sc_trace(wf, receiver.count,"count");

    		i_Req=0;
    		i_SoP=0;
    		i_fifo_full=0;
    		i_nFlit_to_rx=6;
    		i_ready_to_receive=0;

    		for(int i=0; i<2; i++){
    					i_clock = 0;
    					sc_start(10,SC_NS);
    					i_clock = 1;
    					sc_start(10,SC_NS);
    				}

    		i_Req=0;
    		i_SoP=0;
    		i_fifo_full=0;
    		i_ready_to_receive=0;
    				i_reset = 1; //Assert the reset

    				cout << "@" << sc_time_stamp() <<"\t Asserting reset ... RX\n" << endl;

    				for (int i=0; i<2; i++){
    					i_clock = 0;
    					sc_start(10,SC_NS);
    					i_clock = 1;
    					sc_start(10,SC_NS);
    				}

    				i_reset = 0; //De-Assert the reset

    				cout << "@" << sc_time_stamp() <<"\t De-Asserting reset\n" << endl;

    				cout << "@" << sc_time_stamp() <<"\t All configurations:\n" << endl; //Quiero poner todas las combinaciones de las solicitudes.
//Listo para recibir
    				i_Req=0;
    	    		i_SoP=0;
    	    		i_fifo_full=0;
    	    		i_ready_to_receive=1;

    				i_clock = 0;
    				sc_start(10,SC_NS);
    				i_clock = 1;
    				sc_start(10,SC_NS);

//Hay una solicitud
    	    		i_Req=1;
    	    		i_SoP=0;
    	    		i_fifo_full=0;
    	    		i_ready_to_receive=0;

    				//Clock
    				i_clock = 0;
    				sc_start(10,SC_NS);
    				i_clock = 1;
    				sc_start(10,SC_NS);

//Es  un inicio de paquete
    	    		i_SoP=1;
    	    		i_fifo_full=0;
    	    		i_ready_to_receive=0;

    				//Clock
    				i_clock = 0;
    				sc_start(10,SC_NS);
    				i_clock = 1;
    				sc_start(10,SC_NS);
//Hay una s olicitud pero ya no hay espacio en la fifo
    	    		i_Req=1;
    	    		i_SoP=0;
    	    		i_fifo_full=1;
    	    		i_ready_to_receive=0;

    				//Clock
    				i_clock = 0;
    				sc_start(10,SC_NS);
    				i_clock = 1;
    				sc_start(10,SC_NS);
//Sigue habiendo una solicitud y ya no está llena la fifo
    	    		i_Req=1;
    	    		i_SoP=0;
    	    		i_fifo_full=0;
    	    		i_ready_to_receive=0;

    				//Clock
    				i_clock = 0;
    				sc_start(10,SC_NS);
    				i_clock = 1;
    				sc_start(10,SC_NS);
//Me encuentro en S3 y hay una solicitud
    			for(int i=0; i<i_nFlit_to_rx.read(); i++){
    	    		i_Req=1;
    	    		i_SoP=0;
    	    		i_fifo_full=0;
    	    		i_ready_to_receive=0;

    				//Clock
    				i_clock = 0;
    				sc_start(10,SC_NS);
    				i_clock = 1;
    				sc_start(10,SC_NS);

    	    		i_Req=0;
    	    		i_SoP=0;
    	    		i_fifo_full=0;
    	    		i_ready_to_receive=0;
}



cout << "@" << sc_time_stamp() << "Terminating simulation" << endl;
sc_close_vcd_trace_file(wf);
    return 0;
}
