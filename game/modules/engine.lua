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
engine.state =  { login = 0 , load_player = 10 , check_password = 11
                            , new_player  = 20 , ask_password   = 21
                , select_race = 30, select_class = 31
                , play = 50
                , disconnected = 60
                }

----------------------------------------------------------------------------------------------------
-- Process player's login
----------------------------------------------------------------------------------------------------
function engine.process_login(player, input)
  local commands =
  {
    ["L"] = {prompt="#b@yCharacter name:## "            , out="load_player" },
    ["C"] = {prompt="#b@yCharacter name:## "            , out="new_player"  },
    ["~"] = {prompt="##Sorry, your choice is invalid.\n", out="login"       }
  }

  local c = utils.select_command(commands,input)

  player:send(c.prompt, true)
  return engine.state[c.out]
end

----------------------------------------------------------------------------------------------------
-- Take care of loading a player
----------------------------------------------------------------------------------------------------
function engine.load_player(current_player, input)
  local savegame_path = path["saves"] .. input .. ".player";

  current_player.name = input

  if( game.player_exists(current_player) )  then
    game.log('Load player - ' .. input .. ' already logged.')
    current_player:send("@y#b" .. input .. "## is already logged in.\n", true)
    current_player:prompt()
    return engine.state["login"]
  end

  if( utils.file_exists(savegame_path) ) then
    game.log('Loading ' .. savegame_path)
    dofile(savegame_path)
    current_player.data     = player["data"]
    current_player.password = player["password"]
    current_player:send("#b@yPassword:## ", true )
    return engine.state["check_password"]
  else
    game.log('Unknown player ' .. input)
    current_player:send( "@rUnknown player @y" .. input .. "##\n" .. "@y#bCharacter name:## ", true )
    return engine.state["load_player"]
  end
end

----------------------------------------------------------------------------------------------------
-- Check password
----------------------------------------------------------------------------------------------------
function engine.check_password(current_player, input)
  if(current_player.password ~= input) then
    current_player:send("@rIncorrect password.##\n", true )
    current_player:send("#b@yPassword:## ", true )
    return engine.state["check_password"]
  else
    current_player:send( messages["returning_player"], true )
    current_player:enter(0)

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
game.log('Module engine - loaded')
----------------------------------------------------------------------------------------------------

return engine
