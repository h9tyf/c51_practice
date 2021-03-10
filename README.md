# 蓝桥杯单片机踩坑记录

### 数字位数不足

```c
//示例
//错误版本
long aaa_to_time(u8 hour, u8 min, u8 seconds)
{
	long temp = 0;
	temp += hour * 60 * 60;
	temp += min * 60;
	temp += seconds;
	return  temp;
}
//正确版本
long aaa_to_time(u8 hour, u8 min, u8 seconds)
{
	long temp = 0;
	temp += hour * 60L * 60L;
	temp += min * 60L;
	temp += seconds;
	return  temp;
}
```

数字后面加一个L，可以强制转为long。

##### 出现情况

运算出现问题可能是位数不足，如上例。以及计算结果混乱等问题，可能是数据位数不够。

##### 记住

u8范围：

long 16位：

### *** ERROR L107: ADDRESS SPACE OVERFLOW

错误是内存空间溢出，可以改变KEIL的编译的模式，**改为Large模式来达到编译成功**。

找到Options for Target...，设置Memory Model即可。





### 。。。

![image-20210303223645168](C:\Users\hlx\AppData\Roaming\Typora\typora-user-images\image-20210303223645168.png)

错误原因：缺少main函数

# requires ANSI-style prototype

函数声明，

在.h文件中缺少声明

之类的问题

## end = (end + 1)%3 有0和1，2对应的led显示不正常

设置状态为123

而取模后得到的结果为012