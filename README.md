TODO
----

* decide what should be multithreaded, should property class be totally thread safe?

	* if it's done multi-threaded, my only current responsibility is thread-safe: property, map, and vector.

	* but also I don't want to add that yet, for now will just wrap all connections in strand because may
	end up being more work that it's worth with threaded access to stuff like space geometry/physics

* add UDP + TCP support, each inherits a common network_connection class

	* add websocket support to TCP with websocketpp, check first 5 bytes and transfer control to websocketpp if is http request
