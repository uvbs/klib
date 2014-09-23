

//CRC8.h      CRC8校验的实现
static unsigned char checkCRC(unsigned char& crc, unsigned char data)  // crc数据设为引用调用
{
	for (unsigned char i = 0x80; i > 0; i >>= 1) // 进行循环，每轮进行8次循环，处理一个字节
	{
		if ((crc & 0x80) != 0) // 首位为1则移位，并进行异或  
		{ 
			crc <<= 1; // 左移一位
			if ( (data & i) != 0) // 如果新读入的数据为1，将最低位置1
			{
				crc |= 1;
			}
			crc ^= 0x07;// 首位移出，对低8位进行异或运算，7的二进制为0000,0111
		} 
		else // 首位为0，只移位，不进行异或运
		{  
			crc <<= 1;// 左移一位
			if ( (data & i) != 0)// 如果新读入的数据为1，将最低位置1
			{
				crc |= 1;
			}
		}
	}
	return crc;
}