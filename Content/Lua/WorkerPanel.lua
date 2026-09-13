-- Thin, independent view adapter. Native services own authoritative assignment.
local Panel = {}
Panel.__index = Panel

function Panel.new(view, service)
    return setmetatable({view = view, service = service, epoch = 0}, Panel)
end

function Panel:close()
    self.epoch = self.epoch + 1
    if self.unsubscribe then self.unsubscribe(); self.unsubscribe = nil end
    self.worker = nil
    self.data = nil
    self.pending = false
end

function Panel:open(worker)
    self:close()
    self.worker = worker
    local epoch = self.epoch
    local function refresh()
        if self.epoch ~= epoch then return end
        local data = self.service:snapshot(worker)
        if not data then self.view:unavailable(); return end
        self.data = data
        self.view:render(data)
    end
    self.unsubscribe = self.service:subscribe(worker, refresh)
    refresh() -- Late opening must not depend on having observed an earlier event.
end

function Panel:assign()
    if not self.worker or self.pending then return false end
    -- Re-read: a screen's last render is not a permission check.
    local data = self.service:snapshot(self.worker)
    local reason
    if not data then reason = "worker.unavailable"
    elseif not data.unlocked then reason = "worker.unlock_required"
    elseif data.freeSlots <= 0 then reason = "worker.capacity_full"
    elseif data.requiredPower > data.availablePower then reason = "worker.power_limit" end
    if reason then self.view:feedback(reason); return false end

    self.pending = true
    local epoch, worker = self.epoch, self.worker
    self.service:requestAssignment(worker, function(accepted, serverReason)
        if self.epoch ~= epoch then return end
        self.pending = false
        -- Accepted is an acknowledgement, not invented production progress.
        self.view:feedback(accepted and "worker.request_accepted" or
            (serverReason or "worker.request_rejected"))
    end)
    return true
end

return Panel
