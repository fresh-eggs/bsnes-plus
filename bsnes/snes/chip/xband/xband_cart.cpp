//#ifdef XBAND_CPP

XBANDCart xband_cart;

XBANDCart::XBANDCart(){

}
XBANDCart::~XBANDCart(){

}

void XBANDCart::init() {
	fprintf(stderr, "[*][xband_cart.cpp:init] enter \n");
	fprintf(stderr, "[*][xband_cart.cpp:init] exit \n");
}

void XBANDCart::enable() {
	fprintf(stderr, "[*][xband_cart.cpp:enable] enter \n");
	fprintf(stderr, "[*][xband_cart.cpp:enable] exit \n");
}

void XBANDCart::load() {
	fprintf(stderr, "[*][xband_cart.cpp:load] enter \n");
	fprintf(stderr, "[*][xband_cart.cpp:load] exit \n");
}

void XBANDCart::unload() {
}

void XBANDCart::power() {
	fprintf(stderr, "[*][xband_cart.cpp:power] enter \n");
  bus.map(Bus::MapMode::Direct, 0x00, 0x3f, 0x8000, 0xffff, *this);
  bus.map(Bus::MapMode::Direct, 0x40, 0x7d, 0x0000, 0xffff, *this);
  bus.map(Bus::MapMode::Direct, 0x80, 0xbf, 0x8000, 0xffff, *this);
  bus.map(Bus::MapMode::Direct, 0xc0, 0xdf, 0x0000, 0xffff, *this);

  //bus.map(Bus::MapMode::Direct, 0xd0, 0xdf, 0x0000, 0xffff, *this);
  //bus.map(Bus::MapMode::Direct, 0x50, 0x5f, 0x0000, 0xffff, *this);
  
  //Force MMIO Map
  //bus.map(Bus::MapMode::Direct, 0xe0, 0xff, 0x0000, 0xffff, memory::xbandSram);
  
  //map SRAM
  //bus.map(Bus::MapMode::Linear, 0x7d, 0xe0, 0x0000, 0x7fff, memory::xbandSram);

  //bus.map(Bus::MapMode::Linear, 0x80, 0xbf, 0x8000, 0xffff, memory::cartrom);
  fprintf(stderr, "[*][xband_cart.cpp:power] exit \n");
  reset();
}

void XBANDCart::reset() {
	fprintf(stderr, "[*][xband_cart.cpp:reset] enter \n");
  mapset = 0x00;
  read_96 = false;
  fprintf(stderr, "[*][xband_cart.cpp:reset] exit \n");
}

uint8 XBANDCart::read(unsigned addr) {
  if(within<0x00, 0x1f, 0x8000, 0xffff>(addr)) {
    addr = (addr & 0x3f0000 >> 1) | (addr & 0xffff);
    addr = bus.mirror(addr, memory::cartrom.size());
    //addr = (addr & 0x0fffff);
    //fprintf(stderr, "[*]---------------------------------------------\n");
    //fprintf(stderr, "[*][xband_cart.cpp:read][X] addr: 0x%x\n", addr);
    //fprintf(stderr, "[*]---------------------------------------------\n");
    return memory::cartrom.read(addr);
  }
  if(within<0x80, 0x9f, 0x8000, 0xffff>(addr)) {
    addr = ((addr & 0x3f0000 >> 1) | (addr & 0xffff));
    addr = bus.mirror(addr, memory::cartrom.size());
    //addr = (addr & 0x0fffff);
    //fprintf(stderr, "[*]---------------------------------------------\n");
    //fprintf(stderr, "[*][xband_cart.cpp:read][X] addr: 0x%x\n", addr);
    //fprintf(stderr, "[*]---------------------------------------------\n");
    return memory::cartrom.read(addr);
  }
  
  if(within<0x40, 0x7d, 0x0000, 0xffff>(addr)) {
    addr = ((addr & 0x3f0000 >> 1) | (addr & 0xffff));
    addr = bus.mirror(addr, memory::cartrom.size());
    //addr = (addr & 0x0fffff);
    //fprintf(stderr, "[*]---------------------------------------------\n");
    //fprintf(stderr, "[*][xband_cart.cpp:read][X] addr: 0x%x\n", addr);
    //fprintf(stderr, "[*]---------------------------------------------\n");
    return memory::cartrom.read(addr);
  }
  if(within<0xc0, 0xdf, 0x0000, 0xffff>(addr)) {
    //fprintf(stderr, "[*]---------------------------------------------\n");
    //fprintf(stderr, "[*][xband_cart.cpp:read][!] addr: 0x%x\n", addr);
    //addr = (addr & 0x4FFFFF); //make offset into xband rom at 0 for access into data_
    addr = ((addr & 0x3f0000 >> 1) | (addr & 0xffff));
    addr = bus.mirror(addr, memory::cartrom.size());
    //addr = (addr & 0x0fffff);
    //fprintf(stderr, "[*][xband_cart.cpp:read][!] addr_translated: 0x%x\n", addr);
    //fprintf(stderr, "[*]---------------------------------------------\n");
    return memory::cartrom.read(addr);
  }

  ////fprintf(stderr, "[*]---------------------------------------------\n");
  ////fprintf(stderr, "[*][xband_cart.cpp:read][!] addr: 0x%x\n", addr);
  
  ////fprintf(stderr, "[*][xband_cart.cpp:read][!] addr_translated: 0x%x\n", addr);
  ////fprintf(stderr, "[*]---------------------------------------------\n"); 
  //fprintf(stderr, "[*]---------------------------------------------\n");
  //fprintf(stderr, "[*][xband_cart.cpp:read][regular read] addr: 0x%x\n", addr);
  //fprintf(stderr, "[*]---------------------------------------------\n");
  
  return -1;
}

