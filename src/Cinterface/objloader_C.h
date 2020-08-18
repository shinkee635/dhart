#include <vector>

#define C_INTERFACE extern "C" __declspec(dllexport) int

namespace HF {
	namespace Geometry {
		class MeshInfo;
		enum GROUP_METHOD;
	}
}

/**
* @defgroup Geometry
* Read and manipulate meshes.
* @{
*/

/*! 
	\brief Load an obj from the given path then rotate it by x,y, and z
	
	\param obj_path Filepath to the obj file to load. 
	\param gm Method to use for dividing a single obj file into multiple meshes.
	\param xrot Degrees to rotate the mesh on the x axis.
	\param yrot Degrees to rotate the mesh on the y axis.
	\param zrot Degrees to rotate the mesh on the z axis.
	\param out_data_array Output parameter for a pointer to the new array of MeshInfo
	\param num_meshes Output parameter for size of `out_data_array`

	\returns `HF_STATUS::OK` If the input was a valid mesh and the function completed successfully
	\returns `HF_STATUS::GENERIC_ERROR` If the input was empty. 
	\returns `HF_STATUS::INVALID_OBJ` if the path didn't lead to a valid OBJ file
	\returns` HF_STATUS::NOT_FOUND` if the file at the given path couldn't be found.

	\attention
	Call DestroyMeshInfoPtrArray to deallocate out_data_array, and call DestroyMeshInfo on each
	instance of MeshInfo to deallocate them individually. Failure to do any of this will result
	in memory leaks. In the case that `HF_STATUS::OK` is not called, no memory is allocated,
	and as such no memory must be deallocated.
	
	\code
	\endcode
*/
C_INTERFACE LoadOBJ(
	const char* obj_path,
	HF::Geometry::GROUP_METHOD gm,
	float xrot,
	float yrot,
	float zrot,
	HF::Geometry::MeshInfo *** out_data_array,
	int * num_meshes
);

/// <summary>
/// Store a mesh from python as meshinfo
/// </summary>
/// <param name="indices">An array of indices for the triangles in the mesh. Each integer should correspond to 3 values in vertices, and every 3 integers should represent a complete triangle for the mesh. </param>
/// <param name="num_indices">Length of the indices array.</param>
/// <param name="vertices"> Vertices of the mesh. Each 3 floats represent the X,Y, and Z of a point in space.</param>
/// <param name="num_vertices">Length of the vertices array.</param>
/// <param name="name">Desired name for the mesh</param>
/// <param name="id">Desired ID for the mesh</param>
/// <returns>HF_STATUS::OK if the mesh was loaded successfully. HF_STATUS::INVALID_OBJ if the given indices and vertices didn't create a valid mesh. </returns>

/*!
	\code
		// Requires #include "objloader_C.h", #include "meshinfo.h"

		// Prepare parameters for StoreMesh
		std::vector<HF::Geometry::MeshInfo>* info = nullptr;

		int mesh_indices[] = { 0, 1, 2 };
		const int mesh_num_indices = 3;
		float mesh_vertices[] = { 34.1, 63.9, 16.5, 23.5, 85.7, 45.2, 12.0, 24.6, 99.4 };
		const int mesh_num_vertices = 9;

		std::string mesh_name = "This mesh";
		const int mesh_id = 0;

		// Call StoreMesh
		if (StoreMesh(&info, mesh_indices, mesh_num_indices, mesh_vertices, mesh_num_vertices, mesh_name.c_str(), mesh_id)) {
			std::cout << "StoreMesh successful" << std::endl;
		}
		else {
			std::cout << "StoreMesh unsuccessful" << std::endl;
		}

		// Release memory for info once finished with it
		DestroyMeshInfo(info);
	\endcode
*/
C_INTERFACE StoreMesh(
	std::vector<HF::Geometry::MeshInfo>** out_info,
	const int* indices,
	int num_indices,
	const float* vertices,
	int num_vertices,
	const char* name,
	int id
);

/// <summary>
/// Rotate an existing mesh
/// </summary>
/// <param name="xrot"> Rotation around the x axis in degrees.</param>
/// <param name="yrot"> Rotation around the y axis in degrees.</param>
/// <param name="zrot"> Rotation around the z axis in degrees.</param>

/*!
	\code
		// Requires #include "objloader_C.h", #include "meshinfo.h"

		// Prepare parameters for StoreMesh
		std::vector<HF::Geometry::MeshInfo>* info = nullptr;

		int mesh_indices[] = { 0, 1, 2 };
		const int mesh_num_indices = 3;
		float mesh_vertices[] = { 34.1, 63.9, 16.5, 23.5, 85.7, 45.2, 12.0, 24.6, 99.4 };
		const int mesh_num_vertices = 9;

		std::string mesh_name = "This mesh";
		const int mesh_id = 0;

		// Call StoreMesh
		if (StoreMesh(&info, mesh_indices, mesh_num_indices, mesh_vertices, mesh_num_vertices, mesh_name.c_str(), mesh_id)) {
			std::cout << "StoreMesh successful" << std::endl;
		}
		else {
			std::cout << "StoreMesh unsuccessful" << std::endl;
		}

		// Prepare desired rotation values
		const float x_rot = 10;
		const float y_rot = 10;
		const float z_rot = 20;

		// Call RotateMesh
		if (RotateMesh(info, x_rot, y_rot, z_rot)) {
			std::cout << "RotateMesh successful" << std::endl;
		}
		else {
			std::cout << "RotateMesh unsuccessful" << std::endl;
		}

		// Release memory for info once finished with it
		DestroyMeshInfo(info);
	\endcode
*/
C_INTERFACE RotateMesh(
	std::vector<HF::Geometry::MeshInfo>* mesh_to_rotate,
	float xrot,
	float yrot,
	float zrot
);


/// <summary>
/// Delete all instances of mesh info in the given vector
/// </summary>
/// <param name="mesh_to_destroy"> The mesh info to destroy. </param>

/*!
	\code
		// Requires #include "objloader_C.h", #include "meshinfo.h"

		// Prepare parameters for LoadOBJ

		// relative path begins where EXE file is located if file_path is not a path to a
		// valid OBJ file, HF::Exceptions::FileNotFound is thrown
		std::string file_path = "big_teapot.obj";

		const int obj_length = file_path.size();

		const float x_rot = 30;
		const float y_rot = 20;
		const float z_rot = 55;

		std::vector<HF::Geometry::MeshInfo>* info = nullptr;

		// Call LoadOBJ
		if (LoadOBJ(file_path.c_str(), obj_length, x_rot, y_rot, z_rot, &info)) {
			std::cout << "LoadOBJ successful" << std::endl;
		}
		else {
			std::cout << "LoadOBJ unsuccessful" << std::endl;
		}

		// Release memory for info once finished with it
		DestroyMeshInfo(info);
	\endcode
*/
C_INTERFACE DestroyMeshInfo(HF::Geometry::MeshInfo * mesh_to_destroy);

C_INTERFACE DestroyMeshInfoPtrArray(HF::Geometry::MeshInfo** data_array);

/**@}*/

