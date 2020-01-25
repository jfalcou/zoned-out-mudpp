----------------------------------------------------------------------------------------------------
-- List of functions for managing players from outside
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- Load modules
----------------------------------------------------------------------------------------------------
engine = require "engine"

----------------------------------------------------------------------------------------------------
-- Main input processing loop
----------------------------------------------------------------------------------------------------
function process_input(current, player, input)
  local options =
  {
    [engine.state["login"]]          = engine.process_login,
    [engine.state["new_player"]]     = engine.create_player,
    [engine.state["ask_password"]]   = engine.ask_password,
    [engine.state["load_player"]]    = engine.load_player,
    [engine.state["check_password"]] = engine.check_password,
    [engine.state["play"]]           = engine.play
  }

  local func = utils.select_command(options,current)
  return func(player, input)
end

----------------------------------------------------------------------------------------------------
game.log('Player script successfully loaded.')
----------------------------------------------------------------------------------------------------
