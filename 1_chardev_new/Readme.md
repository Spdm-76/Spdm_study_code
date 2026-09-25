
cat@lubancat:~$ sudo insmod chardev.ko
[ 3703.929809] chrdev init
[ 3703.929875] major=234,minor=0
[ 3703.930289] device created
cat@lubancat:~$
cat@lubancat:~$
cat@lubancat:~$ ls -l /dev/CharDev
crw------- 1 root root 234, 0 Aug 26 04:06 /dev/CharDev
cat@lubancat:~$
cat@lubancat:~$
cat@lubancat:~$ sudo ./chardev_app /dev/CharDev
[ 3728.524936] CharDev open---------

[ 3728.524983] write data: Hello World
[ 3728.524983]
[ 3728.524999] CharDev release
[ 3729.525433] CharDev open
--------------读取数据--------------
[ 3729.525607] CharDev release
read data:CharDev Driver
cat@lubancat:~$
cat@lubancat:~$
cat@lubancat:~$ sudo rmmod chardev.ko
[ 3735.367337] chrdev exit!
cat@lubancat:~$
