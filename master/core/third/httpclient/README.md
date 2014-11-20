httpclient
==========

a simple http client, in C++ 

1, 一共5个类，Url, TcpClient, HttpRequest, HttpResponse, HttpClient. 每个类的功能都很简单清晰。

2，HttpClient处理http事务的主要函数是execute。先查看请求是否合法，然后发送，然后读取并解析响应首行及头部。若有内容实体，则继续读入。
   并处理了chunk数据块，gzip压缩数据，以及重定向。各个步骤若有错误，则execute函数返回错误。
   
3，tcp模块中使用了非阻塞connect方式，参考的是《UNIX网络编程》16.3节。先把sockfd设置为非阻塞，然后调用connect和select，连接成功后再
   把sockfd设置回来。这么做的主要目的是通过select为连接操作设置一个超时值。此外，对sockfd的读写操作也设置了超时值。
   
4，没有单独处理cookie，但是作为header的一部分，读取和添加cookie也是比较容易的。

5，对http协议的处理不是特别详尽，只能支持重定向，内容压缩，传输编码等。整个程序应该也有不少bug，水平有限，仅为学习。