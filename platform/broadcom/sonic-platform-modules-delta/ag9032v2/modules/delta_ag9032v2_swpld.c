#include "delta_ag9032v2_common.h"

#define QSFP_RESPONSE_1    0x0a
#define QSFP_RESPONSE_2    0x0b
#define QSFP_RESPONSE_3    0x0c
#define QSFP_RESPONSE_4    0x0d

#define QSFP_LP_MODE_1     0x0e
#define QSFP_LP_MODE_2     0x0f
#define QSFP_LP_MODE_3     0x10
#define QSFP_LP_MODE_4     0x11

#define QSFP_PRESENCE_1    0x12
#define QSFP_PRESENCE_2    0x13
#define QSFP_PRESENCE_3    0x14
#define QSFP_PRESENCE_4    0x15
#define SFP_PRESENCE_5     0x02 //on swpld2, bit7 and bit3

#define QSFP_RESET_1       0x16
#define QSFP_RESET_2       0x17
#define QSFP_RESET_3       0x18
#define QSFP_RESET_4       0x19

#define QSFP_INTERRUPT_1   0x1a
#define QSFP_INTERRUPT_2   0x1b
#define QSFP_INTERRUPT_3   0x1c
#define QSFP_INTERRUPT_4   0x1d
#define SFP_INTERRUPT_5    0x03 //on swpld2, bit7 and bit6

#define SFP_RX_LOS         0x02 //on swpld2, bit6 and bit2
#define SFP_TX_DIS         0x02 //on swpld2, bit5 and bit1
#define SFP_TX_FAULT       0x02 //on swpld2, bit4 and bit0

unsigned char swpld1_reg_addr;
unsigned char swpld2_reg_addr;
unsigned char swpld3_reg_addr;

/*----------------    CPLD  - start   ------------- */
/*    SWPLD1  -- device   */
static struct platform_device swpld1_device = {
    .name               = "delta-ag9032v2-swpld1",
    .id                 = 0,
    .dev                = {
                .platform_data   = ag9032v2_swpld1_platform_data,
                .release         = device_release
    },
};

static struct platform_device swpld2_device = {
    .name               = "delta-ag9032v2-swpld2",
    .id                 = 0,
    .dev                = {
                .platform_data   = ag9032v2_swpld2_platform_data,
                .release         = device_release
    },
};
static struct platform_device swpld3_device = {
    .name               = "delta-ag9032v2-swpld3",
    .id                 = 0,
    .dev                = {
                .platform_data   = ag9032v2_swpld3_platform_data,
                .release         = device_release
    },
};


static ssize_t get_present(struct device *dev, struct device_attribute \
                            *dev_attr, char *buf)
{
    u64 data = 0;
    int value;
    struct cpld_platform_data *pdata;

    /*QSFP1~8*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_PRESENCE_1);
    data = (u64)(value & 0xff);

    /*QSFP9~16*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_PRESENCE_2);
    data |= (u64)(value & 0xff) << 8;

    /*QSFP17~24*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_PRESENCE_3);
    data |= (u64)(value & 0xff) << 16;

    /*QSFP25~32*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_PRESENCE_4);
    data |= (u64)(value & 0xff) << 24;

    /*SFP1*/
    value = i2c_smbus_read_byte_data(pdata[swpld2].client, SFP_PRESENCE_5);
    value = value >> 7;
    data |= (u64)(value & 0x01) << 25;

    /*SFP2*/
    value = i2c_smbus_read_byte_data(pdata[swpld2].client, SFP_PRESENCE_5);
    value = value >> 3;
    data |= (u64)(value & 0x01) << 26;

    return sprintf(buf, "0x%016llx\n", data);
}

