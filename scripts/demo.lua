-- Demo script showcasing the Lua scripting API
-- Features: zones, doors, sounds, timers, entity control, messages

local greeted = false
local kill_count = 0
local regen_timer = nil

function on_start()
  print("Demo script loaded!")
  print("Map size: " .. tostring(select(1, get_map_size())) .. "x" .. tostring(select(2, get_map_size())))

  show_message("Welcome! Press E to interact with doors.", 5.0)

  -- Set up a repeating health regen timer (every 5 seconds)
  regen_timer = set_interval(5.0, "regen_health")

  -- Spawn a decorative entity
  if not entity_exists("guard") then
    spawn_entity("guard", 7.5, 7.5, 8, 64, 64)
    print("Spawned guard entity")
  end
end

function regen_health()
  local hp = get_player_health()
  local max_hp = get_player_max_health()
  if hp > 0 and hp < max_hp then
    set_player_health(math.min(hp + 5, max_hp))
    print("Health regenerated to " .. tostring(get_player_health()))
  end
end

function on_update(dt)
  -- Move the guard entity toward the player slowly
  if entity_exists("guard") then
    local px, py = get_player_pos()
    local gx, gy = get_entity_pos("guard")
    local dist = distance(px, py, gx, gy)
    if dist > 2.0 and dist < 10.0 then
      move_entity_toward("guard", px, py, 0.5 * dt)
    end
  end
end

function on_zone_enter(zone_name)
  print("Entered zone: " .. zone_name)

  if zone_name == "arena" and not greeted then
    show_message("You entered the arena!", 3.0)
    greeted = true
    -- Open all arena doors when player enters
    open_door(7, 4)
    open_door(7, 10)
    open_door(4, 7)
    open_door(10, 7)
  end

  if zone_name == "exit" then
    show_message("You found the exit! Well done!", 5.0)
  end

  if zone_name == "spawn" then
    show_message("Spawn area - you're safe here.", 2.0)
  end
end

function on_zone_exit(zone_name)
  print("Exited zone: " .. zone_name)
end

function on_interact()
  local px, py = get_player_pos()
  print("Player interacted at " .. string.format("%.1f, %.1f", px, py))

  -- Check if near the guard
  if entity_exists("guard") then
    local gx, gy = get_entity_pos("guard")
    if distance(px, py, gx, gy) < 2.0 then
      show_message("The guard acknowledges you.", 2.0)
    end
  end
end

function on_door_opened(x, y)
  print("Door opened at " .. tostring(x) .. ", " .. tostring(y))
  play_sound("door_open")
end

function on_entity_killed(entity_id, entity_name)
  kill_count = kill_count + 1
  print("Entity killed: " .. tostring(entity_name) .. " (total: " .. tostring(kill_count) .. ")")
  show_message("Kill #" .. tostring(kill_count) .. "!", 2.0)
end
