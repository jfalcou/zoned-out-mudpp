----------------------------------------------------------------------------------------------------
-- MUDpp Archetypes
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- List all archetypes
----------------------------------------------------------------------------------------------------
archetypes = {}
dofile("asset/archetypes/fighter.lua")
dofile("asset/archetypes/ranger.lua")
dofile("asset/archetypes/cleric.lua")
dofile("asset/archetypes/caster.lua")

----------------------------------------------------------------------------------------------------
-- Display all archetypes
----------------------------------------------------------------------------------------------------
function display_archetypes(current_player)
  current_player:send("Choose your #bArchetype##:\n\r")
  for _,r in pairs(archetypes) do
    current_player:send("#b@y" .. r.name .. "##\t- " .. r.desc .. "\n\r")
  end
end

----------------------------------------------------------------------------------------------------
-- Handle archetype selection
----------------------------------------------------------------------------------------------------
function handle_archetypes(current_player, input)
  local choice = string.lower(input)
  if( contain_key(archetypes, choice) ) then
    local chosen_archetype = archetypes[choice]
    current_player.data["archetype"] = chosen_archetype.name
    current_player:send("Your #b@yArchetype## is #b@y" .. current_player.data["archetype"] .. "##\n\r")
    return true
  else
    return false
  end
end
