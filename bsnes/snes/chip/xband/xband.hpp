#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define XBAND_REGS 0xE0

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

  void xband_send_identity();
  void debug_modem_registers();
  
  struct XBANDModem {
    uint8_t line_relay;
    uint8_t regs[0x20];
    uint8_t set_ATV25;
  };

  struct XBANDState {
    uint16_t cart_space[0x200000];
    uint8_t regs[XBAND_REGS];
    uint8_t kill;
    uint8_t control;
    struct sockaddr_in server;
    int conn;
    uint8_t modem_line_relay;
    uint8_t modem_regs[0x20];
    uint8_t modem_set_ATV25;
    uint8_t net_step;
    uint8_t rxbuf[16384];
    uint32_t rxbufindex;
    uint32_t rxbufconsumed;
    uint8_t txbuf[16384];
    uint32_t txbufindex;
    uint32_t txbufconsumed;
  };

private:
  uint8 mapset;
  //is this really how this works? How do struct objects work
  //in C++ as opposed typedef'd struct pointers in C ?
  //xband_state *x, obj;
  //x = &obj;
  
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