static ssize_t get_response(struct device *dev, struct device_attribute \
                            *dev_attr, char *buf)
{
    int ret;
    int value;
    u64 data = 0;
    struct cpld_platform_data *pdata;

    /*QSFP1~8*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESPONSE_1);
    data = (u64)(value & 0xff);

    /*QSFP9~16*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESPONSE_2);
    data |= (u64)(value & 0xff) << 8;

    /*QSFP17~24*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESPONSE_3);
    data |= (u64)(value & 0xff) << 16;

    /*QSFP25~32*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESPONSE_4);
    data |= (u64)(value & 0xff) << 24;

    return sprintf(buf, "0x%016llx\n", data);
}

static ssize_t get_lpmode(struct device *dev, struct device_attribute \
                            *dev_attr, char *buf)
{
    int ret;
    int value;
    u64 data = 0;
    struct cpld_platform_data *pdata;

    /*QSFP1~8*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_LP_MODE_1);
    data = (u64)(value & 0xff);

    /*QSFP9~16*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_LP_MODE_2);
    data |= (u64)(value & 0xff) << 8;

    /*QSFP17~24*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_LP_MODE_3);
    data |= (u64)(value & 0xff) << 16;

    /*QSFP25~32*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_LP_MODE_4);
    data |= (u64)(value & 0xff) << 24;

    return sprintf(buf, "0x%016llx\n", data);
}

static ssize_t get_reset(struct device *dev, struct device_attribute \
                            *dev_attr, char *buf)
{
    int ret;
    int value;
    u64 data = 0;
    struct cpld_platform_data *pdata;

    /*QSFP1~8*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESET_1);
    data = (u64)(value & 0xff);

    /*QSFP9~16*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESET_2);
    data |= (u64)(value & 0xff) << 8;

    /*QSFP17~24*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESET_3);
    data |= (u64)(value & 0xff) << 16;

    /*QSFP25~32*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_RESET_4);
    data |= (u64)(value & 0xff) << 24;

    return sprintf(buf, "0x%016llx\n", data);
}



static ssize_t get_interrupt(struct device *dev, struct device_attribute \
                            *dev_attr, char *buf)
{
    int ret;
    int value;
    u64 data = 0;
    struct cpld_platform_data *pdata;

    /*QSFP1~8*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_INTERRUPT_1);
    data = (u64)(value & 0xff);

    /*QSFP9~16*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_INTERRUPT_2);
    data |= (u64)(value & 0xff) << 8;

    /*QSFP17~24*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_INTERRUPT_3);
    data |= (u64)(value & 0xff) << 16;

    /*QSFP25~32*/
    value = i2c_smbus_read_byte_data(pdata[swpld1].client, QSFP_INTERRUPT_4);
    data |= (u64)(value & 0xff) << 24;

    /*SFP1*/
    value = i2c_smbus_read_byte_data(pdata[swpld2].client, SFP_INTERRUPT_5);
    value = value >> 7;
    data |= (u64)(value & 0x01) << 25;

    /*SFP2*/
    value = i2c_smbus_read_byte_data(pdata[swpld2].client, SFP_INTERRUPT_5);
    value = value >> 6;
    data |= (u64)(value & 0x01) << 26;

    return sprintf(buf, "0x%016llx\n", data);
}

static ssize_t set_reset(struct device *dev, struct device_attribute *dev_attr, const char *buf, size_t count)
{
    int err;
    unsigned long long set_data;
    unsigned long set_bytes;
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;

    err = kstrtoull(buf, 16, &set_data);
    if (err){
        return err;
    }

    /*QSFP1~8*/
    set_bytes = (set_data & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESET_1, set_bytes);

    /*QSFP9~16*/
    set_bytes = ((set_data >> 8 ) & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESET_2, set_bytes);

    /*QSFP17~24*/
    set_bytes = ((set_data >> 16 ) & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESET_3, set_bytes);

    /*QSFP25~32*/
    set_bytes = ((set_data >> 24 ) & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESET_4, set_bytes);

    return count;
}

static ssize_t set_lpmode(struct device *dev, struct device_attribute *dev_attr, const char *buf, size_t count)
{
    int err;
    unsigned long long set_data;
    unsigned long set_bytes;
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;

    err = kstrtoull(buf, 16, &set_data);
    if (err){
        return err;
    }

    /*QSFP1~8*/
    set_bytes = (set_data & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_LP_MODE_1, set_bytes);

    /*QSFP9~16*/
    set_bytes = ((set_data >> 8 ) & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_LP_MODE_2, set_bytes);

    /*QSFP17~24*/
    set_bytes = ((set_data >> 16 ) & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_LP_MODE_3, set_bytes);

    /*QSFP25~32*/
    set_bytes = ((set_data >> 24 ) & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_LP_MODE_4, set_bytes);

    return count;
}

