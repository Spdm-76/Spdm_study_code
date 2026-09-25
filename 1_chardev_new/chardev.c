#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>


#define DEV_NAME    "CharDev_Template_Dev"      // 设备名 —— 字符设备模板
#define DEV_CNT     1                           // 设备数量
#define BUFF_SIZE   128                         // 读写缓冲区的大小

static char vbuf[BUFF_SIZE];                    // 设备缓冲区，用于数据读写
static char data[] = {"CharDev Driver"};        // 传输给用户空间的数据

struct Chardev_Temp{
    dev_t devid;                    // 设备号
    struct cdev chardev_temp_cdev;  // 字符设备对象
    struct class *class;            // 设备类
    struct device *device;          // 设备
    int major;                      // 主设备号
    int minor;                      // 副设备号
};
struct Chardev_Temp chardev_temp_dev;

static int chr_dev_open(struct inode *inode, struct file *filp)
{

    printk("CharDev open\n");
    
    filp->private_data = vbuf;
    return 0;
}

static int chr_dev_release(struct inode *inode, struct file *filp)
{
    printk("CharDev release\n");
    return 0;
}

static ssize_t chr_dev_write(struct file *filp, const char __user * buf, size_t count, loff_t *ppos)
{
    int ret = 0;
    char *vbuf = filp->private_data;

    /* 将用户空间的数据拷贝到内核空间的缓冲区 */
    ret = copy_from_user(vbuf, buf, count);
    if (ret == 0) {
        printk("write data: %s\n", vbuf);
    } else {
        printk("Write failure!\n");
    }
    return 0;
}

static ssize_t chr_dev_read(struct file *filp, char __user * buf, size_t count, loff_t *ppos)
{
    int ret = 0;
    char *vbuf = filp->private_data;

    /* 将默认数据拷贝到缓冲区 */
    memcpy(vbuf, data, sizeof(data));
    /* 将缓冲区的数据拷贝到用户空间 */
    ret = copy_to_user(buf, vbuf, count);
    if (ret != 0) {
        /* 若拷贝失败，打印读取失败信息 */
        printk("Read failure!\n");
    }

    return 0;
}

/* 定义字符设备的文件操作结构体，包含了字符设备的各种操作函数 */
static struct file_operations chardev_temp_dev_fops = 
{
    .owner = THIS_MODULE,
    .open = chr_dev_open,
    .release = chr_dev_release,
    .write = chr_dev_write,
    .read = chr_dev_read,
};

/* 字符设备驱动初始化函数，在模块加载时调用 */
static int __init chrdev_temp_init(void)
{
    int ret = 0;
    printk("CharDev init\n");

    /* 1. 动态分配设备号 */
    ret = alloc_chrdev_region(&chardev_temp_dev.devid, 0, DEV_CNT, DEV_NAME);
    if (ret < 0) {
        printk("fail to alloc devno\n");
    }

    chardev_temp_dev.major = MAJOR(chardev_temp_dev.devid);
    chardev_temp_dev.minor = MINOR(chardev_temp_dev.devid);
    printk("major=%d,minor=%d\n", chardev_temp_dev.major, chardev_temp_dev.minor);

    /* 2. 初始化字符设备 */
    chardev_temp_dev.chardev_temp_cdev.owner = THIS_MODULE;
    cdev_init(&chardev_temp_dev.chardev_temp_cdev, &chardev_temp_dev_fops);

    /* 3. 注册字符设备 */
    ret = cdev_add(&chardev_temp_dev.chardev_temp_cdev, chardev_temp_dev.devid, DEV_CNT);
    if (ret < 0){
        printk("fail to add cdev\n");
    }

    /* 4. 创建设备类 */
    chardev_temp_dev.class = class_create(THIS_MODULE, DEV_NAME);
    if (IS_ERR(chardev_temp_dev.class)) {
        printk("fail to add class\n");
    }

    /* 5. 创建设备 */
    chardev_temp_dev.device = device_create(chardev_temp_dev.class, NULL, MKDEV(chardev_temp_dev.major, 0), NULL, DEV_NAME);
    if (IS_ERR(chardev_temp_dev.device)) {
        printk("fail to create device\n");
    }

    printk("device created\n");

    return 0;
}

static void __exit chrdev_temp_exit(void)
{
    printk("chrdev exit!\n");

    /* 1. 注销字符设备 */
    cdev_del(&chardev_temp_dev.chardev_temp_cdev);
    /* 2. 释放字符设备号 */
    unregister_chrdev_region(chardev_temp_dev.devid, DEV_CNT);
    /* 3. 删除设备文件 */
    device_destroy(chardev_temp_dev.class, MKDEV(chardev_temp_dev.major, 0));
    /* 4. 删除设备类 */
    class_destroy(chardev_temp_dev.class);
}

module_init(chrdev_temp_init);
module_exit(chrdev_temp_exit);

MODULE_AUTHOR("SPDM");
MODULE_DESCRIPTION("_CharDev_");
MODULE_LICENSE("GPL");