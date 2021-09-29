#include <nall/string.hpp>

enum {
	PATCH0_LOW,
	PATCH0_MID,
	PATCH0_HI,
	PATCH1_LOW=4,
	PATCH1_MID,
	PATCH1_HI,
	PATCH2_LOW=8,
	PATCH2_MID,
	PATCH2_HI,
	PATCH3_LOW=12,
	PATCH3_MID,
	PATCH3_HI,
	PATCH4_LOW=16,
	PATCH4_MID,
	PATCH4_HI,
	PATCH5_LOW=20,
	PATCH5_MID,
	PATCH5_HI,
	PATCH6_LOW=24,
	PATCH6_MID,
	PATCH6_HI,
	PATCH7_LOW=28,
	PATCH7_MID,
	PATCH7_HI,
	PATCH8_LOW=32,
	PATCH8_MID,
	PATCH8_HI,
	PATCH9_LOW=36,
	PATCH9_MID,
	PATCH9_HI,
	PATCH10_LOW=40,
	PATCH10_MID,
	PATCH10_HI,
	RANGE0_START_LOW=44,
	RANGE0_START_MID,
	RANGE0_START_HI,
	RANGE1_START=48,
	RANGE1_START_MID,
	RANGE1_START_HI,
	MAGIC_LOW=56,
	MAGIC_MID,
	MAGIC_HI,
	RANGE0_END_LOW=64,
	RANGE0_END_MID,
	RANGE0_END_HI,
	RANGE1_END_LOW=68,
	RANGE1_END_MID,
	RANGE1_END_HI,
	RANGE0_DEST_LOW=80,
	RANGE0_DEST_HI,
	RANGE0_MASK,
	RANGE1_DEST_LOW=84,
	RANGE1_DEST_HI,
	RANGE1_MASK,
	
	MORE_MYSTERY=219,
	UNKNOWN_REG=221,
	UNKNOWN_REG2,
	UNKNOWN_REG3,
};

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


XBANDBase xband_base;
XBANDBase::XBANDState *x, obj;

nall::string incoming_messages[62];

int consecutive_reads = 0;
uint32_t packet_index = 0;
bool conn_active = false;
bool injected = false;
uint16_t next_pckFirstByteSeq = 0;
uint8_t loop_count = 0;

uint8_t payload[73] = {
	0x00, 0x05, 0x39, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x02,
	0x17, 0x04, 0x00, 0x40, 0x09, 0x00, 0x00, 0x00, 0x32, 0x9c,
	0x21, 0x21, 0xa9, 0x1f, 0x8d, 0x22, 0x21, 0x9c, 0x22, 0x21, 
  0xa9, 0x0f, 0x8d, 0x00, 0x21, 0xea, 0xea, 0xea, 0xea, 0xea,
  0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea,
  0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 
  0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0x17,
  0x58, 0x10, 0x03
};

// STP payload
//uint8_t payload[27] = {
//	0x00, 0x05, 0x39, 0x00, 0x00, 0x02, 0x93, 0x00, 0x00, 0x02,
//	0x18, 0x04, 0x00, 0x40, 0x09, 0x00, 0x00, 0x00, 0x04, 0xdb,
//	0xdb, 0xdb, 0xdb, 0x0a, 0x20, 0x10, 0x03
//};

//uint8_t payload[53] = {
//	0x00,0x05,0x39,0x00,0x00,0x01,0xAC,0x00,0x00,0x02,
//	0x17,0x04,0x00,0x40,0x10,0x00,0x00,0x00,0x00,0x00,
//	0x00,0x00,0xAE,0xDE,0x10,0x03,0x00,0xBE,0xEF,0x00,
//	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,
//	0x09,0x04,0x00,0x00,0x00,0x22,0xFF,0xFF,0xFF,0x58,
//	0x7D,0x10,0x03
//};