void XBANDCart::write(unsigned addr, uint8 data) {
  addr = ((addr & 0x3f0000 >> 1) | (addr & 0xffff));
  addr = bus.mirror(addr, memory::cartrom.size());
  //addr = (addr & 0x0fffff); //make offset into xband rom at 0 for access into data_
  //fprintf(stderr, "[*][xband_cart.cpp:write] addr: 0x%x, data: %d\n", addr, data);
  memory::cartrom.write(addr, data);
}



uint8 XBANDCart::memory_read(Memory &memory, unsigned addr) {
  addr = bus.mirror(addr, memory.size());
  return memory.read(addr);
}

void XBANDCart::memory_write(Memory &memory, unsigned addr, uint8 data) {
  addr = bus.mirror(addr, memory.size());
  return memory.write(addr, data);
}

// uint8 XBANDCart::memory_access(bool write, Memory &memory, unsigned addr, uint8 data) {
//   if(write == 0) return memory_read(memory, addr);
//   memory_write(memory, addr, data);
// }

// uint8 XBANDCart::memory_read(Memory &memory, unsigned addr) {
//   addr = bus.mirror(addr, memory.size());
//   return memory.read(addr);
// }

// void XBANDCart::memory_write(Memory &memory, unsigned addr, uint8 data) {
//   addr = bus.mirror(addr, memory.size());
//   return memory.write(addr, data);
// }

// //mcu_access() allows mcu_read() and mcu_write() to share decoding logic
// uint8 XBANDCart::mcu_access(bool write, unsigned addr, uint8 data) {
//  if(mapset == 0x00) {
//   if(within<0x00, 0x1f, 0x8000, 0xffff>(addr)) {
//     addr = (addr & 0x3f0000 >> 1) | (addr & 0xffff);
//     return memory_access(write, cartridge.rom, addr, data);
//   }
//   if(within<0x80, 0x9f, 0x8000, 0xffff>(addr)) {
//     addr = ((addr & 0x3f0000 >> 1) | (addr & 0xffff));
//     return memory_access(write, cartridge.rom, addr, data);
//   }
  
//   if(within<0x40, 0x7d, 0x0000, 0xffff>(addr)) {
// 	addr = ((addr & 0x3f0000 >> 1) | (addr & 0xffff));
// 	return memory_access(write, cartridge.rom, addr, data);
//   }
//   if(within<0xc0, 0xdf, 0x0000, 0xffff>(addr)) {
// 	addr = ((addr & 0x3f0000 >> 1) | (addr & 0xffff));
// 	return memory_access(write, cartridge.rom, addr, data);
//   }
  
//   /*
//   if(within<0xe0, 0xfa, 0x0000, 0xffff>(addr)
//   || within<0xfb, 0xfb, 0x0000, 0xbfff>(addr)
//   || within<0xfc, 0xff, 0x0000, 0xffff>(addr)
//   || within<0x60, 0x7d, 0x0000, 0xffff>(addr)) {
//     return memory_access(write, sram, (addr & 0xffff), data);
//   }
  
//   if(within<0xfb, 0xfb, 0xc000, 0xc9ff>(addr)
//   || within<0xfb, 0xfb, 0xfa00, 0xfbff>(addr)) {
// 	switch(addr & 0x1ff) {
// 		case 0x0f8:
// 			break;
// 		case 0x0fa:
// 			break;
// 		case 0x108:   //CARD! bit 0: XBAND Card check
// 			return 0x01;
// 		case 0x110:
// 			break;
// 		case 0x112:
// 			break;
// 		case 0x130:
// 			break;
// 		case 0x138:
// 			break;
// 		case 0x140:
// 			break;
// 		case 0x168:
// 			break;
// 		case 0x188:   //???
// 			return 0x00;
// 		case 0x192:
// 			return 0xff;
// 		case 0x196:   //d53898: DIAL-UP!
// 			return 0x80;
// 		case 0x1b0:   // ??
// 			return 0xff;
// 		case 0x1b2:   //For running XBAND
// 			return 0x46;
// 		case 0x1ba:
// 			break;
// 		case 0x1bc:
// 			return 0x08;
// 		case 0x1be:
// 			break;
// 	}
// 		return 0xff;
//   }
  
//   if(within<0xfb, 0xfb, 0xfc00, 0xffff>(addr)) {
// 	return 0xff;
//   }
//   */
//   return cpu.regs.mdr;
//  }
// }

// uint8 XBANDCart::mcu_read(unsigned addr) {
//   return mcu_access(0, addr);
// }

// void XBANDCart::mcu_write(unsigned addr, uint8 data) {
//   mcu_access(1, addr, data);
// }



//#endif
