//Memory

unsigned Memory::size() const { return 0; }

bool Memory::debugger_access() {
#if defined(DEBUGGER)
  return debugger.bus_access;
#else
  return false;
#endif
}

//StaticRAM

uint8* StaticRAM::data() { return data_; }
unsigned StaticRAM::size() const { return size_; }

uint8 StaticRAM::read(unsigned addr) { return data_[addr]; }
void StaticRAM::write(unsigned addr, uint8 n) { data_[addr] = n; }
uint8& StaticRAM::operator[](unsigned addr) { return data_[addr]; }
const uint8& StaticRAM::operator[](unsigned addr) const { return data_[addr]; }

StaticRAM::StaticRAM(unsigned n) : size_(n) { data_ = new uint8[size_]; }
StaticRAM::~StaticRAM() { delete[] data_; }

//MappedRAM

void MappedRAM::reset() {
  if(data_) {
    delete[] data_;
    data_ = 0;
  }
  size_ = 0;
  write_protect_ = false;
}

void MappedRAM::map(uint8 *source, unsigned length) {
  reset();
  data_ = source;
  size_ = data_ && length > 0 ? length : 0;
}

void MappedRAM::copy(const uint8 *data, unsigned size) {
  if(!data_) {
    size_ = (size & ~255) + ((bool)(size & 255) << 8);
    data_ = new uint8[size_]();
  }
  memcpy(data_, data, min(size_, size));
}

void MappedRAM::write_protect(bool status) { write_protect_ = status; }
uint8* MappedRAM::data() { return data_; }
unsigned MappedRAM::size() const { return size_; }

uint8 MappedRAM::read(unsigned addr) {
  //fprintf(stderr, "[*]---------------------------------------------\n");
  //fprintf(stderr, "[*][MappedRAM::read] data size: %x\n", size_);
  //fprintf(stderr, "[*][MappedRAM::read] reading 0x%x\n", addr);
  //fprintf(stderr, "[*][MappedRAM::read] data: 0x%x\n", data_[addr]);
  //fprintf(stderr, "[*]---------------------------------------------\n");
  return data_[addr];
}
void MappedRAM::write(unsigned addr, uint8 n) { if(!write_protect_ || debugger_access()) data_[addr] = n; }
const uint8& MappedRAM::operator[](unsigned addr) const { return data_[addr]; }
MappedRAM::MappedRAM() : data_(0), size_(0), write_protect_(false) {}

//VRAM

void VRAM::reset() {
  MappedRAM::reset();
  bank(false);
}

void VRAM::map(uint8 *source, unsigned length) {
  MappedRAM::map(source, length);
  bank(false);
}

void VRAM::copy(const uint8 *data, unsigned size) {
  MappedRAM::copy(data, size);
  bank(false);
}

void VRAM::bank(bool enable, unsigned num) {
  if (PPU::SupportsVRAMExpansion && enable && (size() >= 1<<17)) {
    // Super V-Power expansion inverts CPU pin 20 -> VRAM A17
    // assume higher address lines (if any are ever used) would also invert
    num ^= 0x3f;
    
    access_ = data() + ((num << 17) & (size() - 1));
    mask_ = 0x1ffff;
  } else {
    access_ = data();
    mask_ = 0xffff;
  }
}

uint8& VRAM::operator[](unsigned addr) { 
  if (PPU::SupportsVRAMExpansion)
    return access_[addr & mask_];

  // non-accuracy PPU still uses uint16 for VRAM addresses, no casting/masking needed here
  return access_[addr];
}
VRAM::VRAM() : MappedRAM() { reset(); }

//Bus

uint8 Bus::read(uint24 addr) {
  #if defined(CHEAT_SYSTEM)
  if(cheat.active() && cheat.exists(addr)) {
    uint8 r;
    if(cheat.read(addr, r, *this)) return r;
  }
  #endif
  Page &p = page[addr >> 8];
  return p.access->read(p.offset + addr);
}

void Bus::write(uint24 addr, uint8 data) {
  Page &p = page[addr >> 8];
  p.access->write(p.offset + addr, data);
}

bool Bus::is_mirror(uint24 addr1, uint24 addr2) {
  // if the lower bytes of each address are different then they can't be mirrors
  // since pages are always aligned to 256-byte boundaries
  if((addr1 ^ addr2) & 0xff) return false;

  Page &p1 = page[addr1 >> 8];
  Page &p2 = page[addr2 >> 8];
  return (p1.access == p2.access) && (p1.offset + addr1 == p2.offset + addr2);
}
