----------------------------------------------------------------------------------------------------
-- Game Engine module
----------------------------------------------------------------------------------------------------
local engine = {}

----------------------------------------------------------------------------------------------------
-- Load modules
----------------------------------------------------------------------------------------------------
utils   = require "utils"
command = require "command"
require "table"

----------------------------------------------------------------------------------------------------
-- In-game states table
----------------------------------------------------------------------------------------------------
engine.state =  { login = 0 , load_player = 1 , check_password = 2
                            , new_player  = 3 , ask_password    = 4
                , play = 5
                , disconnected = 6
                }

----------------------------------------------------------------------------------------------------
-- Process player's login
----------------------------------------------------------------------------------------------------
function engine.process_login(player, input)
  mudpp_log('Processing login via [' .. input .. ']')

  local commands =
  {
    ["L"] = {prompt="Character name: "                , out="load_player" },
    ["C"] = {prompt="Character name: "                , out="new_player"  },
    ["~"] = {prompt="Sorry, your choice is invalid.\n", out="login"       }
  }

  local c = utils.select_command(commands,input)

  player:send(c.prompt)
  return engine.state[c.out]
end

----------------------------------------------------------------------------------------------------
-- Take care of loading a player
----------------------------------------------------------------------------------------------------
function engine.load_player(current_player, input)
  local savegame_path = path["saves"] .. input .. ".player";

  current_player.name = input

  if( mudpp_player_exists(current_player) )  then
    mudpp_log('Load player - ' .. input .. ' already logged.')
    current_player:send(input .. " is already logged in.\n")
    current_player:prompt()
    return engine.state["login"]
  end

  if( utils.file_exists(savegame_path) ) then
    mudpp_log('Loading ' .. savegame_path)
    dofile(savegame_path)
    current_player.password = player["password"]
    current_player:send("Password: ")
    return engine.state["check_password"]
  else
    mudpp_log('Unknown player ' .. input)
    current_player:send( "Unknown player " .. input .. "\n" .. "Character name:" )
    return engine.state["load_player"]
  end
end

----------------------------------------------------------------------------------------------------
-- Check password
----------------------------------------------------------------------------------------------------
function engine.check_password(current_player, input)
  if(current_player.password ~= input) then
    current_player:send("Incorrect password.\n")
    current_player:send("Password: ")
    return engine.state["check_password"]
  else
    current_player:send( messages["returning_player"] )
    return engine.state["play"]
  end
end

----------------------------------------------------------------------------------------------------
-- Create new character
----------------------------------------------------------------------------------------------------
function engine.create_player(current_player, input)
  if( utils.is_empty(input) ) then
    return engine.state["new_player"]
  else
    current_player.name = input
    current_player:send("Welcome " .. input .. " !\n")
    current_player:send("Choose a password: \n")
    return engine.state["ask_password"]
  end
end

----------------------------------------------------------------------------------------------------
-- Create new character's password
----------------------------------------------------------------------------------------------------
function engine.ask_password(current_player, input)
  if( utils.is_empty(input) ) then
    return engine.state["ask_password"]
  else
    current_player.password = input
    current_player:send("Your password is: " .. input .. "\n")

    current_player:save();
    current_player:send( messages["new_player"] );

    return engine.state["play"]
  end
end

----------------------------------------------------------------------------------------------------
-- Main game loop
----------------------------------------------------------------------------------------------------
function engine.play(current_player, input)
  local args      = utils.split(input)
  local operation = table.remove(args,1)
  return command.perform(current_player, operation, args, engine.state)
end

----------------------------------------------------------------------------------------------------
mudpp_log('Module engine - loaded')
----------------------------------------------------------------------------------------------------

return engine

-- function hello(p)
--   p:send("Hello dear " .. p:name() ..", how are you ?\n")
--   return engine.state["play"]
-- end
