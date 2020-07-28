extends State

const MIN_ANGLE := deg2rad(-60)


func run(delta):
	if(Input.is_action_pressed("move_strafe")):
		#return ["Pinpoint", owner.player]
		return "Strafing"
	
	var target_rot := Quat(owner.rotation)
	
	# point camera at the player (plus offset)
	var focus : Vector3 = root_state.get_offset_pos(owner.player.get_translation())
	focus += root_state.lead_vec
	target_rot = Quat(owner.transform.looking_at(focus, owner.UP).basis)
	
	# clamp camera angle
	var target_rot_euler = target_rot.get_euler()
	target_rot_euler.x = clamp(target_rot_euler.x, MIN_ANGLE, owner.angle)
	
	# move camera the correct distance from player
	var target_pos : Vector3
	if(target_rot_euler.x == target_rot.get_euler().x): # check if the rotation has been clamped or not
		target_pos = owner.player.get_translation() - root_state.to_player().normalized() * owner.distance
	else:
		#print(OS.get_ticks_msec())
		target_pos = focus + owner.transform.basis.z * owner.distance
	
	target_rot = Quat(target_rot_euler)
	
#	if(target_rot.get_euler().x > owner.angle):
#		#print(-target_rot.get_euler().x + owner.angle)
#		target_rot *= Quat(Vector3(-target_rot.get_euler().x + owner.angle, 0, 0))
	
	root_state.target_position = target_pos
	root_state.target_rotation = target_rot
