----------------------------------------------------------------------------------------------------
-- List of functions for managing players from outside
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- Load modules
----------------------------------------------------------------------------------------------------
engine = require "engine"

----------------------------------------------------------------------------------------------------
-- Player creation process
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- Create new character
----------------------------------------------------------------------------------------------------
function create_player(current_player, input)
  if( utils.is_empty(input) ) then
    return engine.state["new_player"]
  else

    if( string.find(input, '^[a-zA-Z0-9_]+$') ~= nil ) then
      local savegame_path = path["saves"] .. input .. ".player";
      if( utils.file_exists(savegame_path) ) then
        current_player:send (   "The name @y" .. input .. "## is already in use.\n\r"
                            ..  "@y#bCharacter name:## ", true
                            )

        return engine.state["new_player"]
      else
        current_player.name = input
        current_player.data = {}
        current_player:send("Welcome @y#b" .. input .. "## !\n\r", true )
        current_player:send("@y#bChoose a password:## \n\r", true )

        return engine.state["ask_password"]
      end
    else
      current_player:send (   "The name @y" .. input .. "## is invalid.\n\r"
                          ..  "@y#bCharacter name:## ", true
                          )

      return engine.state["new_player"]
    end
  end
end

----------------------------------------------------------------------------------------------------
-- Create new character's password
----------------------------------------------------------------------------------------------------
function ask_password(current_player, input)
  if( utils.is_empty(input) ) then
    return engine.state["ask_password"]
  else
    current_player.password = input
    current_player:send("Your password is: " .. input .. "\n\r")

    current_player:save();
    current_player:send( messages["new_player"] );

    current_player:enter(0)

    return engine.state["play"]
  end
end

----------------------------------------------------------------------------------------------------
game.log('Module player - loaded')
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- Main input processing loop
----------------------------------------------------------------------------------------------------
function process_input(current, player, input)
  local options =
  {
    [engine.state["login"]]          = engine.process_login,
    [engine.state["new_player"]]     = create_player,
    [engine.state["ask_password"]]   = ask_password,
    [engine.state["load_player"]]    = engine.load_player,
    [engine.state["check_password"]] = engine.check_password,
    [engine.state["play"]]           = engine.play
  }

  local func = utils.select_command(options,current)
  return func(player, input)
end
