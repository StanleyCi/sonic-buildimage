#include "delta_ag9032v2_common.h"

unsigned char cpupld_reg_addr;
/*----------------    CPLD  - start   ------------- */
/*    CPLD  -- device   */
static struct platform_device ag9032v2_cpld = {
    .name               = "delta-ag9032v2-cpld",
    .id                 = 0,
    .dev                = {
                .platform_data   = ag9032v2_cpld_platform_data,
                .release         = device_release
    },
};

static ssize_t get_cpld_reg(struct device *dev, struct device_attribute *dev_attr, char *buf) 
{
    int ret;
    int mask;
    int value;
    char note[200];
    unsigned char reg;
    struct sensor_device_attribute *attr = to_sensor_dev_attr(dev_attr);
    struct cpld_platform_data *pdata = dev->platform_data;
    
    switch (attr->index) {
        case CPLD_REG_ADDR:
            return sprintf(buf, "0x%02x\n", cpupld_reg_addr);
        case CPLD_REG_VALUE:
            ret = i2c_smbus_read_byte_data(pdata[system_cpld].client, cpupld_reg_addr);
            return sprintf(buf, "0x%02x\n", ret);
        case CPU_PLATFORM_TYPE ... PSU2_ON:
            reg   = attribute_data[attr->index].reg;
            mask  = attribute_data[attr->index].mask;
            value = i2c_smbus_read_byte_data(pdata[system_cpld].client, reg);
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

static ssize_t set_cpld_reg(struct device *dev, struct device_attribute *dev_attr,
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
        case CPLD_REG_ADDR:
            cpupld_reg_addr = set_data;
            return count;
        case CPLD_REG_VALUE:
            i2c_smbus_write_byte_data(pdata[system_cpld].client, cpupld_reg_addr, set_data);
            return count;
        case CPU_PLATFORM_TYPE ... PSU2_ON:
            reg   = attribute_data[attr->index].reg;
            mask  = attribute_data[attr->index].mask;
            value = i2c_smbus_read_byte_data(pdata[system_cpld].client, reg);
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

    i2c_smbus_write_byte_data(pdata[system_cpld].client, reg, set_data);
    return count;
}

static SENSOR_DEVICE_ATTR(cpld_reg_addr,     S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, CPLD_REG_ADDR);
static SENSOR_DEVICE_ATTR(cpld_reg_value,    S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, CPLD_REG_VALUE);

static SENSOR_DEVICE_ATTR(cpu_platform_type, S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, CPU_PLATFORM_TYPE);
static SENSOR_DEVICE_ATTR(mb_rst,            S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, MB_RST);
static SENSOR_DEVICE_ATTR(cpld_lpc_rst,      S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, CPLD_LPC_RST);
static SENSOR_DEVICE_ATTR(mb_rst_done,       S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, MB_RST_DONE);
static SENSOR_DEVICE_ATTR(pcie_rst,          S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, PCIE_RST);
static SENSOR_DEVICE_ATTR(cpld_bus_rst,      S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, CPLD_BUS_RST);
static SENSOR_DEVICE_ATTR(bmc_rst,           S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, BMC_RST);
static SENSOR_DEVICE_ATTR(bcm56870_rst,      S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, BCM56870_RST);
static SENSOR_DEVICE_ATTR(usb_rst,           S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, USB_RST);
static SENSOR_DEVICE_ATTR(psu1_present,      S_IRUGO,           get_cpld_reg, NULL,         PSU1_PRESENT);
static SENSOR_DEVICE_ATTR(psu2_present,      S_IRUGO,           get_cpld_reg, NULL,         PSU2_PRESENT);
static SENSOR_DEVICE_ATTR(psu1_fan_ok,       S_IRUGO,           get_cpld_reg, NULL,         PSU1_FAN_OK);
static SENSOR_DEVICE_ATTR(psu2_fan_ok,       S_IRUGO,           get_cpld_reg, NULL,         PSU2_FAN_OK);
static SENSOR_DEVICE_ATTR(psu1_int,          S_IRUGO,           get_cpld_reg, NULL,         PSU1_INT);
static SENSOR_DEVICE_ATTR(psu2_int,          S_IRUGO,           get_cpld_reg, NULL,         PSU2_INT);
static SENSOR_DEVICE_ATTR(psu1_on,           S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, PSU1_ON);
static SENSOR_DEVICE_ATTR(psu2_on,           S_IRUGO | S_IWUSR, get_cpld_reg, set_cpld_reg, PSU2_ON);


static struct attribute *ag9032v2_cpld_attrs[] = {

    &sensor_dev_attr_cpld_reg_value.dev_attr.attr,
    &sensor_dev_attr_cpld_reg_addr.dev_attr.attr,   


    &sensor_dev_attr_cpu_platform_type.dev_attr.attr,
    &sensor_dev_attr_mb_rst.dev_attr.attr,
    &sensor_dev_attr_cpld_lpc_rst.dev_attr.attr,
    &sensor_dev_attr_mb_rst_done.dev_attr.attr,
    &sensor_dev_attr_pcie_rst.dev_attr.attr,
    &sensor_dev_attr_cpld_bus_rst.dev_attr.attr,
    &sensor_dev_attr_bmc_rst.dev_attr.attr,
    &sensor_dev_attr_bcm56870_rst.dev_attr.attr,
    &sensor_dev_attr_usb_rst.dev_attr.attr,
    &sensor_dev_attr_psu1_present.dev_attr.attr,
    &sensor_dev_attr_psu2_present.dev_attr.attr,
    &sensor_dev_attr_psu1_fan_ok.dev_attr.attr,
    &sensor_dev_attr_psu2_fan_ok.dev_attr.attr,
    &sensor_dev_attr_psu1_int.dev_attr.attr,
    &sensor_dev_attr_psu2_int.dev_attr.attr,
    &sensor_dev_attr_psu1_on.dev_attr.attr,
    &sensor_dev_attr_psu2_on.dev_attr.attr,
    NULL,
};

static struct attribute_group ag9032v2_cpld_attr_grp = {
    .attrs = ag9032v2_cpld_attrs,
};

static int __init cpld_probe(struct platform_device *pdev)
{
    struct cpld_platform_data *pdata;
    struct i2c_adapter *parent;
    int ret;

    pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "CPLD platform data not found\n");
        return -ENODEV;
    }

    parent = i2c_get_adapter(BUS1);
    if (!parent) {
        printk(KERN_WARNING "Parent adapter (%d) not found\n",BUS7);
        return -ENODEV;
    }

    pdata[system_cpld].client = i2c_new_dummy(parent, pdata[system_cpld].reg_addr);
    if (!pdata[system_cpld].client) {
        printk(KERN_WARNING "Fail to create dummy i2c client for addr %d\n", pdata[system_cpld].reg_addr);
        goto error;
    }

    ret = sysfs_create_group(&pdev->dev.kobj, &ag9032v2_cpld_attr_grp);
   if (ret) {
        printk(KERN_WARNING "Fail to create cpld attribute group");
        goto error;
   }

    return 0;

error:
    i2c_unregister_device(pdata[system_cpld].client);
    i2c_put_adapter(parent);

    return -ENODEV;
}

