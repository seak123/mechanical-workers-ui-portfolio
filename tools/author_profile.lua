-- Run at repository root: lua tools/author_profile.lua Content/WorkerProfiles/gatherer.lua
package.path = "./Content/Lua/?.lua;" .. package.path
local Profile = require("AuthoringProfile")
-- Profiles are trusted local Lua, not a sandboxed interchange format.
local ok, value = pcall(dofile, arg[1] or "Content/WorkerProfiles/gatherer.lua")
if not ok then io.stderr:write("Cannot load profile: " .. tostring(value) .. "\n"); os.exit(1) end
local lines, errors = Profile.plan(value)
if not lines then
    for _, message in ipairs(errors) do io.stderr:write(message .. "\n") end
    os.exit(1)
end
for _, line in ipairs(lines) do print(line) end
