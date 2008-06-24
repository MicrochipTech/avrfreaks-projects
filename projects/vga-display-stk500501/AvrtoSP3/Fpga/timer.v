// timer.v

module timer(clk,reset,a,d,O,wdstb,irq,toggle);
    input clk;
    input reset;
    input [7:0]d;
	 output [7:0] O;
    input [2:0]a;
    input wdstb;
    output irq;
    output toggle;

    reg irq;
    wire [7:0] O;
    reg toggle;

    reg [15:0] cntr;            //counter
    reg [15:0] reload;            //counter reload register
    reg [15:0] latch;            //latch register for counter
    wire [7:0] status;          //status register
    reg [7:0] ctrl;             //control register
    reg [7:0] tb;               //timebase divider
    wire [2:0] tbsel;           //timebase selector
    reg tbmux;
    reg tbD1,TimeBase;
    wire EnCntr;                //enable counter
    wire EnIrq;                 //enable interrupt bit
    reg [7:0] rmux;                   //register multiprexor

    parameter TIMER_CNTRL       =   3'h0;   //address of counter low
    parameter TIMER_CNTRH       =   3'h1;
    parameter TIMER_RLDLO       =   3'h2;
    parameter TIMER_RLDHI       =   3'h3;
    parameter TIMER_LCHLO       =   3'h4;
    parameter TIMER_LCHHI       =   3'h5;
    parameter TIMER_STATUS      =   3'h6;   //read only address
    parameter TIMER_DOLATCH     =   3'h6;   //write only address
    parameter TIMER_CTRL        =   3'h7;


    always @ (posedge clk) begin
        tb <= tb + 1;
    end

    assign tbsel = ctrl[2:0];
    assign EnCntr = ctrl[7];
    assign EnIrq = ctrl[6];

    always @ (tbsel or tb) begin
        case (tbsel)
            3'b000:tbmux = tb[0];
            3'b001:tbmux = tb[1];
            3'b010:tbmux = tb[2];
            3'b011:tbmux = tb[3];
            3'b100:tbmux = tb[4];
            3'b101:tbmux = tb[5];
            3'b110:tbmux = tb[6];
            3'b111:tbmux = tb[7];
            default:tbmux = tb[0];
        endcase
    end

    always @(posedge clk) tbD1 <= tbmux;
    always @(posedge clk) TimeBase <= tbD1 & !tbmux;

    always @ (posedge clk or posedge reset) begin
        if(reset)
            cntr <= 0;
        else begin
            if(TimeBase & EnCntr)
                cntr <= cntr + 16'b000000000001;
            else if (cntr == reload)
                cntr <= 16'b0000000000000000;   //reset back to zero
        end
    end

    always @ (posedge clk or posedge reset) begin
        if(reset) 
            reload[7:0] <= 8'hff;
        else begin
            if(wdstb && (a == TIMER_RLDLO) )
                reload[7:0] <= d;
        end
    end

    always @ (posedge clk or posedge reset) begin
        if(reset) 
            reload[15:8] <= 8'hff;
        else begin
            if(wdstb && (a == TIMER_RLDHI) )
                reload[15:8] <= d;
        end
    end

    always @ (posedge clk) begin
        if(wdstb && (a == TIMER_DOLATCH)) begin
            latch <= cntr;
        end
    end

    always @ (posedge clk or posedge reset) begin
        if(reset)
            ctrl <= 0;
        else begin
            if(wdstb && (a == TIMER_CTRL) )
                ctrl <= d;
        end            
    end

    always @ (posedge clk or posedge reset) begin
        if(reset)
            irq <= 0;       //clear interrupt bit
        else begin
            if(EnIrq && (cntr == reload) )
                irq <= 1;       //set interrupt
            else if (wdstb && (a == TIMER_DOLATCH))
                irq <= 0;       //clear interrupt
        end
    end

    assign status[7] = ctrl[7];
    assign status[6] = irq;
    assign status[5:0] = ctrl[5:0];

    /////////////////////////////////////////////////////////////////////////////
    // multiprexor to read out data registers
    /////////////////////////////////////////////////////////////////////////////


    always @ (a or cntr or reload or latch or ctrl or status) begin
        case (a)
            TIMER_CNTRL:rmux = cntr[7:0];
            TIMER_CNTRH:rmux = cntr[15:8];
            TIMER_RLDLO:rmux = reload[7:0];
            TIMER_RLDHI:rmux = reload[15:8];
            TIMER_LCHLO:rmux = latch[7:0];
            TIMER_LCHHI:rmux = latch[15:8];
            TIMER_STATUS:rmux = status;
            TIMER_CTRL:rmux = ctrl;
            default:rmux = 8'h5a;
        endcase
    end

    ///////////////////////////////////////////////////////////////////////////
    // timer port
    ///////////////////////////////////////////////////////////////////////////

    assign O = rmux;

    always @ (posedge clk) begin
       if (cntr == reload)
          toggle = !toggle;   //reset back to zero
    end


endmodule