static ssize_t set_response(struct device *dev, struct device_attribute *dev_attr, const char *buf, size_t count)
{
    int err;
    unsigned long long set_data;
    unsigned long set_bytes;
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;

    err = kstrtoull(buf, 16, &set_data);
    if (err){
        return err;
    }

    /*QSFP1~8*/
    set_bytes = (set_data & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESPONSE_1, set_bytes);

    /*QSFP9~16*/
    set_bytes = ((set_data >> 8 ) & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESPONSE_2, set_bytes);

    /*QSFP17~24*/
    set_bytes = ((set_data >> 16 ) & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESPONSE_3, set_bytes);

    /*QSFP25~32*/
    set_bytes = ((set_data >> 24 ) & 0xff);
    i2c_smbus_write_byte_data(pdata[swpld1].client, QSFP_RESPONSE_4, set_bytes);

    return count;
}









static ssize_t get_swpld_reg(struct device *dev, struct device_attribute *dev_attr, char *buf) 
{
    int ret;
    int mask;
    int value;
    int cmd_data_len;
    char note[200];
    uint8_t cmd_data[4]={0};
    uint8_t get_cmd;       
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;

    cmd_data_len = sizeof(cmd_data);
    get_cmd = CMD_GETDATA;
    cmd_data[0] = BMC_BUS_5;
    cmd_data[3] = 1;
    mask  = attribute_data[attr->index].mask;
    sprintf(note, "\n%s\n",attribute_data[attr->index].note);

    if (attr->index <= SWPLD3_REG_VALUE){
        switch (attr->index) {
            case SWPLD1_REG_ADDR:
                return sprintf(buf, "0x%02x\n", swpld1_reg_addr);
            case SWPLD2_REG_ADDR:
                return sprintf(buf, "0x%02x\n", swpld2_reg_addr);
            case SWPLD3_REG_ADDR:
                return sprintf(buf, "0x%02x\n", swpld3_reg_addr);
            case SWPLD1_REG_VALUE:
                ret = i2c_smbus_read_byte_data(pdata[swpld1].client, swpld1_reg_addr);
                break;
            case SWPLD2_REG_VALUE:
                ret = i2c_smbus_read_byte_data(pdata[swpld2].client, swpld2_reg_addr);
                break;
            case SWPLD3_REG_VALUE:
                ret = i2c_smbus_read_byte_data(pdata[swpld3].client, swpld3_reg_addr);
                break;
            default:
                return sprintf(buf, "%d not found", attr->index);
        }
        ret = ret & 0xff;
        return sprintf(buf, "0x%02x\n", ret);
    }else{

        switch (attr->index) {
            case SWPLD1_MAJOR_VER ... PSU_LED_MODE :
                //cmd_data[1] = SWPLD1_ADDR;
                ret = i2c_smbus_read_byte_data(pdata[swpld1].client, attribute_data[attr->index].reg);
                break;
            case SWPLD2_MAJOR_VER ... FAN_MOD4_LED :
                //cmd_data[1] = SWPLD2_ADDR;
                ret = i2c_smbus_read_byte_data(pdata[swpld2].client, attribute_data[attr->index].reg);
                break;
            case SWPLD3_MAJOR_VER ... PLATFORM_TYPE :
                //cmd_data[1] = SWPLD3_ADDR;
                ret = i2c_smbus_read_byte_data(pdata[swpld3].client, attribute_data[attr->index].reg);
                break;
            default:
                return sprintf(buf, "%d not found", attr->index);
        }
        /*
        cmd_data[2] = attribute_data[attr->index].reg;
        value = dni_bmc_cmd(get_cmd, cmd_data, cmd_data_len);*/

        value = value & mask;
        switch (mask) {
            case 0xFF:
                return sprintf(buf, "0x%02x%s", value, note);
            case 0x0F:
                return sprintf(buf, "0x%01x%s", value, note);
            case 0xF0:
                value = value >> 4;
                return sprintf(buf, "0x%01x%s", value, note);
            case 0xC0:
                value = value >> 6;
                return sprintf(buf, "0x%01x%s", value, note);
            case 0x30:
                value = value >> 4;
                return sprintf(buf, "0x%01x%s", value, note);       
            default :
                value = value >> dni_log2(mask);
                return sprintf(buf, "%d%s", value, note);
        }        
    } 
}

