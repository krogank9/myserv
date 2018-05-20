TODO
----

* decide what should be multithreaded, should property class be totally thread safe?

* add UDP + TCP support, generalize to one network_connection class

	* add websocket support to TCP with websocketpp, check first 5 bytes and transfer control to websocketpp if is http request
