#include "terrainGen.hpp"
#include <SurfaceTool.hpp>
#include <Mesh.hpp>
#include <algorithm>

using namespace godot;

void TerrainGen::_register_methods() {
	//register_property<TerrainGen, Ref<Image>>("heightmap", &TerrainGen::set_heightmap, &TerrainGen::get_heightmap, Ref<Image>(NULL));

	register_method("set_heightmap", &TerrainGen::SetHeightmap);
	register_method("get_heightmap", &TerrainGen::GetHeightmap);
	register_method("generate_mesh", &TerrainGen::GenerateMesh);
	register_method("set_height", &TerrainGen::SetHeight);
	register_method("gen_densitymap", &TerrainGen::GenDensitymap);
	register_method("get_densitymap", &TerrainGen::GetDensitymap);
	register_method("get_mesh", &TerrainGen::GetMesh);
	register_method("density_func", &TerrainGen::DensityFunc);
}

void TerrainGen::_init() {
	std::cout << sizeof(TerrainGen) << std::endl;
}

void TerrainGen::SetHeightmap(Ref<Image> new_heightmap) {
	if (new_heightmap->get_height() != CHUNK_SIZE || new_heightmap->get_width() != CHUNK_SIZE)
		return;

	new_heightmap->lock();
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			_heightmap[i][j] = new_heightmap->get_pixel(i, j).get_v();
		}
	}
}

Array TerrainGen::GetHeightmap() {
	Array heightmapGDArr = Array();
	heightmapGDArr.resize(CHUNK_SIZE);

	for (int i = 0; i < CHUNK_SIZE; ++i) {
		Array row = Array();
		row.resize(CHUNK_SIZE);
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			row[j] = _heightmap[i][j];
		}
		heightmapGDArr[i] = row;
	}
	
	return heightmapGDArr;
}

Array TerrainGen::GetDensitymap() {
	Array densitymapGDArr = Array();
	densitymapGDArr.resize(CHUNK_SIZE);

	for (int i = 0; i < CHUNK_SIZE; ++i) {
		Array layer = Array();
		layer .resize(CHUNK_SIZE);
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			Array column = Array();
			column.resize(CHUNK_SIZE);
				for (int k = 0; k < CHUNK_SIZE; ++k) {
					column[k] = _densitymap[i][j][k];
				}
			layer[j] = column;
		}
		densitymapGDArr[i] = layer;
	}

	return densitymapGDArr;
}

void TerrainGen::GenDensitymap() {
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {
				_densitymap[i][j][k] = DensityFunc(i, j, k);
			}
		}
	}
}

void TerrainGen::GenerateMesh() {

	float corner_densities[8];
	int coordinates[3] = {0};
	short int cubeId;
	int edge_index;
	float cube_offset[3];
	Vertex vertex;
	//for (int i = 0; i < NUM_CUBES; i++) {
	//	vertices[i] = Vector3(0, 2, 0);
	//}

	int index = 0;
	for (int i = 0; i < CHUNK_SIZE - 1; i++) {	//x coordinates
		for (int j = 0; j < CHUNK_SIZE - 1; j++) {	//z coordinates
			for (int k = 0; k < std::min(int(ceil(height)), CHUNK_SIZE - 1); k++) {	//y coordinates
				for (int l = 0; l < 8; l++) {
					//assign xyz coordinates
					coordinates[0] = i + VERTEX_TABLE[l][0];
					coordinates[1] = j + VERTEX_TABLE[l][1];
					coordinates[2] = k + VERTEX_TABLE[l][2];

					corner_densities[l] = _densitymap[coordinates[0]][coordinates[1]][coordinates[2]];
				}

				cubeId = GetCubeId(corner_densities);

				cube_offset[0] = i;
				cube_offset[1] = j;
				cube_offset[2] = k;

				for (int vertex_index = 0; vertex_index < MAX_NUM_VERTICES_PER_CUBE; vertex_index++) {
					edge_index = TRIANGUALATION_TABLE[cubeId][vertex_index];
					if (edge_index != -1) {
						vertex = EdgeVertexPos(edge_index, corner_densities);
						vertex.x += i;
						vertex.y += j;
						vertex.z += k;
						vertices[index] = vertex;
						index++;
					}
					else {
						vertex_index = MAX_NUM_VERTICES_PER_CUBE;
					}
				}
			}
		}
	}

	numVertices = index;
}

void TerrainGen::SetHeight(float newHeight) {
	height = newHeight;
}

PoolVector3Array TerrainGen::GetMesh() {

	Array mesh_gdarr = Array();
	mesh_gdarr.resize(Mesh::ARRAY_MAX);

	Array vertex_gdarr = PoolVector3Array();
	Array uv_gdarr = Array();
	Array normal_gdarr = Array();
	
	vertex_gdarr.resize(numVertices);
	uv_gdarr.resize(numVertices);
	normal_gdarr.resize(numVertices);

	Vertex vertex;
	for (int i = 0; i < numVertices; i++) {
		vertex = vertices[i];
		vertex_gdarr[i] = Vector3(vertex.x, vertex.z, vertex.y);
		uv_gdarr[i] = Vector3(0, 0, 0);
		normal_gdarr[i] = Vector3(0, 0, 0);
	}

	return vertex_gdarr;

	//mesh_gdarr[Mesh::ARRAY_VERTEX] = vertex_gdarr;
	//mesh_gdarr[Mesh::ARRAY_TEX_UV] = vertex_gdarr;
	//mesh_gdarr[Mesh::ARRAY_NORMAL] = vertex_gdarr;

	//Ref<ArrayMesh> mesh;
	//mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, mesh_gdarr);

	//return mesh;

	Ref<SurfaceTool> st = SurfaceTool::_new();
	st->begin(Mesh::PRIMITIVE_TRIANGLES);

	const Vector3 normal = Vector3(0, 0, 1);
	//Vertex vertex;
	for (int i = 0; i < numVertices; i++) {
		vertex = vertices[i];
		st->add_normal(normal);
		st->add_vertex(Vector3(vertex.x, vertex.z, vertex.y));
	}

	st->index();
	st->generate_normals();

	//return st->commit();
}

int TerrainGen::GetCubeId(float values[8]) {
	short int cube_index = 0;
	for (int i = 0; i < 8; i++) {
		if (values[i] > 0) {
			cube_index |= 1 << i;
		}
	}

	return cube_index;
}

//float Cubifier::DensityFunc(int coordinates[3]) {
float TerrainGen::DensityFunc(int x, int y, int z) {
	/*int x = coordinates[0];
	int y = coordinates[1];
	int z = coordinates[2];*/
	float density = z - _heightmap[x][y] * height;
	return density;
}

Vertex TerrainGen::EdgeVertexPos(int edge_index, float corner_densities[8]) {
	Vertex pos;
	
	int edge[2];
	edge[0] = EDGE_TABLE[edge_index][0];
	edge[1] = EDGE_TABLE[edge_index][1];

	float v1 = corner_densities[edge[0]];
	float v2 = corner_densities[edge[1]];
	float density_range = abs(v1 - v2);
	float lerp_value = abs(v1) / density_range;

	float pos_arr[3];
	for (int i = 0; i < 3; i++) {
		//lerp between VERTEX_TABLE[edge[0]][i] and VERTEX_TABLE[edge[1]][i], apparently not already a function
		pos_arr[i] = VERTEX_TABLE[edge[0]][i] + lerp_value * (VERTEX_TABLE[edge[1]][i] - VERTEX_TABLE[edge[0]][i]);
	}

	pos.x = pos_arr[0];
	pos.y = pos_arr[1];
	pos.z = pos_arr[2];

	return pos;
}