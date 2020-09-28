# sftt
Simple and Security File Transport Tool


传输过程：

    1、客户端和服务端相继启动，根据当前时间hash计算出服务端监听的随机端口，计算过程：
        1）port = hash(time)，并且满足：当time2 - time1 < x 秒时，hash(y) 恒等于z，其中 y 属于区间[time1, time2]，支持x可配置。 
      2）当客户端和服务端在相对比较短的时间内同时启动时，服务端和客户端计算出来的端口值一致，服务端用这个端口启动服务，客户端向服务端的这个端口建立连接。
    2、客户端根据命令行参数对文件内容进行读取、加密、传输。
    3、服务端收到文件内容后，对内容进行解密、写入文件。    
    
缺点：

    1、每次传文件的时候，都要重新启动服务端，以便更换端口，比较适合手动传文件的场景。   
    2、客户端可能因为启动时间差过长，导致计算出的端口与服务端不一致，无法连接，此时需要重启服务端。
