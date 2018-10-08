## What is this?
This is a library for Mbed OS designed to make it easy to send [MessagePack](https://msgpack.org/) data to a [FLuentD](https://www.fluentd.org/) ingest server. 

## Why would I use this? 
MessagePack encoding uses less space than JSON, this means less data used to transmit the same amount of information, which means power savings on radio transmission, money savings on data rates, and time savings on amount of time the MCU must be awake. 

## Is this an original work?
No, this library is based on [Mbed 2 library of Yuuichi Akagawa](https://os.mbed.com/users/YuuichiAkagawa/code/FluentLogger/). Minor changes were made to update the library to work with Mbed OS. 

## How do I use this?
See the example program [here](https://github.com/BlackstoneEngineering/mbed-os-example-fluentlogger)


```C
NetworkInterface net;	//Create network interface (wifi, ethernet, cellular, ...etc)

FluentLogger logger(&net,"192.168.x.x",24224); //Create logger object, specify the network interface, the IP of the FluentD server and the port number (default is 24224)

uMP mp(128);    	// Create Message body, 128bytes
mp.init();			// need to initialize every time
mp.start_array(6);	// declare array for # of elements. 
mp.set_str("hi",2);	// 1st element
mp.set_sint(1);		// 2nd element
mp.set_sint(-42);	// 3rd element 
mp.set_uint(128);	// 4th element
mp.set_uint(0xff);	// 5th element
mp.set_float(1.0/4);// 6th element


logger.log("debug.mbed",mp);// Send MessagePack data with tag 'debug.mbed'.
```

## FluentD Config example
Here is an example of a config file for a FluentD server. This specifies that any messagepack tagged `debug.<anything>` will be printed out on the terminal. Anything tagged `td.for_fluent.<anything>` will be forwarded onto TreasureData.

```

## built-in TCP input
## $ echo <json> | fluent-cat <tag>
<source>
  @type forward
  port 24225
  tag debug
</source>

## match tag=debug.** and dump to console
<match debug.**>
  @type stdout
  @id stdout_output
</match>

## Send to TD

<source>
  type forward
  port 24226
</source>

<match td.for_fluent.*>
  @type tdlog
  endpoint api.treasuredata.com
  apikey YOUR_API_KEY_HERE

  auto_create_table

  buffer_type file
  buffer_path /var/log/td-agent/buffer/td

  flush_interval 10
  use_ssl true
</match>
```