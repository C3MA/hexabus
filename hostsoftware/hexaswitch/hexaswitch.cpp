#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/crc.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "../../../hexabus_packet.h"
#define HEXABUS_PORT 61616

int crc16(const char* input, unsigned int length)
{
  boost::crc_optimal<16, 0x1021, 0x0000, 0, true, true> crc; //TODO document this! it's important and it was hard to find!
  crc.process_bytes(input, length);
  return crc.checksum();
}

void send_packet(char* addr, unsigned int port, const char* data, unsigned int length, bool wait_for_reply)
{
  boost::asio::io_service io_service;
  boost::asio::ip::udp::socket socket(io_service);
  boost::asio::ip::udp::endpoint remote_endpoint;

  socket.open(boost::asio::ip::udp::v6());
  // Für zum Broadcasten muss da noch einiges mehr -- siehe http://www.ce.unipr.it/~medici/udpserver.html
  
  remote_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(addr), port);

  boost::system::error_code error; // TODO error message?

  socket.send_to(boost::asio::buffer(data, length), remote_endpoint, 0, error);

  if(wait_for_reply)
  {
    printf("waiting for reply...\n");
    char recv_data[128];
    socket.receive(boost::asio::buffer(recv_data, 6)); // TODO see if the number of recieved bytes can be handled more flexibly
    printf("recieved message. Assuming it's a hexabus boolean value reply.\n");

    struct hxb_packet_bool* packet = (struct hxb_packet_bool*)recv_data;

    printf("Type:\t%d\nFlags:\t%d\nVID:\t%d\nValue:\t%d\nCRC:\t%u\n", packet->type, packet->flags, packet->vid, packet->value, packet->crc);
  }
}

void receive_packet(char* data, unsigned int length)
{

}

void usage()
{
    printf("\nusage: hexaswitch hostname command\n");
    printf("commands are:\n");
    printf("  on      switch device on\n");
    printf("  off     switch device off\n");
}

void send_setvalue(char* addr, bool value)
{
  struct hxb_packet_bool packet;
  packet.type = HXB_PTYPE_SETVALUE;
  packet.flags = 0;
  packet.vid = 0;
  packet.value = value ? HXB_TRUE : HXB_FALSE;
  packet.crc = crc16((char*)&packet, 4);
  // for test, output the Hexabus packet
  printf("Type:\t%d\nFlags:\t%d\nVID:\t%d\nValue:\t%d\nCRC:\t%d\n", packet.type, packet.flags, packet.vid, packet.value, packet.crc);
  printf("...\n");
  //TODO implement -v

  send_packet(addr, HEXABUS_PORT, (char*)&packet, sizeof(packet), false);
  printf("sent.\n");
}

void send_valuerequest(char* addr)
{
  struct hxb_packet_req packet;
  packet.type = HXB_PTYPE_REQVALUE;
  packet.flags = 0;
  packet.vid = 0;
  packet.crc = crc16((char*)&packet, 3);
  // for test, output the Hexabus packet
  printf("Type:\t%d\nFlags:\t%d\nVID:\t%d\nCRC:\t%d\n", packet.type, packet.flags, packet.vid, packet.crc);
  printf("...\n");
  //TODO implement -v

  send_packet(addr, HEXABUS_PORT, (char*)&packet, sizeof(packet), true);
}

int main(int argc, char** argv)
{
  printf("hexaswitch -- command line hexabus client\n");

  if(argc != 3)
  {
    usage();
    exit(1);
  }

  // build the hexabus packet
  if(!strcmp(argv[2], "on"))
    send_setvalue(argv[1], true);
  else if(!strcmp(argv[2], "off"))
    send_setvalue(argv[1], false);
  else if(!strcmp(argv[2], "status"))
  {
    send_valuerequest(argv[1]);
    char data[128];
    receive_packet(data, 128);
  }
  else
  {
    usage();
    exit(1);
  }

  exit(0);
}
