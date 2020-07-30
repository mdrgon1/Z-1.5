extends Spatial
class_name Chunk

var mesh := MeshInstance.new()
var terrain_gen = preload("res://bin/terrainGen.gdns").new()

export var heightmap : Texture
export var height : float

func _generate_mesh():
	terrain_gen.generate_mesh_from_heightmap(heightmap.get_data(), height)
	mesh.set_mesh(terrain_gen.get_mesh())
	
	add_child(mesh)

func _notification(what):
	if what == NOTIFICATION_PREDELETE:
		mesh.free()
