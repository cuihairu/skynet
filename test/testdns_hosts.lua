-- Unit test for skynet.dns: parse_hosts must close the file handle.
--
-- parse_hosts() previously returned without calling f:close() on the normal
-- path, leaking the io.open handle.  This test loads dns.lua with stubbed
-- skynet/socket modules, reaches parse_hosts through the module upvalues, and
-- verifies exactly one open and one close happen.
package.path = "./lualib/?.lua;" .. package.path

local skynet = {}
skynet.error = function() end
skynet.fork = function() end
skynet.now = function() return 0 end
skynet.sleep = function() end
skynet.timeout = function() end
skynet.wait = function() end
skynet.wakeup = function() end
local socket = {}
socket.close = function() end
socket.udp = function() end
socket.udp_connect = function() end
socket.write = function() end
package.preload["skynet"] = function() return skynet end
package.preload["skynet.socket"] = function() return socket end

local dns = require "skynet.dns"

local function findupv(fn, name)
	for i = 1, 30 do
		local n, v = debug.getupvalue(fn, i)
		if not n then break end
		if n == name then return v end
	end
end

local lr = findupv(dns.resolve, "local_resolve")
local ph = findupv(lr, "parse_hosts")
assert(type(ph) == "function", "parse_hosts not reachable")

local realopen = io.open
local opencount, closedcount = 0, 0
io.open = function(path, mode)
	local f = realopen(path, mode)
	if f then
		opencount = opencount + 1
		local proxy = {}
		proxy.lines = function() return f:lines() end
		proxy.close = function(self)
			closedcount = closedcount + 1
			return f:close()
		end
		return proxy
	end
	return f
end

local rts = ph()
io.open = realopen

assert(type(rts) == "table", "parse_hosts should return a table")
assert(opencount == 1, "expected one io.open")
assert(closedcount == 1, "parse_hosts must close its handle")

print("dns: parse_hosts closes handle")
