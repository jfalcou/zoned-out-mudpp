----------------------------------------------------------------------------------------------------
-- Game Engine module
----------------------------------------------------------------------------------------------------
local command = {}

----------------------------------------------------------------------------------------------------
-- Load modules
----------------------------------------------------------------------------------------------------
utils   = require "utils"

----------------------------------------------------------------------------------------------------
-- go command
----------------------------------------------------------------------------------------------------
function command.go(current_player, cmd, args, states)
  local dmap  = { ["N"] = 0, ["NORTH"] = 0
                , ["S"] = 1, ["SOUTH"] = 1
                , ["E"] = 2, ["EAST" ] = 2
                , ["W"] = 3, ["WEST" ] = 3
                , ["U"] = 4, ["UP"   ] = 4
                , ["D"] = 5, ["DOWN" ] = 5
                }

  local direction = string.upper(args[1])

  if(utils.contain_key(dmap,direction)) then
    current_player:go(dmap[direction])
  end

  return states["play"]
end

----------------------------------------------------------------------------------------------------
-- Quit command
----------------------------------------------------------------------------------------------------
function command.quit(current_player, cmd, args, states)
  game.broadcast("@y#b".. current_player.name .."## is leaving the game.\n", true )
  current_player:disconnect()
  return states["disconnected"]
end

----------------------------------------------------------------------------------------------------
-- Shutdown command
----------------------------------------------------------------------------------------------------
function command.shutdown(current_player, cmd, args, states)
  game.broadcast( "@r#bThe server will now shutdown ...##\n", true )
  current_player:shutdown()
  return states["disconnected"]
end

----------------------------------------------------------------------------------------------------
-- Unknown command
----------------------------------------------------------------------------------------------------
function command.unknown(current_player, cmd, args, states)
  if(not utils.is_empty(cmd)) then
    current_player:send("I don't understand. What do you mean by '@r#b".. cmd .."##' ?\n", true)
  end

  return states["play"]
end


----------------------------------------------------------------------------------------------------
-- Command repository
----------------------------------------------------------------------------------------------------
command.supported_commands =
{
  ["/SHUTDOWN"] = { callback=command.shutdown , permission="" },
  ["/QUIT"]     = { callback=command.quit     , permission="" },
  ["GO"]        = { callback=command.go       , permission="" },
  ["~"]         = { callback=command.unknown  , permission="" }
}

----------------------------------------------------------------------------------------------------
-- Select and execute command command
----------------------------------------------------------------------------------------------------
function command.perform(current_player, cmd, args, states)
  local com = utils.select_command(command.supported_commands,cmd)

  -- TODO : check permission with curren_player:has_permission(com.permission)
  return com.callback(current_player, cmd, args, states)
end

----------------------------------------------------------------------------------------------------
game.log('Module command - loaded')
----------------------------------------------------------------------------------------------------

return command
