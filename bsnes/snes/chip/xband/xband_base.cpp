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
}

void XBANDBase::enable() {
  bus.map(Bus::MapMode::Direct, 0xe0, 0xff, 0x0000, 0xffff, *this);
}


void XBANDBase::power() {
  reset();
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
}

void XBANDBase::unload() {
}

void XBANDBase::debug_modem_registers() {
	fprintf(stderr, "\n----------------------------------[Rockwell Registers]----------------------------------\n");
	fprintf(
		stderr, 
		"| RA(0x07:1)     = "BYTE_TO_BINARY_PATTERN" | CONF(0x12:0-7) = "BYTE_TO_BINARY_PATTERN" | VOL(0x13:2-3)     = "BYTE_TO_BINARY_PATTERN"\n",
		BYTE_TO_BINARY(x->modem_regs[0x7]),
		BYTE_TO_BINARY(x->modem_regs[0x12]),
		BYTE_TO_BINARY(x->modem_regs[0x13]));

	fprintf(
		stderr, 
		"| LL(0x09:3)     = "BYTE_TO_BINARY_PATTERN" | DATA(0x09:2)   = "BYTE_TO_BINARY_PATTERN" | NEWC(0x0f:1)      = "BYTE_TO_BINARY_PATTERN"\n",
		BYTE_TO_BINARY(x->modem_regs[0x9]),
		BYTE_TO_BINARY(x->modem_regs[0x9]),
		BYTE_TO_BINARY(x->modem_regs[0xf]));

	fprintf(
		stderr, 
		"| TONEA(0x0b:7)  = "BYTE_TO_BINARY_PATTERN" | DTMF(0x09:5)   = "BYTE_TO_BINARY_PATTERN" | TDBE(0x1e:3)      = "BYTE_TO_BINARY_PATTERN"\n",
		BYTE_TO_BINARY(x->modem_regs[0xb]),
		BYTE_TO_BINARY(x->modem_regs[0x9]),
		BYTE_TO_BINARY(x->modem_regs[0x1e]));

	fprintf(
		stderr, 
		"| XDAL(0x18:0-7) = "BYTE_TO_BINARY_PATTERN" | XDAM(0x19:0-7) = "BYTE_TO_BINARY_PATTERN" | TBUFFER(0x10:0-7) = "BYTE_TO_BINARY_PATTERN"\n",
		BYTE_TO_BINARY(x->modem_regs[0x18]),
		BYTE_TO_BINARY(x->modem_regs[0x19]),
		BYTE_TO_BINARY(x->modem_regs[0x10]));

	fprintf(
		stderr, 
		"| RTS(0x08:0)    = "BYTE_TO_BINARY_PATTERN" | ORG(0x09:4)    = "BYTE_TO_BINARY_PATTERN" | ATV25(0x0b:4)     = "BYTE_TO_BINARY_PATTERN"\n",
		BYTE_TO_BINARY(x->modem_regs[0x8]),
		BYTE_TO_BINARY(x->modem_regs[0x9]),
		BYTE_TO_BINARY(x->modem_regs[0xb]));

	fprintf(
		stderr, 
		"| CTS(0x0f:5)    = "BYTE_TO_BINARY_PATTERN" | RDBF(0x1e:0)   = "BYTE_TO_BINARY_PATTERN" | FE(0x0e:4)        = "BYTE_TO_BINARY_PATTERN"\n",
		BYTE_TO_BINARY(x->modem_regs[0xf]),
		BYTE_TO_BINARY(x->modem_regs[0x1e]),
		BYTE_TO_BINARY(x->modem_regs[0xe]));

	fprintf(
		stderr, 
		"| PE(0x0e:5)     = "BYTE_TO_BINARY_PATTERN" | OE(0x0e:3)     = "BYTE_TO_BINARY_PATTERN" | RLSD(0x0f:7)      = "BYTE_TO_BINARY_PATTERN"\n",
		BYTE_TO_BINARY(x->modem_regs[0xe]),
		BYTE_TO_BINARY(x->modem_regs[0xe]),
		BYTE_TO_BINARY(x->modem_regs[0xf]));

	fprintf(stderr, "| RBUFFER(0x0:0-7) = "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(x->modem_regs[0x0]));
	fprintf(stderr, "----------------------------------------------------------------------------------------\n\n");
}

