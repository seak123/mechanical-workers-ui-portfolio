-- Standalone showcase validator. It does not create or load Unreal assets.
local M = {}
local requiredStates = {"working", "carrying", "no_energy", "no_storage", "no_motivation", "overload"}
local requiredAbilities = {"receive", "work", "finish", "interrupt"}
local function present(value) return type(value) == "string" and value:match("%S") ~= nil end

function M.validate(profile)
    local errors = {}
    local function need(ok, message) if not ok then errors[#errors + 1] = message end end
    if type(profile) ~= "table" then return {"profile must be a table"} end
    for _, key in ipairs({"name", "actorTemplate", "actorBlueprint"}) do
        need(present(profile[key]), "missing " .. key)
    end
    local abilities = type(profile.abilities) == "table" and profile.abilities or {}
    for _, key in ipairs(requiredAbilities) do need(present(abilities[key]), "missing ability: " .. key) end
    local work = type(profile.work) == "table" and profile.work or {}
    local kinds = {gather = true, transport = true, craft = true, water = true, repair = true}
    need(type(work.kind) == "string" and kinds[work.kind], "unsupported work kind")
    need(present(work.targetType), "missing target type")
    need(type(work.timeout) == "number" and work.timeout > 0 and work.timeout < math.huge,
        "timeout must be positive and finite")
    local states, priorities = {}, {}
    local head = type(profile.head) == "table" and profile.head or {}
    for i, row in ipairs(head) do
        if type(row) ~= "table" then errors[#errors + 1] = "invalid head row " .. i
        else
            local validState = false
            for _, key in ipairs(requiredStates) do if row.state == key then validState = true end end
            need(validState, "unknown head state at " .. i)
            if validState then
                need(not states[row.state], "duplicate state: " .. row.state)
                states[row.state] = true
            end
            local validPriority = type(row.priority) == "number" and
                row.priority == math.floor(row.priority) and math.abs(row.priority) < math.huge
            need(validPriority, "invalid priority at " .. i)
            if validPriority then
                need(not priorities[row.priority], "ambiguous priority at " .. i)
                priorities[row.priority] = true
            end
            need(present(row.text), "missing text key at " .. i)
            if not row.suppress then
                need(present(row.icon), "missing icon at " .. i)
                need(present(row.animation), "missing animation at " .. i)
            end
        end
    end
    for _, state in ipairs(requiredStates) do need(states[state], "missing state: " .. state) end
    return errors
end

function M.plan(profile)
    local errors = M.validate(profile)
    if #errors > 0 then return nil, errors end
    local lines = {
        "DRY RUN - symbolic integration plan, no game assets created",
        "1. Create " .. profile.actorBlueprint .. " from " .. profile.actorTemplate,
        "2. Bind work receiver and navigation/ability adapter",
    }
    for _, key in ipairs(requiredAbilities) do lines[#lines + 1] = "   " .. key .. " -> " .. profile.abilities[key] end
    lines[#lines + 1] = "3. Configure " .. profile.work.kind .. " / target: " .. profile.work.targetType
    lines[#lines + 1] = "4. Configure overhead status channels:"
    for _, row in ipairs(profile.head) do
        lines[#lines + 1] = "   " .. row.state .. " -> " .. row.text ..
            " / " .. (row.suppress and "(suppressed)" or row.icon .. " / " .. row.animation)
    end
    lines[#lines + 1] = "5. Test blockers, target changes, interruption, reattachment and payload changes"
    return lines, {}
end
return M
