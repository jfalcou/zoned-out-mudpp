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
function command.go(current_player, args)
  local direction = string.upper(args[1])

  if(utils.contain_key(command.direction_map,direction)) then
    current_player:go(command.direction_map[direction])
  else
    current_player:send("Where are you going ?\n\r")
  end

  return "play"
end

----------------------------------------------------------------------------------------------------
-- look command
----------------------------------------------------------------------------------------------------
function command.look(current_player, args)
  if( utils.empty(args) ) then
    -- no arguments, look at the room
    current_player:send( current_player:location().description )
  else
    -- are we looking in a direction ?
    local target = string.upper(args[1])

    if(utils.contain_key(command.direction_map,target)) then
      local dir       = command.direction_map[target]
      local next_room = current_player:location():go(dir)
      if(next_room ~= -1) then
        local new_room = game.find_room(next_room)
        current_player:send("In this direction, you see " .. new_room.name .. "\n\r")
      else
        current_player:send("There is nothing to look at there\n\r")
      end
      -- elseif( ??? ) Other case ? like mob or items ?
    else
      current_player:send("There is nothing to look at there\n\r")
    end
  end
  return "play"
end

----------------------------------------------------------------------------------------------------
-- Quit command
----------------------------------------------------------------------------------------------------
function command.quit(current_player, args)
  game.broadcast("@y#b".. current_player.name .."## is leaving the game.\n\r")

  local dump = utils.dump_table(current_player.data,"data")

  current_player:save(dump)
  current_player:disconnect()

  return "disconnected"
end

----------------------------------------------------------------------------------------------------
-- Yell command
----------------------------------------------------------------------------------------------------
function command.yell(current_player, args)
  local msg = "@y#b".. current_player.name .. "## yells ''"
  for _,value in pairs(args) do msg = msg .. " " .. value end

  game.yell(current_player:location(), msg .. " ##''\n\r")

  return "play"
end

----------------------------------------------------------------------------------------------------
-- Say command
----------------------------------------------------------------------------------------------------
function command.say(current_player, args)
  local other_player = args[1]
  table.remove(args,1)

  local msg = "@y#b".. current_player.name .. "## says ''"
  for _,value in pairs(args) do msg = msg .. " " .. value end

  local ok = game.say(other_player, msg .."## ''\n\r")

  if(not ok) then
    current_player:send(other_player .. " is not nearby.\n\r")
  end

  return "play"
end

----------------------------------------------------------------------------------------------------
-- Shutdown command
----------------------------------------------------------------------------------------------------
function command.shutdown(current_player, args)
  game.broadcast( "@r#bThe server will now shutdown ...##\n\r")
  current_player:shutdown()
  return "disconnected"
end

----------------------------------------------------------------------------------------------------
-- Unknown command
----------------------------------------------------------------------------------------------------
function command.unknown(current_player, args)
  current_player:send("What do you mean ?\n\r")
  return "play"
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
  ["N"]         = { op = function(p,a) return command.go(p,{"N"},s) end , flags = "" },
  ["S"]         = { op = function(p,a) return command.go(p,{"S"},s) end , flags = "" },
  ["E"]         = { op = function(p,a) return command.go(p,{"E"},s) end , flags = "" },
  ["W"]         = { op = function(p,a) return command.go(p,{"W"},s) end , flags = "" },
  ["U"]         = { op = function(p,a) return command.go(p,{"U"},s) end , flags = "" },
  ["D"]         = { op = function(p,a) return command.go(p,{"D"},s) end , flags = "" },
  --------------------------------------------------------------------------------------------------
  -- Actions
  --------------------------------------------------------------------------------------------------
  ["LOOK"]      = { op = command.look, flags = "" },
  --------------------------------------------------------------------------------------------------
  -- In-Game communication
  --------------------------------------------------------------------------------------------------
  ["YELL"]      = { op = command.yell, flags = "" },
  ["SAY"]       = { op = command.say, flags = "" },
  --------------------------------------------------------------------------------------------------
  ["~"]         = { op = command.unknown  , flags = "" }
}

----------------------------------------------------------------------------------------------------
-- Select and execute command command
----------------------------------------------------------------------------------------------------
function command.perform(current_player, cmd, args)
  local com = utils.select_command(command.supported_commands,cmd,true)

  -- TODO : check flags with curren_player:has_flags(com.flags )
  return com.op (current_player, args)
end

----------------------------------------------------------------------------------------------------
game.log('Module command - loaded')
----------------------------------------------------------------------------------------------------

return command
