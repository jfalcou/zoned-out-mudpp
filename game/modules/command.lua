----------------------------------------------------------------------------------------------------
-- Game Engine module
----------------------------------------------------------------------------------------------------
local command = {}

----------------------------------------------------------------------------------------------------
-- Load modules
----------------------------------------------------------------------------------------------------
utils   = require "utils"

command.direction_map = { ["N"] = 0, ["NORTH"] = 0
                        , ["S"] = 1, ["SOUTH"] = 1
                        , ["E"] = 2, ["EAST" ] = 2
                        , ["W"] = 3, ["WEST" ] = 3
                        , ["U"] = 4, ["UP"   ] = 4
                        , ["D"] = 5, ["DOWN" ] = 5
                        }

----------------------------------------------------------------------------------------------------
-- go command
----------------------------------------------------------------------------------------------------
function command.go(current_player, args, states)
  local direction = string.upper(args[1])

  if(utils.contain_key(command.direction_map,direction)) then
    current_player:go(command.direction_map[direction])
  else
    current_player:send("Where are you going ?\n")
  end

  return states["play"]
end

----------------------------------------------------------------------------------------------------
-- look command
----------------------------------------------------------------------------------------------------
function command.look(current_player, args, states)
  if( utils.empty(args) ) then
    -- no arguments, look at the room
    current_player:send( current_player:location().description, true )
  else
    -- are we looking in a direction ?
    local target = string.upper(args[1])

    if(utils.contain_key(command.direction_map,target)) then
      local dir  = command.direction_map[target]
      local next_room = current_player:location()
      next_room = next_room:go(dir)
      local new_room = game.find_room(next_room)
      current_player:send("In this direction, you see " .. new_room.name .. "\n", true)
    end
  end
  return states["play"]
end

----------------------------------------------------------------------------------------------------
-- Quit command
----------------------------------------------------------------------------------------------------
function command.quit(current_player, args, states)
  game.broadcast("@y#b".. current_player.name .."## is leaving the game.\n", true )
  current_player:disconnect()
  return states["disconnected"]
end

----------------------------------------------------------------------------------------------------
-- Shutdown command
----------------------------------------------------------------------------------------------------
function command.shutdown(current_player, args, states)
  game.broadcast( "@r#bThe server will now shutdown ...##\n", true )
  current_player:shutdown()
  return states["disconnected"]
end

----------------------------------------------------------------------------------------------------
-- Unknown command
----------------------------------------------------------------------------------------------------
function command.unknown(current_player, args, states)
  if(not utils.is_empty(cmd)) then
    current_player:send("What do you mean ?\n", false)
  end

  return states["play"]
end


----------------------------------------------------------------------------------------------------
-- Command repository
----------------------------------------------------------------------------------------------------
command.supported_commands =
{
  --------------------------------------------------------------------------------------------------
  -- Admin commands
  --------------------------------------------------------------------------------------------------
  ["/SHUTDOWN"] = { op = command.shutdown , flags = "" },
  ["/QUIT"]     = { op = command.quit     , flags = "" },

  --------------------------------------------------------------------------------------------------
  -- Directions
  --------------------------------------------------------------------------------------------------
  ["GO"]        = { op = command.go       , flags = "" },
  ["N"]         = { op = function(p,a,s) return command.go(p,{"N"},s) end , flags = "" },
  ["S"]         = { op = function(p,a,s) return command.go(p,{"S"},s) end , flags = "" },
  ["E"]         = { op = function(p,a,s) return command.go(p,{"E"},s) end , flags = "" },
  ["W"]         = { op = function(p,a,s) return command.go(p,{"W"},s) end , flags = "" },
  ["U"]         = { op = function(p,a,s) return command.go(p,{"U"},s) end , flags = "" },
  ["D"]         = { op = function(p,a,s) return command.go(p,{"D"},s) end , flags = "" },
  --------------------------------------------------------------------------------------------------
  -- Actions
  --------------------------------------------------------------------------------------------------
  ["LOOK"]      = { op = command.look, flags = "" },
  --------------------------------------------------------------------------------------------------
  ["~"]         = { op = command.unknown  , flags = "" }
}

----------------------------------------------------------------------------------------------------
-- Select and execute command command
----------------------------------------------------------------------------------------------------
function command.perform(current_player, cmd, args, states)
  local com = utils.select_command(command.supported_commands,cmd)

  -- TODO : check flags with curren_player:has_flags(com.flags )
  return com.op (current_player, args, states)
end

----------------------------------------------------------------------------------------------------
game.log('Module command - loaded')
----------------------------------------------------------------------------------------------------

return command
