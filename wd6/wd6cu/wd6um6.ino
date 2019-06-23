int um6_checksum(UM6_packet *um6p);
int um6_makepacket(UM6_packet *um6p, uint8_t addr, byte *data);
int  um6_makecommpacket(UM6_packet *um6p, uint8_t broadcast_rate, uint8_t baud_rate, uint8_t GPS_baud_rate,
             uint8_t SAT, uint8_t SUM, uint8_t VEL, uint8_t REL, uint8_t POS, 
             uint8_t TEMP, uint8_t COV, uint8_t EU, uint8_t QT, uint8_t MP, uint8_t AP, 
             uint8_t GP, uint8_t MR, uint8_t AR, uint8_t GR, uint8_t BEN);
int parse_serial_data(uint8_t* rx_data, uint8_t rx_length, UM6_packet * packet);
int readum6(UM6_packet *um6p);
int read_um6_packet(uint8_t addr, UM6_packet *um6p);
int read_um6_packet_any(UM6_packet *um6p);

UM6_packet g_um6pr={0},g_um6pw={0};

int um6_checksum(UM6_packet *um6p)
{
  um6p->Checksum='s'+'n'+'p'+um6p->PT+um6p->Address;
  return(0);
}

int um6_makepacket(UM6_packet *um6p, uint8_t addr, byte *data)
{
  uint8_t i;

  um6p->Address=addr;
  if(data == NULL) {
    um6p->PT=0;
  } else {
    um6p->PT=1<<7;
  }
  um6_checksum(um6p);
  um6p->data[0]='s';
  um6p->data[1]='n';
  um6p->data[2]='p';
  um6p->data[3]=um6p->PT;
  um6p->data[4]=um6p->Address;
  if(data == NULL) {
    i=5;
  } else {
    for(i=0;i < 4;i++) {
      um6p->data[5+i]=data[3-i];
      um6p->Checksum+=data[3-i];
    }
    i=9;
  }
  um6p->data[i]=(uint8_t)(um6p->Checksum>>8);
  um6p->data[i+1]=(uint8_t)um6p->Checksum;
  um6p->data_length=i+2; 
  return(0);
}

int  um6_makecommpacket(UM6_packet *um6p, uint8_t broadcast_rate, uint8_t baud_rate, uint8_t GPS_baud_rate,
             uint8_t SAT, uint8_t SUM, uint8_t VEL, uint8_t REL, uint8_t POS, 
             uint8_t TEMP, uint8_t COV, uint8_t EU, uint8_t QT, uint8_t MP, uint8_t AP, 
             uint8_t GP, uint8_t MR, uint8_t AR, uint8_t GR, uint8_t BEN)
{
  uint8_t i;

  um6p->Address=UM6_REG_COMMUNICATION;
  um6p->PT=1<<7;
  
  um6p->data[0]='s';
  um6p->data[1]='n';
  um6p->data[2]='p';
  um6p->data[3]=um6p->PT;
  um6p->data[4]=um6p->Address;

  um6p->data[5]=BEN<<6|GR<<5|AR<<4|MR<<3|GP<<2|AP<<1|MP;
  um6p->data[6]=QT<<7|EU<<6|COV<<5|TEMP<<4|POS<<3|REL<<2|VEL<<1|SUM;
  um6p->data[7]=SAT<<7|GPS_baud_rate<<3|baud_rate;
  um6p->data[8]=broadcast_rate;

  um6p->Checksum=um6p->data[0];
  for(i=1;i < 9;i++) {
    um6p->Checksum+=um6p->data[i];
  }
  um6p->data[9]=(uint8_t)(um6p->Checksum>>8);
  um6p->data[10]=(uint8_t)um6p->Checksum;
  um6p->data_length=11; 
  return(0);

}

