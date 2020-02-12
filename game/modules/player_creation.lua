----------------------------------------------------------------------------------------------------
-- Player Creation module
----------------------------------------------------------------------------------------------------
local player_creation = {}

----------------------------------------------------------------------------------------------------
-- Load modules
----------------------------------------------------------------------------------------------------
utils   = require "utils"
command = require "command"
require "table"

----------------------------------------------------------------------------------------------------
-- Player load state machine
----------------------------------------------------------------------------------------------------
local sm_create_player =
{
    outputs = { "play", "login" }
  , states = {
    ["new_player"] =
    {
      trigger = function(from,current_player,input)
                  if( utils.is_empty(input) ) then
                    return "new_player"
                  else
                    -- validate character name as being numbers,letters or underscore
                    if( string.find(input, '^[a-zA-Z0-9_]+$') ~= nil ) then
                      local savegame_path = path["saves"] .. input .. ".player";

                      if( utils.file_exists(savegame_path) ) then
                        current_player:send (   "The name @y" .. input .. "## is already in use.\n\r"
                                            ..  "@y#bCharacter name:## ", true
                                            )

                        return "new_player"
                      else
                        current_player.name = input
                        current_player.data = {}

                        current_player:send("Welcome @y#b" .. input .. "## !\n\r", true )
                        current_player:send("@y#bChoose a password:## \n\r", true )

                        return "ask_password"
                      end
                    else
                      current_player:send (   "The name @y" .. input .. "## is invalid.\n\r"
                                          ..  "@y#bCharacter name:## ", true
                                          )

                      return "new_player"
                    end
                  end
                end
    },

    ["ask_password"] =
    {
      trigger = function(from,current_player,input)
                  if( utils.is_empty(input) ) then
                    return "ask_password"
                  else
                    current_player.password = input
                    current_player:send("Your password is: " .. input .. "\n\r", true)

                    -- Save character before starting the game
                    current_player:save(utils.dump_table(current_player.data,"data"))

                    current_player:send( messages["new_player"], true );
                    current_player:enter(0)

                    return "play"
                  end
                end
    },

    ["~"] =
    {
      trigger = function(from,current_player,input)
                  current_player:send("##Sorry, your choice is invalid.\n\r", true)
                  return from
                end
    }
  }
}

----------------------------------------------------------------------------------------------------
-- Main Player Creation process
----------------------------------------------------------------------------------------------------
function player_creation.process(from, current_player, input)
  local state = utils.select_command(sm_create_player.states,current_player.create_state,false)
  next_state  = state.trigger(current_player.create_state, current_player, input )

  if( not utils.contain_value(sm_create_player.outputs, next_state) ) then
    current_player.create_state = next_state
    next_state = from
  end

  return next_state
end

----------------------------------------------------------------------------------------------------
game.log('Module player_creation - loaded')
----------------------------------------------------------------------------------------------------

return player_creation