void XBANDBase::xband_send_identity() {
	char hdr1[1024] = "///////EMU-";
	char *id = "Waj04qaASNfmaRNw";
	strcat(hdr1, id);
	strcat(hdr1, "\x0a");
	::write(x->conn, &hdr1, strlen(hdr1));
  fprintf(stderr, "[+][modem] Identity Sent %s\n", hdr1);
	x->net_step = 2;
}

uint8 XBANDBase::read(unsigned addr) {
	//fprintf(stderr, "[*][xband_base.cpp:mmimo_read] addr: 0x%x\n", addr);
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

  //0xFBC000 -- 0xFBFE00
  // seems if I change the read width of this beyond fbfdff, xband wont start...
  if(within<0xfb, 0xfb, 0xc000, 0xfdff>(addr)) {
	  uint8 reg = (addr-0xFBC000)/2;
	  //fprintf(stderr, "[*][xband_base.cpp:read] register: 0x%x\n", reg);
		
    if (reg == 0x7d)
			return 0x80;
		//if (reg == 0x7c) //kAddrStatus
		//	return 0;
		if (reg == 0xb4) //kLEDData
			return 0x7f;

		if (reg == 0x94) { //krxbuff
			if (x->rxbufused >= x->rxbufpos) return 0;
			uint8_t r = x->rxbuf[x->rxbufused];
			x->rxbufused++;
			if (x->rxbufused == x->rxbufpos) {
				x->rxbufused = x->rxbufpos = 0;
			}
			fprintf(stderr, "[+][modem][krxbuff] FRED FIFO Read: 0x%x\n", r);
			return r;
		}
		if (reg == 0x98) { //kreadmstatus2
			if (x->net_step) {
				ssize_t ret = ::read(x->conn, &x->rxbuf[x->rxbufpos], sizeof(x->rxbuf) - x->rxbufpos);
				if (ret != -1 && ret != 0) {
					if (x->net_step == 1) {
						xband_send_identity();
						//debug_modem_registers();
						::write(x->conn, x->txbuf, x->txbufpos);
						fprintf(stderr, "[+][modem][] Write post identity send\n");
						x->txbufpos = 0;
					}
					x->rxbufpos += ret;
				} else if (ret != 0) {
					fprintf(stderr, "[+]Read error\n");
				}
				if (x->rxbufused < x->rxbufpos) {
					return 1;
				}
			}
			return 0;
		}
		if (reg == 0xa0) {
			fprintf(stderr, "[+]Fred modem status 1 read\n");
			return 0;
		}
		if (reg >= 0xc0 && reg <= 0xff) {  //begins @ 0xFBC180 (180/2 == c0)
			uint8_t modemreg = reg - 0xc0;
			uint8_t ret = 0;

			//fprintf(stderr, "[*][xband_base.cpp:read] modem register: 0x%x\n", modemreg);
			if (modemreg == 0x00) {
				fprintf(stderr, "[+]Modem RX read\n");
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
						fprintf(stderr, "ATV25 was read\n");
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
				//fprintf(stderr, "Regsister read: 0%X\n", addr);
				return x->regs[offset];
			} else {
				fprintf(stderr, "[*][xband_base.cpp:read] UNHANDLED REGISTER: 0x%x\n", reg);
				return 0x5D;
			}
		} else {
			if (addr == 0xFBFE01) {
				fprintf(stderr, "KILL READ 3bfe01: %x\n", x->kill);
				return x->kill;
			} else if (addr == 0xFBFE03) {
				fprintf(stderr, "CONTROL READ 3bfe03: %x\n", x->control);
				return x->control;
			} else {
				fprintf(stderr, "[*][xband_base.cpp:read] UNHANDLED REGISTER: 0x%x\n", reg);
				return 0x5D;
			}
		}
	}
  return 0x00;
}

