extends State

const ROT_SLERP = 7
const POS_LERP = 1
const LEAD_AMOUNT := 6
const LEAD_LERP := 2

var lead_vec : Vector3

onready var target_position : Vector3 = owner.transform.origin
onready var target_rotation := Quat(owner.transform.basis)

func enter(_args):
	lead_vec = Vector3.ZERO
	.enter(_args)

func run(delta):
	if(owner.player.movement.vel_2D != Vector2(0, 0)):
		lead_vec = lerp(lead_vec, owner.player.movement.vel_3D.normalized() * root_state.LEAD_AMOUNT, root_state.LEAD_LERP * delta)

func to_player() -> Vector3:
	return owner.player.get_translation() - owner.get_translation()

#returns vector from the focus to the player
func focus_to_player() -> Vector3:
	return owner.player.get_translation() - owner.to_global(owner.focus.get_translation())

#returns vector from the focus to a target
func focus_to(target : Spatial) -> Vector3:
	return target.get_translation() - owner.to_global(owner.focus.get_translation())

func get_offset_pos(pos : Vector3) -> Vector3:
	pos += owner.offset.y * owner.transform.basis.y
	pos += owner.offset.x * owner.transform.basis.x
	return pos
