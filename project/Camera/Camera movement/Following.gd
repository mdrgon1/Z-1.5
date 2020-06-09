extends State

#func enter(args):
#	var rot = Vector3(owner.angle, 0, 0)
#	rot.x = owner.angle
#	rot.z = 0
#	root_state.target_rotation = rot

func update(delta):
	if(Input.is_action_pressed("move_strafe")):
		return ["Pinpoint", owner.player]
		#return "Strafing"
	
	#angle from the forward vector to the player's target velocity
	var forward_to_velocity = owner.forward2D.angle_to(root_state.player.movement.vel_2D)
	
	#rotate the camera to align with player motion
	if(root_state.player.movement.get_input_vector() != Vector2(0, 0)):
		var rot = Vector3(owner.angle, 0, 0)

		#get angle out of player's movement vector
		var player_movement = root_state.player.movement.vel_3D
		rot.y = Vector2(player_movement.z, player_movement.x).angle() + PI
		
		
		print((rot.y / PI) * 180)
		root_state.target_rotation = rot
	
	if(Input.is_key_pressed(KEY_Q)):
		root_state.target_rotation.y += delta
	
	if(Input.is_key_pressed(KEY_E)):
		root_state.target_rotation.y -= delta
		
	#Lerp focus to player position
	root_state.target_translation = owner.get_translation() + root_state.focus_to_player()