void import_sram() {
	fprintf(stderr, "[+] IMPORTING SRAM\n");
  file fp;

  if(fp.open("xband_sram.bin", file::mode::read)) {
    unsigned filesize = fp.size();
    for (unsigned i = 0; i < SNES::memory::xbandSram.size() && i < filesize; i++) {
    	SNES::memory::xbandSram.write(i, fp.read());
    }
    fp.close();
  }
  else
  	fprintf(stderr, "[X] FAILED TO READ SRAM\n");
}

void XBANDBase::Enter() {
	xband_base.enter();
}

void XBANDBase::enter() {
	while(true) {
    scheduler.synchronize();
	  step(240*2);
    synchronize_cpu();
  }
}

void XBANDBase::init() {
	SNES::memory::xbandSram.map(allocate<uint8_t>(0x10000, 0x00), 0x10000);
}

void XBANDBase::enable() {
  bus.map(Bus::MapMode::Direct, 0xe0, 0xff, 0x0000, 0xffff, *this);
}


void XBANDBase::power() {
  reset();
  import_sram();
}

void XBANDBase::reset() {
	x = &obj;
	
	// power on values of these registers on cart
	memset(x->regs, 0, sizeof(x->regs));
	x->regs[0x7C] = 0;
	x->regs[0x7D] = 0x80;
	x->regs[0xB4] = 0x7F;
	//x->regs[0xD9] = 0x46;
	
	//modem regs settings from _PUResetModem in xband source
	x->modem_regs[0x19] = 0x46;
	//x->modem_regs[0x1C] = 0x36;
	//x->modem_regs[0x1D] = 0x8A;

	x->regs[UNKNOWN_REG2] = 8;

	incoming_messages[1] = "msUnusedMessageHandler";
	incoming_messages[2] = "msEndOfStream";
	incoming_messages[3] = "msGamePatch";
	incoming_messages[4] = "msSetDateAndTime";
	incoming_messages[5] = "msServerMiscControl";
	incoming_messages[9] = "msExecuteCode";
	incoming_messages[10] = "msPatchOSCode";
	incoming_messages[12] = "msRemoveDBTypeOpCode";
	incoming_messages[13] = "msRemoveMessageHandler";
	incoming_messages[14] = "msRegisterPlayer";
	incoming_messages[15] = "msNewNGPList";
	incoming_messages[16] = "msSetBoxSerialNumber";
	incoming_messages[17] = "msGetTypeIDsFromDB";
	incoming_messages[18] = "msAddItemToDB";
	incoming_messages[19] = "msDeleteItemFromDB";
	incoming_messages[20] = "msGetItemFromDB";
	incoming_messages[21] = "msGetFirstItemIDFromDB";
	incoming_messages[22] = "msGetNextItemIDFromDB";
	incoming_messages[23] = "msClearSendQ";
	incoming_messages[27] = "msLoopBack";
	incoming_messages[28] = "msWaitForOpponent";
	incoming_messages[29] = "msOpponentPhoneNumber";
	incoming_messages[30] = "msReceiveMail";
	incoming_messages[31] = "msNewsHeader";
	incoming_messages[32] = "msNewsPage";
	incoming_messages[33] = "msUNUSED1";
	incoming_messages[34] = "msQDefDialog";
	incoming_messages[35] = "msAddAddressBookEntry";
	incoming_messages[36] = "msDeleteAddressBookEntry";
	incoming_messages[37] = "msReceiveRanking";
	incoming_messages[38] = "msDeleteRanking";
	incoming_messages[39] = "msGetNumRankings";
	incoming_messages[40] = "msGetFirstRankingID";
	incoming_messages[41] = "msGetNextRankingID";
	incoming_messages[42] = "msGetRankingData";
	incoming_messages[43] = "msSetBoxPhoneNumber";
	incoming_messages[44] = "msSetLocalAccessPhoneNumber";
	incoming_messages[45] = "msSetConstants";
	incoming_messages[46] = "msReceiveValidPers";
	incoming_messages[47] = "msGetInvalidPers";
	incoming_messages[48] = "msDeleteUncorrelatedAddressBookEntries";
	incoming_messages[49] = "msCorrelateAddressBookEntry";
	incoming_messages[50] = "msReceiveWriteableString";
	incoming_messages[51] = "msReceiveCredit";
	incoming_messages[52] = "msReceiveRestrictions";
	incoming_messages[53] = "msReceiveCreditToken";
	incoming_messages[54] = "msSetCurrentUserName";
	incoming_messages[56] = "msSetBoxHometown";
	incoming_messages[57] = "msGetConstant";
	incoming_messages[58] = "msReceiveProblemToken";
	incoming_messages[59] = "msReceiveValidationToken";
	incoming_messages[60] = "msLiveDebitSmartCard";
	incoming_messages[61] = "msSendDialScript";
	incoming_messages[62] = "msSetCurrentUserNumber";
	incoming_messages[63] = "msBoxWipeMind";
	incoming_messages[64] = "msGetHiddenSerials";
	incoming_messages[66] = "msGetLoadedGameInfo";
	incoming_messages[67] = "msClearNetOpponent";
	incoming_messages[68] = "msGetBoxMemStats";
	incoming_messages[69] = "msReceiveRentalSerialNumber";
	incoming_messages[70] = "msReceiveNewsIndex";
	incoming_messages[71] = "msReceiveBoxNastyLong";
}

