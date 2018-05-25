#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/dmi.h>
#include <linux/version.h>
#include <linux/ctype.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c/pca954x.h>
#include <linux/i2c-mux.h>
#include <linux/i2c-mux-gpio.h>
#include <linux/i2c/sff-8436.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>

#include "delta_ag9032v2_common.h"

#define QSFP_PRESENCE_1    0x12
#define QSFP_PRESENCE_2    0x13
#define QSFP_PRESENCE_3    0x14
#define QSFP_PRESENCE_4    0x15
#define SFP_PRESENCE_5     0x02 //swpld2, bit 7, bit 3

#define QSFP_LPMODE_1      0x0e
#define QSFP_LPMODE_2      0x0f 
#define QSFP_LPMODE_3      0x10
#define QSFP_LPMODE_4      0x11

#define QSFP_RESET_1       0x16
#define QSFP_RESET_2       0x17
#define QSFP_RESET_3       0x18
#define QSFP_RESET_4       0x19

#define QSFP_INTERRUPT_1   0x1a
#define QSFP_INTERRUPT_2   0x1b
#define QSFP_INTERRUPT_3   0x1c
#define QSFP_INTERRUPT_4   0x1d 

#define QSFP_MODSEL_1   0x0a
#define QSFP_MODSEL_2   0x0b
#define QSFP_MODSEL_3   0x0c
#define QSFP_MODSEL_4   0x0d

#define SFF8436_INFO(data) \
    .type = "sff8436", .addr = 0x50, .platform_data = (data)

#define SFF_8436_PORT(eedata) \
    .byte_len = 256, .page_size = 1, .flags = SFF_8436_FLAG_READONLY
    
#define ag9032v2_i2c_device_num(NUM){                                         \
        .name                   = "delta-ag9032v2-i2c-device",                \
        .id                     = NUM,                                        \
        .dev                    = {                                           \
                    .platform_data = &ag9032v2_i2c_device_platform_data[NUM], \
                    .release       = device_release,                          \
        },                                                                    \
}

#define ag9032v2_swpld1_device_num(NUM){                                      \
        .name                   = "delta-ag9032v2-swpld1",                    \
        .id                     = NUM,                                        \
        .dev                    = {                                           \
                    .platform_data = &ag9032v2_swpld_platform_data[NUM],      \
                    .release       = device_release,                          \
        },                                                                    \
}

#define ag9032v2_swpld2_device_num(NUM){                                      \
        .name                   = "delta-ag9032v2-swpld2",                    \
        .id                     = NUM,                                        \
        .dev                    = {                                           \
                    .platform_data = &ag9032v2_swpld_platform_data[NUM],      \
                    .release       = device_release,                          \
        },                                                                    \
}

#define ag9032v2_swpld3_device_num(NUM){                                      \
        .name                   = "delta-ag9032v2-swpld3",                    \
        .id                     = NUM,                                        \
        .dev                    = {                                           \
                    .platform_data = &ag9032v2_swpld_platform_data[NUM],      \
                    .release       = device_release,                          \
        },                                                                    \
}


void device_release(struct device *dev)
{
    return;
}
EXPORT_SYMBOL(device_release);

unsigned char dni_log2 (unsigned char num){
    unsigned char num_log2 = 0;
    while(num > 0){
        num = num >> 1;
        num_log2 += 1;
    }
    return num_log2 -1;
}
EXPORT_SYMBOL(dni_log2);

/*Define struct to get client of i2c_new_deivce */
struct i2c_client * i2c_client_9548;

unsigned char reverse_8bits(unsigned char c)
{
        unsigned char s = 0;
        int i;
        for (i = 0; i < 8; ++i) {
                s <<= 1;
                s |= c & 1;
                c >>= 1;
        }
        return s;
}
EXPORT_SYMBOL(reverse_8bits);

/*----------------   I2C device   - start   ------------- */
struct i2c_device_platform_data {
    int parent;
    struct i2c_board_info           info;
    struct i2c_client              *client;
};

/* pca9548 - add 8 bus */
static struct pca954x_platform_mode pca954x_mode[] = {
  { .adap_id = 2,
    .deselect_on_exit = 1,
  },
  { .adap_id = 3,
    .deselect_on_exit = 1,
  },
  { .adap_id = 4,
    .deselect_on_exit = 1,
  },
  { .adap_id = 5,
    .deselect_on_exit = 1,
  },
  { .adap_id = 6,
    .deselect_on_exit = 1,
  },
  { .adap_id = 7,
    .deselect_on_exit = 1,
  },
  { .adap_id = 8,
    .deselect_on_exit = 1,
  },
  { .adap_id = 9,
    .deselect_on_exit = 1,
  },
};

static struct pca954x_platform_data pca954x_data = {
  .modes = pca954x_mode,
  .num_modes = ARRAY_SIZE(pca954x_mode),
};

static struct i2c_board_info __initdata i2c_info_pca9548[] =
{
        {
            I2C_BOARD_INFO("pca9548", 0x70),
            .platform_data = &pca954x_data, 
        },
};

static struct sff_8436_platform_data sff_8436_port[] = {
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
    { SFF_8436_PORT() },
};

