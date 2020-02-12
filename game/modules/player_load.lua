----------------------------------------------------------------------------------------------------
-- Player Loading module
----------------------------------------------------------------------------------------------------
local player_load = {}

----------------------------------------------------------------------------------------------------
-- Load modules
----------------------------------------------------------------------------------------------------
utils   = require "utils"
command = require "command"
require "table"

----------------------------------------------------------------------------------------------------
-- Player load state machine
----------------------------------------------------------------------------------------------------
local sm_load_player =
{
    outputs = { "play", "login" }
  , states = {
    ["load_player"] =
    {
      trigger = function(from,current_player,input)
                  local savegame_path = path["saves"] .. input .. ".player";

                  current_player.name = input

                  if( game.player_exists(current_player) )  then
                    game.log('Load player - ' .. input .. ' already logged.')
                    current_player:send("@y#b" .. input .. "## is already logged in.\n\r")
                    current_player:prompt()
                    return "login"
                  end

                  if( utils.file_exists(savegame_path) ) then
                    game.log('Loading ' .. savegame_path)
                    dofile(savegame_path)
                    current_player.data     = player["data"]
                    current_player.password = player["password"]
                    current_player:send("#b@yPassword:## ")
                    return "check_password"
                  else
                    game.log('Unknown player ' .. input)
                    current_player:send (   "@rUnknown player @y" .. input .. "##\n\r"
                                        ..  "@y#bCharacter name:## "
                                        )
                    return "load_player"
                  end
                end
    },

    ["check_password"] =
    {
      trigger = function(from,current_player,input)
                  if(current_player.password ~= input) then
                    current_player:send("@rIncorrect password.##\n\r")
                    current_player:send("#b@yPassword:## ", true )
                    return "check_password"
                  else
                    current_player:send( messages["returning_player"])
                    current_player:enter(0)

                    return "play"
                  end
                end
    },

    ["~"] =
    {
      trigger = function(from,current_player,input)
                  current_player:send("##Sorry, your choice is invalid.\n\r")
                  return from
                end
    }
  }
}

----------------------------------------------------------------------------------------------------
-- Main Player Creation process
----------------------------------------------------------------------------------------------------
function player_load.process(from, current_player, input)
  local state = utils.select_command(sm_load_player.states,current_player.load_state,false)
  next_state  = state.trigger(current_player.load_state, current_player, input )

  if( not utils.contain_value(sm_load_player.outputs, next_state) ) then
    current_player.load_state = next_state
    next_state = from
  end

  return next_state
end

----------------------------------------------------------------------------------------------------
game.log('Module player_load - loaded')
----------------------------------------------------------------------------------------------------

return player_load