void XBANDBase::unload() {
}

uint8_t process_adsp_packet_in(uint32_t index) {
	if ((x->rx_packet_dbg[14] == 0x2D) && !injected) {
		fprintf(stderr, "[+] INJECTING PAYLOAD INTO RXBUF\n\n");
		// should we check that there is enough room in the rxbuff ?
		//packet_index points to the last byte of our trigger packet so start
		//writing over the next one
		index += 1;
		fprintf(stderr, "[-] BYTE INJECTION INDEX: %d\n", index);
		for(int i = 0; i < sizeof(payload); i++) {
			fprintf(stderr, "[+] OLD BYTE: 0x%02X\n", x->rxbuf[index + i]);
			x->rxbuf[index + i] = payload[i];
			fprintf(stderr, "[+] NEW BYTE: 0x%02X\n", x->rxbuf[index + i]);
		}
		injected = true;
		return sizeof(payload);
	}
	return 0;
}

void print_adsp_debug_in(uint8_t packet_size) {
	if(packet_size < 14) {
		fprintf(stderr, "[X] BAD PACKET SIZE:\t\t%d\n", packet_size);
		return;
	}

	fprintf(stderr, "<<<<<<<<<<<<<<<<<<<<<<<<[ADSP PACKET]<<<<<<<<<<<<<<<<<<<<<<<<\n");
	uint8_t byte_count = 0;
	for (uint16_t i = 0; i <= packet_size; i++) {
		fprintf(stderr, "0x%02X ", x->rx_packet_dbg[i]);
		byte_count++;
		
		if (byte_count >= 15) {
			byte_count = 0;
			fprintf(stderr, "\n");
		}
	}
	fprintf(stderr, "\n\n<<<<<<<<<<<<<<<<<<<<<<<<<<<[HEADER]<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	for (uint8_t i = 1; i <= 13; i++) {
		fprintf(stderr, "0x%02X ", x->rx_packet_dbg[i]);
	}
	fprintf(stderr, "\n\n");
	fprintf(stderr, "[Source ConnID]:\t\t0x%02X%02X\n[PktFirstByteSeq]:\t\t0x%02X%02X%02X%02X\n[PktNextRecvSeq]:\t\t0x%02X%02X%02X%02X\n",
		x->rx_packet_dbg[1], x->rx_packet_dbg[2], x->rx_packet_dbg[3], x->rx_packet_dbg[4], 
		x->rx_packet_dbg[5], x->rx_packet_dbg[6], x->rx_packet_dbg[7], x->rx_packet_dbg[8],
		x->rx_packet_dbg[9], x->rx_packet_dbg[10]);
	fprintf(stderr, "[PktRecvWindow]:\t\t0x%02X%02X\n[ADSP Descriptor]:\t\t0x%02X\n",
		x->rx_packet_dbg[11], x->rx_packet_dbg[12], x->rx_packet_dbg[13]);
	
	if(packet_size < 14) {
		return;
	}

	fprintf(stderr, "\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<[DATA]<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	byte_count = 0;
	uint16_t byte_total = 0;
	for (uint16_t i = 0x0E; i <= packet_size; i++) {
		fprintf(stderr, "0x%02X ", x->rx_packet_dbg[i]);
		byte_count++;
		byte_total++;
		
		if (byte_count >= 15) {
			byte_count = 0;
			fprintf(stderr, "\n");
		}
	}
	fprintf(stderr, "\n[DATA LENGTH]:\t\t%02X | %d\n", byte_total, byte_total);
	next_pckFirstByteSeq = x->rx_packet_dbg[3] +
						 						 x->rx_packet_dbg[4] +
						 						 x->rx_packet_dbg[5] +
						 						 x->rx_packet_dbg[6] +
			      						 (byte_total-4); //remove the CRC and EOP bytes
	fprintf(stderr, "[NEXT VALID pckFirstByteSeq]:\t\t%02X", next_pckFirstByteSeq);
	fprintf(stderr, "\n\n");
	const char* message_name = incoming_messages[x->rx_packet_dbg[14]];
	fprintf(stderr, "[XBAND MSG ID]:\t\t0x%X  | %s\n",  x->rx_packet_dbg[14], message_name);
	fprintf(stderr, "\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	fprintf(stderr, "\n\n\n\n\n\n");
}

void XBANDBase::xband_send_identity() {
	char hdr1[1024] = "///////EMU-";
	char *id = "Waj04qaASNfmaRNw";
	strcat(hdr1, id);
	strcat(hdr1, "\x0a");
	::write(x->conn, &hdr1, strlen(hdr1));
	x->net_step = 2;
}

uint8 XBANDBase::read(unsigned addr) {
  // 0xE00000 - 0xFAFFFF
  // 0xFB0000 - 0xFBBFFF
  //(0xFBC000 - 0xFBFE00)
  // 0xFC0000 - 0xFFFFFF
  // 0x600000 - 0x7DFFFF
  if(within<0xe0, 0xfa, 0x0000, 0xffff>(addr)
  || within<0xfb, 0xfb, 0x0000, 0xbfff>(addr)
  || within<0xfc, 0xff, 0x0000, 0xffff>(addr)
  || within<0x60, 0x7d, 0x0000, 0xffff>(addr)) {
  	addr = (addr & 0xffff);
		addr = bus.mirror(addr, memory::xbandSram.size());
    return memory::xbandSram.read(addr);
  }

  //0xFBC000 -- 0xFBFDFF
  // seems if I change the read width of this beyond fbfdff, xband wont start...
  if(within<0xfb, 0xfb, 0xc000, 0xfdff>(addr)) {
	  uint8 reg = (addr-0xFBC000)/2;
		
    if (reg == 0x7d)
			return 0x80;
		if (reg == 0xb4) //kLEDData
			return 0x7f;

		if (reg == 0x94) { //krxbuff
			if (x->rxbufconsumed >= x->rxbufindex) return 0;
			uint8 r = x->rxbuf[x->rxbufconsumed];
			//fprintf(stderr, "[---] RXBUF BYTE CONSUMED: 0x%02X | rxbufconsumed: 0x%02X\n", r, x->rxbufconsumed);
			x->rxbufconsumed++;
			if (x->rxbufconsumed == x->rxbufindex) {
				//fprintf(stderr, "[*] reset the rxbufconsumed and the rxbufindex to 0\n");
				x->rxbufconsumed = x->rxbufindex = 0;
			}
			return r;
		}

		if (reg == 0x98) { //kreadmstatus2
			if (x->net_step) {
				ssize_t ret = ::read(x->conn, &x->rxbuf[x->rxbufindex], sizeof(x->rxbuf) - x->rxbufindex);
				
				if (ret != -1 && ret != 0) {
					if (x->net_step == 1) {
						xband_send_identity();
						::write(x->conn, x->txbuf, x->txbufindex);
						x->txbufindex = 0;
					}
					
					// debugging ---------------------------
					// copy each of the new bytes added to the ring buffer this read
					// to the packet debug array and process them.
     		  for (int i = 0; i < ret; i++) {
        		x->rx_packet_dbg[packet_index] = x->rxbuf[x->rxbufindex + i];
        		//fprintf(stderr, "[-] BYTE IN THE RXBUF 0x%02X | loop: %d | rxbufindex: %d | loop_count: %d\n", x->rxbuf[x->rxbufindex + i], i, x->rxbufindex, loop_count);
        		if (x->rx_packet_dbg[packet_index] == 0x03) {
       		 		if (x->rx_packet_dbg[packet_index - 1] == 0x10) {
       		 			//fprintf(stderr, "\n\n\n****** FOUND END OF PACKET ******\n\n\n");
       		 			print_adsp_debug_in(packet_index);
								uint16_t written = process_adsp_packet_in(x->rxbufindex + i);
								
								//if written > remaining buffer space

								ret += written;

       		 			packet_index = 0;
     		   			for (int q = 0; q < ret; q++) {
     		   				x->rx_packet_dbg[q] = 0x00;
     		   			}
   		     		}
   		     		else
   		     			packet_index++;
   		     	}
   		     	else
   		     		packet_index++;
  	      }
  	      // debugging ---------------------------
  	      loop_count += 1;
  	      x->rxbufindex += ret;
				} else if (ret != 0) {
				  // no op
				}
				if (x->rxbufconsumed < x->rxbufindex) {
					consecutive_reads++;

					if (consecutive_reads >= 127) {
					  consecutive_reads = 0;
					  return 0;
					}  
					return 1;
				}
			}
			consecutive_reads = 0;
			return 0;
		}
		if (reg == 0xa0) {
			return 0;
		}
		if (reg >= 0xc0 && reg <= 0xff) {  //begins @ 0xFBC180 (180/2 == c0)
			uint8_t modemreg = reg - 0xc0;
			uint8_t ret = 0;

			if (modemreg == 0x00) {
			}
			switch (modemreg) {
        case 0x19: // X-RAM Data (16bit)
          ret = x->modem_regs[modemreg];
          break;
				case 0x09:
					ret = x->modem_regs[modemreg];
					break;
				case 0x0b:
					if (x->modem_line_relay) ret |= (1<<7); //TONEA
					if (x->modem_set_ATV25) {
						ret |= (1<<4); //ATV25
						x->modem_set_ATV25 = 0;
					}
					break;
				case 0x0d:
					ret |= (1<<3); //U1DET
					break;
				case 0x0e:
					ret |= 3; //k2400Baud
					break;
				case 0x0f:
					ret |= (1<<7) | (1<<5); //RLSD, CTS
					break;
				case 0x1c:
					ret = x->modem_regs[0x1c];
					break;
				case 0x1d:
					ret = x->modem_regs[0x1d];
					break;
				case 0x1e:
					ret = x->modem_regs[0x1e] | (1<<3); //TDBE
					break;
				case 0x1f:
					ret = x->modem_regs[0x1f];
					break;
				default:
					break;
			}
			return ret;
		}

		if (addr < 0xFBFE00) {
			uint32_t offset = (addr - 0xFBC001) / 2;
			if (offset < XBAND_REGS) {
				return x->regs[offset];
			} else {
				return 0x5D;
			}
		}
	}

	if (addr == 0xFBFE01) {
		return x->kill;
	} else if (addr == 0xFBFE03) {
		return x->control;
	} else {
		return 0x5D;
	}
	
  return 0x00;
}

void XBANDBase::write(unsigned addr, uint8 data) {
	if(within<0xe0, 0xfa, 0x0000, 0xffff>(addr)
  || within<0xfb, 0xfb, 0x0000, 0xbfff>(addr)
  || within<0xfc, 0xff, 0x0000, 0xffff>(addr)
  || within<0x60, 0x7d, 0x0000, 0xffff>(addr)) {
	  addr = (addr & 0xffff); //trying to match read
		addr = bus.mirror(addr, memory::xbandSram.size());
    return memory::xbandSram.write(addr, data);
  }

	uint32_t reg = (addr-0xFBC000)/2;  // collect the "register" by subtracting the default internal offset 
	if (reg == 0x90) {
		if (x->net_step == 2) {
			//debug_modem_registers();
			::write(x->conn, &data, 1);
		} else if (x->net_step == 1) {
			x->txbuf[x->txbufindex++] = data;
		}
	}

	if (reg >= 0xc0 && reg <= 0xff) { 
		uint8_t modemreg = reg - 0xc0;
		if (modemreg == 0x10) {
		}
		if (modemreg == 0x08 && (data & 1) && (x->net_step < 1)) {
			struct addrinfo hints;
			struct addrinfo *res = NULL;
			char *addr = "16bit.retrocomputing.network";
			char *port = "56969";
			printf("connecting to %s:%s...\n", addr, port);
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			int err = getaddrinfo(addr, port, &hints, &res);
			if (err != 0) {
				return;
			}

			x->conn = socket(AF_INET, SOCK_STREAM, 0);
			if (x->conn == NULL) {
				return;
			}

			err = connect(x->conn, res->ai_addr, res->ai_addrlen);
			if (err != 0) {
				return;
			}

			fcntl(x->conn, F_SETFL, O_NONBLOCK);
			freeaddrinfo(res);
			x->net_step = 1;
		}
		if (modemreg == 0x12) {
			if (data == 0x84) { //kV22bisMode
				x->modem_set_ATV25 = 1;
			}
		}
		switch(modemreg) {
			case 0x07:
				x->modem_line_relay = data & 0b10;
				if (x->modem_line_relay == 0 && x->net_step) {
					close(x->conn);
					x->net_step = 0;
					x->txbufindex = 0;
					x->rxbufindex = x->rxbufconsumed = 0;
				}
				break;
			case 0x09:
				x->modem_regs[modemreg] = data;
				break;
			case 0x1e:
				x->modem_regs[0x1e] = data & 0b00100100; //TDBIE, RDBIE
				break;
			case 0x1f:
				if (data & 0b1) printf("NEWCONF\n");
				x->modem_regs[0x1f] = data & 0b00010100; //NSIE, NCIE
				break;
			default:
				break;
		}
		return;
	}
	if (!(addr & 1)) {
		//ignore write to event address
		return;
	}

	if (addr < 0xFBFE00) {
		uint32_t offset = (addr - 0xFBC001) / 2;
		if (offset < XBAND_REGS) {
			if (offset == 0x84) {
				exit(1);
			}
			switch (offset)
			{
			  case MORE_MYSTERY:
			  case UNKNOWN_REG:
	  			data = data & 0x7F;
  				break;
			  case UNKNOWN_REG3:
			  	data = data & 0xFE;
		  		break;
			}
			x->regs[offset] = data;
		} else {
		}
	} else {
		if (addr == 0xFBFE01) {
			x->kill = data;
		} else if (addr == 0xFBFE03) {
			x->control = data;
		} else {
		}
	}
	return;
}
