----------------------------------------------------------------------------------------------------
-- List of functions for managing players from outside
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
mudpp_log('Player script successfully loaded.')
----------------------------------------------------------------------------------------------------

function hello(p)
  p:send("Hello dear " .. p:name() ..", how are you ?\n")
  return state["play"]
end

----------------------------------------------------------------------------------------------------
-- Process player's login
----------------------------------------------------------------------------------------------------
function process_login(player, input)
  if( input == "L" or input == "l") then
    mudpp_log('Processing login - Load character.')
    player:send("Character name: ")
    return state["load_player"]
  elseif( input == "C" or input == "c") then
    mudpp_log('Processing login - Create character.')
    player:send("Character name: ")
    return state["new_player"]
  else
    player:send("Sorry, your choice is invalid.\n")
    return state["login"]
  end
end

----------------------------------------------------------------------------------------------------
-- Take care of loading a player
----------------------------------------------------------------------------------------------------
function load_player(current_player, input)
  local savegame_path = path["saves"] .. input .. ".player";

  current_player.name = input

  if( mudpp_player_exists(current_player) )  then
    mudpp_log('Load player - ' .. input .. ' already logged.')
    current_player:send(input .. " is already logged in.\n")
    current_player:prompt()
    return state["login"]
  end

  if( file_exists(savegame_path) ) then
    mudpp_log('Loading ' .. savegame_path)
    dofile(savegame_path)
    current_player.password = player["password"]
    current_player:send("Password: ")
    return state["check_password"]
  else
    mudpp_log('Unknown player ' .. input)
    current_player:send( "Unknown player " .. input .. "\n" .. "Character name:" )
    return state["load_player"]
  end
end

----------------------------------------------------------------------------------------------------
-- Check password
----------------------------------------------------------------------------------------------------
function check_password(current_player, input)
  if(current_player.password ~= input) then
    current_player:send("Incorrect password.\n")
    current_player:send("Password: ")
    return state["check_password"]
  else
    current_player:send( messages["returning_player"] )
    return state["play"]
  end
end

----------------------------------------------------------------------------------------------------
-- Create new character
----------------------------------------------------------------------------------------------------
function create_player(current_player, input)
  if( is_empty(input) ) then
    return state["new_player"]
  else
    current_player.name = input
    current_player:send("Welcome " .. input .. " !\n")
    current_player:send("Choose a password: \n")
    return state["ask_password"]
  end
end

----------------------------------------------------------------------------------------------------
-- Create new character's password
----------------------------------------------------------------------------------------------------
function ask_password(current_player, input)
  if( is_empty(input) ) then
    return state["ask_password"]
  else
    current_player.password = input
    current_player:send("Your password is: " .. input .. "\n")

    current_player:save();
    current_player:send( messages["new_player"] );

    return state["play"]
  end
end

----------------------------------------------------------------------------------------------------
-- Main game loop
----------------------------------------------------------------------------------------------------
function play(current_player, input)

  if( input == "/shutdown" ) then
    current_player:send("Bye " .. current_player.name .. " !\nThe server will now shutdown ...\n");
    return state["disconnected"]
  end

  --   if(input == "/quit")
  --   {
  --     game_context_.log(std::cout,"PLAYER") << name_ << " quitting." << std::endl;
  --     send("Bye " + name_ + " !\n");
  --     session_.disconnect();

  --     return state::disconnected;
  --   }
  --   else if(input == "/shutdown")
  --   {

  --   }
  --   else if(input == "/hello")
  --   {
  --     return game_context_.call<state>("hello"sv,*this);
  --   }
  --   else
  --   {
  current_player:send("No comprendo senor!\n");
  --   }

  return state["play"]
end

----------------------------------------------------------------------------------------------------
-- Main input processing loop
----------------------------------------------------------------------------------------------------
function process_input(current, player, input)
  local switch_table =
  {
    [state["login"]]          = process_login,
    [state["new_player"]]     = create_player,
    [state["ask_password"]]   = ask_password,
    [state["load_player"]]    = load_player,
    [state["check_password"]] = check_password,
    [state["play"]]           = play
  }

  local func = switch_table[current]
  return func(player, input)
end