static ssize_t set_swpld_reg(struct device *dev, struct device_attribute *dev_attr,
             const char *buf, size_t count)
{
    int err; 
    int value; 
    int set_data; 
    int cmd_data_len;    
    uint8_t cmd_data[4]={0};
    uint8_t set_cmd;
    uint8_t get_cmd;
    unsigned long set_data_ul;
    unsigned char mask;  
    unsigned char mask_out;      
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;

    cmd_data_len = sizeof(cmd_data);
    set_cmd = CMD_SETDATA;
    get_cmd = CMD_GETDATA;

    err = kstrtoul(buf, 0, &set_data_ul);
    if (err){
        return err;
    }

    if (set_data > 0xff){
        printk(KERN_ALERT "address out of range (0x00-0xFF)\n");
        return count;
    }

    set_data = (int)set_data_ul;
    if (attr->index <= SWPLD3_REG_VALUE){
        cmd_data[0] = BMC_BUS_5;
        cmd_data[3] = set_data;
        switch (attr->index) {
        //reg_addr
            case SWPLD1_REG_ADDR:
                swpld1_reg_addr = set_data;
                return count;
            case SWPLD2_REG_ADDR:
                swpld2_reg_addr = set_data;
                return count;
            case SWPLD3_REG_ADDR:
                swpld3_reg_addr = set_data;
                return count;         
        //reg_value       
            case SWPLD1_REG_VALUE:
                i2c_smbus_write_byte_data(pdata[swpld1].client, SWPLD1_REG_VALUE, set_data);

                break;
            case SWPLD2_REG_VALUE:
                i2c_smbus_write_byte_data(pdata[swpld1].client, SWPLD2_REG_VALUE, set_data);

                break;
            case SWPLD3_REG_VALUE:
                i2c_smbus_write_byte_data(pdata[swpld1].client, SWPLD3_REG_VALUE, set_data);

                break;
            default :
                return sprintf(buf, "%d not found", attr->index); 
        }
        //dni_bmc_cmd(set_cmd, cmd_data, cmd_data_len);
        return count;
    }
    else{

        switch (attr->index) {
        //attributes      
            case SWPLD1_MAJOR_VER ... PSU_LED_MODE:  //SWPLD1
                value = i2c_smbus_read_byte_data(pdata[swpld1].client, attribute_data[attr->index].reg);
                //cmd_data[1] = SWPLD1_ADDR;
                break;
            case SWPLD2_MAJOR_VER ... FAN_MOD4_LED:  //SWPLD2
                value = i2c_smbus_read_byte_data(pdata[swpld2].client, attribute_data[attr->index].reg);
                //cmd_data[1] = SWPLD2_ADDR;
                break;
            case SWPLD3_MAJOR_VER ... PLATFORM_TYPE: //SWPLD3
                value = i2c_smbus_read_byte_data(pdata[swpld3].client, attribute_data[attr->index].reg);
                //cmd_data[1] = SWPLD3_ADDR;
                break;
            default:
                return sprintf(buf, "%d not found", attr->index); 
        }

        //value = dni_bmc_cmd(get_cmd, cmd_data, cmd_data_len);
        mask  = attribute_data[attr->index].mask;
        mask_out = value & ~(mask);
        cmd_data[3] = set_data;
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
            case 0xC0:
                set_data = set_data << 6;
                set_data = mask_out | (set_data & mask);
                break;
            case 0x30:
                set_data = set_data << 4;
                set_data = mask_out | (set_data & mask);
                break;
            default :
                set_data = mask_out | (set_data << dni_log2(mask) );        
        }   

        //dni_bmc_cmd(set_cmd, cmd_data, cmd_data_len);
        switch (attr->index) {
        //attributes      
            case SWPLD1_MAJOR_VER ... PSU_LED_MODE:  //SWPLD1
                i2c_smbus_write_byte_data(pdata[swpld1].client, attribute_data[attr->index].reg, set_data);
                break;
            case SWPLD2_MAJOR_VER ... FAN_MOD4_LED:  //SWPLD2
                i2c_smbus_write_byte_data(pdata[swpld2].client, attribute_data[attr->index].reg, set_data);
                break;
            case SWPLD3_MAJOR_VER ... PLATFORM_TYPE: //SWPLD3
                i2c_smbus_write_byte_data(pdata[swpld3].client, attribute_data[attr->index].reg, set_data);
                break;
            default:
                return sprintf(buf, "%d not found", attr->index); 
        }
        return count;
    }
}

