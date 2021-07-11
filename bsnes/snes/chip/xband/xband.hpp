class XBANDBase : public Coprocessor, public Memory  {
public:
  MappedRAM sram;

  void init();
  void load();
  void unload();
  void power();
  void reset();
  void enable();
  static void Enter();
  void enter();

  uint8 memory_access(bool write, Memory &memory, unsigned addr, uint8 data);
  uint8 memory_read(Memory &memory, unsigned addr);
  void memory_write(Memory &memory, unsigned addr, uint8 data);

  uint8 mcu_access(bool write, unsigned addr, uint8 data = 0x00);
  uint8 mcu_read(unsigned addr);
  void mcu_write(unsigned addr, uint8 data);

  uint8 mmio_read(unsigned addr);
  void mmio_write(unsigned addr, uint8 data);

  uint8 read(unsigned addr);
  void write(unsigned addr, uint8 data);

  void serialize(serializer&);

private:
  uint8 mapset;
  
  bool read_96;
};

class XBANDCart : public Memory {
  public:
    void init();
    void load();
    void unload();
    void power();
    void reset();
    void enable();

    void serialize(serializer&);

    uint8 read(unsigned addr);
    void write(unsigned addr, uint8 data);

    uint8 memory_read(Memory &memory, unsigned addr);
    void memory_write(Memory &memory, unsigned addr, uint8 data);

    XBANDCart();
    ~XBANDCart();
  
  private:
    uint8 mapset;
  
    bool read_96;
};

extern XBANDBase  xband_base;
extern XBANDCart  xband_cart;
