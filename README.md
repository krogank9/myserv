TODO
----

#### current:

* get client working to eventually become the client/bot testing lib

#### up next:

* add UDP + TCP support, each inherits a common network_connection class

	* add websocket support to TCP with websocketpp, peek first 5 bytes and transfer control to websocketpp if is http request

	* http://netty.io/3.9/xref/org/jboss/netty/example/portunification/PortUnificationServerHandler.html
	( banned initial MSG_IDs will be ASCII 'C'(67), 'D'(68), 'G'(71), 'H'(72), 'O'(79), 'P'(80), 'T'(84) )
	should be fine as long as HELLO_MSG is always forced as first message id sent. and if that is the case
	it would be much simpler probably to just check if first 2 bytes aren't the 0 expected from HELLO_MSG,
	and if so transfer to websocket. only issue i could foresee is the password system planned. and even
	that should be fine. so probably just going to check for HELLO_MSG

* template function binding system for registering messages & exposing entity functions to messages

#### eventual:

* decide what should be multithreaded, should property class be totally thread safe?

	* if it's done multi-threaded, my only current responsibility is thread-safe: property, map, and vector.

	* but also I don't want to add that yet, for now will just wrap all connections in strand because may
	end up being more work that it's worth with threaded access to stuff like space geometry/physics
