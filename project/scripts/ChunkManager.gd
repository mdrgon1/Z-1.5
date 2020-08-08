extends Node

const INITIAL_NUM_CHUNKS_POOLED := 8

var chunk_pool : Array
var loading_queue : Array
var loaded_chunks : Dictionary
var chunks : Dictionary
var heightmap : Image = preload("res://heightmap.png").get_data()

func _ready():
	for i in INITIAL_NUM_CHUNKS_POOLED:
		chunk_pool.push_back(Chunk.new())
	
	for i in 16:
		var coordinates := Vector2(i % 4, int(i / 4))
		loading_queue.push_back(coordinates)

func _process(delta):
	
	# generate chunks, don't add them to tree just yet 
	if(loading_queue.size() != 0):# this is just to limit chunk generation to once a frame, TODO replace this with actual multithreading or something
		
		var coords = loading_queue.pop_front()
		load_chunk(coords)
		
		add_chunk(coords)
	else:
		var keys = chunks.keys()
		for key in keys:
			unload_chunk(key)
		loading_queue += keys

# generate terrain geometry
# does not add the chunk to the scenetree, just creates all the data
func load_chunk(coords : Vector2) -> void:
	
	# pull chunk from pool if possible
	var chunk : Chunk
	if(chunk_pool.size() == 0):
		chunk = Chunk.new()
	else:
		chunk = chunk_pool.pop_back()
	
	chunk.heightmap = heightmap.get_rect(Rect2(coords * 15, Vector2(16, 16)))
	chunk.height = 5
	chunk.translation.x = coords.x * 15
	chunk.translation.y = -10
	chunk.translation.z = coords.y * 15
	chunk._generate_mesh()
	
	loaded_chunks[coords] = chunk

# add a generated chunk to the tree
func add_chunk(coords : Vector2) -> void:
	
	if(!loaded_chunks.has(coords)):
		load_chunk(coords)
	
	add_child(loaded_chunks[coords])
	chunks[coords] = loaded_chunks[coords]

# remove a chunk from the tree
func remove_chunk(coords : Vector2) -> void:
	remove_child(chunks[coords])
	
	# chunks are returned to the pool after deletion, even if they didn't come from there
	chunk_pool.push_back(chunks[coords])
	
	chunks.erase(coords)

# delete the chunk from memory
func unload_chunk(coords : Vector2) -> void:
	
	if chunks.has(coords):
		remove_chunk(coords)
	
	loaded_chunks[coords].queue_free()
	loaded_chunks.erase(coords)