//SWPLD
static SENSOR_DEVICE_ATTR(swpld1_reg_addr,   S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD1_REG_ADDR);
static SENSOR_DEVICE_ATTR(swpld1_reg_value,  S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD1_REG_VALUE);
static SENSOR_DEVICE_ATTR(swpld2_reg_addr,   S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD2_REG_ADDR);
static SENSOR_DEVICE_ATTR(swpld2_reg_value,  S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD2_REG_VALUE);
static SENSOR_DEVICE_ATTR(swpld3_reg_addr,   S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD3_REG_ADDR);
static SENSOR_DEVICE_ATTR(swpld3_reg_value,  S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD3_REG_VALUE);

static DEVICE_ATTR(qsfp_present,   S_IRUGO,           get_present,   NULL);
/*
static DEVICE_ATTR(qsfp_lpmode,    S_IWUSR | S_IRUGO, get_lpmode,    set_lpmode);
static DEVICE_ATTR(qsfp_reset,     S_IWUSR | S_IRUGO, get_reset,     set_reset);
static DEVICE_ATTR(qsfp_modsel,    S_IWUSR | S_IRUGO, get_response,  set_response);
static DEVICE_ATTR(qsfp_interrupt, S_IRUGO,           get_interrupt, NULL);
*/

//SWPLD1
/*
static SENSOR_DEVICE_ATTR(swpld1_major_ver,  S_IRUGO,           get_swpld_reg, NULL,          SWPLD1_MAJOR_VER);
static SENSOR_DEVICE_ATTR(swpld1_minor_ver,  S_IRUGO,           get_swpld_reg, NULL,          SWPLD1_MINOR_VER);
static SENSOR_DEVICE_ATTR(swpld1_scrtch_reg, S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD1_SCRTCH_REG);
static SENSOR_DEVICE_ATTR(psu1_pwr_ok,       S_IRUGO,           get_swpld_reg, NULL,          PSU1_PWR_OK);
static SENSOR_DEVICE_ATTR(psu1_int,          S_IRUGO,           get_swpld_reg, NULL,          PSU1_INT);
static SENSOR_DEVICE_ATTR(psu2_pwr_ok,       S_IRUGO,           get_swpld_reg, NULL,          PSU2_PWR_OK);
static SENSOR_DEVICE_ATTR(psu2_int,          S_IRUGO,           get_swpld_reg, NULL,          PSU2_INT);
static SENSOR_DEVICE_ATTR(synce_int,         S_IRUGO,           get_swpld_reg, NULL,          SYNCE_INT);
static SENSOR_DEVICE_ATTR(synce_rst,         S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SYNCE_RST);
static SENSOR_DEVICE_ATTR(synce_eeprom_wp,   S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SYNCE_EEPROM_WP);
static SENSOR_DEVICE_ATTR(psu1_green_led,    S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, PSU1_GREEN_LED);
static SENSOR_DEVICE_ATTR(psu1_red_led,      S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, PSU1_RED_LED);
static SENSOR_DEVICE_ATTR(psu2_green_led,    S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, PSU2_GREEN_LED);
static SENSOR_DEVICE_ATTR(psu2_red_led,      S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, PSU2_RED_LED);
static SENSOR_DEVICE_ATTR(psu_led_mode,      S_IRUGO,           get_swpld_reg, NULL,          PSU_LED_MODE);
//SWPLD2
static SENSOR_DEVICE_ATTR(swpld2_major_ver,  S_IRUGO,           get_swpld_reg, NULL,          SWPLD2_MAJOR_VER);
static SENSOR_DEVICE_ATTR(swpld2_minor_ver,  S_IRUGO,           get_swpld_reg, NULL,          SWPLD2_MINOR_VER);
static SENSOR_DEVICE_ATTR(swpld2_scrtch_reg, S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD2_SCRTCH_REG);
static SENSOR_DEVICE_ATTR(fan_led,           S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, FAN_LED);
static SENSOR_DEVICE_ATTR(sys_led,           S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SYS_LED);
static SENSOR_DEVICE_ATTR(fan_mod1_led,      S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, FAN_MOD1_LED);
static SENSOR_DEVICE_ATTR(fan_mod2_led,      S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, FAN_MOD2_LED);
static SENSOR_DEVICE_ATTR(fan_mod3_led,      S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, FAN_MOD3_LED);
static SENSOR_DEVICE_ATTR(fan_mod4_led,      S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, FAN_MOD4_LED);
//SWPLD3
static SENSOR_DEVICE_ATTR(swpld3_major_ver,  S_IRUGO,           get_swpld_reg, NULL,          SWPLD3_MAJOR_VER);
static SENSOR_DEVICE_ATTR(swpld3_minor_ver,  S_IRUGO,           get_swpld_reg, NULL,          SWPLD3_MINOR_VER);
static SENSOR_DEVICE_ATTR(swpld3_scrtch_reg, S_IRUGO | S_IWUSR, get_swpld_reg, set_swpld_reg, SWPLD3_SCRTCH_REG);
static SENSOR_DEVICE_ATTR(sb_ver,            S_IRUGO,           get_swpld_reg, NULL,          SB_VER);
static SENSOR_DEVICE_ATTR(platform_type,     S_IRUGO,           get_swpld_reg, NULL,          PLATFORM_TYPE);
*/
static struct attribute *swpld1_device_attrs[] = {
    &sensor_dev_attr_swpld1_reg_value.dev_attr.attr,
    &sensor_dev_attr_swpld1_reg_addr.dev_attr.attr,

