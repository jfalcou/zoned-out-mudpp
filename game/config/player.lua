----------------------------------------------------------------------------------------------------
-- List of functions for managing players from outside
----------------------------------------------------------------------------------------------------
dofile("config/utils.lua")
dofile("asset/races.lua")
dofile("asset/archetypes.lua")

----------------------------------------------------------------------------------------------------
-- Load a player from a .player file
----------------------------------------------------------------------------------------------------
function load_player(current_player, path)
  dofile(path)
  current_player.data     = player["data"]
  current_player.password = player["password"]
end

----------------------------------------------------------------------------------------------------
-- Prepare a player dynamic data
----------------------------------------------------------------------------------------------------
function setup_player(current_player)
  current_player.data = {}
end

----------------------------------------------------------------------------------------------------
-- Save a player to a .player file
----------------------------------------------------------------------------------------------------
function save_player(current_player)
  local dump = dump_table(current_player.data,"data")
  current_player:save(dump)
end

----------------------------------------------------------------------------------------------------
-- Process name input
----------------------------------------------------------------------------------------------------
local function handle_name(current_player,input)
  if( game.is_valid_name(input) ) then
    if(game.file_exists(input)) then
      current_player:send ( "The name @y" .. input .. "## is already in use.\n\r")
      return false
    else
      current_player.name = input
      setup_player(current_player)
      current_player:send( "Welcome @y#b" .. input .. "## !\n\r");
      return true
    end
  else
    current_player:send( "The name @y" .. input .. "## is invalid.\n\r");
    return false
  end
end
----------------------------------------------------------------------------------------------------
-- Handle player's character generation
----------------------------------------------------------------------------------------------------
local pc_menus =  { [0] = function(p) p:send("#b@yCharacter name:## ") end
                  , [1] = function(p) p:send("#b@yPassword:## ") end
                  , [2] = display_races
                  , [3] = display_archetypes
                  }

local pc_handlers = { [0] = handle_name
                    , [1] = function(p,i) p.password  = i return true end
                    , [2] = handle_races
                    , [3] = handle_archetypes
                    }

function creation_max_step()
  return length(pc_menus)
end

function creation_menu(current_player, step)
  local fn = pc_menus[step]
  fn(current_player)
end

function creation_handler(current_player, step, input)
  local fn = pc_handlers[step]
  if( fn(current_player, input) ) then
    return step+1
  else
    return step
  end
end

----------------------------------------------------------------------------------------------------
game.log('Module player - loaded')
----------------------------------------------------------------------------------------------------