static struct i2c_device_platform_data ag9032v2_i2c_device_platform_data[] = {
    {
        /* id eeprom (0x53) */
        .parent = 1,
        .info = { I2C_BOARD_INFO("24c02", 0x53) },
        .client = NULL,
    },
    {
        /* qsfp 1 (0x50) */
        .parent = 20,
        .info = { SFF8436_INFO(&sff_8436_port[0]) },
        .client = NULL,
    },
    {
        /* qsfp 2 (0x50) */
        .parent = 21,
        .info = { SFF8436_INFO(&sff_8436_port[1]) },
        .client = NULL,
    },
    {
        /* qsfp 3 (0x50) */
        .parent = 22,
        .info = { SFF8436_INFO(&sff_8436_port[2]) },
        .client = NULL,
    },
    {
        /* qsfp 4 (0x50) */
        .parent = 23,
        .info = { SFF8436_INFO(&sff_8436_port[3]) },
        .client = NULL,
    },
    {
        /* qsfp 5 (0x50) */
        .parent = 24,
        .info = { SFF8436_INFO(&sff_8436_port[4]) },
        .client = NULL,
    },
    {
        /* qsfp 6 (0x50) */
        .parent = 25,
        .info = { SFF8436_INFO(&sff_8436_port[5]) },
        .client = NULL,
    },
    {
        /* qsfp 7 (0x50) */
        .parent = 26,
        .info = { SFF8436_INFO(&sff_8436_port[6]) },
        .client = NULL,
    },
    {
        /* qsfp 8 (0x50) */
        .parent = 27,
        .info = { SFF8436_INFO(&sff_8436_port[7]) },
        .client = NULL,
    },
    {
        /* qsfp 9 (0x50) */
        .parent = 28,
        .info = { SFF8436_INFO(&sff_8436_port[8]) },
        .client = NULL,
    },
    {
        /* qsfp 10 (0x50) */
        .parent = 29,
        .info = { SFF8436_INFO(&sff_8436_port[9]) },
        .client = NULL,
    },
    {
        /* qsfp 11 (0x50) */
        .parent = 30,
        .info = { SFF8436_INFO(&sff_8436_port[10]) },
        .client = NULL,
    },
    {
        /* qsfp 12 (0x50) */
        .parent = 31,
        .info = { SFF8436_INFO(&sff_8436_port[11]) },
        .client = NULL,
    },
    {
        /* qsfp 13 (0x50) */
        .parent = 32,
        .info = { SFF8436_INFO(&sff_8436_port[12]) },
        .client = NULL,
    },
    {
        /* qsfp 14 (0x50) */
        .parent = 33,
        .info = { SFF8436_INFO(&sff_8436_port[13]) },
        .client = NULL,
    },
    {
        /* qsfp 15 (0x50) */
        .parent = 34,
        .info = { SFF8436_INFO(&sff_8436_port[14]) },
        .client = NULL,
    },
    {
        /* qsfp 16 (0x50) */
        .parent = 35,
        .info = { SFF8436_INFO(&sff_8436_port[15]) },
        .client = NULL,
    },
    {
        /* qsfp 17 (0x50) */
        .parent = 36,
        .info = { SFF8436_INFO(&sff_8436_port[16]) },
        .client = NULL,
    },
    {
        /* qsfp 18 (0x50) */
        .parent = 37,
        .info = { SFF8436_INFO(&sff_8436_port[17]) },
        .client = NULL,
    },
    {
        /* qsfp 19 (0x50) */
        .parent = 38,
        .info = { SFF8436_INFO(&sff_8436_port[18]) },
        .client = NULL,
    },
    {
        /* qsfp 20 (0x50) */
        .parent = 39,
        .info = { SFF8436_INFO(&sff_8436_port[19]) },
        .client = NULL,
    },
    {
        /* qsfp 21 (0x50) */
        .parent = 40,
        .info = { SFF8436_INFO(&sff_8436_port[20]) },
        .client = NULL,
    },
    {
        /* qsfp 22 (0x50) */
        .parent = 41,
        .info = { SFF8436_INFO(&sff_8436_port[21]) },
        .client = NULL,
    },
    {
        /* qsfp 23 (0x50) */
        .parent = 42,
        .info = { SFF8436_INFO(&sff_8436_port[22]) },
        .client = NULL,
    },
    {
        /* qsfp 24 (0x50) */
        .parent = 43,
        .info = { SFF8436_INFO(&sff_8436_port[23]) },
        .client = NULL,
    },
    {
        /* qsfp 25 (0x50) */
        .parent = 44,
        .info = { SFF8436_INFO(&sff_8436_port[24]) },
        .client = NULL,
    },
    {
        /* qsfp 26 (0x50) */
        .parent = 45,
        .info = { SFF8436_INFO(&sff_8436_port[25]) },
        .client = NULL,
    },
    {
        /* qsfp 27 (0x50) */
        .parent = 46,
        .info = { SFF8436_INFO(&sff_8436_port[26]) },
        .client = NULL,
    },
    {
        /* qsfp 28 (0x50) */
        .parent = 47,
        .info = { SFF8436_INFO(&sff_8436_port[27]) },
        .client = NULL,
    },
    {
        /* qsfp 29 (0x50) */
        .parent = 48,
        .info = { SFF8436_INFO(&sff_8436_port[28]) },
        .client = NULL,
    },
    {
        /* qsfp 30 (0x50) */
        .parent = 49,
        .info = { SFF8436_INFO(&sff_8436_port[29]) },
        .client = NULL,
    },
    {
        /* qsfp 31 (0x50) */
        .parent = 50,
        .info = { SFF8436_INFO(&sff_8436_port[30]) },
        .client = NULL,
    },
    {
        /* qsfp 32 (0x50) */
        .parent = 51,
        .info = { SFF8436_INFO(&sff_8436_port[31]) },
        .client = NULL,
    },
    {
        /* sfp 1 (0x50) */
        .parent = 52,
        .info = { SFF8436_INFO(&sff_8436_port[32]) },
        .client = NULL,
    },
    {
        /* sfp 2 (0x50) */
        .parent = 53,
        .info = { SFF8436_INFO(&sff_8436_port[33]) },
        .client = NULL,
    },
};

static struct platform_device ag9032v2_i2c_device[] = {
    ag9032v2_i2c_device_num(0),
    ag9032v2_i2c_device_num(1),
    ag9032v2_i2c_device_num(2),
    ag9032v2_i2c_device_num(3),
    ag9032v2_i2c_device_num(4),
    ag9032v2_i2c_device_num(5),
    ag9032v2_i2c_device_num(6),
    ag9032v2_i2c_device_num(7),
    ag9032v2_i2c_device_num(8),
    ag9032v2_i2c_device_num(9),
    ag9032v2_i2c_device_num(10),
    ag9032v2_i2c_device_num(11),
    ag9032v2_i2c_device_num(12),
    ag9032v2_i2c_device_num(13),
    ag9032v2_i2c_device_num(14),
    ag9032v2_i2c_device_num(15),
    ag9032v2_i2c_device_num(16),
    ag9032v2_i2c_device_num(17),
    ag9032v2_i2c_device_num(18),
    ag9032v2_i2c_device_num(19),
    ag9032v2_i2c_device_num(20),
    ag9032v2_i2c_device_num(21),
    ag9032v2_i2c_device_num(22),
    ag9032v2_i2c_device_num(23),
    ag9032v2_i2c_device_num(24),
    ag9032v2_i2c_device_num(25),
    ag9032v2_i2c_device_num(26),
    ag9032v2_i2c_device_num(27),
    ag9032v2_i2c_device_num(28),
    ag9032v2_i2c_device_num(29),
    ag9032v2_i2c_device_num(30),
    ag9032v2_i2c_device_num(31),
    ag9032v2_i2c_device_num(32),
    ag9032v2_i2c_device_num(33),
    ag9032v2_i2c_device_num(34),
};

