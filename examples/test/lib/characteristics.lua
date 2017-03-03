-- character.lua
-- give the dog somee basic emotions and characteristics
-- basically works like a rpg character sheet
-- version: 	0.1.0
-- date:		2016/07/26

character = {}

--[[
	basic function to set a characteristic
	try to avoid this and use specific functions
--]]
function character.set_emotion(name, value)
	persist(name, value)
end

--[[
	basic function to get a characteristic
	try to avoid this and use specific functions
--]]	
function character.get_emotion(name)
	return retrieve (name)
end

--[[
	arousal -50 is calm 50 is excited
--]]
function character.get_arousal()
	return character.get_emotion("arousal")
end
function character.set_arousal(value)
	if value < 0
		value = 0
	end
	if value > 100
		value = 100
	end
		
	character.set_emotion("arousal", value)
end
function character.alter_arousal(value)
	newval = character.get_arousal() + value
	character.set_arousal(newval)
end

--[[
	valence -50 is negative 50 is positive
--]]
function character.get_valence()
	return character.get_emotion("valence")
end
function character.set_valence(value)
	if value < 0
		value = 0
	end
	if value > 100
		value = 100
	end
		
	character.set_emotion("valence", value)
end
function character.alter_valence(value)
	newval = character.get_valence() + value
	character.set_valence(newval)
end

--[[
	aim is a list of hashes 
	each entry has alway a "type" field
--]]
function character.get_aim() 
	return character.get_emotion("aim")
end
function character.set_aim(aim) 
	return character.set_emotion("aim", aim)
end
	
return character