    &dev_attr_qsfp_present.attr,
    /*    
    &dev_attr_qsfp_lpmode.attr,
    &dev_attr_qsfp_reset.attr,
    &dev_attr_qsfp_modsel.attr,
    &dev_attr_qsfp_interrupt.attr,
*/
    /*
    &sensor_dev_attr_swpld1_major_ver.dev_attr.attr,
    &sensor_dev_attr_swpld1_minor_ver.dev_attr.attr,
    &sensor_dev_attr_swpld1_scrtch_reg.dev_attr.attr,
    &sensor_dev_attr_psu1_pwr_ok.dev_attr.attr,
    &sensor_dev_attr_psu1_int.dev_attr.attr,
    &sensor_dev_attr_psu2_pwr_ok.dev_attr.attr,
    &sensor_dev_attr_psu2_int.dev_attr.attr,
    &sensor_dev_attr_synce_int.dev_attr.attr,
    &sensor_dev_attr_synce_rst.dev_attr.attr,
    &sensor_dev_attr_synce_eeprom_wp.dev_attr.attr,
    &sensor_dev_attr_psu1_green_led.dev_attr.attr,
    &sensor_dev_attr_psu1_red_led.dev_attr.attr,
    &sensor_dev_attr_psu2_green_led.dev_attr.attr,
    &sensor_dev_attr_psu2_red_led.dev_attr.attr,
    &sensor_dev_attr_psu_led_mode.dev_attr.attr,
    NULL,*/
};

static struct attribute *swpld2_device_attrs[] = {
    &sensor_dev_attr_swpld2_reg_value.dev_attr.attr,
    &sensor_dev_attr_swpld2_reg_addr.dev_attr.attr,
    /*
    &sensor_dev_attr_swpld2_major_ver.dev_attr.attr,
    &sensor_dev_attr_swpld2_minor_ver.dev_attr.attr,
    &sensor_dev_attr_swpld2_scrtch_reg.dev_attr.attr,
    &sensor_dev_attr_fan_led.dev_attr.attr,
    &sensor_dev_attr_sys_led.dev_attr.attr,
    &sensor_dev_attr_fan_mod1_led.dev_attr.attr,
    &sensor_dev_attr_fan_mod2_led.dev_attr.attr,
    &sensor_dev_attr_fan_mod3_led.dev_attr.attr,
    &sensor_dev_attr_fan_mod4_led.dev_attr.attr,
    NULL,*/
};