/*----------------   I2C device   - end   ------------- */

/*----------------   I2C driver   - start   ------------- */
static int __init i2c_device_probe(struct platform_device *pdev)
{
    struct i2c_device_platform_data *pdata;
    struct i2c_adapter *parent;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
        return -ENODEV;
    }

    parent = i2c_get_adapter(pdata->parent);
    if (!parent) {
        dev_err(&pdev->dev, "Parent adapter (%d) not found\n",
            pdata->parent);
        return -ENODEV;
    }

    pdata->client = i2c_new_device(parent, &pdata->info);
    if (!pdata->client) {
        dev_err(&pdev->dev, "Failed to create i2c client %s at %d\n",
            pdata->info.type, pdata->parent);
        return -ENODEV;
    }

    return 0;
}

static int __exit i2c_deivce_remove(struct platform_device *pdev)
{
    struct i2c_adapter *parent;
    struct i2c_device_platform_data *pdata;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
        return -ENODEV;
    }

    if (pdata->client) {
        parent = i2c_get_adapter(pdata->parent);
        i2c_unregister_device(pdata->client);
        i2c_put_adapter(parent);
    }

    return 0;
}
static struct platform_driver i2c_device_driver = {
    .probe = i2c_device_probe,
    .remove = __exit_p(i2c_deivce_remove),
    .driver = {
        .owner = THIS_MODULE,
        .name = "delta-ag9032v2-i2c-device",
    },
};

/*----------------   I2C driver   - end   ------------- */

/*----------------    CPLD - start   ------------- */

/*    CPLD  -- device   */

unsigned char swpld1_reg_addr;
unsigned char swpld2_reg_addr;
unsigned char swpld3_reg_addr;

static struct cpld_platform_data ag9032v2_swpld_platform_data[] = {
    [swpld1] = {
        .reg_addr = SWPLD1_ADDR,
    },
    [swpld2] = {
        .reg_addr = SWPLD2_ADDR,
    },
    [swpld3] = {
        .reg_addr = SWPLD3_ADDR,
    },
};

/*    SWPLD1  -- device   */
static struct platform_device swpld_device[] = {
    ag9032v2_swpld1_device_num(0),
    ag9032v2_swpld2_device_num(1),
    ag9032v2_swpld3_device_num(2),
};


static ssize_t get_swpld_reg(struct device *dev, struct device_attribute *dev_attr, char *buf) 
{
    int ret;
    int mask;
    int value;
    char note[200];
    unsigned char reg;
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;
    switch (attr->index) {
        case SWPLD1_REG_ADDR:
            return sprintf(buf, "0x%02x\n", swpld1_reg_addr);
        case SWPLD2_REG_ADDR:
            return sprintf(buf, "0x%02x\n", swpld2_reg_addr);
        case SWPLD3_REG_ADDR:
            return sprintf(buf, "0x%02x\n", swpld3_reg_addr);
        case SWPLD1_REG_VALUE:
            ret = i2c_smbus_read_byte_data(pdata->client, swpld1_reg_addr);
            return sprintf(buf, "0x%02x\n", ret);
        case SWPLD2_REG_VALUE:
            ret = i2c_smbus_read_byte_data(pdata->client, swpld2_reg_addr);
            return sprintf(buf, "0x%02x\n", ret);            
        case SWPLD3_REG_VALUE:
            ret = i2c_smbus_read_byte_data(pdata->client, swpld3_reg_addr);
            return sprintf(buf, "0x%02x\n", ret);
        case SFP1_RXLOS ... BOARD_STAGE://swpld1
            reg   = attribute_data[attr->index].reg;
            mask  = attribute_data[attr->index].mask;
            value = i2c_smbus_read_byte_data(pdata->client, reg);
            sprintf(note, "\n%s\n",attribute_data[attr->index].note);
            value = (value & mask);
            break;
        case CPLD_PCIE_RST ... CPLD_GPIO_RST://swpld2
            reg   = attribute_data[attr->index].reg;
            mask  = attribute_data[attr->index].mask;
            value = i2c_smbus_read_byte_data(pdata->client, reg);
            sprintf(note, "\n%s\n",attribute_data[attr->index].note);
            value = (value & mask);
            break;
        case CPLD_UPGRADE_RST ... CPLD_GPIO_RST_EN://swpld3
            reg   = attribute_data[attr->index].reg;
            mask  = attribute_data[attr->index].mask;
            value = i2c_smbus_read_byte_data(pdata->client, reg);
            sprintf(note, "\n%s\n",attribute_data[attr->index].note);
            value = (value & mask);
            break;
        default:
            return sprintf(buf, "%d not found", attr->index);
    } 

    switch (mask) {
        case 0xFF:
            return sprintf(buf, "0x%02x%s", value, note);
        case 0x0F:
            return sprintf(buf, "0x%01x%s", value, note);
        case 0xF0:
            value = value >> 4;
            return sprintf(buf, "0x%01x%s", value, note);
        default :
            value = value >> dni_log2(mask);
            return sprintf(buf, "%d%s", value, note);
    }
}

