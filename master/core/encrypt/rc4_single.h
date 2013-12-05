
#if !defined(_Klib_rc4_H_)
#define _Klib_rc4_H_


namespace klib{
namespace encode{

class rc4_encrypt
{
public:
    void set_key(const char* pszKey, int iLen)
    {
        rc4_init(m_sBox,(unsigned char *)pszKey, iLen);
    }

    void encrypt(unsigned char* pszBuff, int iLen)
    {
        rc4_crypt(m_sBox, pszBuff, iLen);
    }

    void decrypt(unsigned char* pszBuff, int iLen)
    {
        rc4_crypt(m_sBox, pszBuff, iLen);
    }

protected:
    
/*
　　RC4加密算法是大名鼎鼎的RSA三人组中的头号人物Ron Rivest在1987年设计的密钥长度可变的流加密算法簇。
      之所以称其为簇，是由于其核心部分的S-box长度可为任意，但一般为256字节。
      该算法的速度可以达到DES加密的10倍左右，且具有很高级别的非线性。RC4起初是用于保护商业机密的。
      但是在1994年9月，它的算法被发布在互联网上，也就不再有什么商业机密了。
      RC4也被叫做ARC4（Alleged RC4——所谓的RC4），因为RSA从来就没有正式发布过这个算法。
*/

    //@todo 封装成一个类，方便使用
    void rc4_init(unsigned char *s, unsigned char *key, unsigned long Len) //初始化函数
    {
	    int i =0, j = 0;
	    char k[256] = {0};
	    unsigned char tmp = 0;
	    for(i=0;i<256;i++)
	    {
		    s[i]=i;
		    k[i]=key[i%Len];
	    }
	    for (i=0; i<256; i++)
	    {
		    j=(j+s[i]+k[i])%256;
		    tmp = s[i];
		    s[i] = s[j]; //交换s[i]和s[j]
		    s[j] = tmp;
	    }
    }
    void rc4_crypt(unsigned char *s, unsigned char *Data, unsigned long Len) //加解密
    {
	    int i = 0, j = 0, t = 0;
	    unsigned long k = 0;
	    unsigned char tmp;
	    for(k=0;k<Len;k++)
	    {
		    i=(i+1)%256;
		    j=(j+s[i])%256;
		    tmp = s[i];
		    s[i] = s[j]; //交换s[x]和s[y]
		    s[j] = tmp;
		    t=(s[i]+s[j])%256;
		    Data[k] ^= s[t];
	    }
    }

protected:
    unsigned char m_sBox[256];
};

/*
void main()
{
	unsigned char s[256] = {0},s2[256] = {0}; //S-box
	char key[256] = {"just for test"};
	char pData[512] = "这是一个用来加密的数据Data";
	unsigned long len = strlen(pData);
	printf("pData = %s\n",pData);
	printf("key = %s, length = %d\n\n",key,strlen(key));
	rc4_init(s,(unsigned char *)key, strlen(key)); //已经完成了初始化
	printf("完成对S[i]的初始化，如下：\n\n");
	for (int i=0; i<256; i++)
	{
		printf("%-3d ",s[i]);
	}
	printf("\n\n");
	for(i=0;i<256;i++)//用s2[i]暂时保留经过初始化的s[i]，很重要的！！！
	{
		s2[i]=s[i];
	}
	printf("已经初始化，现在加密:\n\n");
	rc4_crypt(s,(unsigned char *)pData,len);//加密
	printf("pData = %s\n\n",pData);
	printf("已经加密，现在解密:\n\n");
	rc4_init(s,(unsigned char *)key, strlen(key)); //初始化密钥
	rc4_crypt(s2,(unsigned char *)pData,len);//解密
	printf("pData = %s\n\n",pData);
}
*/


/***

<?php

//$pwd 密钥
//$data 要加密的数据
function rc4 ($pwd, $data) {//$pwd密钥 $data需加密字符串
    $key[] ="";
    $box[] ="";
    $pwd_length = strlen($pwd);
    $data_length = strlen($data);
	$cipher = '';
    for ($i = 0; $i < 256; $i++){
        $key[$i] = ord($pwd[$i % $pwd_length]);
        $box[$i] = $i;
    }
    for ($j = $i = 0; $i < 256; $i++){
        $j = ($j + $box[$i] + $key[$i]) % 256;
        $tmp = $box[$i];
        $box[$i] = $box[$j];
        $box[$j] = $tmp;
    }
    for ($a = $j = $i = 0; $i < $data_length; $i++){
        $a = ($a + 1) % 256;
        $j = ($j + $box[$a]) % 256;
        $tmp = $box[$a];
        $box[$a] = $box[$j];
        $box[$j] = $tmp;
        $k = $box[(($box[$a] + $box[$j]) % 256)];
        $cipher .= chr(ord($data[$i]) ^ $k);
    }
    return $cipher;
}
$key = '5201314';                   //原始KEY
$pwd = md5(md5($key).'我是常量');   //md5+常量
$pwd = "just for test";
$data = '这是一个用来加密的数据Data';          //要加密的數據
$cipher = rc4($pwd, $data);         //AC4 加密算法
$c = rc4($pwd, $cipher);            //AC4 加密算法還原 (還原只需要重新加密一次)
echo '<pre>';
var_dump($key);
var_dump($pwd);
var_dump($data);
var_dump($cipher);
var_dump($c);
?>

*/

}} // namespace 

#endif