static struct attribute *swpld3_device_attrs[] = {
    &sensor_dev_attr_swpld3_reg_value.dev_attr.attr,
    &sensor_dev_attr_swpld3_reg_addr.dev_attr.attr,
    /*
    &sensor_dev_attr_swpld3_major_ver.dev_attr.attr,
    &sensor_dev_attr_swpld3_minor_ver.dev_attr.attr,
    &sensor_dev_attr_swpld3_scrtch_reg.dev_attr.attr,
    &sensor_dev_attr_sb_ver.dev_attr.attr,
    &sensor_dev_attr_platform_type.dev_attr.attr,
    NULL,*/
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
    int ret;
    ret = sysfs_create_group(&pdev->dev.kobj, &swpld1_device_attr_grp);
   if (ret) {
        printk(KERN_WARNING "Fail to create cpld attribute group");
        return -ENODEV;
   }
    return 0;
}

static int __init swpld2_probe(struct platform_device *pdev)
{
    int ret;
    ret = sysfs_create_group(&pdev->dev.kobj, &swpld2_device_attr_grp);
   if (ret) {
        printk(KERN_WARNING "Fail to create cpld attribute group");
        return -ENODEV;
   }
    return 0;
}

static int __init swpld3_probe(struct platform_device *pdev)
{
    int ret;
    ret = sysfs_create_group(&pdev->dev.kobj, &swpld3_device_attr_grp);
   if (ret) {
        printk(KERN_WARNING "Fail to create cpld attribute group");
        return -ENODEV;
   }
    return 0;
}

static int __exit swpld1_remove(struct platform_device *pdev)
{
    sysfs_remove_group(&pdev->dev.kobj, &swpld1_device_attr_grp);
    return 0;
}

static int __exit swpld2_remove(struct platform_device *pdev)
{
    sysfs_remove_group(&pdev->dev.kobj, &swpld2_device_attr_grp);
    return 0;
}

static int __exit swpld3_remove(struct platform_device *pdev)
{
    sysfs_remove_group(&pdev->dev.kobj, &swpld3_device_attr_grp);
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

/*----------------   module initialization     ------------- */
static int __init delta_ag9032v2_swpld_init(void)
{
    int ret;
    printk(KERN_WARNING "ag9032v2_platform_swpld module initialization\n");

    ret = dni_create_user();
    if (ret != 0){
        printk(KERN_WARNING "Fail to create IPMI user\n");
    }

    // set the SWPLD prob and remove
    ret = platform_driver_register(&swpld1_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register swpld driver\n");
        goto error_swpld1_driver;
    }

    // register the SWPLD
    ret = platform_device_register(&swpld1_device);
    if (ret) {
        printk(KERN_WARNING "Fail to create swpld device\n");
        goto error_swpld1_device;
    }

    // set the SWPLD prob and remove
    ret = platform_driver_register(&swpld2_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register swpld driver\n");
        goto error_swpld2_driver;
    }

    // register the SWPLD
    ret = platform_device_register(&swpld2_device);
    if (ret) {
        printk(KERN_WARNING "Fail to create swpld device\n");
        goto error_swpld2_device;
    }

    // set the SWPLD prob and remove
    ret = platform_driver_register(&swpld3_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register swpld driver\n");
        goto error_swpld3_driver;
    }

    // register the SWPLD
    ret = platform_device_register(&swpld3_device);
    if (ret) {
        printk(KERN_WARNING "Fail to create swpld device\n");
        goto error_swpld3_device;
    }
    return 0;

error_swpld3_device:
    platform_driver_unregister(&swpld3_driver);
error_swpld3_driver:
    platform_device_unregister(&swpld2_device);
error_swpld2_device:
    platform_driver_unregister(&swpld2_driver);
error_swpld2_driver:
    platform_device_unregister(&swpld1_device);
error_swpld1_device:
    platform_driver_unregister(&swpld1_driver);
error_swpld1_driver:
    return ret;
}

static void __exit delta_ag9032v2_swpld_exit(void)
{
    platform_device_unregister(&swpld1_device);
    platform_driver_unregister(&swpld1_driver);  
    platform_device_unregister(&swpld2_device);
    platform_driver_unregister(&swpld2_driver);
    platform_device_unregister(&swpld3_device);
    platform_driver_unregister(&swpld3_driver);
}
module_init(delta_ag9032v2_swpld_init);
module_exit(delta_ag9032v2_swpld_exit);

MODULE_DESCRIPTION("DNI ag9032v2 CPLD Platform Support");
MODULE_AUTHOR("Stanley Chi <stanley.chi@deltaww.com>");
MODULE_LICENSE("GPL"); 
