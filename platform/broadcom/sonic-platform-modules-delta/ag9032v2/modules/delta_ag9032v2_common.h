#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/ipmi.h>
#include <linux/ipmi_smi.h>
#include <linux/hwmon-sysfs.h>
#include <linux/hwmon.h>

#define IPMI_MAX_INTF (4)
#define DELTA_NETFN 0x38
#define BMC_BUS_5   0x04
#define CMD_SETDATA 0x03
#define CMD_GETDATA 0x02

#define CPUPLD_ADDR 0x31
#define SWPLD1_ADDR 0x6a
#define SWPLD2_ADDR 0x73
#define SWPLD3_ADDR 0x75

#define DEFAULT_NUM    1
#define BUS1_DEV_NUM  34
#define BUS1_BASE_NUM 20

#define BUS1_MUX_REG 0x1f

extern int dni_bmc_cmd(char set_cmd, char *cmd_data, int cmd_data_len);
extern unsigned char dni_log2 (unsigned char num);
extern unsigned char reverse_8bits(unsigned char c);

extern void device_release(struct device *dev);
extern void msg_handler(struct ipmi_recv_msg *recv_msg,void* handler_data);
extern void dummy_smi_free(struct ipmi_smi_msg *msg);
extern void dummy_recv_free(struct ipmi_recv_msg *msg);

static ipmi_user_t ipmi_mh_user = NULL;
static struct ipmi_user_hndl ipmi_hndlrs = { .ipmi_recv_hndl = msg_handler,};
static atomic_t dummy_count = ATOMIC_INIT(0);

static struct ipmi_smi_msg halt_smi_msg = {
        .done = dummy_smi_free
};
static struct ipmi_recv_msg halt_recv_msg = {
        .done = dummy_recv_free
};

enum{
    BUS0 = 0,
    BUS1,
    BUS2,
    BUS3,
    BUS4,
    BUS5,
    BUS6,
    BUS7,
    BUS8,
    BUS9,
    BUS10,
    BUS11,
    BUS12,
    BUS13,
    BUS14,
};

struct cpld_attribute_data {
    uint8_t bus;
    uint8_t addr;
    uint8_t reg;
    uint8_t mask;
    char note[200];
};

enum cpld_type {
    system_cpld,

};

enum swpld_type {
    swpld1,
    swpld2,
    swpld3,
};

struct cpld_platform_data {
    int reg_addr;
    struct i2c_client *client;
};

static struct cpld_platform_data ag9032v2_cpld_platform_data[] = {
    [system_cpld] = {
        .reg_addr = CPUPLD_ADDR,
    },
};

enum cpld_qsfp
{
    QSFP_PRESENT_2,
    QSFP_PRESENT_32,
    QSFP_PRESENT,
    QSFP_LPMODE,
    QSFP_RESET,
    QSFP_MODSEL,
    QSFP_INTERRUPT,
};

enum cpld_attributes {
    CPLD_REG_ADDR,
    CPLD_REG_VALUE,
    SWPLD1_REG_ADDR,
    SWPLD1_REG_VALUE,
    SWPLD2_REG_ADDR,
    SWPLD2_REG_VALUE,
    SWPLD3_REG_ADDR,
    SWPLD3_REG_VALUE,
     //CPLD   
    CPU_PLATFORM_TYPE,
    MB_RST,
    CPLD_LPC_RST,
    MB_RST_DONE,
    PCIE_RST,
    CPLD_BUS_RST,
    BMC_RST,
    BCM56870_RST,
    USB_RST,
    PSU1_PRESENT,
    PSU2_PRESENT,
    PSU1_FAN_OK,
    PSU2_FAN_OK,
    PSU1_INT,
    PSU2_INT,
    PSU1_ON,
    PSU2_ON,
    //SWPLD1
    SFP1_RXLOS,
    SFP2_RXLOS,
    SFP1_TXDIS,
    SFP2_TXDIS,
    SFP1_TXFAULT,
    SFP2_TXFAULT,
    SFP1_INT,
    SFP2_INT,
    BOARD_STAGE,
    //SWPLD2
    CPLD_PCIE_RST,
    CPLD_GPIO_RST,
    //SWPLD3
    CPLD_UPGRADE_RST,
    CPLD_GPIO_RST_EN,
};