static ssize_t set_swpld_reg(struct device *dev, struct device_attribute *dev_attr,
             const char *buf, size_t count)
{
    int err;
    int value;
    int set_data;
    unsigned long set_data_ul;
    unsigned char reg;
    unsigned char mask;  
    unsigned char mask_out;      

    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;

    err = kstrtoul(buf, 0, &set_data_ul);
    if (err){
        return err;
    }

    set_data = (int)set_data_ul;
    if (set_data > 0xff){
        printk(KERN_ALERT "address out of range (0x00-0xFF)\n");
        return count;
    }

    switch (attr->index) {
        case SWPLD1_REG_ADDR:
            swpld1_reg_addr = set_data;
            return count;
        case SWPLD2_REG_ADDR:
            swpld2_reg_addr = set_data;
            return count;
        case SWPLD3_REG_ADDR:
            swpld3_reg_addr = set_data;
            return count;
        case SWPLD1_REG_VALUE:
            i2c_smbus_write_byte_data(pdata->client, swpld1_reg_addr, set_data);
            return count;
        case SWPLD2_REG_VALUE:
            i2c_smbus_write_byte_data(pdata->client, swpld2_reg_addr, set_data);
            return count;
        case SWPLD3_REG_VALUE:
            i2c_smbus_write_byte_data(pdata->client, swpld3_reg_addr, set_data);
            return count;    
        case SFP1_RXLOS ... BOARD_STAGE:    //swpld1
            reg   = attribute_data[attr->index].reg;
            mask  = attribute_data[attr->index].mask;
            value = i2c_smbus_read_byte_data(pdata->client, reg);
            mask_out = value & ~(mask);
            break;
        case CPLD_PCIE_RST ... CPLD_GPIO_RST:   //swpld2
            reg   = attribute_data[attr->index].reg;
            mask  = attribute_data[attr->index].mask;
            value = i2c_smbus_read_byte_data(pdata->client, reg);
            mask_out = value & ~(mask);
            break;
        case CPLD_UPGRADE_RST ... CPLD_GPIO_RST_EN: //swpld3
            reg   = attribute_data[attr->index].reg;
            mask  = attribute_data[attr->index].mask;
            value = i2c_smbus_read_byte_data(pdata->client, reg);
            mask_out = value & ~(mask);
            break;
        default:
            return sprintf(buf, "%d not found", attr->index); 
    }

    switch (mask) {
        case 0xFF:
            set_data = mask_out | (set_data & mask);
            break;
        case 0x0F:
            set_data = mask_out | (set_data & mask);
            break;
        case 0xF0:
            set_data = set_data << 4;
            set_data = mask_out | (set_data & mask);
            break; 
        default :
            set_data = mask_out | (set_data << dni_log2(mask) );        
    } 

    switch (attr->index) {
        case SFP1_RXLOS ... BOARD_STAGE:    //swpld1
            i2c_smbus_write_byte_data(pdata->client, reg, set_data);
            break;
        case CPLD_PCIE_RST ... CPLD_GPIO_RST:   //swpld2
            i2c_smbus_write_byte_data(pdata->client, reg, set_data);
            break;
        case CPLD_UPGRADE_RST ... CPLD_GPIO_RST_EN: //swpld3
            i2c_smbus_write_byte_data(pdata->client, reg, set_data);
            break;
        default:
            return sprintf(buf, "%d not found", attr->index); 
    }
    return count;
}

static ssize_t get_qsfp(struct device *dev, struct device_attribute *dev_attr,
             const char *buf, size_t count)
{
    int ret;
    int sfp1;
    int sfp2;
    int ret_sfp;
    u64 data = 0;
    char note[200];
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;

    switch (attr->index) {
        case QSFP_PRESENT_2:
            /*SFP1 and SFP2*/
            ret = i2c_smbus_read_byte_data(pdata->client, SFP_PRESENCE_5);
            sfp1 = (ret & (1 << 7)) >> 7;
            sfp2 = (ret & (1 << 3)) >> 2;
            ret_sfp = sfp1 | sfp2;

            return sprintf(buf, "0x%x%08x\n", ret_sfp, 0);

        case QSFP_PRESENT_32:
            /*QSFP1~8*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_PRESENCE_1);
            data = (u64)(reverse_8bits((unsigned char)ret) & 0xff);
            /*QSFP9~16*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_PRESENCE_2);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 8;
            /*QSFP17~24*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_PRESENCE_3);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 16;
            /*QSFP25~32*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_PRESENCE_4);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 24;

            return sprintf(buf, "0x%09llx\n", data);
    
        case QSFP_LPMODE:
            /*QSFP1~8*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_LPMODE_1);
            data = (u64)(reverse_8bits((unsigned char)ret) & 0xff);
            /*QSFP9~16*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_LPMODE_2);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 8;
            /*QSFP17~24*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_LPMODE_3);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 16;
            /*QSFP25~32*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_LPMODE_4);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 24;
            return sprintf(buf, "0x%09llx\n", data);

        case QSFP_RESET:
            /*QSFP1~8*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESET_1);
            data = (u64)(reverse_8bits((unsigned char)ret) & 0xff);
            /*QSFP9~16*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESET_2);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 8;
            /*QSFP17~24*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESET_3);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 16;
            /*QSFP25~32*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESET_4);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 24;
            return sprintf(buf, "0x%09llx\n", data);

        case QSFP_INTERRUPT:
            /*QSFP1~8*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_INTERRUPT_1);
            data = (u64)(reverse_8bits((unsigned char)ret) & 0xff);
            /*QSFP9~16*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_INTERRUPT_2);
            data |=(u64)(reverse_8bits((unsigned char)ret) & 0xff) << 8;
            /*QSFP17~24*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_INTERRUPT_3);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 16;
            /*QSFP25~32*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_INTERRUPT_4);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 24;
            return sprintf(buf, "0x%09llx\n", data);

        case QSFP_MODSEL:
            /*QSFP1~8*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_MODSEL_1);
            data = (u64)(reverse_8bits((unsigned char)ret) & 0xff);
            /*QSFP9~16*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_MODSEL_2);
            data |=(u64)(reverse_8bits((unsigned char)ret) & 0xff) << 8;
            /*QSFP17~24*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_MODSEL_3);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 16;
            /*QSFP25~32*/
            ret = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_MODSEL_4);
            data |= (u64)(reverse_8bits((unsigned char)ret) & 0xff) << 24;
            return sprintf(buf, "0x%09llx\n", data);
        default:
            return sprintf(buf, "%d not found", attr->index); 
    }
}