int parse_serial_data(uint8_t* rx_data, uint8_t rx_length, UM6_packet * packet)
{
  uint8_t index;
  uint8_t packet_index;
  uint8_t PT;
  uint8_t packet_has_data;
  uint8_t  packet_is_batch;
  uint8_t batch_length;
  uint8_t data_length;
  uint16_t computed_checksum;
  uint16_t received_checksum;

// Make sure that the data buffer provided is long enough to contain a full packet
// The minimum packet length is 7 bytes
  if( rx_length < 7 ) {
    return(-1);
  }
// Try to find the ‘snp’ start sequence for the packet
  for( index = 0; index < (rx_length-2); index++ ) {
// Check for ‘snp’.  If found, immediately exit the loop
    if( rx_data[index] == 's' && rx_data[index+1] == 'n' && rx_data[index+2] == 'p' ) {
      break;
    }
  }
  packet_index=index;
// Check to see if the variable ‘packet_index’ is equal to (rx_length -2).  If it is, then the above
// loop executed to completion and never found a packet header.
  if( packet_index ==(rx_length-2) ) {
    return(-2);
  }
// If we get here, a packet header was found.  Now check to see if we have enough room
// left in the buffer to contain a full packet.  Note that at this point, the variable ‘packet_index’ 
// contains the location of the ‘s’ character in the buffer (the first byte in the header)
  if( (rx_length-packet_index) < 7 ) {
    return(-3);
  }
// We’ve found a packet header, and there is enough space left in the buffer for at least
// the smallest allowable packet length (7 bytes).  Pull out the packet type byte to determine
// the actual length of this packet
  PT = rx_data[packet_index + 3];
// Do some bit-level manipulation to determine if the packet contains data and if it is a batch
// We have to do this because the individual bits in the PT byte specify the contents of the
// packet.
  packet_has_data = (PT >> 7) & 0x01;     
// Check bit 7 (HAS_DATA)
  packet_is_batch = (PT >> 6) & 0x01;      
// Check bit 6 (IS_BATCH)
  batch_length = (PT >> 2) & 0x0F;            
// Extract the batch length (bits 2 through 5)

// Now finally figure out the actual packet length
  data_length = 0;
  if( packet_has_data ) {
    if( packet_is_batch ) {
// Packet has data and is a batch.  This means it contains ‘batch_length'registers, each
// of which has a length of 4 bytes
      data_length = 4*batch_length;
    } else {
// Packet has data but is not a batch.  This means it contains one register (4 bytes)
      data_length = 4;
    }
  } else {
// Packet has no data
    data_length = 0;
  }
// At this point, we know exactly how long the packet is.  Now we can check to make sure
// we have enough data for the full packet.
  if( (rx_length-packet_index) < (data_length + 5) ) {
    return(-4);
  }
// If we get here, we know that we have a full packet in the buffer.  All that remains is to pull
// out the data and make sure the checksum is good.
// Start by extracting all the data
  packet->Address = rx_data[packet_index + 4];
  packet->PT = PT;
// Get the data bytes and compute the checksum all in one step
  packet->data_length = data_length;
//computed_checksum = 's' + 'n' + 'p'+ packet_data->PT + packet_data->Address;
  computed_checksum = 's' + 'n' + 'p'+ packet->PT + packet->Address;
  for( index = 0; index < data_length; index++ ) {
// Copy the data into the packet structure’s data array
    packet ->data[index] = rx_data[packet_index + 5 + index];
// Add the new byte to the checksum
    computed_checksum+= packet->data[index];
  }
// Now see if our computed checksum matches the received checksum
// First extract the checksum from the packet
  received_checksum = (rx_data[packet_index + 5 + data_length] << 8);
  received_checksum |= rx_data[packet_index + 6 + data_length];
// Now check to see if they don’t match
  if( received_checksum != computed_checksum ) {
    return(-5);
  }
// At this point, we’ve received a full packet with a good checksum.  It is already
// fully parsed and copied to the ‘packet’ structure, so return 0 to indicate that a packet was
// processed.
  return(0);
}

int readum6(UM6_packet *um6p)
{
  int psd;
  unsigned char c1;

  while(Serial2.available()) {
    c1=(unsigned char)Serial2.read();
    if(um6p->spl >= sizeof(um6p->spd)) return(-1);
    um6p->spd[um6p->spl++]=c1;
    if(um6p->spl == 1) {
      if(c1 != 's') {
        um6p->spl=0;
      }
      return(1);
    }
    if(um6p->spl == 2) {
      if(c1 != 'n') {
        um6p->spl=0;
      }
      return(2);
    }
    if(um6p->spl == 3) {
      if(c1 != 'p') {
        um6p->spl=0;
      }
      return(3);
    }
    psd=parse_serial_data(um6p->spd,um6p->spl,um6p);
    return(psd);
  }
  return(4);
}

int read_um6_packet(uint8_t addr, UM6_packet *um6p)
{
  int psd,zc=0;

  um6p->spl=0;
  for(;;) {
    psd=readum6(um6p);
    if(psd == 0) {
      zc=0;
      if(um6p->Address == addr) return(1);
    } else {
      if(psd == -1) zc++;
      if(zc > 3) break;
    }
  }

  return(0);
}

int read_um6_packet_any(UM6_packet *um6p)
{
  int psd,zc=0;

  um6p->spl=0;
  for(;;) {
    psd=readum6(um6p);
    if(psd == 0) {
      zc=0;
      return(1);
    } else {
      if(psd == -1) zc++;
      if(zc > 3) break;
    }
  }

  return(0);
}

int um6_setup(void)
{
  Serial2.begin(115200);

  um6_makecommpacket(&g_um6pw,0,5,0,0,0,0,0,0,1,0,1,0,1,0,0,1,1,1,0);
  Serial2.write(g_um6pw.data,g_um6pw.data_length);
  for(;;) {
    if(read_um6_packet_any(&g_um6pr) != 1) {
      Serial.println("***** UM6 comm packet read FAIL!");
      return(-1);
    }
    if(g_um6pr.Address == UM6_REG_COMMUNICATION) break;
  }
  um6_makepacket(&g_um6pw,UM6_CMD_GET_FW_VERSION,NULL);
  Serial2.write(g_um6pw.data,g_um6pw.data_length);
  read_um6_packet_any(&g_um6pr);
  um6_makepacket(&g_um6pw,UM6_CMD_GET_FW_VERSION,NULL);
  Serial2.write(g_um6pw.data,g_um6pw.data_length);
  if(read_um6_packet_any(&g_um6pr) == 1) {
    Serial.print("***** UM6 fw: ");
    Serial.print(g_um6pr.data[0]);
    Serial.print(g_um6pr.data[1]);
    Serial.print(g_um6pr.data[2]);
    Serial.print(g_um6pr.data[3]);
    Serial.println();
  } else {
    Serial.println("***** UM6 fw packet read FAIL!");
  }
  return(0);
}