static struct cpld_attribute_data attribute_data[] = {
    [CPLD_REG_ADDR] = {
    },
    [CPLD_REG_VALUE] = {
    },
    [SWPLD1_REG_ADDR] = {
    },
    [SWPLD1_REG_VALUE] = {
    },
    [SWPLD2_REG_ADDR] = {
    },
    [SWPLD2_REG_VALUE] = {
    },
    [SWPLD3_REG_ADDR] = {
    },
    [SWPLD3_REG_VALUE] = {
    },
//CPLD    
    [CPU_PLATFORM_TYPE] = {
        .bus  = BUS0,       .addr = CPUPLD_ADDR,
        .reg  = 0x01,       .mask = 0x0F ,
        .note = "“0000”: ET-X032BFRG (DENVERTON)\n“0001”: ET-X032BFRG (BROADWELL-DE)\n“0010~1111”: RSVP"
    },
    [MB_RST] = {
        .bus  = BUS0,      .addr = CPUPLD_ADDR,
        .reg  = 0x02,      .mask = 1 << 7,
        .note = "“1” = Normal operation\n“0” = Reset"
    },
    [CPLD_LPC_RST] = {
        .bus  = BUS0,      .addr = CPUPLD_ADDR,
        .reg  = 0x02,      .mask = 1 << 6,
        .note = "“1” = Normal operation\n“0” = Reset"
    },
    [MB_RST_DONE] = {
        .bus  = BUS0,       .addr = CPUPLD_ADDR,
        .reg  = 0x02,       .mask = 1 << 5,
        .note = "“1” = Normal operation\n“0” = Reset"
    },
    [PCIE_RST] = {
        .bus  = BUS0,       .addr = CPUPLD_ADDR,
        .reg  = 0x02,       .mask = 1 << 4,
        .note = "“1” = Normal operation\n“0” = Reset"
    },
    [CPLD_BUS_RST] = {
        .bus  = BUS0,       .addr = CPUPLD_ADDR,
        .reg  = 0x02,       .mask = 1 << 3,
        .note = "“1” = Normal operation\n“0” = Reset"
    },
    [BMC_RST] = {
        .bus  = BUS0,       .addr = CPUPLD_ADDR,
        .reg  = 0x02,       .mask = 1 << 2,
        .note = "“1” = Normal operation\n“0” = Reset"
    },
    [BCM56870_RST] = {
        .bus  = BUS0,       .addr = CPUPLD_ADDR,
        .reg  = 0x02,       .mask = 1 << 1,
        .note = "“1” = Normal operation\n“0” = Reset"
    },
    [USB_RST] = {
        .bus  = BUS0,       .addr = CPUPLD_ADDR,
        .reg  = 0x02,       .mask = 1 << 0,
        .note = "“1” = Normal operation\n“0” = Reset"
    },
    [PSU1_PRESENT] = {
        .bus  = BUS0,      .addr = CPUPLD_ADDR,
        .reg  = 0x03,      .mask = 1 << 7,
        .note = "“0” = PS1 is present\n“1” = PS1 is not present"
    },
    [PSU2_PRESENT] = {
        .bus  = BUS0,      .addr = CPUPLD_ADDR,
        .reg  = 0x03,      .mask = 1 << 3,
        .note = "“0” = PS2 is present\n“1” = PS2 is not present"
    },
    [PSU1_FAN_OK] = {
        .bus  = BUS0,      .addr = CPUPLD_ADDR,
        .reg  = 0x03,      .mask = 1 << 6,
        .note = "“0” = Power Supply is failed or not present\n“1” = Power Supply output is good"
    },
    [PSU2_FAN_OK] = {
        .bus  = BUS0,      .addr = CPUPLD_ADDR,
        .reg  = 0x03,      .mask = 1 << 2,
        .note = "“0” = Power Supply is failed or not present\n“1” = Power Supply output is good"
    },
    [PSU1_INT] = {
        .bus  = BUS0,      .addr = CPUPLD_ADDR,
        .reg  = 0x03,      .mask = 1 << 5,
        .note = "“0” = Critical events or Warning events happen.\n“1” = PSU is operating good"
    },
    [PSU2_INT] = {
        .bus  = BUS0,      .addr = CPUPLD_ADDR,
        .reg  = 0x03,      .mask = 1 << 1,
        .note = "“0” = Critical events or Warning events happen.\n“1” = PSU is operating good"
    },
    [PSU1_ON] = {
        .bus  = BUS0,      .addr = CPUPLD_ADDR,
        .reg  = 0x03,      .mask = 1 << 4,
        .note = "“0” = PSU1 turn on\n“1” = PSU1 turn off"
    },
    [PSU2_ON] = {
        .bus  = BUS0,      .addr = CPUPLD_ADDR,
        .reg  = 0x03,      .mask = 1 << 0,
        .note = "“0” = PSU2 turn on\n“1” = PSU2 turn off"
    },
//SWPLD1
    [SFP1_RXLOS] = {
        .bus  = BUS0,      .addr = SWPLD1_ADDR,
        .reg  = 0x02,      .mask = 1 << 6,
        .note = "“0” = SFP28 module receiver is NOT asserted.\n“1” = SFP28 module receiver is asserted."
    },

