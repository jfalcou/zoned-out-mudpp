----------------------------------------------------------------------------------------------------
-- List of functions for managing players from outside
----------------------------------------------------------------------------------------------------

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
-- Dump a table to a string
-- Thanks to https://stackoverflow.com/questions/9168058
----------------------------------------------------------------------------------------------------
local function dump_table(node,name)
  local cache, stack, output = {},{},{}
  local depth = 1
  local output_str = name .. " = {\n\r"

  while true do
    local size = 0
    for k,v in pairs(node) do
      size = size + 1
    end

    local cur_index = 1
    for k,v in pairs(node) do
      if (cache[node] == nil) or (cur_index >= cache[node]) then

        if (string.find(output_str,"}",output_str:len())) then
          output_str = output_str .. ",\n\r"
        elseif not (string.find(output_str,"\n\r",output_str:len())) then
          output_str = output_str .. "\n\r"
        end

        -- This is necessary for working with HUGE tables otherwise
        -- we run out of memory using concat on huge strings
        table.insert(output,output_str)
        output_str = ""

        local key
        if (type(k) == "number" or type(k) == "boolean") then
          key = "["..tostring(k).."]"
        else
          key = "['"..tostring(k).."']"
        end

        if (type(v) == "number" or type(v) == "boolean") then
          output_str = output_str .. string.rep(' ',12*depth) .. key .. " = "..tostring(v)
        elseif (type(v) == "table") then
          output_str = output_str .. string.rep(' ',12*depth) .. key .. " = {\n\r"
          table.insert(stack,node)
          table.insert(stack,v)
          cache[node] = cur_index+1
          break
        else
          output_str = output_str .. string.rep(' ',12*depth) .. key .. " = '"..tostring(v).."'"
        end

        if (cur_index == size) then
          output_str = output_str .. "\n\r" .. string.rep(' ',12*(depth-1)) .. "}"
        else
          output_str = output_str .. ","
        end
      else
        -- close the table
        if (cur_index == size) then
          output_str = output_str .. "\n\r" .. string.rep(' ',12*(depth-1)) .. "}"
        end
      end

      cur_index = cur_index + 1
    end

    if (size == 0) then
      output_str = output_str .. "\n\r" .. string.rep(' ',12*(depth-1)) .. "}"
    end

    if (#stack > 0) then
      node = stack[#stack]
      stack[#stack] = nil
      depth = cache[node] == nil and depth + 1 or depth - 1
    else
      break
    end
  end

  -- This is necessary for working with HUGE tables otherwise
  -- we run out of memory using concat on huge strings
  table.insert(output,output_str)
  output_str = table.concat(output)

  return output_str
end

----------------------------------------------------------------------------------------------------
-- Save a player to a .player file
----------------------------------------------------------------------------------------------------
function save_player(current_player)
  local dump = dump_table(current_player.data,"data")
  current_player:save(dump)
end

----------------------------------------------------------------------------------------------------
game.log('Module player - loaded')
----------------------------------------------------------------------------------------------------
