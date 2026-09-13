-- Symbolic showcase asset names, not original game paths or Unreal assets.
return {
    name = "MeadowGatherer",
    actorTemplate = "WorkerCreatureBase",
    actorBlueprint = "BP_MeadowGatherer",
    abilities = {
        receive = "GA_WorkReceived", work = "GA_GatherLoop",
        finish = "GA_WorkFinished", interrupt = "GA_WorkInterrupted"
    },
    work = {kind = "gather", targetType = "resource_node", timeout = 12},
    head = {
        {state = "working", text = "worker.gathering", icon = "leaf", animation = "work_loop", priority = 100},
        {state = "carrying", text = "worker.transporting", icon = "payload", animation = "work_loop", priority = 90},
        {state = "no_energy", text = "worker.needs_energy", icon = "energy", animation = "warning", priority = 10},
        {state = "no_storage", text = "worker.storage_missing", icon = "storage_error", animation = "warning", priority = 20},
        {state = "no_motivation", text = "worker.needs_rest", icon = "rest", animation = "warning", priority = 30},
        {state = "overload", text = "worker.overloaded", icon = "load", animation = "pulse", priority = 40}
    }
}
