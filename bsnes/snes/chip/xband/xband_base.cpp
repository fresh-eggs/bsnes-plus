XBANDBase xband_base;

void XBANDBase::Enter() {
	xband_base.enter();
}

void XBANDBase::enter() {
	fprintf(stderr, "[*][XBANDBase][enter_start]\n");
	while(true) {
    scheduler.synchronize();
	  step(288*2);
    synchronize_cpu();
  }
}

void XBANDBase::init() {
		fprintf(stderr, "[*][XBANDBase][init_start]\n");
}

void XBANDBase::enable() {
	fprintf(stderr, "[*][XBANDBase][enable_start]\n");
  memory::mmio.map(0xc180, 0xc1bf, *this);
  fprintf(stderr, "[*][XBANDBase][enable_end]\n");
}


void XBANDBase::power() {
	fprintf(stderr, "[*][XBANDBase][power_start]\n");
  reset();
  fprintf(stderr, "[*][XBANDBase][power_end]\n");
}

void XBANDBase::reset() {
	fprintf(stderr, "[*][XBANDBase][reset_start]\n");
	create(XBANDBase::Enter, 224*1000);
	fprintf(stderr, "[*][XBANDBase][reset_end]\n");
}

void XBANDBase::unload() {
}


uint8 XBANDBase::mmio_read(unsigned addr) {  
	fprintf(stderr, "[*][xband_cart.cpp:mmimo_read] addr: 0x%x\n", addr);
  if(within<0xe0, 0xfa, 0x0000, 0xffff>(addr)
  || within<0xfb, 0xfb, 0x0000, 0xbfff>(addr)
  || within<0xfc, 0xff, 0x0000, 0xffff>(addr)
  || within<0x60, 0x7d, 0x0000, 0xffff>(addr)) {
  	addr = (addr & 0xffff);
		addr = bus.mirror(addr, memory::xbandSram.size());
    return memory::xbandSram.read(addr);
  }

  
  if(within<0xfb, 0xfb, 0xc000, 0xc9ff>(addr)
  || within<0xfb, 0xfb, 0xfa00, 0xfbff>(addr)) {
	switch(addr & 0x1ff) {
		case 0x0f8:
			break;
		case 0x0fa:
			break;
		case 0x108:   //CARD! bit 0: XBAND Card check
			return 0x01;
		case 0x110:
			break;
		case 0x112:
			break;
		case 0x130:
			break;
		case 0x138:
			break;
		case 0x140:
			break;
		case 0x168:
			break;
		case 0x188:   //???
			return 0x00;
		case 0x192:
			return 0xff;
		case 0x196:   //d53898: DIAL-UP!
			return 0x80;
		case 0x1b0:   // ??
			return 0xff;
		case 0x1b2:   //For running XBAND
			return 0x46;
		case 0x1ba:
			break;
		case 0x1bc:
			return 0x08;
		case 0x1be:
			break;
	}
		return 0x00;
  }
  
  /*
  switch(addr) {
    case 0xfbc108:   //CARD! bit 0: XBAND Card check
		return 0x01;
	case 0xfbc188:   //???
		return 0x00;
	case 0xfbc192:
		break;
	case 0xfbc196:   //d53898: DIAL-UP!
		return 0x80;
	case 0xfbc1b0:   // ??
		break;
	case 0xfbc1b2:   //For running XBAND
		return 0x46;
	case 0xfbc1ba:
		break;
	case 0xfbc1bc:
		return 0x08;
	case 0xfbc1be:
		break;
  }
  */
  return 0xff;
}

void XBANDBase::mmio_write(unsigned addr, uint8 data) {

	if(within<0xe0, 0xfa, 0x0000, 0xffff>(addr)) {
	  addr = (addr & 0x1fffff);
		addr = bus.mirror(addr, memory::xbandSram.size());
    return memory::xbandSram.write(addr, data);
  }
}