static int __exit cpld_remove(struct platform_device *pdev)
{
    struct i2c_adapter *parent = NULL;
    struct cpld_platform_data *pdata = pdev->dev.platform_data;
    sysfs_remove_group(&pdev->dev.kobj, &ag9032v2_cpld_attr_grp);

    if (!pdata) {
        dev_err(&pdev->dev, "Missing platform data\n");
    }
    else {
        if (pdata[system_cpld].client) {
            if (!parent) {
                parent = (pdata[system_cpld].client)->adapter;
            }
        i2c_unregister_device(pdata[system_cpld].client);
        }
    }
    i2c_put_adapter(parent);

    return 0;
}

static struct platform_driver cpld_driver = {
    .probe  = cpld_probe,
    .remove = __exit_p(cpld_remove),
    .driver = {
        .owner  = THIS_MODULE,
        .name   = "delta-ag9032v2-cpld",
    },
};

/*----------------    CPLD  - end   ------------- */

/*----------------   module initialization     ------------- */
static int __init delta_ag9032v2_cpupld_init(void)
{
    int ret;
    printk(KERN_WARNING "ag9032v2_platform_cpupld module initialization\n");

    if (ret != 0){
        printk(KERN_WARNING "Fail to create IPMI user\n");
    }

    // set the CPUPLD prob and remove
    ret = platform_driver_register(&cpld_driver);
    if (ret) {
        printk(KERN_WARNING "Fail to register cpupld driver\n");
        goto error_cpupld_driver;
    }

    // register the CPUPLD
    ret = platform_device_register(&ag9032v2_cpld);
    if (ret) {
        printk(KERN_WARNING "Fail to create cpupld device\n");
        goto error_ag9032v2_cpupld;
    }
    return 0;

error_ag9032v2_cpupld:
    platform_driver_unregister(&cpld_driver);
error_cpupld_driver:
    return ret;
}

static void __exit delta_ag9032v2_cpupld_exit(void)
{
    platform_device_unregister(&ag9032v2_cpld);
    platform_driver_unregister(&cpld_driver);  
}
module_init(delta_ag9032v2_cpupld_init);
module_exit(delta_ag9032v2_cpupld_exit);

MODULE_DESCRIPTION("DNI ag9032v2 CPLD Platform Support");
MODULE_AUTHOR("Stanley Chi <stanley.chi@deltaww.com>");
MODULE_LICENSE("GPL"); 
