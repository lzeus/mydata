## 基于java原生NIO的echo服务

```java
import com.lzeus.common.Logger;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.Set;

public class EchoNioServer {

    private static Logger logger = Logger.getLogger();
    /**
     * 初始化ServerSocketChannel
     * 注册其相关操作到seletor
     * @param selector
     * @param port
     */
    public static void initAndRegisterServer(Selector selector,int port){
        try{
            ServerSocketChannel serverChannel = ServerSocketChannel.open();
            serverChannel.configureBlocking(false);
            ServerSocket serverSocket = serverChannel.socket();
            serverSocket.bind(new InetSocketAddress(port));

            serverChannel.register(selector, SelectionKey.OP_ACCEPT);
        }catch (IOException e){
            logger.error("初始化在端口["+port+"]上的监听服务出错",e);
        }
    }
    /**
     * accept客户端求
     * 初始化客户端channel
     * 注册其相关操作到selector
     * @param selector
     * @param selectionKey
     */
    public static void acceptAndRegisterClient(Selector selector,SelectionKey selectionKey){
        if(!selectionKey.isValid() || !selectionKey.isAcceptable()){
            return;
        }

        ServerSocketChannel serverChannel = (ServerSocketChannel)selectionKey.channel();
        try{
            SocketChannel clientChannel = serverChannel.accept();
            clientChannel.configureBlocking(false);
            clientChannel.register(selector,SelectionKey.OP_READ | SelectionKey.OP_WRITE, ByteBuffer.allocate(1024));
        }catch (IOException e){
            logger.error("监听服务["+serverChannel.socket()+"]accept出错",e);
        }
    }
    /**
     * 客户端读
     * @param selectionKey
     */
    public static void clientRead(SelectionKey selectionKey){

        if(!selectionKey.isValid() || !selectionKey.isReadable()){
            return;
        }

        SocketChannel clientChannel = (SocketChannel)selectionKey.channel();
        ByteBuffer buffer = (ByteBuffer)selectionKey.attachment();
        if(buffer != null){
            try{
                clientChannel.read(buffer);
            }catch (IOException e){
                logger.error("从client["+clientChannel.socket()+"]读取数据出错",e);

                closeSocketChannel(clientChannel);
            }
        }
    }
    /**
     * 客户端写
     * @param selectionKey
     */
    public static void clientWrite(SelectionKey selectionKey){
        if(!selectionKey.isValid() || !selectionKey.isWritable()){
            return;
        }

        SocketChannel clientChannel = (SocketChannel)selectionKey.channel();
        ByteBuffer buffer = (ByteBuffer)selectionKey.attachment();
        if(buffer != null){
            buffer.flip();
            try{
                clientChannel.write(buffer);
            }catch (IOException e){
                logger.error("从client["+clientChannel.socket()+"]读取数据出错",e);

                closeSocketChannel(clientChannel);
            }
            buffer.compact();
        }
    }

    static void closeSocketChannel(SocketChannel socketChannel){
        if(socketChannel != null){
            try{
                socketChannel.close();
            }catch (IOException e){

            }
        }
    }

    static void closeSelector(Selector selector){
        if(selector != null){
            try{
                selector.close();
            }catch (IOException e){

            }
        }
    }
    /**
     * 
     * 打开 nio selector
     * @return
     */
    static Selector openSelector(){
        Selector selector = null;
        try{
            selector = Selector.open();
        }catch (IOException e){
            logger.error("新建selector出错",e);
            System.exit(1);
        }
        return selector;
    }

    static void doSelect(Selector selector){
        while(true){
            try{
                selector.select();
            }catch (IOException e){
                logger.error("selector出错",e);
                closeSelector(selector);
                return;
            }
            Set<SelectionKey> selectionKeys = selector.selectedKeys();
            Iterator<SelectionKey> keyIterator = selectionKeys.iterator();

            while(keyIterator.hasNext()){
                try{
                    SelectionKey selectionKey = keyIterator.next();
                    acceptAndRegisterClient(selector,selectionKey);
                    clientRead(selectionKey);
                    clientWrite(selectionKey);
                }finally {
                    keyIterator.remove();
                }
            }
        }
    }

    public static void serve(int port){
        Selector selector = openSelector();
        initAndRegisterServer(selector,port);
        doSelect(selector);
    }


    public static void main(String args[]){
        serve(1111);
    }

}
```

