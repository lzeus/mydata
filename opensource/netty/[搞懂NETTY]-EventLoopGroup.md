## AbstractBootstrap.initAndRegister()
基于netty实现的网络客户端和服务端分别使用Bootstrap和ServerBootstrap进行端口 IP 线程池模型等配置工作，配置完成后，分别调用

客户端使用

`public ChannelFuture connect()`

进行连接操作。

服务端使用

`public ChannelFuture bind()`

进行监听操作。

Bootstrap和ServerBootstrap继承了公共的父类io.netty.bootstrap.AbstractBootstrap，继承关系如下:

  ![Bootstrap继承关系](https://github.com/lzeus/mydata/raw/master/opensource/netty/Bootstrap.png)
  
客户端的connect方法和服务端的bind方法都调用了AbstractBootstrap的initAndRegister方法，其源码如下：

```java

final ChannelFuture initAndRegister() {
		  //1.实例化Channel实现类
        final Channel channel = channelFactory().newChannel();
        try {
        //2.初始化Channel
        //Bootstrap和ServerBootStrap分别Override该init方法
            init(channel);
        } catch (Throwable t) {
            channel.unsafe().closeForcibly();
            return channel.newFailedFuture(t);
        }

        ChannelPromise regPromise = channel.newPromise();
        //3.调用EventLoopGroup的register方法进行注册
        group().register(channel, regPromise);
        if (regPromise.cause() != null) {
            if (channel.isRegistered()) {
                channel.close();
            } else {
                channel.unsafe().closeForcibly();
            }
        }

        // If we are here and the promise is not failed, it's one of the following cases:
        // 1) If we attempted registration from the event loop, the registration has been completed at this point.
        //    i.e. It's safe to attempt bind() or connect() now beause the channel has been registered.
        // 2) If we attempted registration from the other thread, the registration request has been successfully
        //    added to the event loop's task queue for later execution.
        //    i.e. It's safe to attempt bind() or connect() now:
        //         because bind() or connect() will be executed *after* the scheduled registration task is executed
        //         because register(), bind(), and connect() are all bound to the same thread.

        return regPromise;
    }

```

对应的调用时序图如下:

![NioSocketChannel_init_register](https://github.com/lzeus/mydata/raw/master/opensource/netty/AbstractBootstrap_init_register.png)

下面以NIO Bootstrap 和 ServerBootstrap为例，按照initAndRegister的三个步骤，分别看下该方法具体的调用流程。

- 实例化Channel实现类

  Bootstrap和ServerBootstrap在配置阶段，都会调用静态channel方法
  
  ```java
  public B channel(Class<? extends C> channelClass) {
        if (channelClass == null) {
            throw new NullPointerException("channelClass");
        }
        return channelFactory(new BootstrapChannelFactory<C>(channelClass));
    }
  ```
  channelFactory().newChannel()会调用NioSocketChannel或NioServerSocketChannel的默认构造器，实例化Channel对象
  
- 初始化Channel

  Bootstrap的init方法如下
    
  ```java
  void init(Channel channel) throws Exception {
        ChannelPipeline p = channel.pipeline();
        //bootstrap.handler(new MyHandler(){});
        p.addLast(handler());

        final Map<ChannelOption<?>, Object> options = options();
        synchronized (options) {
            for (Entry<ChannelOption<?>, Object> e: options.entrySet()) {
                try {
                    if (!channel.config().setOption((ChannelOption<Object>) e.getKey(), e.getValue())) {
                        logger.warn("Unknown channel option: " + e);
                    }
                } catch (Throwable t) {
                    logger.warn("Failed to set a channel option: " + channel, t);
                }
            }
        }

        final Map<AttributeKey<?>, Object> attrs = attrs();
        synchronized (attrs) {
            for (Entry<AttributeKey<?>, Object> e: attrs.entrySet()) {
                channel.attr((AttributeKey<Object>) e.getKey()).set(e.getValue());
            }
        }
    }
  ```
    
  该方法的主要逻辑是把配置阶段handler方法设置的ChannelHandler对象添加到ChannelPipeline的最后。
  
  ServerBootstrap的init方法如下
  
  ```java
  @Override
    void init(Channel channel) throws Exception {
        final Map<ChannelOption<?>, Object> options = options();
        synchronized (options) {
            channel.config().setOptions(options);
        }

        final Map<AttributeKey<?>, Object> attrs = attrs();
        synchronized (attrs) {
            for (Entry<AttributeKey<?>, Object> e: attrs.entrySet()) {
                @SuppressWarnings("unchecked")
                AttributeKey<Object> key = (AttributeKey<Object>) e.getKey();
                channel.attr(key).set(e.getValue());
            }
        }

        ChannelPipeline p = channel.pipeline();
        if (handler() != null) {
            p.addLast(handler());
        }

        final EventLoopGroup currentChildGroup = childGroup;
        final ChannelHandler currentChildHandler = childHandler;
        final Entry<ChannelOption<?>, Object>[] currentChildOptions;
        final Entry<AttributeKey<?>, Object>[] currentChildAttrs;
        synchronized (childOptions) {
            currentChildOptions = childOptions.entrySet().toArray(newOptionArray(childOptions.size()));
        }
        synchronized (childAttrs) {
            currentChildAttrs = childAttrs.entrySet().toArray(newAttrArray(childAttrs.size()));
        }

        p.addLast(new ChannelInitializer<Channel>() {
            @Override
            public void initChannel(Channel ch) throws Exception {
                ch.pipeline().addLast(new ServerBootstrapAcceptor(
                        currentChildGroup, currentChildHandler, currentChildOptions, currentChildAttrs));
            }
        });
    }
  ```
    
- 调用EventLoopGroup的register方法进行注册

  以io.netty.channel.nio.NioEventLoopGroup为例，该对象的register方法实际是调用的io.netty.channel.nio.NioEventLoop的register方法
  
  io.netty.channel.nio.NioEventLoopGroup和io.netty.channel.nio.NioEventLoop的继承关系如下：
  
  ![EventLoopGroup关系](https://github.com/lzeus/mydata/raw/master/opensource/netty/EventLoop继承关系.png)

#### io.netty.util.concurrent
- EventExecutorGroup
  - 继承了ScheduledExecutorService接口
     - 使得实现类具备的线程组的能力
  - 定义了EventExecutor next()接口
     - 使得实现类具备了返回EventExecutor实例的能力
- EventExecutor
   -  EventExecutorGroup parent()
     - 获取当前EventExecutor实例所属的EventExecutorGroup
   - boolean inEventLoop()
     - 判断调用该方法的执行流线程是否是事件循环线程
   - boolean inEventLoop(Thread thread)
     - 判断参数thread表示的线程是否是事件循环线程

#### io.netty.channel
- EventLoop
  - EventLoopGroup parent()
     - 返回该对象所属的EventLoopGroup实例
- EventLoopGroup
  - ChannelFuture register(Channel channel)
     - 注册channel到当前EventLoopGroup
     - 内部实现会调用next()返回一个EventLoop，并调用该EventLoop实例的register方法
  - ChannelFuture register(Channel channel, ChannelPromise promise)

io.netty.channel.nio.NioEventLoopGroup的register方法源码如下：
  
```java
public ChannelFuture register(Channel channel, ChannelPromise promise) {
       return next().register(channel, promise);
}
```

register方法调用了继承自io.netty.util.concurrent.MultithreadEventExecutorGroup的next方法，源码如下

```java
@Override
public EventExecutor next() {
    return children[Math.abs(childIndex.getAndIncrement() % children.length)];
}
```
children是io.netty.channel.nio.NioEventLoop对象数组，该方法会按照数组中的元素顺序依次从第一个开始取元素作为返回结果，当取到数组最后一个元素时，下一次调用将再次取数组中第一个元素作返回结果。

返回的io.netty.channel.nio.NioEventLoop的register方法继承自io.netty.channel.SingleThreadEventLoop，源码如下:

```java
@Override
    public ChannelFuture register(final Channel channel, final ChannelPromise promise) {
        if (channel == null) {
            throw new NullPointerException("channel");
        }
        if (promise == null) {
            throw new NullPointerException("promise");
        }

        channel.unsafe().register(this, promise);
        return promise;
    }
```

该方法内部调用的是第一个参数channel的unsafe方法返回的对象的register方法，该方法继承自AbstractUnsafe，源码如下:

```java
@Override
public final void register(EventLoop eventLoop, final ChannelPromise promise) {
    if (eventLoop == null) {
        throw new NullPointerException("eventLoop");
    }
    if (isRegistered()) {
        promise.setFailure(new IllegalStateException("registered to an event loop already"));
        return;
    }
    if (!isCompatible(eventLoop)) {
        promise.setFailure(new IllegalStateException("incompatible event loop type: " + eventLoop.getClass().getName()));
        return;
    }
    //每个channel的生命周期只会绑定一个eventLoop
    //但是一个eventLoop可能会被多个channel绑定
    AbstractChannel.this.eventLoop = eventLoop;
	 //判断当前执行流是否是eventLoop的线程
	 //确保注册方法register0在eventLoop的线程执行流中被执行
    if (eventLoop.inEventLoop()) {
        register0(promise);
    } else {
        try {
            eventLoop.execute(new Runnable() {
                @Override
                public void run() {
                    register0(promise);
                }
            });
        } catch (Throwable t) {
            logger.warn("Force-closing a channel whose registration task was unaccepted by an event loop: {}",AbstractChannel.this, t);
            closeForcibly();
            promise.setFailure(t);
        }
    }
}
```
