----------------------------------------------------------------------------------------------------
-- List of functions for managing players from outside
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- Load modules
----------------------------------------------------------------------------------------------------
utils   = require "utils"
command = require "command"
pc = require "player_creation"
pl = require "player_load"

local sm_login =
{
    outputs = {}
  , states =
  {
    ["login"] =
    {
      trigger = function(from, current_player, input)
                  local commands =
                  {
                    ["L"] = {prompt="#b@yCharacter name:## "              , out="load_player" },
                    ["C"] = {prompt="#b@yCharacter name:## "              , out="new_player"  },
                    ["~"] = {prompt="##Sorry, your choice is invalid.\n\r", out="login"       }
                  }

                  local c = utils.select_command(commands,input,true)
                  current_player:send(c.prompt, true)

                  -- Going back to login requires a prompt
                  if(c.out == "login") then
                    current_player:prompt()
                  end

                  return c.out
                end
    },

    ["load_player"] =
    {
      trigger = function(from,current_player,input)
                  current_player.load_state = current_player.load_state or "load_player"
                  return pl.process("load_player", current_player,input)
                end
    },

    ["new_player"] =
    {
      trigger = function(from,current_player,input)
                  current_player.create_state = current_player.create_state or "new_player"
                  return pc.process("new_player", current_player,input)
                end
    },

    ["play"] =
    {
      trigger = function(from,current_player,input)
                  local args      = utils.split(input)
                  local operation = table.remove(args,1)
                  return command.perform(current_player, operation, args)
                end
    }

    , ["~"] =
    {
      trigger = function(from,current_player,input)
                  current_player:send("##Sorry, your choice is invalid.\n\r", true)
                  return from
                end
    }
  }
}

----------------------------------------------------------------------------------------------------
game.log('Module player - loaded')
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- Main input processing loop
----------------------------------------------------------------------------------------------------
function process_input(current, current_player, input)
  local state = utils.select_command(sm_login.states,current,false)
  return state.trigger(current, current_player, input )
end
