local skynet = require "skynet"
local netpack = require "skynet.netpack"
local socketdriver = require "skynet.socketdriver"

-- Self contained test for skynet.netpack ( lualib-src/lua-netpack.c )
-- A gate-like service receives data through netpack.filter, and an
-- embedded client service ( skynet.socket API ) plays the peer role.

local queue
local events = {}

local function record(typ, ...)
	events[#events+1] = { type = typ, n = select('#', ...), ... }
end

local function wait_event(typ, count, timeout)
	-- wait until `count` events of type typ arrived (default 1)
	count = count or 1
	timeout = timeout or 100	-- 1s
	local tick = 0
	while tick < timeout do
		local c = 0
		for _, e in ipairs(events) do
			if e.type == typ then
				c = c + 1
			end
		end
		if c >= count then
			return true
		end
		skynet.sleep(1)
		tick = tick + 1
	end
	return false, "timeout waiting event " .. typ
end

local function last_data()
	for i = #events, 1, -1 do
		local e = events[i]
		if e.type == "data" then
			return e[1]
		end
	end
end

local MSG = {}

function MSG.data(fd, msg, sz)
	-- netpack.tostring converts (and frees) the message buffer
	record("data", netpack.tostring(msg, sz), fd)
end

function MSG.more(fd)
	local n = 0
	for fd, msg, sz in netpack.pop, queue do
		record("data", netpack.tostring(msg, sz), fd)
		n = n + 1
	end
	record("more", n, fd)
end

function MSG.open(fd, addr)
	record("open", addr, fd)
	socketdriver.start(fd)
end

function MSG.close(fd)
	record("close", fd)
end

function MSG.error(fd, msg)
	record("error", msg, fd)
	socketdriver.shutdown(fd)
end

function MSG.warning(fd, size)
	record("warning", size, fd)
end

function MSG.init(id, addr, port)
	record("init", addr, port, id)
end

skynet.register_protocol {
	name = "socket",
	id = skynet.PTYPE_SOCKET,
	unpack = function(msg, sz)
		return netpack.filter(queue, msg, sz)
	end,
	dispatch = function(_, _, q, type, ...)
		queue = q
		if type then
			MSG[type](...)
		end
	end
}

local function u16(n)
	return string.char(math.floor(n / 256) % 256, n % 256)
end

-- test pack / tostring / clear (pure part) ------------------------------

local function test_pack()
	local ptr, sz = netpack.pack("hello")
	assert(type(ptr) == "userdata", "pack returns lightuserdata")
	assert(sz == 7, "pack adds 2 bytes header")
	-- netpack.tostring converts the buffer to a lua string and frees it
	assert(netpack.tostring(ptr, sz) == u16(5) .. "hello")
	assert(netpack.tostring(nil, 0) == "", "tostring(nil) is empty string")

	-- pack a lightuserdata ( buffer, size ) pair ; use a fresh buffer
	local ptr1, sz1 = netpack.pack("world")
	local ptr2, sz2 = netpack.pack(ptr1, sz1)
	assert(sz2 == 9)
	assert(netpack.tostring(ptr2, sz2) == u16(7) .. u16(5) .. "world")

	-- empty string
	local ptr3, sz3 = netpack.pack("")
	assert(netpack.tostring(ptr3, sz3) == u16(0))

	-- pack raises on message >= 64K
	local ok, err = pcall(netpack.pack, string.rep("x", 0x10000))
	assert(not ok, "oversize pack should fail")

	-- pop / clear on an empty (nil) queue
	assert(netpack.pop(nil) == nil, "pop empty queue returns nothing")
	assert(netpack.clear(nil) == nil, "clear nil queue is a no-op")

	print("[netpack] pack/tostring ok")
end

-- the embedded peer service ---------------------------------------------

-- NOTE: this function must have only one upvalue (_ENV)
local function client_main()
	local skynet = require "skynet"
	local socket = require "skynet.socket"
	local fd

	local CMD = {}

	function CMD.connect(host, port)
		fd = assert(socket.open(host, port))
		return fd
	end

	function CMD.send(data)
		socket.write(fd, data)
	end

	function CMD.close()
		socket.close(fd)
		fd = nil
	end

	function CMD.write_raw(data)
		-- same as send, named differently for readability
		socket.write(fd, data)
	end

	skynet.dispatch("lua", function(_, _, cmd, ...)
		local f = assert(CMD[cmd])
		skynet.ret(skynet.pack(f(...)))
	end)
end

local client_addr

local function call_client(cmd, ...)
	return skynet.call(client_addr, "lua", cmd, ...)
end

-- main -------------------------------------------------------------------

skynet.start(function()
	test_pack()

	-- 1. listen, the first init event of the listen fd carries the
	--    real bind address/port ( the second one answers `start` )
	local listen_fd = socketdriver.listen("127.0.0.1", 0)
	socketdriver.start(listen_fd)
	assert(wait_event("init", 1), "no init event for listen fd")
	local port
	for _, e in ipairs(events) do
		if e.type == "init" and e[1] == "127.0.0.1" and e[2] > 0 then
			port = e[2]
		end
	end
	port = assert(port, "init event carries port")

	-- 2. launch the embedded client service
	local service = require "skynet.service"
	client_addr = service.new("netpack_client", client_main)
	call_client("connect", "127.0.0.1", port)
	assert(wait_event("open", 1), "no open event on connect")

	-- 3. one exact package -> data
	call_client("send", u16(5) .. "hello")
	assert(wait_event("data", 1))
	assert(last_data() == "hello", "exact package")

	-- 4. two packages in a single write -> data + more + pop
	events = {}
	call_client("send", u16(3) .. "abc" .. u16(3) .. "def")
	assert(wait_event("data", 2), "sticky packages")
	assert(events[1][1] == "abc" and events[2][1] == "def", "sticky content")

	-- 5. one byte header first ( read == -1 branch ), then the rest
	events = {}
	call_client("send", u16(8):sub(1,1))
	skynet.sleep(10)
	call_client("send", u16(8):sub(2,2) .. "one")
	skynet.sleep(10)
	call_client("send", "byte!")
	assert(wait_event("data", 1), "half package with split header")
	assert(last_data() == "onebyte!", "split header content")

	-- 6. split inside the body ( read >= 0, size < need ), fill exactly
	events = {}
	call_client("send", u16(6) .. "hea")
	skynet.sleep(10)
	call_client("send", "der")
	assert(wait_event("data", 1), "half body package")
	assert(last_data() == "header", "half body content")

	-- 7. fill more than needed in the last chunk ( size > need )
	events = {}
	call_client("send", u16(3) .. "xy")
	skynet.sleep(10)
	call_client("send", "z" .. u16(2) .. "ok")
	assert(wait_event("data", 2), "overfilled package")
	assert(events[1][1] == "xyz" and events[2][1] == "ok", "overfilled content")

	-- 8. three packages in one write, then a huge sticky burst to
	--    force the internal ring queue to expand ( > 1024 packages )
	events = {}
	local burst = {}
	for i = 1, 1030 do
		burst[#burst+1] = u16(4) .. string.format("p%03d", i)
	end
	local all = table.concat(burst) call_client("send", all)
	local dn=0 skynet.sleep(300) for _,e in ipairs(events) do if e.type=="data" then dn=dn+1 end end print("DBG data events=", dn, "total events=", #events)
	local okcount = 0
	for _, e in ipairs(events) do
		if e.type == "data" and e[1]:match("^p%d+$") then
			okcount = okcount + 1
		end
	end
	print("DBG okcount=", okcount, "events=", #events) assert(okcount == 1030, "burst content count " .. okcount)

	-- 9. close with an unfinished package pending ( close_uncomplete )
	events = {}
	call_client("send", u16(100) .. "partial")
	skynet.sleep(10)
	call_client("close")
	assert(wait_event("close", 1), "close event")
	for _, e in ipairs(events) do
		assert(e.type ~= "data", "no data after close")
	end

	-- 10. clear the queue explicitly while it holds nothing
	netpack.clear(queue)

	print("[netpack] all filter cases ok")
	skynet.exit()
end)
