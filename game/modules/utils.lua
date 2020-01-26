----------------------------------------------------------------------------------------------------
-- Helpers functions module
----------------------------------------------------------------------------------------------------
local utils = {}

----------------------------------------------------------------------------------------------------
-- String shortcut syntax for sub string/indexing
----------------------------------------------------------------------------------------------------
getmetatable('').__index = function(str,i) return string.sub(str,i,i) end

getmetatable('').__call = function(str,i,j)
  if type(i)~='table' then return string.sub(str,i,j)
    else local t={}
    for k,v in ipairs(i) do t[k]=string.sub(str,v,v) end
    return table.concat(t)
    end
  end

----------------------------------------------------------------------------------------------------
-- Check a file exists
----------------------------------------------------------------------------------------------------
function utils.file_exists(name)
   local f=io.open(name,"r")
   if f~=nil then io.close(f) return true else return false end
end

----------------------------------------------------------------------------------------------------
-- Checks a string is empty
----------------------------------------------------------------------------------------------------
function utils.split(inputstr, sep)
  sep = sep or "%s"

  local t={}

  for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
    table.insert(t, str)
  end

  return t
end

----------------------------------------------------------------------------------------------------
-- Checks a string is empty
----------------------------------------------------------------------------------------------------
function utils.is_empty(s)
  return s == nil or s == ''
end

----------------------------------------------------------------------------------------------------
-- Checks if a table has data associated to a given key
----------------------------------------------------------------------------------------------------
function utils.contain_key(values, key)
  for k, _ in pairs(values) do
    if k == key then
      return true
    end
  end
  return false
end

----------------------------------------------------------------------------------------------------
-- Select a command from a command table
----------------------------------------------------------------------------------------------------
function utils.select_command(commands,choice)
  local current = choice

  if(type(current) == "string") then
    current = string.upper(current)
  end

  if( not utils.contain_key(commands,current) ) then
    current = "~"
  end

  return commands[current]
end

----------------------------------------------------------------------------------------------------
game.log('Module utils - loaded')
----------------------------------------------------------------------------------------------------

return utils

