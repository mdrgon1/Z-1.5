extends State

const ROT_SLERP = 7
const POS_LERP = 1

onready var target_position : Vector3 = owner.transform.origin
onready var target_rotation := Quat(owner.transform.basis)

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
