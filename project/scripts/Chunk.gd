extends Spatial
class_name Chunk

var mesh := MeshInstance.new()
var terrain_gen : Object = preload("res://bin/terrainGen.gdns").new()
var coordinates : Vector2	# should be vector2i, TODO fix this when I update Godot

export var heightmap : Image
export var height : float

func _ready():
	add_child(mesh)

func _generate_mesh() -> void:
	terrain_gen.generate_mesh_from_heightmap(heightmap, height)
	mesh.set_mesh(terrain_gen.get_mesh())

func _notification(what):
	if what == NOTIFICATION_PREDELETE:
		terrain_gen.free()
