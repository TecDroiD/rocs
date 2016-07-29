-- intarray.lua
-- helps working with integer arrays from strings
-- version: 	0.1.0
-- date:		2016/07/26

intarray = {}

--[[ 
  gets the size of the array
  @param array - a string which contains the integer data
  @param bytes - bytes each element uses
  @return the number of elements in the array
--]] 
function intarray.getsize(array,bytes) 
	return string.len(array) / bytes
end

--[[
  checks if the array size matches the byte size
  @param array - a string which contains the integer data
  @param bytes - bytes each element uses
  @return true if the array size is n times the element size, otherwise false
--]]
function intarray.matches(array,bytes)
	if 0 == (string.len(array) % bytes)
		return true
	else
		return false
	end -- if	
end

--[[
  gets an element of the array
  ! it does not check bounds intentionally !
   
  @param array - a string which contains the integer data
  @param number - the number of the element to get
  @param bytes - bytes each element uses
  @return the element value  
--]]
function intarray.get(array, number, bytes)   
  local val = 0
	for i=1,bytes,+1
	do
		local elemid = bytes * number
		-- val << 8 + new element value
		val = (val * 2^8) + string.byte(array,elemid)
	end -- for

  return val
end

--[[ return the intarray object --]]
return intarray