static ssize_t set_qsfp(struct device *dev, struct device_attribute *dev_attr,
             const char *buf, size_t count)
{
    int err;
    int ret;
    int ret_sfp;
    u64 data = 0;
    unsigned char set_bytes;
    unsigned long long set_data;
    char note[200];
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;
    
    err = kstrtoull(buf, 16, &set_data);
    if (err){
        return err;
    } 

    switch (attr->index) {
        case QSFP_LPMODE:
            /*QSFP1~8*/
            set_bytes = reverse_8bits(set_data & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_LPMODE_1, set_bytes);

            /*QSFP9~16*/
            set_bytes = reverse_8bits((set_data >> 8 ) & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_LPMODE_2, set_bytes);

            /*QSFP17~24*/
            set_bytes = reverse_8bits((set_data >> 16 ) & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_LPMODE_3, set_bytes);

            /*QSFP25~32*/
            set_bytes = reverse_8bits((set_data >> 24 ) & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_LPMODE_4, set_bytes);

            return sprintf(buf, "0x0%09llx\n", data);

        case QSFP_RESET:
            /*QSFP1~8*/
            set_bytes = reverse_8bits(set_data & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESET_1, set_bytes);

            /*QSFP9~16*/
            set_bytes = reverse_8bits((set_data >> 8 ) & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESET_2, set_bytes);

            /*QSFP17~24*/
            set_bytes = reverse_8bits((set_data >> 16 ) & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESET_3, set_bytes);

            /*QSFP25~32*/
            set_bytes = reverse_8bits((set_data >> 24 ) & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESET_4, set_bytes);

            return sprintf(buf, "0x0%09llx\n", data);

        case QSFP_MODSEL:
            /*QSFP1~8*/
            set_bytes = reverse_8bits(set_data & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_MODSEL_1, set_bytes);

            /*QSFP9~16*/
            set_bytes = reverse_8bits((set_data >> 8 ) & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_MODSEL_2, set_bytes);

            /*QSFP17~24*/
            set_bytes = reverse_8bits((set_data >> 16 ) & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_MODSEL_3, set_bytes);

            /*QSFP25~32*/
            set_bytes = reverse_8bits((set_data >> 24 ) & 0xff);
            i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_MODSEL_4, set_bytes);

            return sprintf(buf, "0x%09llx\n", data);

        default:
            return sprintf(buf, "%d not found", attr->index); 
    }
}

//QSFP
static SENSOR_DEVICE_ATTR(sfp_present_2,  S_IRUGO,           get_qsfp, NULL,     QSFP_PRESENT_2);
static SENSOR_DEVICE_ATTR(sfp_present_32, S_IRUGO,           get_qsfp, NULL,     QSFP_PRESENT_32);
static SENSOR_DEVICE_ATTR(sfp_lpmode,     S_IWUSR | S_IRUGO, get_qsfp, set_qsfp, QSFP_LPMODE);
static SENSOR_DEVICE_ATTR(sfp_reset,      S_IWUSR | S_IRUGO, get_qsfp, set_qsfp, QSFP_RESET);
static SENSOR_DEVICE_ATTR(sfp_modsel,     S_IWUSR | S_IRUGO, get_qsfp, set_qsfp, QSFP_MODSEL);
static SENSOR_DEVICE_ATTR(sfp_interrupt,  S_IRUGO,           get_qsfp, NULL,     QSFP_INTERRUPT);

//SWPLD
static SENSOR_DEVICE_ATTR(swpld1_reg_addr,   S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD1_REG_ADDR);
static SENSOR_DEVICE_ATTR(swpld1_reg_value,  S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD1_REG_VALUE);
static SENSOR_DEVICE_ATTR(swpld2_reg_addr,   S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD2_REG_ADDR);
static SENSOR_DEVICE_ATTR(swpld2_reg_value,  S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD2_REG_VALUE);
static SENSOR_DEVICE_ATTR(swpld3_reg_addr,   S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD3_REG_ADDR);
static SENSOR_DEVICE_ATTR(swpld3_reg_value,  S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD3_REG_VALUE);

//SWPLD1
static SENSOR_DEVICE_ATTR(sfp1_rxlos,        S_IRUGO,           get_swpld_reg, NULL,          SFP1_RXLOS);
static SENSOR_DEVICE_ATTR(sfp2_rxlos,        S_IRUGO,           get_swpld_reg, NULL,          SFP2_RXLOS);
static SENSOR_DEVICE_ATTR(sfp1_txdis,        S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SFP1_TXDIS);
static SENSOR_DEVICE_ATTR(sfp2_txdis,        S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SFP2_TXDIS);
static SENSOR_DEVICE_ATTR(sfp1_txfault,      S_IRUGO,           get_swpld_reg, NULL,          SFP1_TXFAULT);
static SENSOR_DEVICE_ATTR(sfp2_txfault,      S_IRUGO,           get_swpld_reg, NULL,          SFP2_TXFAULT);
static SENSOR_DEVICE_ATTR(sfp1_int,          S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SFP1_INT);
static SENSOR_DEVICE_ATTR(sfp2_int,          S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SFP2_INT);
static SENSOR_DEVICE_ATTR(board_stage,       S_IRUGO,           get_swpld_reg, NULL,          BOARD_STAGE);

//SWPLD2
static SENSOR_DEVICE_ATTR(cpld_pcie_rst,     S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, CPLD_PCIE_RST);
static SENSOR_DEVICE_ATTR(cpld_gpio_rst,     S_IRUGO,           get_swpld_reg, NULL,          CPLD_GPIO_RST);

//SWPLD3
static SENSOR_DEVICE_ATTR(cpld_upgrade_rst,  S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, CPLD_UPGRADE_RST);
static SENSOR_DEVICE_ATTR(cpld_gpio_rst_en,  S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, CPLD_GPIO_RST_EN);

