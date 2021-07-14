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
  //memory::mmio.map(0xc180, 0xc1bf, *this);
  bus.map(Bus::MapMode::Direct, 0xe0, 0xff, 0x0000, 0xffff, *this);
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


uint8 XBANDBase::read(unsigned addr) {  
	//fprintf(stderr, "[*][xband_base.cpp:mmimo_read] addr: 0x%x\n", addr);
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
    fprintf(stderr, "[*][xband_base.cpp:mmio_read] addr: 0x%x\n", addr);
	  uint8 reg = (addr-0xFBC000)/2;
	  fprintf(stderr, "[*][xband_base.cpp:mmio_read] offset: 0x%x\n", reg);
		
    //Rockwell Modem Registers (C180h-C1BEh)
    if (reg >= 0xc0 && reg <= 0xff) {
      uint8 modemreg = reg - 0xc0;
      fprintf(stderr, "[*][xband_base.cpp:mmio_read_rockwell] offset: 0x%x\n", modemreg);

      switch(modemreg){
      	case 0x09:
      	  return 0x00;
      	case 0x0b:
      	  return 0x80;
        case 0x19:
          return 0x46;
        case 0x1e:
          return 0x08;
        default:
          return 0x00;
      }
    }

		switch(reg){
			case 0x7d:
			  return 0x80;
			case 0x7c:
			  return 0;
			case 0xb4:
			  return 0x7f;
			default:
			  return 0x00;
    }
  }
  return 0xff;
}

void XBANDBase::write(unsigned addr, uint8 data) {
  //fprintf(stderr, "[*][xband_base][mimo_write] addr: 0x%x | data: 0x%x\n", addr, data);
	if(within<0xe0, 0xfa, 0x0000, 0xffff>(addr)) {
	  addr = (addr & 0x1fffff);
		addr = bus.mirror(addr, memory::xbandSram.size());
    return memory::xbandSram.write(addr, data);
  }
}
