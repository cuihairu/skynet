local skynet = require "skynet"
local manager = require "skynet.manager"	-- import skynet.abort , skynet.kill

-- Coverage/test runner helper.
-- Launch the target test service, wait until it exits (or force abort
-- after a timeout), then abort the whole node so the process exits
-- normally and coverage data (.gcda) is flushed to disk.

local testname = skynet.getenv "autotest_name"
local timeout = tonumber(skynet.getenv "autotest_timeout" or 60)

skynet.start(function()
	if not testname then
		print("Autotest: no autotest_name in config")
		skynet.abort()
		return
	end
	skynet.fork(function()
		local ok, handle = pcall(skynet.newservice, testname)
		if not ok then
			print("Autotest: failed to launch", testname, handle)
			skynet.abort()
			return
		end
		-- the call below returns (with error) as soon as the test
		-- service dies, no matter whether it answered anything.
		pcall(skynet.call, handle, "lua", "__autotest_wait__")
		print("Autotest: finished", testname)
		skynet.abort()
	end)
	skynet.timeout(timeout * 100, function()
		print("Autotest: timeout, force abort", testname)
		skynet.abort()
	end)
end)
