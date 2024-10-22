#ifndef RX_H_
#define RX_H_

#include <iostream>

using namespace std;
#include "systemc.h"

#define DATA_WIDTH (8)

SC_MODULE(RX){
	sc_in_clk 		i_clock;	//Input: System Clock
	sc_in<bool>		i_reset;	//Input: System Reset
	sc_in<bool>		i_Req;		//Output:  Indicates whether the request was granted
	sc_in<bool> 	i_SoP;		//Output:  Indicates the start of a packet
	sc_in<bool>		i_ready_to_receive;	//Input: Indicates that there is a pack to receive
	sc_in<bool>		i_fifo_full;
	sc_in<sc_uint<DATA_WIDTH>>		i_fifo_dataOut;	//Input: Data_out from FIFO
	sc_out<bool>	o_OnOff;	//Output: Indicates availability to receive a flit
	sc_out<bool> 	o_fifo_push;		//Output:  Add an element to  FIFO

	sc_signal<int> state, prev_state;

	sc_int<9> count;

	void transition(){
		if (i_reset.read() && i_clock.posedge()){
			state = 0;
			o_OnOff.write(0);
			o_fifo_push.write(0);
			prev_state = 0;
			cout<<"@"<< sc_time_stamp() <<":: Reseted IDLE "<< endl;
		}else if(i_clock.posedge()){
			prev_state = state;		// Mantengo actualizado el estado anterior
			  switch (state.read()) {
			  	  case 0:
					o_OnOff.write(0);
					o_fifo_push.write(0);

					// Cuando se indique que el módulo está listo para recibir, iremos al estado 1 con las salidas en 0
					if(i_ready_to_receive.read()){

						state = 1;
						cout<<":: from S0:RESET to S1:IDLE"<< endl;

			      } else { state = 0;} break;

			  	  case 1:

						o_OnOff.write(1);
						o_fifo_push.write(0);

			    	  if (i_Req.read() && i_SoP.read()) {
			    	  state = 2;

			    	  cout<<":: from S1:IDLE to S2:START OF RECEPTION"<< endl;

			      } else { state = 1;} break;

			  	  case 2:
						o_OnOff.write(0);
						o_fifo_push.write(1);
						count = i_fifo_dataOut.read() + 1;	//Input: Data_out from FIFO.read();
						//Las siguientes dos líneas las junto en la de arriba (58) sólo sumándole 1 a la cuenta.
//						count += 2;
//						count--;

						if ( !(i_fifo_full.read()) ) {
							state = 3;

							cout<<":: from S2:START OF RECEPTION to S3:RECIVING"<< endl;

				      } else { state = 2;}break;

			      case 3:
			    	  o_OnOff.write(1);
			    	  o_fifo_push.write(0);

			    	  if(count == 0){
			    		state = 1;
  			    		cout<<":: from S3:RECIVING to S1:IDLE"<< endl;
		    	 	  } else if (i_Req.read()) {
			    	 	state = 4;
			    	 	cout<<":: from S3:RECIVING to S4::RECEIVED"<< endl;
			    	  } else { state = 3;}break;

			      default:
					o_OnOff.write(0);
					o_fifo_push.write(1);
		    	 	count--;

		    	 	if(!i_fifo_full){
		    	 		state = 3;
			    		cout<<":: from S4:RECEIVED to S3:RECIVING"<< endl;
		    	 	} else { state = 4;} break;
			  	  } // end switch
			 }// end else->reset
		}// end transition

			    SC_CTOR(RX): //	Labeling ports
			    	i_clock("i_clock"),	//Input: System Clock
			    	i_reset("i_reset"),	//Input: System Reset
					i_Req("i_Req"),	//Output:  Indicates whether the request was granted
					i_SoP("i_SoP"),		//Output:  Indicates the start of a packet
					i_ready_to_receive("i_ready_to_receive"),	//Input: Indicates that there is a pack to receive
					i_fifo_full("i_fifo_full"),
					i_fifo_dataOut("i_fifo_dataOut"),
					o_OnOff("o_OnOff"),	//Output: Indicates availability to receive a flit
					o_fifo_push("o_fifo_push")		//Output:  Add an element to  FIFO
			    {
			        SC_METHOD(transition);
			        sensitive << i_clock.pos() << i_reset;
			    }
			};
#endif /* RX_H_ */
