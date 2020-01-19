----------------------------------------------------------------------------------------------------
-- Helpers functions module
----------------------------------------------------------------------------------------------------
local utils = {}

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
function utils.is_empty(s)
  return s == nil or s == ''
end

----------------------------------------------------------------------------------------------------
-- Checks if a table has data associated to a given key
----------------------------------------------------------------------------------------------------
function utils.contain_key(table, key)
  for k, _ in pairs(table) do
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
mudpp_log('Module utils - loaded')
----------------------------------------------------------------------------------------------------

return utils

