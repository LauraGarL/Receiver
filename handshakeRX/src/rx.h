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
	sc_in<sc_uint<9>>		i_nFlit_to_rx;
	sc_out<bool>	o_OnOff;	//Output: Indicates availability to receive a flit
	sc_out<bool> 	o_fifo_push;		//Output:  Add an element to  FIFO

	sc_signal<int> state, next_state;

	sc_int<9> count;

	void transition(){
		if (i_reset.read()){
			next_state = 0;
			o_OnOff.write(0);
			o_fifo_push.write(0);
			cout<<"@"<< sc_time_stamp() <<":: Reseted IDLE "<< endl;
		}else{
			  switch (state.read()) {
			  	  case 0:

			  		  if(i_ready_to_receive.read()){

			    	  next_state = 1;

			    	  cout<<":: from S0:RESET to S1:IDLE"<< endl;

						o_OnOff.write(1);
						o_fifo_push.write(0);

			      } else {
			          next_state = 0;}break;
			      case 1:
			    	  if (i_Req.read() && i_SoP.read()) {
			    	  next_state = 2;

			    	  cout<<":: from S1:IDLE to S2:START OF RECEPTION"<< endl;

						o_OnOff.write(0);
						o_fifo_push.write(1);
						count = i_nFlit_to_rx.read();

			      } else {
			          next_state = 1;}break;
			      case 2:

			    	  if ( !(i_fifo_full.read()) ) {
			    	  	next_state = 3;

			    	  	cout<<":: from S2:START OF RECEPTION to S3:RECIVING"<< endl;

						o_OnOff.write(1);
						o_fifo_push.write(0);

				      } else {
				    	next_state = 2;
				    	o_fifo_push.write(0);
				      }break;
			      default:
			    	  if(count == 0){
			    		  next_state = 1;
  			    		 cout<<":: from S3:RECIVING to S0:IDLE"<< endl;
  			    		 o_OnOff.write(0);
  						 o_fifo_push.write(0);
		    	 	 } else
			    	  if (i_Req.read()) {
			    	 	next_state = 2;

			    	 	cout<<":: from S3:RECIVING to S2::CURRENT RECEPTION"<< endl;

						o_OnOff.write(0);
						o_fifo_push.write(1);
			    	 	count--;

			    	  } else {
			    	    next_state = 3;}break;
			            	} // end switch
			        }// end else->reset
		}// end transition

	void update() {		//State Update
	    if (i_reset.read()) {
	    	state = 0;
		} else if (i_clock.posedge()) {
		    state = next_state;
		}
					}

			    SC_CTOR(RX): //	Labeling ports
			    	i_clock("i_clock"),	//Input: System Clock
			    	i_reset("i_reset"),	//Input: System Reset
					i_Req("i_Req"),	//Output:  Indicates whether the request was granted
					i_SoP("i_SoP"),		//Output:  Indicates the start of a packet
					i_ready_to_receive("i_ready_to_receive"),	//Input: Indicates that there is a pack to receive
					i_fifo_full("i_fifo_full"),
					i_nFlit_to_rx("i_nFlit_to_rx"),
					o_OnOff("o_OnOff"),	//Output: Indicates availability to receive a flit
					o_fifo_push("o_fifo_push")		//Output:  Add an element to  FIFO
			    {
			        SC_METHOD(transition);
			        sensitive << i_clock.pos() << i_reset << i_ready_to_receive << i_fifo_full << i_Req << i_SoP;

			        SC_METHOD(update);
			        sensitive << i_clock.pos() << i_reset;
			    }
			};
#endif /* RX_H_ */