- ServerSocketChannel 和 SocketChannel

  java.nio.channels.ServerSocketChannel和java.nio.channels.SocketChannel分别代表服务socket通道和socket通道，他们继承了公共的父类AbstractSelectableChannel，关系如下：

  ![SelectorChannel继承关系](https://github.com/lzeus/mydata/raw/master/opensource/netty/SelectableChannel.png)


  java.nio.channels.spi.AbstractSelectableChannel实现了java.nio.channels.SelectableChannel接口，SelectableChannel的具体实现类代表的是一种可注册Selector选择器的通道。
  
  注册的功能在register方法中实现，jdk源码如下：
  
  ```java
  /**
     * Registers this channel with the given selector, returning a selection key.
     *
     * <p>  This method first verifies that this channel is open and that the
     * given initial interest set is valid.
     *
     * <p> If this channel is already registered with the given selector then
     * the selection key representing that registration is returned after
     * setting its interest set to the given value.
     *
     * <p> Otherwise this channel has not yet been registered with the given
     * selector, so the {@link AbstractSelector#register register} method of
     * the selector is invoked while holding the appropriate locks.  The
     * resulting key is added to this channel's key set before being returned.
     * </p>
     * 
     * @throws  ClosedSelectorException {@inheritDoc}
     *
     * @throws  IllegalBlockingModeException {@inheritDoc}
     *
     * @throws  IllegalSelectorException {@inheritDoc}
     *
     * @throws  CancelledKeyException {@inheritDoc}
     *
     * @throws  IllegalArgumentException {@inheritDoc}
     */
    public final SelectionKey register(Selector sel, int ops,
                                       Object att)
        throws ClosedChannelException
    {
        synchronized (regLock) {
            if (!isOpen())
                throw new ClosedChannelException();
            if ((ops & ~validOps()) != 0)
                throw new IllegalArgumentException();
            if (blocking)
                throw new IllegalBlockingModeException();
            SelectionKey k = findKey(sel);
            //已注册过的SelectaleChannel
            if (k != null) {
                //更新感兴趣的操作集位
                k.interestOps(ops);
                //更新附属对象
                k.attach(att);
            }
            //新注册的SelectaleChannel
            if (k == null) {
                // New registration
                synchronized (keyLock) {
                    if (!isOpen())
                        throw new ClosedChannelException();
                    //调用的AbstractSelector的register
                    k = ((AbstractSelector)sel).register(this, ops, att);
                    addKey(k);
                }
            }
            return k;
        }
    }
  ```
     
  从上面的实现可以看到，SelectableChannel的register方法实际是调用的第一个参数Selector的具体实现类的register方法。

- Selector

  java.nio.channels.Selector用于多通道的I/O多路复用，在通道注册时，作为SelectableChannel的register方法的第一个参数传入方法栈中。
  
  调用Selector的select方法，就可以进行注册好的SelectableChannel的感兴趣的操作集的检查逻辑。select方法的签名如下

  `public int select()`
  
   一直阻塞，直到有注册的SelectableChannel的感兴趣的操作集可以进行的操作，返回的结果表示可以操作的SelectableChannel的个数(可能为0)。
  
  `public int select(long timeout)`
  
  一直阻塞，直到有注册的SelectableChannel的感兴趣的操作集可以进行的操作，或者参数`timeout`指定的时间已到，返回的结果表示可以操作的SelectableChannel的个数(可能为0)。

  `public int selectNow()`
  
  不阻塞立即返回，返回的结果表示当前可以操作的SelectableChannel的个数(可能为0)。

- SelectionKey
  
  java.nio.channels.SelectableChannel代表一个注册的SelectableChannel。
  
  
  `public abstract int interestOps()`
  
  返回SelectionKey实例代表的注册的SelectableChannel感兴趣的操作集。
  
  `public abstract SelectionKey interestOps(int ops)`
  
  设置SelectionKey实例代表的注册的SelectableChannel感兴趣的操作集，SelectableChannel的注册方法进行注册的时候，内部调用的就是该方法进行`interestOps`的设置。
  
  
  `SelectionKey.OP_ACCEPT`
  
  ServerSocketChannel关心的可accept操作的位表示。
  
  `SelectionKey.OP_READ`
  
  SocketChannel关心的可读操作的位表示。
  
  `SelectionKey.OP_WRIT`
  
  SocketChannel关心的可写操作的位表示。
  
  `SelectionKey.OP_CONNECT`
  
  SocketChannel关心的可连接操作的位表示。


## 基于netty的NIO实现的echo服务
### 示例代码
```java
import com.lzeus.common.Logger;
import io.netty.bootstrap.ServerBootstrap;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioServerSocketChannel;

import java.net.InetSocketAddress;

public class EchoNettyServer {

    private static Logger logger = Logger.getLogger();

    public static void serve(int port) throws Exception{
        EventLoopGroup group = new NioEventLoopGroup();
        ServerBootstrap bootstrap = new ServerBootstrap();
        bootstrap
                .group(group)
                .channel(NioServerSocketChannel.class)
                .localAddress(new InetSocketAddress(port))
                .childHandler(new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(final SocketChannel socketChannel) throws Exception {
                        socketChannel.pipeline().addLast(new ChannelInboundHandlerAdapter(){
                            @Override
                            public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
                                if(msg instanceof ByteBuf && msg != null){
                                    String s = ButeBufUtil.stringfy((ByteBuf)(msg));
                                    logger.info("服务器收到("+s.length()+"):["+s+"]");
                                }
                                ctx.write(msg);
                            }

                            @Override
                            public void channelReadComplete(ChannelHandlerContext ctx) throws Exception {
                                ctx
                                        .writeAndFlush(Unpooled.EMPTY_BUFFER);
                                        //.addListener(ChannelFutureListener.CLOSE);
                            }

                            @Override
                            public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {

                            }
                        });
                    }
                });

        ChannelFuture future = bootstrap.bind().sync();

        future.channel().closeFuture().sync();
    }

    public static void main(String args[]) throws Exception{
        serve(1111);
    }
}
```