static struct attribute *swpld1_device_attrs[] = {
    &sensor_dev_attr_sfp_present_32.dev_attr.attr,
    &sensor_dev_attr_sfp_lpmode.dev_attr.attr,
    &sensor_dev_attr_sfp_reset.dev_attr.attr,
    &sensor_dev_attr_sfp_modsel.dev_attr.attr,
    &sensor_dev_attr_sfp_interrupt.dev_attr.attr,
    &sensor_dev_attr_swpld1_reg_value.dev_attr.attr,
    &sensor_dev_attr_swpld1_reg_addr.dev_attr.attr,
    &sensor_dev_attr_sfp1_rxlos.dev_attr.attr,
    &sensor_dev_attr_sfp2_rxlos.dev_attr.attr,
    &sensor_dev_attr_sfp1_txdis.dev_attr.attr,
    &sensor_dev_attr_sfp2_txdis.dev_attr.attr,
    &sensor_dev_attr_sfp1_txfault.dev_attr.attr,
    &sensor_dev_attr_sfp2_txfault.dev_attr.attr,
    &sensor_dev_attr_sfp1_int.dev_attr.attr,
    &sensor_dev_attr_sfp2_int.dev_attr.attr,
    &sensor_dev_attr_board_stage.dev_attr.attr,
    NULL,
};

static struct attribute *swpld2_device_attrs[] = {
    &sensor_dev_attr_sfp_present_2.dev_attr.attr,
    &sensor_dev_attr_swpld2_reg_value.dev_attr.attr,
    &sensor_dev_attr_swpld2_reg_addr.dev_attr.attr,
    &sensor_dev_attr_cpld_pcie_rst.dev_attr.attr,
    &sensor_dev_attr_cpld_gpio_rst.dev_attr.attr,
    NULL,
};

static struct attribute *swpld3_device_attrs[] = {
    &sensor_dev_attr_swpld3_reg_value.dev_attr.attr,
    &sensor_dev_attr_swpld3_reg_addr.dev_attr.attr,
    &sensor_dev_attr_cpld_upgrade_rst.dev_attr.attr,
    &sensor_dev_attr_cpld_gpio_rst_en.dev_attr.attr,
    NULL,
};

static struct attribute_group swpld1_device_attr_grp = {
    .attrs = swpld1_device_attrs,
};

static struct attribute_group swpld2_device_attr_grp = {
    .attrs = swpld2_device_attrs,
};

static struct attribute_group swpld3_device_attr_grp = {
    .attrs = swpld3_device_attrs,
};

static int __init swpld1_probe(struct platform_device *pdev)
{
    struct cpld_platform_data *pdata;
    struct i2c_adapter *parent;
    int ret;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "CPLD platform data not found\n");
        return -ENODEV;
    }

    parent = i2c_get_adapter(BUS5);
    if (!parent) {
        printk(KERN_WARNING "Parent adapter (%d) not found\n", BUS5);
        return -ENODEV;
    }
    
    pdata->client = i2c_new_dummy(parent, pdata->reg_addr);
    if (!pdata->client) {
        printk(KERN_WARNING "Fail to create dummy i2c client for addr %d\n", pdata->reg_addr);
        goto error;
    }

    ret = sysfs_create_group(&pdev->dev.kobj, &swpld1_device_attr_grp);
    if (ret) {
        printk(KERN_WARNING "Fail to create cpld attribute group");
        return -ENODEV;
    }    

    return 0;

error:
    i2c_unregister_device(pdata->client);
    i2c_put_adapter(parent);

    return -ENODEV; 
}

static int __init swpld2_probe(struct platform_device *pdev)
{
    struct cpld_platform_data *pdata;
    struct i2c_adapter *parent;
    int ret;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "CPLD platform data not found\n");
        return -ENODEV;
    }

    parent = i2c_get_adapter(BUS5);
    if (!parent) {
        printk(KERN_WARNING "Parent adapter (%d) not found\n", BUS5);
        return -ENODEV;
    }
    
    pdata->client = i2c_new_dummy(parent, pdata->reg_addr);
    if (!pdata->client) {
        printk(KERN_WARNING "Fail to create dummy i2c client for addr %d\n", pdata->reg_addr);
        goto error;
    }

    ret = sysfs_create_group(&pdev->dev.kobj, &swpld2_device_attr_grp);
    if (ret) {
        printk(KERN_WARNING "Fail to create cpld attribute group");
        return -ENODEV;
    }    

    return 0;

error:
    i2c_unregister_device(pdata->client);
    i2c_put_adapter(parent);

    return -ENODEV; 
}

static int __init swpld3_probe(struct platform_device *pdev)
{
    struct cpld_platform_data *pdata;
    struct i2c_adapter *parent;
    int ret;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "CPLD platform data not found\n");
        return -ENODEV;
    }

    parent = i2c_get_adapter(BUS5);
    if (!parent) {
        printk(KERN_WARNING "Parent adapter (%d) not found\n", BUS5);
        return -ENODEV;
    }
    
    pdata->client = i2c_new_dummy(parent, pdata->reg_addr);
    if (!pdata->client) {
        printk(KERN_WARNING "Fail to create dummy i2c client for addr %d\n", pdata->reg_addr);
        goto error;
    }

    ret = sysfs_create_group(&pdev->dev.kobj, &swpld3_device_attr_grp);
    if (ret) {
        printk(KERN_WARNING "Fail to create cpld attribute group");
        return -ENODEV;
    }    

    return 0;

error:
    i2c_unregister_device(pdata->client);
    i2c_put_adapter(parent);

    return -ENODEV;
}

static int __exit swpld1_remove(struct platform_device *pdev)
{
    struct i2c_adapter *parent = NULL;
    struct cpld_platform_data *pdata = pdev->dev.platform_data;

    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
    } 
    else {
        if (pdata->client) {
            if (!parent) {
                parent = (pdata->client)->adapter;
            }
            i2c_unregister_device(pdata->client);
        }
    }
    sysfs_remove_group(&pdev->dev.kobj, &swpld1_device_attr_grp);
    i2c_put_adapter(parent);

    return 0;
}

static int __exit swpld2_remove(struct platform_device *pdev)
{
    struct i2c_adapter *parent = NULL;
    struct cpld_platform_data *pdata = pdev->dev.platform_data;

    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
    } 
    else {
        if (pdata->client) {
            if (!parent) {
                parent = (pdata->client)->adapter;
            }
            i2c_unregister_device(pdata->client);
        }
    }
    sysfs_remove_group(&pdev->dev.kobj, &swpld2_device_attr_grp);
    i2c_put_adapter(parent);

    return 0;
}