void XBANDBase::write(unsigned addr, uint8 data) {
  //fprintf(stderr, "[*][xband_base][mimo_write] addr: 0x%x | data: 0x%x\n", addr, data);
	if(within<0xe0, 0xfa, 0x0000, 0xffff>(addr)
  || within<0xfb, 0xfb, 0x0000, 0xbfff>(addr)
  || within<0xfc, 0xff, 0x0000, 0xffff>(addr)
  || within<0x60, 0x7d, 0x0000, 0xffff>(addr)) {
	  addr = (addr & 0xffff); //trying to match read
		addr = bus.mirror(addr, memory::xbandSram.size());
    return memory::xbandSram.write(addr, data);
  }

	uint32_t reg = (addr-0xFBC000)/2;  // collect the "register" by subtracting the default internal offset 
	//fprintf(stderr, "[*][xband_base][mimo_write] register: 0x%x | data: 0x%x\n", reg, data);
	if (reg == 0x90) {
		if (x->net_step == 2) {
			//debug_modem_registers();
			fprintf(stderr, "[+][modem] FRED FIFO write | net_step == 2 | data: %x\n", data);
			::write(x->conn, &data, 1);
		} else if (x->net_step == 1) {
			fprintf(stderr, "[+][modem] FRED FIFO write | net_step == 1 | data: %x\n", data);
			x->txbuf[x->txbufpos++] = data;
		}
	}

	if (reg >= 0xc0 && reg <= 0xff) { 
		//fprintf(stderr, "[*][xband_base][mimo_write] modem register: 0x%x | data: 0x%x\n", reg, data);
		uint8_t modemreg = reg - 0xc0;
		if (modemreg == 0x10) {
			fprintf(stderr, "[+]Modem TX 0x%02x\n", data);
		}
		if (modemreg == 0x08 && (data & 1) && (x->net_step < 1)) {
			fprintf(stderr, "[*]RTS was set, restarting connection | net_step == %d\n", x->net_step);
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
				fprintf(stderr, "error 3 - %d\n", err);
				return;
			}

			x->conn = socket(AF_INET, SOCK_STREAM, 0);
			if (x->conn == NULL) {
				fprintf(stderr, "error 1\n");
				return;
			}

			err = connect(x->conn, res->ai_addr, res->ai_addrlen);
			if (err != 0) {
				fprintf(stderr, "error 2 - %d\n", err);
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
					printf(stderr, "[-]Box hung up, killing connection\n");
					close(x->conn);
					x->net_step = 0;
					x->txbufpos = 0;
					x->rxbufpos = x->rxbufused = 0;
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
				fprintf(stderr, "[+] WROTE TO SMART STATUS\n");
				exit(1);
			}
			switch (offset)
			{
			  case MORE_MYSTERY:
			  case UNKNOWN_REG:
		  		fprintf(stderr, "Write to mysterious reg: %X: %X\n", addr, data);
	  			data = data & 0x7F;
  				break;
			  case UNKNOWN_REG3:
			  	fprintf(stderr, "Write to mysterious reg: %X: %X\n", addr, data);
			  	data = data & 0xFE;
		  		break;
			}
			x->regs[offset] = data;
		} else {
			fprintf(stderr, "[*][xband_base][mimo_write] UNHANDLED REGISTER reg: 0x%x | data: 0x%x\n", reg, data);
		}
	} else {
		if (addr == 0xFBFE01) {
			x->kill = data;
			fprintf(stderr, "Write to kill register %X\n", data);
		} else if (addr == 0xFBFE03) {
			x->control = data;
			fprintf(stderr, "Write to control register %X\n", data);
		} else {
			fprintf(stderr, "[*][xband_base][mimo_write] UNHANDLED REGISTER reg: 0x%x | data: 0x%x\n", reg, data);
		}
	}
	return;
}
