-- Demo script showcasing the Lua scripting API
-- This script demonstrates zones, doors, sounds, and player interaction

local timer = 0
local greeted = false

function on_start()
  print("Demo script loaded!")
end

function on_update(dt)
  timer = timer + dt

  -- Periodic health regen example (every 5 seconds)
  if timer > 5.0 then
    timer = 0
    local hp = get_player_health()
    if hp > 0 and hp < 100 then
      set_player_health(math.min(hp + 5, 100))
      print("Health regenerated to " .. tostring(get_player_health()))
    end
  end
end

function on_zone_enter(zone_name)
  print("Entered zone: " .. zone_name)

  if zone_name == "arena" and not greeted then
    show_message("Welcome to the arena!")
    greeted = true
    -- Open all arena doors when player enters
    open_door(7, 4)
    open_door(7, 10)
    open_door(4, 7)
    open_door(10, 7)
  end

  if zone_name == "exit" then
    show_message("You found the exit!")
  end
end

function on_zone_exit(zone_name)
  print("Exited zone: " .. zone_name)
  if zone_name == "arena" then
    clear_message()
  end
end

function on_interact()
  local px, py = get_player_pos()
  print("Player interacted at " .. string.format("%.1f, %.1f", px, py))
end

function on_door_opened(x, y)
  print("Door opened at " .. tostring(x) .. ", " .. tostring(y))
  play_sound("door_open")
end

function on_entity_killed(entity_id)
  print("Entity killed: " .. tostring(entity_id))
end