static int __exit swpld3_remove(struct platform_device *pdev)
{
    struct i2c_adapter *parent = NULL;
    struct cpld_platform_data *pdata = pdev->dev.platform_data;

    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
    } 
    else {
        if (pdata->client) {
            if (!parent) {
                parent = (pdata->client)->adapter;
            }
            i2c_unregister_device(pdata->client);
        }
    }
    sysfs_remove_group(&pdev->dev.kobj, &swpld3_device_attr_grp);
    i2c_put_adapter(parent);

    return 0;
}

static struct platform_driver swpld1_driver = {
    .probe  = swpld1_probe,
    .remove = __exit_p(swpld1_remove),
    .driver = {
        .owner  = THIS_MODULE,
        .name   = "delta-ag9032v2-swpld1",
    },
};

static struct platform_driver swpld2_driver = {
    .probe  = swpld2_probe,
    .remove = __exit_p(swpld2_remove),
    .driver = {
        .owner  = THIS_MODULE,
        .name   = "delta-ag9032v2-swpld2",
    },
};

static struct platform_driver swpld3_driver = {
    .probe  = swpld3_probe,
    .remove = __exit_p(swpld3_remove),
    .driver = {
        .owner  = THIS_MODULE,
        .name   = "delta-ag9032v2-swpld3",
    },
};
/*----------------    CPLD  - end   ------------- */

/*----------------    MUX   - start   ------------- */

struct swpld_mux_platform_data {
    int parent;
    int base_nr;
    int reg_addr;
    struct i2c_client *cpld;
};

struct swpld_mux {
    struct i2c_adapter *parent;
    struct i2c_adapter **child;
    struct swpld_mux_platform_data data;
};
static struct swpld_mux_platform_data ag9032v2_swpld_mux_platform_data[] = {
    {
        .parent         = BUS1, 
        .base_nr        = BUS1_BASE_NUM, 
        .cpld           = NULL,
        .reg_addr       = BUS1_MUX_REG ,// the i2c register address which for select mux TEMP(FAN)
    },
};

static struct platform_device ag9032v2_swpld_mux[] = {
    {
        .name           = "delta-ag9032v2-swpld-mux",
        .id             = 0,
        .dev            = {
                .platform_data   = &ag9032v2_swpld_mux_platform_data[0],
                .release         = device_release,
        },
    },
};

static int cpld_reg_write_byte(struct i2c_client *client, u8 regaddr, u8 val)
{
    union i2c_smbus_data data;

    data.byte = val;
    return client->adapter->algo->smbus_xfer(client->adapter, client->addr,
                                             client->flags,
                                             I2C_SMBUS_WRITE,
                                             regaddr, I2C_SMBUS_BYTE_DATA, &data);
}

static int swpld_mux_select(struct i2c_adapter *adap, void *data, u8 chan)
{
    struct swpld_mux *mux = data;
    u8 swpld_mux_val=0; 

    if ( mux->data.base_nr == BUS1_BASE_NUM ){
        swpld_mux_val = (u8)(chan) + 0x00;
    }
    else
    {
        swpld_mux_val = 0x00;
    }

    return cpld_reg_write_byte(mux->data.cpld, mux->data.reg_addr, (u8)(swpld_mux_val & 0xff));
}

static int __init swpld_mux_probe(struct platform_device *pdev)
{
    struct swpld_mux *mux;
    struct swpld_mux_platform_data *pdata;
    struct i2c_adapter *parent;
    int i, ret, dev_num;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "SWPLD platform data not found\n");
        return -ENODEV;
    }

    parent = i2c_get_adapter(pdata->parent);
    if (!parent) {
        dev_err(&pdev->dev, "Parent adapter (%d) not found\n", pdata->parent);
        return -ENODEV;
    }
    
    /* Judge bus number to decide how many devices*/
    switch (pdata->parent) {
        case BUS1:
            dev_num = BUS1_DEV_NUM;
            break;
        default :
            dev_num = DEFAULT_NUM;  
            break;
    }

    mux = kzalloc(sizeof(*mux), GFP_KERNEL);
    if (!mux) {
        ret = -ENOMEM;
        printk(KERN_ERR "Failed to allocate memory for mux\n");
        goto alloc_failed;
    }

    mux->parent = parent;
    mux->data = *pdata;
    mux->child = kzalloc(sizeof(struct i2c_adapter *) * dev_num, GFP_KERNEL);
    if (!mux->child) {
        ret = -ENOMEM;
        printk(KERN_ERR "Failed to allocate memory for device on mux\n");
        goto alloc_failed2;
    }

    for (i = 0; i < dev_num; i++) {
        int nr = pdata->base_nr + i;
        unsigned int class = 0;

        mux->child[i] = i2c_add_mux_adapter(parent, &pdev->dev, mux,
                           nr, i, class,
                           swpld_mux_select, NULL);
        if (!mux->child[i]) {
            ret = -ENODEV;
            dev_err(&pdev->dev, "Failed to add adapter %d\n", i);
            goto add_adapter_failed;
        }
    }

    platform_set_drvdata(pdev, mux);
    return 0;

add_adapter_failed:
    for (; i > 0; i--)
        i2c_del_mux_adapter(mux->child[i - 1]);
    kfree(mux->child);
alloc_failed2:
    kfree(mux);
alloc_failed:
    i2c_put_adapter(parent);

    return ret;
}


static int __exit swpld_mux_remove(struct platform_device *pdev)
{
    int i;
    struct swpld_mux *mux = platform_get_drvdata(pdev);
    struct swpld_mux_platform_data *pdata;
    struct i2c_adapter *parent;
    int dev_num;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "SWPLD platform data not found\n");
        return -ENODEV;
    }

    parent = i2c_get_adapter(pdata->parent);
    if (!parent) {
        dev_err(&pdev->dev, "Parent adapter (%d) not found\n",
            pdata->parent);
        return -ENODEV;
    }
    switch (pdata->parent) {
        case BUS1:
            dev_num = BUS1_DEV_NUM;
            break;
        default :
            dev_num = DEFAULT_NUM;  
            break;
    }

    for (i = 0; i < dev_num; i++)
        i2c_del_mux_adapter(mux->child[i]);

    platform_set_drvdata(pdev, NULL);
    i2c_put_adapter(mux->parent);
    kfree(mux->child);
    kfree(mux);

    return 0;
}

