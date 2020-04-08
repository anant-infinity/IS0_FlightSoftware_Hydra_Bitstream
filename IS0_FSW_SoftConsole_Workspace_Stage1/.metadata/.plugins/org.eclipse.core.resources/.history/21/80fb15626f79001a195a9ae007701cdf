`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    17:55:36 02/14/2018 
// Design Name: 
// Module Name:    PPS_B_GEN 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module i2c_clk_gen(
     input CLK,
     input reset,
     input [7:0] limit,
	 output reg I2C_SCL,
	 output reg I2C_SCL_shifted
	 );
	 
	 localparam hh = 0;
	 localparam hl = 1;
	 localparam ll = 2;
	 localparam lh = 3;
	 reg [1:0] state;
	 
	 reg [23:0] c;
	 
	 always @ (posedge CLK)
	 begin
		 if(reset == 1)
		 begin
		 	
		 	case(state)

			 	hh:
			 	begin
			 		I2C_SCL = 1;
			 		I2C_SCL_shifted = 1;
			 		if(c < limit)
			 		begin
			 			c = c + 1;
			 		end
			 		else begin
			 			c = 1;
			 			state = ll;
			 		end
			 	end

			 	hl:
			 	begin
			 		I2C_SCL = 1;
			 		I2C_SCL_shifted = 0;
			 		if(c < limit)
			 		begin
			 			c = c + 1;
			 		end
			 		else begin
			 			c = 1;
			 			state = hh;
			 		end
			 	end

			 	ll:
			 	begin
			 		I2C_SCL = 0;
			 		I2C_SCL_shifted = 0;
			 		if(c < limit)
			 		begin
			 			c = c + 1;
			 		end
			 		else begin
			 			c = 1;
			 			state = lh;
			 		end
			 	end

			 	lh:
			 	begin
			 		I2C_SCL = 0;
			 		I2C_SCL_shifted = 1;
			 		if(c < limit)
			 		begin
			 			c = c + 1;
			 		end
			 		else begin
			 			c = 1;
			 			state = hl;
			 		end
			 	end

			 	default:
			 	begin
			 		state = hh;
			 	end

		 	endcase
		 end
		 else begin
		 	I2C_SCL = 1;
		 	I2C_SCL_shifted = 1;
		 	c = 1;
		 end
	 end

	 
endmodule
