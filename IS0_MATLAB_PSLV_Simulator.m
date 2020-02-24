%% Clear everything and close serial ports 
if ~isempty(instrfind)
     fclose(instrfind);
      delete(instrfind);
end

clear all;
close all;
clc;
%%
%Setup Code 
%Run this one time, to setup serial port and TCP socket
%Close the present 
if ~isempty(instrfind)
     fclose(instrfind);
      delete(instrfind);
end
serial_port = serial('COM5', 'BaudRate', 2000000, 'Timeout', 10);
fopen(serial_port);

% Configuration and connection
tcp_port = tcpip('localhost', 17100, 'NetworkRole', 'client');

% Open socket and wait before sending data
fopen(tcp_port);

% This is a one byte command sent by the PSLV, can be changed to exact
% command once finalized
PSLV_Command = 'A';
packet_size = 156;

%%
% Data collection Code 
% Collects a packet from the IS0 Payload using the serial port 
% Sends the collected packet to a TCP port to Hydra for decoding 

% Collecting a beacon packet every 10 seconds 
% Collecting the packet two bytes at a time by sending the PSLV Command 
i=0;
%while(i<100)
    packet_array=zeros(1,156);
    for command_number=1: (packet_size/2)
        %Putting a delay of 0.01 seconds between consecutive packets 
        pause(0.01);
        fwrite(serial_port,PSLV_Command );
        resp = fread(serial_port, 2); 
        packet_array(2*command_number-1) = resp(1);
        packet_array(2*command_number) = resp(2);
    end
    % Writing the Packet to the TCP Socket 
    fwrite(tcp_port,packet_array);
    i=i+1;
    pause(5);
%end