static struct platform_driver swpld_mux_driver = {
    .probe  = swpld_mux_probe,
    .remove = __exit_p(swpld_mux_remove), /* TODO */
    .driver = {
        .owner  = THIS_MODULE,
        .name   = "delta-ag9032v2-swpld-mux",
    },
};
/*----------------    MUX   - end   ------------- */

/*----------------   module initialization     ------------- */
static void __init delta_ag9032v2_platform_init(void)
{
    struct i2c_client *client;
    struct i2c_adapter *adapter;
    struct cpld_platform_data *cpld_pdata;
    struct swpld_mux_platform_data *swpld_mux_pdata;
    int ret,i = 0;
    printk("ag9032v2_platform module initialization\n");
    
    //Use pca9548 in i2c_mux_pca954x.c
    adapter = i2c_get_adapter(BUS1); 
    //client = i2c_new_device(adapter, &i2c_info_pca9548[0]);
    i2c_client_9548 = i2c_new_device(adapter, &i2c_info_pca9548[0]);
    
    i2c_put_adapter(adapter);

    // set the SWPLD prob and remove
    ret = platform_driver_register(&swpld1_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register cpld driver\n");
        goto error_swpld1_driver;
    }

    ret = platform_driver_register(&swpld2_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register cpld driver\n");
        goto error_swpld2_driver;
    }

    ret = platform_driver_register(&swpld3_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register cpld driver\n");
        goto error_swpld3_driver; 
    }
    // register the mux prob which call the CPLD
    ret = platform_driver_register(&swpld_mux_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register swpld mux driver\n");
        goto error_swpld_mux_driver;
    }

    // register the i2c devices    
    ret = platform_driver_register(&i2c_device_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register i2c device driver\n");
        goto error_i2c_device_driver;
    }

    // register the CPLD
    ret = platform_device_register(&swpld_device[swpld1]);
    if (ret) {
        printk(KERN_WARNING "Fail to create cpld device\n");
        goto error_swpld1_device;
    }

    ret = platform_device_register(&swpld_device[swpld2]);
    if (ret) {
        printk(KERN_WARNING "Fail to create cpld device\n");
        goto error_swpld2_device;
    }

    ret = platform_device_register(&swpld_device[swpld3]);
    if (ret) {
        printk(KERN_WARNING "Fail to create cpld device\n");
        goto error_swpld3_device;
    }

    // link the CPLD and the Mux
    cpld_pdata = swpld_device[swpld1].dev.platform_data;
    for (i = 0; i < ARRAY_SIZE(ag9032v2_swpld_mux); i++)
    {
        swpld_mux_pdata = ag9032v2_swpld_mux[i].dev.platform_data;
        swpld_mux_pdata->cpld = cpld_pdata[swpld1].client;  
        ret = platform_device_register(&ag9032v2_swpld_mux[i]);          
        if (ret) {
            printk(KERN_WARNING "Fail to create swpld mux %d\n", i);
            goto error_ag9032v2_swpld_mux;
        }
    }

    for (i = 0; i < ARRAY_SIZE(ag9032v2_i2c_device); i++)
    {
        ret = platform_device_register(&ag9032v2_i2c_device[i]);
        if (ret) {
            printk(KERN_WARNING "Fail to create i2c device %d\n", i);
            goto error_ag9032v2_i2c_device;
        }
    }

    if (ret)
        goto error_ag9032v2_swpld_mux;

    return 0;

error_ag9032v2_i2c_device:
    i--;
    for (; i >= 0; i--) {
        platform_device_unregister(&ag9032v2_i2c_device[i]);
    }
    i = ARRAY_SIZE(ag9032v2_swpld_mux);    
error_ag9032v2_swpld_mux:
    i--;
    for (; i >= 0; i--) {
        platform_device_unregister(&ag9032v2_swpld_mux[i]);
    }
    platform_driver_unregister(&swpld_device[swpld1]);
error_swpld3_device:
    platform_driver_unregister(&swpld_device[swpld2]);
error_swpld2_device:
    platform_driver_unregister(&swpld_device[swpld3]);
error_swpld1_device:
    platform_driver_unregister(&i2c_device_driver);
error_i2c_device_driver:
    platform_driver_unregister(&swpld_mux_driver);
error_swpld_mux_driver:
    platform_driver_unregister(&swpld3_driver);
error_swpld3_driver:
    platform_driver_unregister(&swpld2_driver);
error_swpld2_driver:
    platform_driver_unregister(&swpld1_driver);
error_swpld1_driver:
    return ret;
}

static void __exit delta_ag9032v2_platform_exit(void)
{
    int i = 0;

    for ( i = 0; i < ARRAY_SIZE(ag9032v2_i2c_device); i++ ) {
        platform_device_unregister(&ag9032v2_i2c_device[i]);
    }

    for (i = 0; i < ARRAY_SIZE(ag9032v2_swpld_mux); i++) {
        platform_device_unregister(&ag9032v2_swpld_mux[i]);
    }

    platform_driver_unregister(&swpld_mux_driver);
    platform_device_unregister(&swpld_device[swpld1]);
    platform_device_unregister(&swpld_device[swpld2]);
    platform_device_unregister(&swpld_device[swpld3]);

    platform_driver_unregister(&i2c_device_driver);
    platform_driver_unregister(&swpld1_driver);
    platform_driver_unregister(&swpld2_driver);
    platform_driver_unregister(&swpld3_driver);

    i2c_unregister_device(i2c_client_9548);
}

module_init(delta_ag9032v2_platform_init);
module_exit(delta_ag9032v2_platform_exit);

MODULE_DESCRIPTION("DNI ag9032v2 Platform Support");
MODULE_AUTHOR("Stanley Chi <stanley.chi@deltaww.com>");
MODULE_LICENSE("GPL"); 