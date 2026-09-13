package.path = "./Content/Lua/?.lua;" .. package.path
local Panel, Profile = require("WorkerPanel"), require("AuthoringProfile")
local count = 0
local function check(ok, name) assert(ok, name); count = count + 1; print("PASS " .. name) end
local function fixture() return dofile("Content/WorkerProfiles/gatherer.lua") end
check(#Profile.validate(fixture()) == 0, "valid authoring profile")
local p = fixture(); p.actorBlueprint = ""; check(#Profile.validate(p) > 0, "missing Actor BP")
p = fixture(); p.abilities.interrupt = nil; check(#Profile.validate(p) > 0, "missing interrupt GA")
p = fixture(); p.head[2].priority = p.head[1].priority; check(#Profile.validate(p) > 0, "conflicting priorities")
p = fixture(); p.head[1].animation = ""; check(#Profile.validate(p) > 0, "missing animation")
p = fixture(); p.work.timeout = 0/0; check(#Profile.validate(p) > 0, "NaN timeout")
p = fixture(); p.work.timeout = math.huge; check(#Profile.validate(p) > 0, "infinite timeout")
p = fixture(); table.remove(p.head); check(#Profile.validate(p) > 0, "incomplete state coverage")
check(#Profile.validate(false) > 0, "invalid root type")
p = fixture(); p.head[1].state = "invented"; check(#Profile.validate(p) > 0, "unknown head state")
p = fixture(); p.head[1].suppress = true; p.head[1].icon = nil; p.head[1].animation = nil
check(#Profile.validate(p) == 0, "intentional bubble suppression")
local plan = Profile.plan(fixture()); check(plan and plan[1]:find("DRY RUN"), "explicitly labelled integration plan")

local view = {renders = 0}
function view:render(data) self.renders = self.renders + 1; self.data = data end
function view:feedback(reason) self.reason = reason end
function view:unavailable() self.missing = true end
local service = {requests = 0, unsubscribed = 0,
    data = {unlocked = true, freeSlots = 1, requiredPower = 5, availablePower = 6}}
function service:snapshot() return self.data end
function service:subscribe(_, callback)
    self.event = callback
    return function() self.unsubscribed = self.unsubscribed + 1 end
end
function service:requestAssignment(worker, callback)
    self.requests = self.requests + 1; self.requestWorker = worker; self.reply = callback
end
local ui = Panel.new(view, service)
check(not ui:assign(), "closed panel cannot assign")
ui:open(7); check(view.renders == 1, "open renders current state")
service.data.unlocked = false; check(not ui:assign() and view.reason == "worker.unlock_required", "unlock feedback")
service.data.unlocked = true; service.data.freeSlots = 0
check(not ui:assign() and view.reason == "worker.capacity_full", "capacity feedback")
service.data.freeSlots = 1; service.data.availablePower = 3
check(not ui:assign() and view.reason == "worker.power_limit", "power feedback")
service.data.availablePower = 6; check(ui:assign(), "valid request forwarded")
check(not ui:assign() and service.requests == 1, "duplicate pending click ignored")
service.reply(false, "worker.server_capacity_changed")
check(view.reason == "worker.server_capacity_changed", "authoritative rejection shown")
check(ui:assign(), "rejection releases pending state")
local staleReply, staleEvent = service.reply, service.event
ui:open(8); local renders, reason = view.renders, view.reason
staleReply(true); staleEvent()
check(view.renders == renders and view.reason == reason, "old callbacks ignored after rebind")
check(service.unsubscribed == 1, "old subscription released")
service.data = nil
check(not ui:assign() and view.reason == "worker.unavailable", "disappeared worker cannot assign")
ui:close(); check(service.unsubscribed == 2, "close releases subscription")
print(count .. " Lua checks passed")