    [SFP2_RXLOS] = {
        .bus  = BUS0,      .addr = SWPLD1_ADDR,
        .reg  = 0x02,      .mask = 1 << 2,
        .note = "“0” = SFP28 module receiver is NOT asserted.\n“1” = SFP28 module receiver is asserted."
    },
    [SFP1_TXDIS] = {
        .bus  = BUS0,      .addr = SWPLD1_ADDR,
        .reg  = 0x02,      .mask = 1 << 5,
        .note = "“0” = SFP28 module transmitter is turn ON.\n“1” = SFP28 module transmitter is turn OFF."
    },
    [SFP2_TXDIS] = {
        .bus  = BUS0,      .addr = SWPLD1_ADDR,
        .reg  = 0x02,      .mask = 1 << 1,
        .note = "“0” = SFP28 module transmitter is turn ON.\n“1” = SFP29 module transmitter is turn OFF."
    },
    [SFP1_TXFAULT] = {
        .bus  = BUS0,      .addr = SWPLD1_ADDR,
        .reg  = 0x02,      .mask = 1 << 4,
        .note = "“0” = SFP28 module receiver is NOT asserted.\n“1” = SFP28 module receiver is asserted."
    },
    [SFP2_TXFAULT] = {
        .bus  = BUS0,      .addr = SWPLD1_ADDR,
        .reg  = 0x02,      .mask = 1 << 0,
        .note = "“0” = SFP28 module receiver is NOT asserted.\n“1” = SFP28 module receiver is asserted."
    },
    [SFP1_INT] = {
        .bus  = BUS0,      .addr = SWPLD1_ADDR,
        .reg  = 0x03,      .mask = 1 << 7,
        .note = "“1” = Interrupt function is work\n“0” = Interrupt is masked"
    },
    [SFP2_INT] = {
        .bus  = BUS0,      .addr = SWPLD1_ADDR,
        .reg  = 0x03,      .mask = 1 << 6,
        .note = "“1” = Interrupt function is work\n“0” = Interrupt is masked"
    },
    [BOARD_STAGE] = {
        .bus  = BUS0,      .addr = SWPLD1_ADDR,
        .reg  = 0x06,      .mask = 0xF0 ,
        .note = "0110 : P/R\n0001 : proto-B\n0000 : proto-A"
    },
//SWPLD2
    [CPLD_PCIE_RST] = {
        .bus  = BUS0,       .addr = SWPLD2_ADDR,
        .reg  = 0x04,       .mask = 1 << 4,
        .note = "“1” = Normal operation.\n“0” = Reset."
    },
    [CPLD_GPIO_RST] = {
        .bus  = BUS0,       .addr = SWPLD2_ADDR,
        .reg  = 0x05,       .mask = 1 << 0,
        .note = "1 = Set\n0 = Reset"
    },
//SWPLD3
    [CPLD_UPGRADE_RST] = {
        .bus  = BUS0,       .addr = SWPLD3_ADDR,
        .reg  = 0x05,       .mask = 1 << 7,
        .note = "“1” = Reset.\n“0” = Set."
    },
    [CPLD_GPIO_RST_EN] = {
        .bus  = BUS0,       .addr = SWPLD3_ADDR,
        .reg  = 0x05,       .mask = 1 << 0,
        .note = "“0” = Reset.\n“1” = Set."
    },
};

MODULE_DESCRIPTION("DNI ag9032v2 Platform Support");
MODULE_AUTHOR("Stanley Chi <stanley.chi@deltaww.com>");
MODULE_LICENSE("GPL"); 