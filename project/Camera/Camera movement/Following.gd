extends State

const MIN_ANGLE := -60 * (PI / 180)
const LEAD_AMOUNT := 6
const LEAD_LERP := 2

var lead_vec : Vector3

func enter(_args):
	lead_vec = Vector3(0, 0, 0)

func run(delta):
	if(Input.is_action_pressed("move_strafe")):
		#return ["Pinpoint", owner.player]
		return "Strafing"
	
	var target_rot := Quat(owner.rotation)
	
	if(owner.player.movement.vel_2D != Vector2(0, 0)):
		lead_vec += (owner.player.movement.vel_3D.normalized() * LEAD_AMOUNT - lead_vec) * LEAD_LERP * delta
	
	# point camera at the player (plus offset)
	var focus : Vector3 = root_state.get_offset_pos(owner.player.get_translation())
	focus += lead_vec
	target_rot = Quat(owner.transform.looking_at(focus, owner.UP).basis)
	
	# move camera the correct distance from player
	var target_pos : Vector3 = owner.player.get_translation() - root_state.to_player().normalized() * owner.distance
	
	# clamp camera height based on min and max angle
	var min_height : float = owner.player.get_translation().y + sin(-owner.angle) * owner.distance
	var max_height : float = owner.player.get_translation().y + sin(-MIN_ANGLE) * owner.distance
	target_pos.y = clamp(target_pos.y, min_height, max_height)
	
#	if(target_rot.get_euler().x > owner.angle):
#		#print(-target_rot.get_euler().x + owner.angle)
#		target_rot *= Quat(Vector3(-target_rot.get_euler().x + owner.angle, 0, 0))
	
	root_state.target_position = target_pos
	root_state.target_rotation = target_rot
